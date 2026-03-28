<#
.SYNOPSIS
    Downloads and compiles tree-sitter grammar DLLs for WinMerge.
.DESCRIPTION
    Reads grammars.json, downloads release tarballs from GitHub, reads each
    repo's tree-sitter.json for source layout, compiles to DLL via MSVC cl.exe.
.PARAMETER OutDir
    Output directory for DLLs and .scm files.
.PARAMETER Platform
    Target platform: x64, x86, or ARM64. Default: x64.
.PARAMETER Configuration
    Build configuration: Release or Debug. Default: Release.
.PARAMETER GrammarFilter
    Optional regex to build only matching grammar names.
.EXAMPLE
    .\build-grammars.ps1
    .\build-grammars.ps1 -GrammarFilter "fsharp"
#>
param(
    [string]$OutDir,
    [ValidateSet("x64","x86","ARM64")]
    [string]$Platform = "x64",
    [ValidateSet("Release","Debug")]
    [string]$Configuration = "Release",
    [string]$GrammarFilter
)

$ErrorActionPreference = "Continue"

$ScriptDir  = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot   = (Resolve-Path (Join-Path $ScriptDir "..\..")).Path
if (-not $OutDir) {
    $OutDir = Join-Path $RepoRoot "Build\$Platform\$Configuration\TreeSitterGrammars"
}
$TempBase   = Join-Path $RepoRoot "BuildTmp\grammar-sources"
$ConfigFile = Join-Path $ScriptDir "grammars.json"

# ---- Locate and import MSVC environment ----

function Find-VcVarsAll {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) {
        $vswhere = Join-Path $env:ProgramFiles "Microsoft Visual Studio\Installer\vswhere.exe"
    }
    if (-not (Test-Path $vswhere)) { throw "vswhere.exe not found" }
    $ip = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2>$null
    if (-not $ip) { throw "No VS with C++ tools found" }
    $vcvars = Join-Path $ip "VC\Auxiliary\Build\vcvarsall.bat"
    if (-not (Test-Path $vcvars)) { throw "vcvarsall.bat not found at $vcvars" }
    return $vcvars
}

function Import-VcEnvironment {
    param([string]$Arch)
    $vcvars = Find-VcVarsAll
    Write-Host "Importing MSVC environment ($Arch) ..."
    $batContent = "@call `"$vcvars`" $Arch >nul 2>&1`r`n@set > `"$env:TEMP\vcvars_env.txt`"`r`n"
    [System.IO.File]::WriteAllText("$env:TEMP\vcvars_capture.bat", $batContent)
    cmd.exe /c "$env:TEMP\vcvars_capture.bat"
    if (Test-Path "$env:TEMP\vcvars_env.txt") {
        foreach ($line in (Get-Content "$env:TEMP\vcvars_env.txt")) {
            if ($line -match '^([^=]+)=(.*)$') {
                [Environment]::SetEnvironmentVariable($matches[1], $matches[2], "Process")
            }
        }
        Remove-Item "$env:TEMP\vcvars_capture.bat","$env:TEMP\vcvars_env.txt" -Force -EA SilentlyContinue
    }
    $cl = Get-Command cl.exe -EA SilentlyContinue
    if (-not $cl) { throw "cl.exe not found after importing vcvars" }
    Write-Host "  cl.exe: $($cl.Source)"
}

function Get-VcArch {
    switch ($Platform) {
        "x64"   { return "amd64" }
        "x86"   { return "x86" }
        "ARM64" { return "amd64_arm64" }
        default { return "amd64" }
    }
}

# ---- Download grammar source ----

function Get-GrammarSource {
    param([string]$Repo, [string]$Tag)
    $name = ($Repo -split '/')[-1]
    $extractDir = Join-Path $TempBase $name
    if (Test-Path (Join-Path $extractDir "tree-sitter.json")) {
        Write-Host "  Cached: $extractDir"
        return $extractDir
    }
    New-Item -ItemType Directory -Path $TempBase -Force | Out-Null
    New-Item -ItemType Directory -Path $extractDir -Force | Out-Null

    # Try .tar.gz first, then .tar.xz (older releases use xz)
    $downloaded = $false
    foreach ($ext in @("tar.gz", "tar.xz")) {
        $tarUrl  = "https://github.com/$Repo/releases/download/$Tag/$name.$ext"
        $tarFile = Join-Path $TempBase "$name.$ext"
        try {
            Write-Host "  Downloading $name.$ext ..."
            Invoke-WebRequest -Uri $tarUrl -OutFile $tarFile -UseBasicParsing -ErrorAction Stop
            $downloaded = $true
            break
        } catch {
            # Try next format
        }
    }
    if (-not $downloaded) {
        throw "No release tarball found for $Repo $Tag"
    }

    Write-Host "  Extracting ..."
    # Use git-bash tar with Unix-style paths (Windows tar.exe fails on ./prefixed entries)
    $unixTarFile = $tarFile -replace '\\','/' -replace '^([A-Za-z]):','/$1'
    $unixExtractDir = $extractDir -replace '\\','/' -replace '^([A-Za-z]):','/$1'
    bash -c "tar -xf '$unixTarFile' -C '$unixExtractDir'"
    Remove-Item $tarFile -Force
    return $extractDir
}

function Resolve-NodeModulesPath {
    param([string]$Candidate)

    $normalized = $Candidate -replace '/','\'
    if (-not $normalized.StartsWith("node_modules\")) {
        return $null
    }

    $parts = $normalized -split '\\'
    if ($parts.Count -lt 3) {
        return $null
    }

    $packageName = $parts[1]
    $restIndex = 2
    $cacheDirs = New-Object System.Collections.Generic.List[string]
    if ($packageName.StartsWith('@')) {
        if ($parts.Count -lt 4) {
            return $null
        }
        $packageName = "$packageName\$($parts[2])"
        $restIndex = 3
        $cacheDirs.Add((Join-Path $TempBase $packageName))
        $packageName = $parts[2]
    }
    $cacheDirs.Add((Join-Path $TempBase $packageName))

    $relativePath = $parts[$restIndex..($parts.Count - 1)] -join '\'
    foreach ($cachedSourceDir in $cacheDirs) {
        if (-not (Test-Path $cachedSourceDir)) {
            continue
        }
        $resolvedPath = Join-Path $cachedSourceDir $relativePath
        if (Test-Path $resolvedPath) {
            return $resolvedPath
        }
    }

    return $null
}

function Resolve-QueryFiles {
    param(
        [object]$QuerySpec,
        [string]$SourceDir,
        [string]$RepoDir,
        [string]$FallbackRelativePath
    )

    $resolved = New-Object System.Collections.Generic.List[string]

    if ($QuerySpec) {
        $queryPaths = if ($QuerySpec -is [array]) { $QuerySpec } else { @($QuerySpec) }
        foreach ($candidate in $queryPaths) {
            $tryPaths = @(
                (Join-Path $RepoDir $candidate),
                (Join-Path $SourceDir $candidate),
                (Resolve-NodeModulesPath -Candidate $candidate)
            ) | Where-Object { $_ }

            foreach ($tryPath in $tryPaths) {
                if ((Test-Path $tryPath) -and (-not $resolved.Contains($tryPath))) {
                    $resolved.Add($tryPath)
                    break
                }
            }
        }
    }

    if ($resolved.Count -eq 0 -and $FallbackRelativePath) {
        $fallbacks = @(
            (Join-Path $SourceDir $FallbackRelativePath),
            (Join-Path $RepoDir $FallbackRelativePath)
        )
        foreach ($fallback in $fallbacks) {
            if ((Test-Path $fallback) -and (-not $resolved.Contains($fallback))) {
                $resolved.Add($fallback)
                break
            }
        }
    }

    return $resolved.ToArray()
}

function Write-QueryBundle {
    param(
        [string]$DestinationPath,
        [string[]]$SourcePaths
    )

    if (-not $SourcePaths -or $SourcePaths.Count -eq 0) {
        return $false
    }

    $contents = foreach ($sourcePath in $SourcePaths) {
        [System.IO.File]::ReadAllText($sourcePath)
    }
    $bundle = ($contents -join "`r`n`r`n").TrimEnd()
    [System.IO.File]::WriteAllText($DestinationPath, $bundle + "`r`n", [System.Text.UTF8Encoding]::new($false))
    return $true
}

# ---- Compile a grammar to DLL ----

function Build-GrammarDll {
    param(
        [string]$GrammarName,
        [string]$SourceDir,
        [string]$RepoDir,
        [string[]]$HighlightsScm,
        [string[]]$LocalsScm,
        [string[]]$InjectionsScm,
        [string]$DllName
    )
    $srcDir   = Join-Path $SourceDir "src"
    $parserC  = Join-Path $srcDir "parser.c"
    $scannerC = Join-Path $srcDir "scanner.c"
    if (-not (Test-Path $parserC)) {
        Write-Warning "  parser.c not found at $parserC - skipping $GrammarName"
        return $false
    }
    $sources = @($parserC)
    if (Test-Path $scannerC) { $sources += $scannerC }

    $buildDir = Join-Path $RepoRoot "BuildTmp\grammar-build\$DllName\$Platform\$Configuration"
    New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
    New-Item -ItemType Directory -Path $OutDir   -Force | Out-Null
    $dllPath = Join-Path $OutDir "$DllName.dll"

    $cflags = @("/nologo","/c","/TC","/W3","/D_USRDLL","/D_WINDOWS","/wd4996","/wd4267","/wd4244","/wd4101")
    if ($Configuration -eq "Release") {
        $cflags += @("/O2","/MD","/DNDEBUG","/GL")
    } else {
        $cflags += @("/Od","/MDd","/D_DEBUG","/Zi")
    }

    $objFiles = @()
    foreach ($src in $sources) {
        $objName = [IO.Path]::GetFileNameWithoutExtension($src) + ".obj"
        $objPath = Join-Path $buildDir $objName
        $objFiles += $objPath
        $fileName = [IO.Path]::GetFileName($src)
        Write-Host "  Compiling $fileName ..."
        $allArgs = $cflags + @("/I`"$srcDir`"", "/Fo`"$objPath`"", "`"$src`"")
        $p = Start-Process cl.exe -ArgumentList $allArgs -NoNewWindow -Wait -PassThru
        if ($p.ExitCode -ne 0) {
            Write-Error "  cl.exe failed for $fileName (exit $($p.ExitCode))"
            return $false
        }
    }

    Write-Host "  Linking $DllName.dll ..."
    $linkArgs = @("/nologo","/DLL","/OUT:`"$dllPath`"")
    if ($Configuration -eq "Release") {
        $linkArgs += @("/LTCG","/OPT:REF","/OPT:ICF")
    } else {
        $linkArgs += @("/DEBUG")
    }
    $linkArgs += $objFiles
    $p = Start-Process link.exe -ArgumentList $linkArgs -NoNewWindow -Wait -PassThru
    if ($p.ExitCode -ne 0) {
        Write-Error "  link.exe failed for $DllName (exit $($p.ExitCode))"
        return $false
    }

    if (Write-QueryBundle -DestinationPath (Join-Path $OutDir "$GrammarName-highlights.scm") -SourcePaths $HighlightsScm) {
        Write-Host "  Copied highlights -> $GrammarName-highlights.scm"
    } else {
        Write-Warning "  No highlights.scm for $GrammarName"
    }
    if (Write-QueryBundle -DestinationPath (Join-Path $OutDir "$GrammarName-locals.scm") -SourcePaths $LocalsScm) {
        Write-Host "  Copied locals -> $GrammarName-locals.scm"
    }
    if (Write-QueryBundle -DestinationPath (Join-Path $OutDir "$GrammarName-injections.scm") -SourcePaths $InjectionsScm) {
        Write-Host "  Copied injections -> $GrammarName-injections.scm"
    }
    Write-Host "  OK: $dllPath"
    return $true
}

# ---- Main ----

$succeeded = [int]0
$failed    = [int]0
$skipped   = [int]0

Write-Host "=== WinMerge Tree-Sitter Grammar Builder ==="
Write-Host "Platform:      $Platform"
Write-Host "Configuration: $Configuration"
Write-Host "Output:        $OutDir"
Write-Host ""

Import-VcEnvironment -Arch (Get-VcArch)
Write-Host ""

$config = Get-Content $ConfigFile -Raw | ConvertFrom-Json

foreach ($entry in $config.grammars) {
    $repo     = $entry.repo
    $tag      = $entry.tag
    $repoName = ($repo -split '/')[-1]
    Write-Host "--- $repoName ($tag) ---"

    try {
        $repoDir = Get-GrammarSource -Repo $repo -Tag $tag
    } catch {
        Write-Warning "  Download failed: $_"
        $failed++
        continue
    }

    $tsJsonPath = Join-Path $repoDir "tree-sitter.json"
    if (-not (Test-Path $tsJsonPath)) {
        Write-Warning "  No tree-sitter.json - skipping"
        $skipped++
        continue
    }
    $tsJson = Get-Content $tsJsonPath -Raw | ConvertFrom-Json

    foreach ($g in $tsJson.grammars) {
        $gName = $g.name
        $gPath = $g.path
        if (-not $gPath) { $gPath = "." }

        if ($GrammarFilter -and ($gName -notmatch $GrammarFilter)) {
            Write-Host "  Skipping $gName (filtered)"
            $skipped++
            continue
        }

        $sourceDir = if ($gPath -eq ".") { $repoDir } else { Join-Path $repoDir $gPath }
        $dllName   = "tree-sitter-$gName"

        # Resolve .scm query files. When tree-sitter.json uses an array, all
        # matching files are bundled in order so inherited queries are kept.
        $hlScm = Resolve-QueryFiles -QuerySpec $g.highlights -SourceDir $sourceDir -RepoDir $repoDir -FallbackRelativePath "queries\highlights.scm"
        $lcScm = Resolve-QueryFiles -QuerySpec $g.locals -SourceDir $sourceDir -RepoDir $repoDir -FallbackRelativePath "queries\locals.scm"
        $ijScm = Resolve-QueryFiles -QuerySpec $g.injections -SourceDir $sourceDir -RepoDir $repoDir -FallbackRelativePath "queries\injections.scm"

        Write-Host "  Grammar: $gName (path: $gPath)"
        $ok = Build-GrammarDll -GrammarName $gName -SourceDir $sourceDir -RepoDir $repoDir -HighlightsScm $hlScm -LocalsScm $lcScm -InjectionsScm $ijScm -DllName $dllName
        if ($ok) { $succeeded++ } else { $failed++ }
    }
    Write-Host ""
}

Write-Host "=== Done: $succeeded succeeded, $failed failed, $skipped skipped ==="
if ($failed -gt 0) { exit 1 }
