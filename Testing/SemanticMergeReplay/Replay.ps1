<#
.SYNOPSIS
  Replays real merge commits through WinMerge to measure merge outcomes.

.DESCRIPTION
  collect: walks the merge commits of the given repositories and extracts
           every file that both parents changed and that a line-based
           three-way merge (git merge-file) cannot merge cleanly. Each case
           holds the base, both parents and the committed result.
  run:     merges every case with WinMerge, once per configuration, and
           compares the output with the committed result.

  tokens:  runs only the token-level merge over every conflict of the corpus
           (through the unit test binary, in seconds) and reports how many
           it merges and how many of those the committed file confirms.

  A merge that leaves no conflict but differs from what was committed is
  reported as "differs": either a wrong merge, or the committer changed more
  than the merge needed. Those are the cases to read by hand.

.EXAMPLE
  .\Replay.ps1 collect -Repos C:\git\Nu,C:\git\FsCheck -MaxPerRepo 100
  .\Replay.ps1 run
#>
param(
    [Parameter(Mandatory, Position = 0)][ValidateSet('collect', 'run', 'tokens')][string]$Command,
    [string[]]$Repos = @(),
    [string]$Corpus = (Join-Path $PSScriptRoot '..\..\Build\SemanticReplay\corpus'),
    [string]$Output = (Join-Path $PSScriptRoot '..\..\Build\SemanticReplay\run'),
    [string]$WinMerge = (Join-Path $PSScriptRoot '..\..\Build\x64\Release\WinMergeU.exe'),
    [string[]]$Extensions = @('.c', '.h', '.cpp', '.cs', '.fs', '.py', '.java', '.js', '.ts', '.go', '.rs', '.rb', '.php'),
    [int]$MaxPerRepo = 200,
    [int]$MaxFileBytes = 400000,
    [int]$Parallel = 6,
    [int]$TimeoutSeconds = 90,
    [string]$Filter = '*'
)

$ErrorActionPreference = 'Stop'
$Repos = @($Repos | ForEach-Object { $_ -split ',' } | Where-Object { $_ })

function Save-Blob([string]$repo, [string]$spec, [string]$path) {
    # cmd keeps the bytes as they are; PowerShell would re-encode the text
    cmd /c "git -C `"$repo`" show `"$spec`" > `"$path`" 2>nul"
    return $LASTEXITCODE -eq 0
}

function Invoke-Collect {
    New-Item -ItemType Directory -Force $Corpus | Out-Null
    foreach ($repo in $Repos) {
        $name = Split-Path $repo -Leaf
        $count = 0
        $merges = git -C $repo rev-list --merges --min-parents=2 --max-parents=2 --all
        foreach ($merge in $merges) {
            if ($count -ge $MaxPerRepo) { break }
            $parents = (git -C $repo rev-list --parents -n 1 $merge) -split ' '
            $ours = $parents[1]; $theirs = $parents[2]
            $base = @(git -C $repo merge-base $ours $theirs 2>$null)[0]
            if (-not $base) { continue }
            $changedOurs = @(git -C $repo diff --name-only --diff-filter=M $base $ours 2>$null)
            if ($changedOurs.Count -eq 0) { continue }
            $changedTheirs = @(git -C $repo diff --name-only --diff-filter=M $base $theirs 2>$null)
            $both = $changedOurs | Where-Object { $changedTheirs -contains $_ -and $Extensions -contains [IO.Path]::GetExtension($_).ToLower() }
            foreach ($file in $both) {
                if ($count -ge $MaxPerRepo) { break }
                $ext = [IO.Path]::GetExtension($file).ToLower()
                $dir = Join-Path $Corpus ("{0}\{1}_{2:d3}" -f $name, $merge.Substring(0, 10), $count)
                New-Item -ItemType Directory -Force $dir | Out-Null
                $ok = (Save-Blob $repo "${base}:$file" "$dir\base$ext") -and
                      (Save-Blob $repo "${ours}:$file" "$dir\ours$ext") -and
                      (Save-Blob $repo "${theirs}:$file" "$dir\theirs$ext") -and
                      (Save-Blob $repo "${merge}:$file" "$dir\result$ext")
                $conflicts = 0
                if ($ok -and (Get-Item "$dir\base$ext").Length -le $MaxFileBytes) {
                    cmd /c "git merge-file -p `"$dir\ours$ext`" `"$dir\base$ext`" `"$dir\theirs$ext`" > `"$dir\gitmerge$ext`" 2>nul"
                    $conflicts = $LASTEXITCODE
                }
                # only keep what a line-based merge cannot do (exit code = number of conflicts)
                if (-not $ok -or $conflicts -le 0 -or $conflicts -ge 127) {
                    Remove-Item -Recurse -Force $dir
                    continue
                }
                "repo=$name`nmerge=$merge`nfile=$file`ngitconflicts=$conflicts" | Set-Content "$dir\case.txt"
                $count++
            }
        }
        Write-Host ("{0}: {1} cases" -f $name, $count)
    }
}

function Invoke-Run {
    New-Item -ItemType Directory -Force $Output | Out-Null
    $exe = (Resolve-Path $WinMerge).Path
    $configs = [ordered]@{
        line     = @()
        semantic = @('-semantic')
    }
    $cases = Get-ChildItem $Corpus -Directory | ForEach-Object { Get-ChildItem $_.FullName -Directory } |
        Where-Object { $_.FullName -like "*$Filter*" }
    Write-Host ("{0} cases" -f $cases.Count)

    $rows = $cases | ForEach-Object -ThrottleLimit $Parallel -Parallel {
        $case = $_
        $exe = $using:exe; $configs = $using:configs; $timeout = $using:TimeoutSeconds
        $base = Get-ChildItem $case.FullName -Filter 'base.*' | Select-Object -First 1
        $ext = $base.Extension
        $meta = @{}
        Get-Content "$($case.FullName)\case.txt" | ForEach-Object { $k, $v = $_ -split '=', 2; $meta[$k] = $v }
        $row = [ordered]@{ case = "$($case.Parent.Name)\$($case.Name)"; file = $meta.file; git = [int]$meta.gitconflicts }
        foreach ($name in $configs.Keys) {
            $out = "$($case.FullName)\out_$name$ext"
            Remove-Item $out -ErrorAction SilentlyContinue
            # base in the middle, merging into it: the result is the base plus both sides' changes
            $arguments = @('-noprefs', '-noninteractive', '-minimize', '-u', '-am') + $configs[$name] +
                @('-o', "`"$out`"", "`"$($case.FullName)\ours$ext`"", "`"$($base.FullName)`"", "`"$($case.FullName)\theirs$ext`"")
            $process = Start-Process $exe -ArgumentList $arguments -PassThru -WindowStyle Minimized
            if (-not $process.WaitForExit($timeout * 1000)) {
                $process.Kill()
                $row[$name] = 'timeout'
                $row["${name}_conflicts"] = -1
                continue
            }
            if (-not (Test-Path $out)) {
                # nothing could be merged, so nothing was written
                $row[$name] = 'conflict'
                $row["${name}_conflicts"] = [int]$meta.gitconflicts
                continue
            }
            $markers = @(Select-String -Path $out -Pattern '^<<<<<<< ' -CaseSensitive).Count
            $row["${name}_conflicts"] = $markers
            if ($markers -gt 0) { $row[$name] = 'conflict'; continue }
            # the comparison ignores blank lines, line ends and white space
            $normalize = {
                param($path)
                ([IO.File]::ReadAllText($path) -split "`r`n|`n|`r" | ForEach-Object { $_ -replace '[ \t]', '' } | Where-Object { $_ -ne '' }) -join "`n"
            }
            $row[$name] = ((& $normalize $out) -ceq (& $normalize "$($case.FullName)\result$ext")) ? 'match' : 'differs'
        }
        [pscustomobject]$row
    }

    $csv = Join-Path $Output 'results.csv'
    $rows | Sort-Object case | Export-Csv $csv -NoTypeInformation
    Write-Host "`nOutcome per configuration (files):"
    foreach ($name in $configs.Keys) {
        $groups = $rows | Group-Object $name | Sort-Object Name
        $conflicts = ($rows | Where-Object { $_."${name}_conflicts" -gt 0 } | Measure-Object "${name}_conflicts" -Sum).Sum
        Write-Host ("  {0,-9} {1}   conflicts left: {2}" -f $name, (($groups | ForEach-Object { "$($_.Name)=$($_.Count)" }) -join '  '), $conflicts)
    }
    $changed = $rows | Where-Object { $_.line -ne $_.semantic -or $_.line_conflicts -ne $_.semantic_conflicts }
    Write-Host ("`nCases where the semantic configuration changes the outcome: {0}" -f @($changed).Count)
    $changed | Format-Table case, git, line, line_conflicts, semantic, semantic_conflicts -AutoSize | Out-String | Write-Host
    Write-Host "Results: $csv"
}

function Invoke-Tokens {
    # Fast check of the token-level merge alone: every conflict of the corpus
    # goes through the unit test binary, which reports how many it merges and
    # how many of those merges the committed file confirms.
    $languages = @{ '.c' = 'c'; '.h' = 'cpp'; '.cpp' = 'cpp'; '.cs' = 'c_sharp'; '.fs' = 'fsharp'; '.py' = 'python'
        '.java' = 'java'; '.js' = 'javascript'; '.ts' = 'typescript'; '.go' = 'go'; '.rs' = 'rust'; '.rb' = 'ruby'; '.php' = 'php' }
    New-Item -ItemType Directory -Force $Output | Out-Null
    $list = Join-Path (Resolve-Path $Output) 'conflicts.txt'
    $entries = foreach ($case in Get-ChildItem $Corpus -Directory | ForEach-Object { Get-ChildItem $_.FullName -Directory }) {
        $base = Get-ChildItem $case.FullName -Filter 'base.*' | Select-Object -First 1
        $ext = $base.Extension
        $conflicts = "$($case.FullName)\conflicts.diff3"
        if (-not (Test-Path $conflicts)) {
            cmd /c "git merge-file -p --diff3 -L ours -L base -L theirs `"$($case.FullName)\ours$ext`" `"$($base.FullName)`" `"$($case.FullName)\theirs$ext`" > `"$conflicts`" 2>nul"
        }
        "$($languages[$ext])|$conflicts|$($case.FullName)\result$ext"
    }
    $entries | Set-Content $list
    $env:TOKEN_MERGE_CORPUS = $list
    $tests = Join-Path $PSScriptRoot '..\GoogleTest\UnitTests\UnitTests.exe'
    & $tests --gtest_also_run_disabled_tests --gtest_filter=TokenMergeCorpus.* | Tee-Object (Join-Path $Output 'tokens.txt') | Select-Object -Last 3
}

switch ($Command) {
    'collect' { Invoke-Collect }
    'run' { Invoke-Run }
    'tokens' { Invoke-Tokens }
}
