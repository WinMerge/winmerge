param(
    [string]$InnoSetupPath = (Join-Path $PSScriptRoot '..\Installer\InnoSetup')
)

$resolvedInnoSetupPath = (Resolve-Path -Path $InnoSetupPath).Path

Get-ChildItem -Path $resolvedInnoSetupPath -Filter *.iss | ForEach-Object {
    $content = [System.IO.File]::ReadAllText($_.FullName, [System.Text.Encoding]::UTF8)
    $newContent = $content -replace '\.\.\\\.\.\\Translations\\WinMerge\\(.+?\.po)', '..\..\BuildTmp\Translations\WinMerge\$1'
    $newContent = $newContent -replace '\.\.\\\.\.\\Translations\\ShellExtension\\(.+?\.po)', '..\..\BuildTmp\Translations\ShellExtension\$1'
    if ($content -cne $newContent) {
        [System.IO.File]::WriteAllText($_.FullName, $newContent, [System.Text.Encoding]::UTF8)
        Write-Host "Updated $($_.Name)"
    }
}
