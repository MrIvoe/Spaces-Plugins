param(
    [string]$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path,
    [string]$OutputRoot = "",
    [string]$CatalogFileName = "catalog.json",
    [string]$CatalogVersion = "1.0.0",
    [string]$BaseDownloadUrl = "",
    [switch]$IncludePreview,
    [switch]$FailOnWarnings
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($OutputRoot)) {
    $OutputRoot = Join-Path $RepoRoot "dist\marketplace"
}

$pluginsRoot = Join-Path $RepoRoot "plugins"
$packagesRoot = Join-Path $OutputRoot "packages"
$catalogRoot = Join-Path $OutputRoot "catalog"
$catalogPath = Join-Path $catalogRoot $CatalogFileName

New-Item -ItemType Directory -Force -Path $packagesRoot | Out-Null
New-Item -ItemType Directory -Force -Path $catalogRoot | Out-Null

function Normalize-Category {
    param([string]$PluginId, [object]$Manifest)

    if ($Manifest.PSObject.Properties.Name -contains "category" -and -not [string]::IsNullOrWhiteSpace($Manifest.category)) {
        return [string]$Manifest.category
    }

    $parts = $PluginId.Split('.')
    if ($parts.Length -ge 2) {
        return $parts[1].Replace('_', '-')
    }

    return "general"
}

function Get-Compatibility {
    param([object]$Manifest)

    if ($Manifest.PSObject.Properties.Name -contains "compatibility" -and $null -ne $Manifest.compatibility) {
        return @{ 
            hostVersion = @{ 
                min = [string]$Manifest.compatibility.hostVersion.min
                max = [string]$Manifest.compatibility.hostVersion.max
            }
            hostApiVersion = @{
                min = [string]$Manifest.compatibility.hostApiVersion.min
                max = [string]$Manifest.compatibility.hostApiVersion.max
            }
        }
    }

    return @{
        hostVersion = @{
            min = [string]$Manifest.minHostVersion
            max = [string]$Manifest.maxHostVersion
        }
        hostApiVersion = @{
            min = [string]$Manifest.minHostApiVersion
            max = [string]$Manifest.maxHostApiVersion
        }
    }
}

function Build-DownloadUrl {
    param(
        [string]$Base,
        [string]$PackageName
    )

    if ([string]::IsNullOrWhiteSpace($Base)) {
        return ""
    }

    if ($Base.EndsWith('/')) {
        return "$Base$PackageName"
    }

    return "$Base/$PackageName"
}

$pluginDirs = Get-ChildItem -Path $pluginsRoot -Directory | Sort-Object Name
$entries = @()
$warnings = @()

foreach ($dir in $pluginDirs) {
    $manifestPath = Join-Path $dir.FullName "plugin.json"
    if (-not (Test-Path $manifestPath)) {
        $warnings += "Skipping '$($dir.Name)': missing plugin.json"
        continue
    }

    $manifest = Get-Content -Raw -Path $manifestPath | ConvertFrom-Json

    $channel = [string]$manifest.updateChannelId
    if ([string]::IsNullOrWhiteSpace($channel)) {
        $channel = "stable"
    }

    if ($channel -eq "preview" -and -not $IncludePreview.IsPresent) {
        continue
    }

    $pluginId = [string]$manifest.id
    $version = [string]$manifest.version
    $safePackageName = ($pluginId -replace '[^a-zA-Z0-9._-]', '_') + "-$version.zip"
    $zipPath = Join-Path $packagesRoot $safePackageName

    if (Test-Path $zipPath) {
        Remove-Item -Force $zipPath
    }

    Compress-Archive -Path (Join-Path $dir.FullName '*') -DestinationPath $zipPath -CompressionLevel Optimal
    $hash = (Get-FileHash -Path $zipPath -Algorithm SHA256).Hash.ToLowerInvariant()

    $compat = Get-Compatibility -Manifest $manifest

    $entry = [ordered]@{
        id = $pluginId
        displayName = [string]$manifest.displayName
        author = [string]$manifest.author
        description = [string]$manifest.description
        category = Normalize-Category -PluginId $pluginId -Manifest $manifest
        version = $version
        channel = $channel
        downloadUrl = Build-DownloadUrl -Base $BaseDownloadUrl -PackageName $safePackageName
        hash = $hash
        hashAlgorithm = "sha256"
        compatibility = $compat
        capabilities = @($manifest.capabilities)
        supportsSettingsPage = [bool]$manifest.supportsSettingsPage
        restartRequired = if ($manifest.PSObject.Properties.Name -contains "restartRequired") { [bool]$manifest.restartRequired } else { $true }
        settingsPageSupport = [ordered]@{
            supportsSettingsPage = [bool]$manifest.supportsSettingsPage
            supportsMainContentPage = [bool]$manifest.supportsMainContentPage
            supportsHostedSummaryPanel = [bool]$manifest.supportsHostedSummaryPanel
        }
    }

    $entries += $entry
}

$catalog = [ordered]@{
    version = 1
    catalogVersion = $CatalogVersion
    generatedAtUtc = (Get-Date).ToUniversalTime().ToString("o")
    sourceRepository = "https://github.com/MrIvoe/Spaces-Plugins"
    plugins = $entries
}

$catalog | ConvertTo-Json -Depth 12 | Set-Content -Path $catalogPath -Encoding UTF8

if ($warnings.Count -gt 0) {
    $warnings | ForEach-Object { Write-Warning $_ }
    if ($FailOnWarnings.IsPresent) {
        throw "Warnings encountered during marketplace artifact generation."
    }
}

Write-Host "Generated marketplace artifacts"
Write-Host "  Packages: $($entries.Count)"
Write-Host "  Catalog : $catalogPath"
