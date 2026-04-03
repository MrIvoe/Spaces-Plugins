# Productivity Actions

Productivity Actions is a catalog-aligned sample that demonstrates practical multi-step fence workflows.

## Capabilities

- commands
- tray_contributions
- settings_pages
- desktop_context

## Commands

- productivity.create_project_fence
- productivity.archive_old
- productivity.open_all
- productivity.batch_rename
- productivity.snapshot.save

## Settings

- plugin.enabled
- plugin.log_actions
- plugin.show_notifications
- plugin.safe_mode
- plugin.default_mode
- plugin.config_source
- plugin.refresh_interval_seconds
- prod.enabled
- prod.confirm_batch
- prod.templates.default
- prod.archive.threshold_days
- prod.archive.action
- prod.archive.destination
- prod.rename.pattern

## Behavior summary

- Creates template-based project fences.
- Archives old files by move/copy/prompt policy.
- Opens all items in the selected fence.
- Batch-renames files using {name} and {index} tokens.
- Writes a plain-text fence snapshot report.

## Host registration snippet

```cpp
#include "plugins/community/productivity_actions/ProductivityActionsPlugin.h"

plugins.push_back(std::make_unique<ProductivityActionsPlugin>());
```
