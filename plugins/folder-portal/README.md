# Folder Portal

Folder Portal is a plugin-backed fence content provider that links a fence to an existing folder path instead of moving files into a fence-owned folder.

## Current state

This sample now mirrors the host-side plugin patterns and provides:

- `folder_portal` provider registration with runtime callbacks
- source enumeration from `FenceMetadata.contentSource`
- drop and delete routing via provider callbacks
- tray and fence context commands using routed command payloads
- health state updates: `ready`, `disconnected`, `needs_source`

## Capabilities

- `fence_content_provider`
- `commands`
- `tray_contributions`
- `settings_pages`

## Commands

- `portal.new`
- `portal.reconnect_all`
- `portal.refresh_all`
- `portal.pause_updates_toggle`
- `portal.open_source`
- `portal.convert_to_static`

## Settings keys

- `plugin.show_notifications`
- `plugin.refresh_interval_seconds`
- `portal.general.enabled`
- `portal.general.default_mode`
- `portal.general.allow_open`
- `portal.general.allow_rename`
- `portal.general.allow_delete`
- `portal.general.show_hidden`
- `portal.general.show_system`
- `portal.watch.enabled_live_refresh`
- `portal.watch.debounce_ms`
- `portal.watch.rescan_interval_seconds`
- `portal.watch.pause_while_dragging`
- `portal.watch.retry_unavailable`
- `portal.watch.retry_interval_seconds`
- `portal.safety.read_only_network_default`
- `portal.safety.warn_destructive`
- `portal.safety.block_drop_readonly`
- `portal.safety.keep_visible_when_missing`
- `portal.display.show_path_tooltip`
- `portal.display.show_health_badge`

## Provider callbacks

- `enumerateItems(fence)` reads source folder entries and updates health state.
- `handleDrop(fence, paths)` supports `read_only`, `copy_in`, and `move_in`.
- `deleteItem(fence, item)` deletes source entries when allowed by settings.

## Settings pages

- General
- Watching
- Safety

## Host integration notes

Remaining host-side enhancements for production parity:

1. source path picker flow for `portal.new`
2. watcher lifecycle controls tied to `portal.pause_updates_toggle`
3. richer health badges and recovery UI
4. optional ACL and network policy checks before mutating source paths

## Safety posture

- no destructive source action enabled by default
- missing or unavailable source should stay visible with recovery state
- diagnostics should record reconnect and refresh attempts

## Host registration snippet

```cpp
#include "plugins/community/folder_portal/FolderPortalPlugin.h"

plugins.push_back(std::make_unique<FolderPortalPlugin>());
```
