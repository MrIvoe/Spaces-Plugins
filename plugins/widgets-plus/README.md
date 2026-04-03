# Widgets Plus

Widgets Plus is a catalog-aligned widgets sample that provides command routing, persisted widget settings, and bounded refresh controls.

## Capabilities

- widgets
- commands
- settings_pages
- tray_contributions

## Commands

- widgets.add.clock
- widgets.add.notes
- widgets.add.checklist
- widgets.refresh_all
- widgets.pause_toggle

## Settings

- plugin.show_notifications
- plugin.refresh_interval_seconds
- widgets.enabled
- widgets.refresh_seconds_default
- widgets.pause_on_battery_or_fullscreen
- widgets.clock.mode
- widgets.clock.use_24h
- widgets.notes.auto_save
- widgets.notes.max_chars

## Behavior summary

- Registers tray commands for widget creation and refresh control.
- Uses settings-backed behavior for clock and notes widgets.
- Supports pause/resume for refresh requests.
- Refresh-all targets fences with widget_panel content type when host support is present.

## Host registration snippet

```cpp
#include "plugins/community/widgets_plus/WidgetsPlusPlugin.h"

plugins.push_back(std::make_unique<WidgetsPlusPlugin>());
```
