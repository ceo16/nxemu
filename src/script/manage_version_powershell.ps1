param(
    [Parameter(Mandatory=$true)]
    [ValidateSet("dev", "release")]
    [string]$Mode,
    
    [Parameter(Mandatory=$true)]
    [string]$VersionFile
)

# Verify file exists
if (-not (Test-Path $VersionFile)) {
    Write-Error "Version file not found: $VersionFile"
    exit 1
}

Write-Host "Processing file: $VersionFile"

# Read the file content line by line to preserve exact format
$lines = Get-Content -Path $VersionFile -Encoding UTF8

$minorValue = $null
$prefixLine = $null
$minorLine = $null

# Find version values and line numbers
for ($i = 0; $i -lt $lines.Count; $i++) {
    if ($lines[$i] -match '^#define\s+VERSION_MINOR\s+(\d+)') {
        $minorValue = [int]$matches[1]
        $minorLine = $i
        Write-Host "Found VERSION_MINOR: $minorValue at line $($i+1)"
    }
    elseif ($lines[$i] -match '^#define\s+VERSION_PREFIX\s+"(.*)"') {
        $prefixLine = $i
        Write-Host "Found VERSION_PREFIX: '$($matches[1])' at line $($i+1)"
    }
}

if ($null -eq $minorLine) {
    Write-Error "Could not find VERSION_MINOR in the file"
    exit 1
}

if ($null -eq $prefixLine) {
    Write-Error "Could not find VERSION_PREFIX in the file"
    exit 1
}

# Make the appropriate changes based on mode
if ($Mode -eq "dev") {
    Write-Host "Setting version prefix to '' and incrementing VERSION_MINOR..."
    $newMinor = $minorValue + 1
    Write-Host "New VERSION_MINOR: $newMinor"

    Write-Host "Setting version prefix to 'dev'..."
    # Update the VERSION_PREFIX line
    $lines[$prefixLine] = $lines[$prefixLine] -replace '".*"', '"dev"'
    
    # Update the VERSION_MINOR line
    $lines[$minorLine] = $lines[$minorLine] -replace '\d+(?=\s*$)', $newMinor
}
elseif ($Mode -eq "release") {   
    # Update the VERSION_PREFIX line
    Write-Host "Setting version prefix to ''..."
    $lines[$prefixLine] = $lines[$prefixLine] -replace '".*"', '""'
}

# Write the updated content back to the file
$lines | Set-Content -Path $VersionFile -Encoding UTF8

Write-Host "Version update complete."