# Plugin Development Guide

This guide explains how to build a SimpleFences plugin that is aligned with the current host API and easy for other contributors to extend.

## Current phase

The host is still in its plugin-foundation phase. Treat this as a real integration surface, but not yet a fully mature extension SDK.

Reliable surfaces today:

- plugin manifests and lifecycle
- settings page registration and persistence
- content-provider registration metadata

Surfaces still evolving in the host:

- richer widget rendering contracts
- live provider callbacks for non-file-backed fence content
- plugin management UX and failure reporting

Build your plugin so it works cleanly with the current contracts and degrades safely when a future host feature is not present yet.

## Minimum requirements

- Windows 10 or later
- Visual Studio 2022 or Build Tools with Desktop C++
- CMake 3.16+
- Local checkout of [IVOESimpleFences](https://github.com/MrIvoe/IVOESimpleFences)
- C++17

## Start from the template

The fastest path is to copy [plugin-template](plugin-template) into a new folder under `plugins/` and rename the files and identifiers.

That gives you:

- a valid `plugin.json`
- a minimal `IPlugin` implementation
- a settings page example
- naming patterns that match the rest of the hub

## 1. Create `plugin.json`

Every plugin ships a manifest.

```json
{
  "id": "community.my_awesome_plugin",
  "displayName": "My Awesome Plugin",
  "version": "1.0.0",
  "description": "Short one-line description visible in the Settings Hub.",
  "author": "Your Name",
    "minHostVersion": "0.0.012",
    "maxHostVersion": "0.1.999",
    "minHostApiVersion": "0.0.012",
    "maxHostApiVersion": "0.1.999",
    "enabledByDefault": true,
    "supportsSettingsPage": true,
    "supportsMainContentPage": false,
    "supportsHostedSummaryPanel": true,
    "icon": "",
    "updateChannelId": "stable",
    "hostedSummaryPanel": {
        "panelId": "community.my_awesome_plugin.summary",
        "title": "My Awesome Plugin Overview",
        "schemaVersion": "1",
        "layout": "cards",
        "themeTokenNamespace": "win32_theme_system",
        "sections": [
            {
                "id": "status",
                "title": "Status",
                "description": "Operational health and capability summary rendered by the host.",
                "iconToken": "host.icon.status",
                "surfaceToken": "host.surface.card.status"
            }
        ]
    },
  "capabilities": ["settings_pages"],
  "repository": "https://github.com/MrIvoe/Simple-Fences-Plugins"
}
```

### Required keys

| Key | Type | Description |
| --- | --- | --- |
| `id` | string | Unique id. Community plugins should use the `community.` prefix. |
| `displayName` | string | Name shown in plugin listings and settings navigation. |
| `version` | string | Semantic version string. |
| `description` | string | Short summary of the plugin. |
| `author` | string | Plugin author or organization. |
| `minHostVersion` | string | Minimum host version your plugin expects. Keep this aligned with the host branch you tested. |
| `maxHostVersion` | string | Maximum host version the plugin is validated for. |
| `minHostApiVersion` | string | Minimum plugin API contract supported by the plugin build. |
| `maxHostApiVersion` | string | Maximum plugin API contract supported by the plugin build. |
| `enabledByDefault` | bool | Whether the host should enable the plugin on first install. |
| `supportsSettingsPage` | bool | Whether the plugin contributes settings UI pages. |
| `supportsMainContentPage` | bool | Whether the plugin provides a main fence content experience. |
| `supportsHostedSummaryPanel` | bool | Whether the plugin provides host-rendered summary/details panel metadata. |
| `icon` | string | Optional icon path or package-relative icon id. |
| `updateChannelId` | string | Optional release channel id such as `stable` or `preview`. |
| `hostedSummaryPanel` | object | Host-rendered summary/details panel contract mapped to theme tokens. |
| `capabilities` | array | One or more capability strings. |
| `repository` | string | Source repository URL for plugin metadata and docs. |

### Capability choices

Choose only the capabilities you actually implement.

| Capability | Use it when your plugin... |
| --- | --- |
| `settings_pages` | registers UI settings fields |
| `fence_content_provider` | introduces a new fence content type |
| `appearance` | influences fence styling or theme choices |
| `widgets` | adds fence-embedded widget behavior |
| `commands` | exposes command-dispatch actions |
| `tray_contributions` | contributes tray items |
| `desktop_context` | extends desktop-context behavior |

## 2. Implement `IPlugin`

Header:

```cpp
#pragma once

#include "extensions/PluginContracts.h"

class MyPlugin final : public IPlugin
{
public:
    PluginManifest GetManifest() const override;
    bool Initialize(const PluginContext& context) override;
    void Shutdown() override;
};
```

Implementation:

```cpp
#include "MyPlugin.h"

#include "extensions/PluginSettingsRegistry.h"
#include "extensions/SettingsSchema.h"
#include "../../plugins/shared/PluginUiPatterns.h"

PluginManifest MyPlugin::GetManifest() const
{
    PluginManifest m;
    m.id           = L"community.my_awesome_plugin";
    m.displayName  = L"My Awesome Plugin";
    m.version      = L"1.0.0";
    m.description  = L"Short one-line description visible in the Settings Hub.";
    m.capabilities = {L"settings_pages"};
    return m;
}

bool MyPlugin::Initialize(const PluginContext& context)
{
    if (context.settingsRegistry)
    {
        PluginSettingsPage page;
        page.pluginId = L"community.my_awesome_plugin";
        page.pageId   = L"my_plugin.general";
        page.title    = L"General";
        page.order    = 10;

        PluginUiPatterns::AppendBaselineSettingsFields(page.fields, 1, 60, false);

        page.fields.push_back(SettingsFieldDescriptor{
            L"my_plugin.general.enabled",
            L"Enable plugin",
            L"Turns the plugin behavior on or off.",
            SettingsFieldType::Bool, L"true", {}, 10
        });

        page.fields.push_back(SettingsFieldDescriptor{
            L"my_plugin.general.mode",
            L"Mode",
            L"Select the operating mode for the plugin.",
            SettingsFieldType::Enum, L"safe",
            {
                {L"safe",  L"Safe"},
                {L"fast",  L"Fast"},
                {L"debug", L"Debug"},
            },
            20
        });

        context.settingsRegistry->RegisterPage(std::move(page));
    }

    return true;
}

void MyPlugin::Shutdown() {}
```

## 3. Register content providers correctly

If your plugin adds a new fence content type, use the current host descriptor shape:

```cpp
#include "extensions/FenceExtensionRegistry.h"

if (context.fenceExtensionRegistry)
{
    FenceContentProviderDescriptor descriptor;
    descriptor.providerId   = L"community.my_awesome_plugin";
    descriptor.contentType  = L"my_content_type";
    descriptor.displayName  = L"My Content Type";
    descriptor.isCoreDefault = false;

    context.fenceExtensionRegistry->RegisterContentProvider(descriptor);
}
```

Use these fields deliberately:

- `contentType`: the stable saved fence content classification
- `providerId`: the concrete implementation that serves that content type
- `displayName`: what users see in UI
- `isCoreDefault`: only `true` when your provider is the default implementation for that content type

## 4. Register the plugin with the host

In the host repo, add your plugin source files to the build and register the plugin in `BuiltinPlugins.cpp`.

```cpp
#include "plugins/community/my_awesome_plugin/MyPlugin.h"

plugins.push_back(std::make_unique<MyPlugin>());
```

## 5. Supported field types

| `SettingsFieldType` | Control rendered | `defaultValue` format |
| --- | --- | --- |
| `Bool` | Checkbox | `"true"` or `"false"` |
| `Int` | Numeric edit box | Decimal integer string such as `"60"` |
| `String` | Text edit box | Any string |
| `Enum` | Drop-down | One of the declared option values |

Values are persisted automatically to `%LOCALAPPDATA%\SimpleFences\settings.json`.

## 5.1 Shared UI consistency helper

The host renders plugin settings pages with a common Win32 form layout. For consistent UI/UX across plugins, use `plugins/shared/PluginUiPatterns.h` for baseline fields and ordering.

Recommended baseline keys on every plugin settings surface:

- `plugin.show_notifications`
- `plugin.refresh_interval_seconds`

This keeps plugin configuration pages visually and behaviorally aligned with host expectations.

Standard UI guidance:

- no hardcoded colors by default; use host resources first
- standard margins and padding
- standard settings layout
- standard section headers/cards
- host icons/resources first

The shared baseline helper now exposes UI policy fields under `plugin.ui.*` to keep this consistent.

## 6. Naming conventions

| Thing | Convention |
| --- | --- |
| Plugin id | `community.<snake_case>` |
| Field key | `<plugin_short_name>.<page>.<field>` |
| Content type | short, stable noun such as `network_drive` |
| Provider id | usually the manifest id |
| Source files | `PascalCasePlugin.h` and `.cpp` |
| Plugin folder | `plugins/<kebab-case-plugin-name>/` |

## 7. Design for good settings pages

A strong plugin usually has more than one setting that matters in practice. Useful settings tend to fall into a few buckets:

- behavior toggles
- display and visual tuning
- refresh or polling intervals
- default paths or startup targets
- safety controls for anything that launches processes or touches the filesystem

Do not add settings just to look configurable. Every field should map to a clear runtime decision.

## 8. Manual verification checklist

Run these checks against the main host repo after copying in your plugin:

1. Confirm `SimpleFences.exe` is not already running.
2. Build Debug.
3. Launch the app.
4. Open Settings and locate the plugin page.
5. Change multiple fields, including enum and text values.
6. Restart the app.
7. Confirm those values persisted.
8. If the plugin adds a fence content type, verify that the new type is discoverable and saved correctly.

Build commands:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
.\build\bin\Debug\SimpleFences.exe
```

## 9. Current platform limits to keep in mind

At the moment, this hub can define real contracts and realistic samples, but some advanced plugin behaviors still require host-side growth.

Examples:

- a PowerShell fence needs an embeddable terminal surface in the host
- richer widgets need a stronger rendering contract than simple metadata registration
- portal-style providers need non-file content materialization hooks

If your idea depends on one of those gaps, submit the plugin scaffold here and open a matching host issue so the contract work is tracked explicitly.

## 10. Pattern: fence organization plugins

A high-value plugin pattern is organization and cleanup tooling that operates on fence backing folders.

The `fence-organizer` sample demonstrates this architecture:

- add fence-context menu items through `MenuContributionRegistry`
- route each menu action through `CommandDispatcher`
- expose user preferences through `PluginSettingsRegistry`
- query the active fence through `IApplicationCommands` fence metadata methods
- perform file operations in plugin code with safe error handling and diagnostics

Why this pattern works well:

- the host remains lean and generic
- plugin behavior is isolated and replaceable
- future plugins can reuse the same host query contract
- failures are local to the plugin and logged cleanly

If you build a similar plugin, keep file operations defensive:

1. verify active fence metadata exists before operating
2. verify the backing folder exists and is accessible
3. catch filesystem exceptions and log diagnostics
4. avoid destructive operations without confirmation settings

## 11. Recommended plugin update model

Use package distribution and manifest-based updates. Avoid direct consumption of plugin repository folders by the main app.

Recommended lifecycle:

1. Build each plugin into a package artifact.
2. Publish package + version + metadata + compatibility + manifest JSON.
3. Main app reads manifest feed and checks compatibility.
4. Main app downloads and verifies package hash.
5. Main app stages replacement.
6. Main app activates update on next launch or safe reload.

Minimal update entry:

```json
{
    "pluginId": "ExamplePlugin",
    "displayName": "Example Plugin",
    "version": "0.0.014",
    "author": "MrIvoe",
    "description": "Example plugin for IVOESimpleFences",
    "minHostVersion": "0.0.010",
    "maxHostVersion": "0.1.999",
    "packageUrl": "https://example.invalid/ExamplePlugin-0.0.014.sfplugin",
    "sha256": "..."
}
```

Host-enforced gates (implemented in IVOESimpleFences):

1. Install gate:
    - validate plugin manifest against `plugin-manifest.schema.json`
    - require `updateChannelId` to be `stable` or `preview`
2. Stage gate:
    - validate feed payload against `plugin-update-feed.schema.json`
    - verify package `sha256`
    - verify signature metadata and certificate chain trust
3. Activate gate:
    - enforce host/app and plugin API compatibility bounds
    - reject activation on validation failure and keep prior plugin version

Signing policy:

- require detached signature metadata for every package update
- require signing certificate chain to validate to host-trusted root(s)
- require leaf certificate thumbprint match against update feed metadata
