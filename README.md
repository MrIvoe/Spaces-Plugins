# Simple Fences — Plugin Hub

A community repository for [SimpleFences](https://github.com/MrIvoe/IVOESimpleFences) plugins, inspired by the [RuneLite plugin hub](https://github.com/runelite/plugin-hub).

## What is SimpleFences?

SimpleFences is a Win32 C++ desktop organiser that lets you group files into named "fences" on your desktop, similar to Stardock Fences. The plugin system allows you to extend every aspect of the application — new content providers, tray actions, widgets, themes, and more.

## Plugin capabilities

| Capability | What it unlocks |
|---|---|
| `commands` | Register keyboard / command-bar actions |
| `tray_contributions` | Add entries to the system tray menu |
| `fence_content_provider` | Supply a new fence content type (e.g. network drive, RSS feed) |
| `appearance` | Override colours, fonts, or visual style of fence windows |
| `widgets` | Add embeddable widget panels inside a fence |
| `desktop_context` | Add entries to the desktop right-click context menu |
| `settings_pages` | Register a settings page with interactive controls (checkboxes, dropdowns, text fields) |

All field values declared in `settings_pages` are persisted automatically to  
`%LOCALAPPDATA%\SimpleFences\settings.json`. No extra persistence code needed.

---

## Included example plugins

| Plugin | Capability | Description |
|---|---|---|
| [`dark-glass-theme`](plugins/dark-glass-theme/) | `appearance` | Translucent dark theme with blur, opacity, and corner radius controls |
| [`network-drive-fence`](plugins/network-drive-fence/) | `fence_content_provider` | Fence that maps a UNC / mapped network drive path |
| [`clock-widget`](plugins/clock-widget/) | `widgets` | Live digital or analogue clock inside a fence |

---

## Installing a plugin

1. Clone or download this repository.
2. Copy the plugin's `src/` files into `<SimpleFences>/src/plugins/community/<plugin-id>/`.
3. Add those `.h`/`.cpp` files to the `SimpleFences` target in `CMakeLists.txt`.
4. In `BuiltinPlugins.cpp`, include the plugin header and add it to `CreateBuiltinPlugins()`:

```cpp
#include "plugins/community/clock_widget/ClockWidgetPlugin.h"

// Inside CreateBuiltinPlugins():
plugins.push_back(std::make_unique<ClockWidgetPlugin>());
```

5. Rebuild and launch. Your plugin appears in **Settings → left rail** automatically.

> **Roadmap:** A DLL-based loader is planned so plugins can be distributed pre-built without recompiling the host.

---

## Submitting a plugin

1. Fork this repository.
2. Create `plugins/<your-plugin-id>/` containing `plugin.json` + `src/`.
3. Run through the [Plugin Development Guide](PLUGIN_GUIDE.md) checklist.
4. Open a pull request.

### Naming conventions

| Thing | Convention |
|---|---|
| Plugin id | `community.<snake_case>` |
| Field storage key | `<plugin_short_name>.<page>.<field>` |
| Source files | `PascalCasePlugin.h` / `.cpp` |
| Hub folder | `plugins/<kebab-case>/` |

---

## Building SimpleFences locally

```powershell
# From the SimpleFences repo root
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
.\build\bin\Debug\SimpleFences.exe
```

---

## License

MIT — see [LICENSE](LICENSE) for details. Plugin authors retain copyright of their own plugin code.
