# Context Actions

Context Actions adds plugin-defined right-click actions for desktop, fence, and item workflows.

## Current state

This sample now demonstrates payload-aware command routing:

- desktop/fence/item menu contributions
- handlers consuming routed `CommandContext` (fence/item metadata)
- forwarding actions to organizer and appearance commands with preserved context
- provider refresh targeting the selected fence
- diagnostics that log the exact routed metadata

## Capabilities

- `commands`
- `desktop_context`
- `settings_pages`

## Commands

- `context.new_folder_portal_here`
- `context.sort_this_fence`
- `context.cleanup_this_fence`
- `context.apply_theme_this_fence`
- `context.pin_selected`
- `context.refresh_provider`
- `context.copy_item_metadata`
- `context.open_plugin_settings`

## Settings

- `plugin.show_notifications`
- `plugin.refresh_interval_seconds`
- `context.enabled`
- `context.show_advanced`
- `context.compact_mode`
- `context.group_under_submenu`
- `context.show_desktop_background`
- `context.show_fence_background`
- `context.show_fence_items`

## Routed behavior

- `context.sort_this_fence` forwards to `organizer.by_type`
- `context.cleanup_this_fence` forwards to `organizer.cleanup_empty`
- `context.apply_theme_this_fence` forwards to `appearance.mode.focus`
- `context.new_folder_portal_here` forwards to `portal.new`
- `context.pin_selected` logs pin intent for selected item payload
- `context.refresh_provider` refreshes the selected fence via app commands
- `context.copy_item_metadata` logs routed item metadata payload
- `context.open_plugin_settings` logs settings deep-link intent

## Settings page

- Context Actions

## Host integration notes

Current sample assumes host support for:

1. command dispatch payload routing
2. fence and item context menu plugin injection

Additional future enhancements:

1. visibility filters per surface and selection state
2. grouped submenu rendering
3. non-blocking execution with toast feedback

## Safety posture

- no destructive operations are executed by default
- menu actions are currently command stubs with diagnostics logging
- future action handlers should enforce context validation before execution

## Host registration snippet

```cpp
#include "plugins/community/context_actions/ContextActionsPlugin.h"

plugins.push_back(std::make_unique<ContextActionsPlugin>());
```
