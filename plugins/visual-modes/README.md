# Visual Modes

Visual Modes is a Tier 1 catalog sample that applies global or per-fence visual behavior presets through plugin commands and persisted settings.

## Capabilities

- appearance
- commands
- tray_contributions
- settings_pages

## Commands

- theme.switch
- theme.compact_toggle
- theme.presentation_toggle
- theme.apply_current_to_fence
- theme.reset_fence

## Settings

- plugin.enabled
- plugin.log_actions
- plugin.show_notifications
- plugin.safe_mode
- plugin.default_mode
- plugin.config_source
- plugin.refresh_interval_seconds
- theme.preset
- theme.apply_global
- theme.allow_per_fence_override
- theme.colors.background
- theme.colors.header
- theme.colors.border
- theme.colors.text
- theme.effects.transparency
- theme.effects.opacity_percent
- theme.effects.blur
- theme.effects.corner_radius_px

## Behavior summary

- Switch command cycles preset values and persists the active preset.
- Compact and presentation toggles switch between preset mode and default mode.
- Apply/reset commands target a single fence when command context includes fence payload.
- When global apply is enabled, preset switches can apply to all fences.

## Host API usage

This sample demonstrates:

- routed command payload consumption via CommandContext
- persisted settings updates via PluginSettingsRegistry
- tray command contributions via MenuContributionRegistry
- visual behavior updates via IApplicationCommands::UpdateFencePresentation

## Notes

- Color and blur values are persisted and ready for host renderer consumption.
- The sample maps presets to FencePresentationSettings so behavior can be tested even before full compositor support is present.

## Host registration snippet

```cpp
#include "plugins/community/visual_modes/VisualModesPlugin.h"

plugins.push_back(std::make_unique<VisualModesPlugin>());
```
