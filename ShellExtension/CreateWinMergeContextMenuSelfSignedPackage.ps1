$vspath = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath
$instid = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property instanceId
Import-Module "$vspath\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
Enter-VsDevShell $instid -DevCmdArguments -arch=x64

cd $(Split-Path $MyInvocation.MyCommand.Path -Parent)

Get-Content WinMergePkg\AppxManifest.xml.in | % { $_ -replace '\${Publisher}','CN=winmerge.org' } | Set-Content WinMergePkg\AppxManifest.xml

$certList = dir Cert:\CurrentUser\My | where { $_.Subject -eq 'CN=winmerge.org' }
if ($certList.Length -eq 0)  {
  $cert = New-SelfSignedCertificate -Type Custom -Subject "CN=winmerge.org" -KeyUsage DigitalSignature -FriendlyName "Your friendly name goes here" -CertStoreLocation "Cert:\CurrentUser\My" -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3", "2.5.29.19={text}")
} else {
  $cert = $certList[0]
}

$CertPassword = ConvertTo-SecureString -String "test" -Force -AsPlainText
Export-PfxCertificate -Cert $cert -FilePath selfsigncert.pfx -Password $CertPassword
Export-Certificate -Type CERT -Cert $cert -FilePath selfsigncert.cer

MakeAppx.exe pack /d WinMergePkg /p WinMergeContextMenuPackage.msix /nv

SignTool.exe sign /fd SHA256 /a /f selfsigncert.pfx /p test /v WinMergeContextMenuPackage.msix
