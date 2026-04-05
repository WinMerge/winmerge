param(
    [Parameter(Mandatory=$true)]
    [string]$Path
)

if (Test-Path $Path) {
    if ((Get-Item $Path) -is [System.IO.FileInfo]) {
        $files = @(Get-Item $Path)
    } else {
        $files = Get-ChildItem -Path $Path -Filter *.po -Recurse
    }

    foreach ($file in $files) {
        $lines = [System.IO.File]::ReadAllLines($file.FullName, [System.Text.Encoding]::UTF8)
        # Remove lines starting with #: (references) and #. (extracted comments)
        $stripped = $lines | Where-Object { (-not $_.StartsWith('#:')) -and (-not $_.StartsWith('#.')) }
        [System.IO.File]::WriteAllLines($file.FullName, $stripped, [System.Text.Encoding]::UTF8)
    }
} else {
    Write-Warning "Path not found: $Path"
}
