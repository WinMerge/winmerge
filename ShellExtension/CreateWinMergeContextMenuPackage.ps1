$vspath = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath
$instid = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property instanceId
Import-Module "$vspath\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
Enter-VsDevShell $instid -DevCmdArguments -arch=x64

cd $(Split-Path $MyInvocation.MyCommand.Path -Parent)

if (($env:SIGNBAT_PATH -eq "") -or !(Test-Path publisher.txt) -or (($args[0] -ne $null) -and ($args[0].ToLower() -eq "selfsignedpackage"))) {
  $publisher = "CN=winmergedev"
} else {
  $publisher = Get-Content publisher.txt
  $certList = dir Cert:\CurrentUser\My | where { $_.Subject -eq $publisher }
  if ($certList.Length -eq 0) {
    $publisher = "CN=winmergedev"
  }
}

Get-Content WinMergePkg\AppxManifest.xml.in | % { $_ -replace '\${Publisher}',$publisher } | Set-Content WinMergePkg\AppxManifest.xml
if (Test-Path WinMergeContextMenuPackage.msix) { del WinMergeContextMenuPackage.msix }
MakeAppx.exe pack /d WinMergePkg /p WinMergeContextMenuPackage.msix /nv

if ($publisher -eq "CN=winmergedev") {
  $certList = dir Cert:\CurrentUser\My | where { $_.Subject -eq $publisher  }
  if ($certList.Length -eq 0)  {
    $cert = New-SelfSignedCertificate -Type Custom -Subject $publisher -KeyUsage DigitalSignature -FriendlyName "Your friendly name goes here" -CertStoreLocation "Cert:\CurrentUser\My" -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3", "2.5.29.19={text}")
  } else {
    $cert = $certList[0]
  }

  $certPassword = ConvertTo-SecureString -String "test" -Force -AsPlainText
  Export-PfxCertificate -Cert $cert -FilePath selfsigncert.pfx -Password $certPassword
  Export-Certificate -Type CERT -Cert $cert -FilePath selfsigncert.cer

  SignTool.exe sign /fd SHA256 /a /f selfsigncert.pfx /p test /v WinMergeContextMenuPackage.msix
} else {
  & $env:SIGNBAT_PATH WinMergeContextMenuPackage.msix
}

foreach($i in @("", "X64\", "ARM64\")) {
  foreach($j in @("Debug", "Release")) {
    if (!(Test-Path ..\Build\$i$j)) { mkdir ..\Build\$i$j }
    copy WinMergeContextMenuPackage.msix ..\Build\$i$j
  }
}
