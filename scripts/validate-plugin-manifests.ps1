$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$pluginFiles = Get-ChildItem (Join-Path $root 'plugins') -Recurse -Filter 'plugin.json'

$requiredKeys = @(
    'id',
    'displayName',
    'version',
    'description',
    'author',
    'minHostVersion',
    'maxHostVersion',
    'minHostApiVersion',
    'maxHostApiVersion',
    'enabledByDefault',
    'supportsSettingsPage',
    'supportsMainContentPage',
    'icon',
    'updateChannelId',
    'capabilities',
    'repository'
)

$failures = @()

foreach ($file in $pluginFiles)
{
    try
    {
        $json = Get-Content $file.FullName -Raw | ConvertFrom-Json
    }
    catch
    {
        $failures += "[$($file.FullName)] invalid JSON: $($_.Exception.Message)"
        continue
    }

    foreach ($key in $requiredKeys)
    {
        if (-not ($json.PSObject.Properties.Name -contains $key))
        {
            $failures += "[$($file.FullName)] missing required key: $key"
        }
    }

    if ($json.PSObject.Properties.Name -contains 'id' -and ($json.id -notmatch '^community\.[a-z0-9_]+$'))
    {
        $failures += "[$($file.FullName)] id must match community.<snake_case>"
    }

    if ($json.PSObject.Properties.Name -contains 'version' -and ($json.version -notmatch '^\d+\.\d+\.\d+$'))
    {
        $failures += "[$($file.FullName)] version must be semantic (x.y.z)"
    }

    if ($json.PSObject.Properties.Name -contains 'capabilities')
    {
        $caps = @($json.capabilities)
        $hasSettings = $caps -contains 'settings_pages'
        $hasHostedContent = ($caps -contains 'fence_content_provider') -or ($caps -contains 'widgets')

        if ($json.PSObject.Properties.Name -contains 'supportsSettingsPage' -and [bool]$json.supportsSettingsPage -ne [bool]$hasSettings)
        {
            $failures += "[$($file.FullName)] supportsSettingsPage does not match capabilities"
        }

        if ($json.PSObject.Properties.Name -contains 'supportsMainContentPage' -and [bool]$json.supportsMainContentPage -ne [bool]$hasHostedContent)
        {
            $failures += "[$($file.FullName)] supportsMainContentPage does not match capabilities"
        }
    }
}

if ($failures.Count -gt 0)
{
    Write-Host 'Manifest validation failed:' -ForegroundColor Red
    $failures | ForEach-Object { Write-Host " - $_" -ForegroundColor Red }
    exit 1
}

Write-Host "Validated $($pluginFiles.Count) plugin manifest files successfully." -ForegroundColor Green
