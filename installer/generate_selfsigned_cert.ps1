# â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
#  generate_selfsigned_cert.ps1
#  Generates a free self-signed code signing certificate and signs:
#    - The IzoClone.vst3 DLL
#    - The NSIS installer .exe
#
#  Run from PowerShell as Administrator:
#    Set-ExecutionPolicy -Scope Process Bypass
#    .\generate_selfsigned_cert.ps1
# â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

param(
    [string]`$PublisherName  = "YourStudio",
    [string]`$CertPassword   = "IzoClone2025!",   # Change this
    [string]`$CertOutputPath = ".\installer\IzoClone_CodeSign.pfx",
    [string]`$VST3Path       = ".\build\IzoClone_artefacts\Release\VST3\IzoClone.vst3\Contents\x86_64-win\IzoClone.vst3",
    [string]`$InstallerPath  = ".\IzoClone_Setup_v1.0.0.exe"
)

Write-Host "â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•" -ForegroundColor Cyan
Write-Host "  IzoClone Self-Signed Code Signing Tool" -ForegroundColor Cyan
Write-Host "â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•" -ForegroundColor Cyan
Write-Host ""

# â”€â”€ Step 1: Generate certificate â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
Write-Host "â–º Generating self-signed code signing certificate..." -ForegroundColor Yellow

`$cert = New-SelfSignedCertificate ``
    -Type CodeSigningCert ``
    -Subject "CN=`$PublisherName, O=`$PublisherName, C=US" ``
    -KeyAlgorithm RSA ``
    -KeyLength 4096 ``
    -HashAlgorithm SHA256 ``
    -CertStoreLocation "Cert:\CurrentUser\My" ``
    -NotAfter (Get-Date).AddYears(5) ``
    -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3")

Write-Host "  Certificate created: `$(`$cert.Thumbprint)" -ForegroundColor Green

# â”€â”€ Step 2: Export to PFX â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
Write-Host "â–º Exporting certificate to PFX..." -ForegroundColor Yellow

`$securePassword = ConvertTo-SecureString -String `$CertPassword -Force -AsPlainText
Export-PfxCertificate ``
    -Cert "Cert:\CurrentUser\My\`$(`$cert.Thumbprint)" ``
    -FilePath `$CertOutputPath ``
    -Password `$securePassword | Out-Null

Write-Host "  Exported to: `$CertOutputPath" -ForegroundColor Green

# â”€â”€ Step 3: Trust the certificate locally â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
Write-Host "â–º Adding certificate to Trusted Root store (local machine)..." -ForegroundColor Yellow

`$store = New-Object System.Security.Cryptography.X509Certificates.X509Store(
    "Root", "LocalMachine")
`$store.Open("ReadWrite")
`$store.Add(`$cert)
`$store.Close()

Write-Host "  Certificate trusted locally." -ForegroundColor Green

# â”€â”€ Step 4: Sign the VST3 â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
if (Test-Path `$VST3Path) {
    Write-Host "â–º Signing VST3 plugin DLL..." -ForegroundColor Yellow
    
    `$signResult = Set-AuthenticodeSignature ``
        -FilePath `$VST3Path ``
        -Certificate `$cert ``
        -HashAlgorithm SHA256 ``
        -TimestampServer "http://timestamp.digicert.com"

    if (`$signResult.Status -eq "Valid") {
        Write-Host "  VST3 signed successfully." -ForegroundColor Green
    } else {
        Write-Host "  Warning: `$(`$signResult.StatusMessage)" -ForegroundColor Yellow
    }
} else {
    Write-Host "  VST3 not found at `$VST3Path â€” skipping. Sign manually after build." -ForegroundColor DarkYellow
}

# â”€â”€ Step 5: Sign the installer â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
if (Test-Path `$InstallerPath) {
    Write-Host "â–º Signing installer EXE..." -ForegroundColor Yellow

    `$signResult = Set-AuthenticodeSignature ``
        -FilePath `$InstallerPath ``
        -Certificate `$cert ``
        -HashAlgorithm SHA256 ``
        -TimestampServer "http://timestamp.digicert.com"

    if (`$signResult.Status -eq "Valid") {
        Write-Host "  Installer signed successfully." -ForegroundColor Green
    } else {
        Write-Host "  Warning: `$(`$signResult.StatusMessage)" -ForegroundColor Yellow
    }
} else {
    Write-Host "  Installer not found at `$InstallerPath â€” build it first with NSIS." -ForegroundColor DarkYellow
}

# â”€â”€ Done â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
Write-Host ""
Write-Host "â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•" -ForegroundColor Cyan
Write-Host "  Done! Summary:" -ForegroundColor Cyan
Write-Host "  â€¢ Certificate:  `$CertOutputPath" -ForegroundColor White
Write-Host "  â€¢ Thumbprint:   `$(`$cert.Thumbprint)" -ForegroundColor White
Write-Host ""
Write-Host "  IMPORTANT NOTES:" -ForegroundColor Yellow
Write-Host "  â€¢ Self-signed certs work locally and for people you trust" -ForegroundColor White
Write-Host "  â€¢ Windows SmartScreen will still warn on first run for others" -ForegroundColor White
Write-Host "  â€¢ To remove the warning permanently, get a cert from DigiCert" -ForegroundColor White
Write-Host "    or use SignPath.io (free for open source projects)" -ForegroundColor White
Write-Host "â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•" -ForegroundColor Cyan

