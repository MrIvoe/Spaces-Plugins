# Fence Sort and Clean Up

Fence Sort and Clean Up is a catalog-aligned sample focused on sorting and cleanup workflows for fence contents.

## Capabilities

- commands
- tray_contributions
- settings_pages
- desktop_context

## Commands

- sort.current.name
- sort.current.type
- sort.current.modified
- cleanup.current
- align.current.grid
- autosort.toggle

## Settings

- plugin.enabled
- plugin.log_actions
- plugin.show_notifications
- plugin.safe_mode
- plugin.default_mode
- plugin.config_source
- plugin.refresh_interval_seconds
- sort.mode.default
- sort.mode.direction
- sort.case_sensitive
- sort.locale_aware
- sort.folders_first
- layout.cleanup_after_sort
- layout.align_to_grid
- layout.grid_spacing_px
- layout.preserve_pinned
- auto.on_item_add
- auto.debounce_ms

## Behavior summary

- Sort commands operate on the active or routed fence backing folder.
- Cleanup command removes empty subfolders and refreshes the fence.
- Autosort toggle persists auto.on_item_add state in plugin settings.
- Grid align command logs a non-blocking alignment intent using configured spacing.

## Host registration snippet

```cpp
#include "plugins/community/sorting_cleanup/SortingCleanupPlugin.h"

plugins.push_back(std::make_unique<SortingCleanupPlugin>());
```
