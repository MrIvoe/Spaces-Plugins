# Plugin Catalog

Production-oriented plugin catalog aligned to the SimpleFences capability model:

- `commands`
- `tray_contributions`
- `fence_content_provider`
- `appearance`
- `widgets`
- `desktop_context`
- `settings_pages`

Supported settings field types:

- `Bool`
- `Int`
- `String`
- `Enum`

## Standard plugin manifest fields

All plugins should expose a consistent metadata surface:

- `id`
- `displayName`
- `version`
- `description`
- `author`
- `minHostVersion`
- `maxHostVersion`
- `minHostApiVersion`
- `maxHostApiVersion`
- `supportsSettingsPage`
- `supportsMainContentPage`
- optional `icon`
- optional `updateChannelId`
- `capabilities`
- `repository`

## UI standard

- no hardcoded colors by default
- host resources first
- standard margins and padding
- standard settings layout
- standard section headers/cards

Use the shared helper in `plugins/shared/PluginUiPatterns.h` and keep plugin-specific pages aligned with host layout conventions.

## Recommended plugin update model

Use package publishing + update manifest feed, not direct repository folder consumption.

Recommended flow:

1. Build plugin packages in this repo.
2. Publish package file + metadata + compatibility + manifest JSON.
3. Main app checks manifest feed.
4. Main app downloads newer package.
5. Main app stages replacement.
6. Main app loads update on next launch or safe reload.

Example update manifest entry:

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

## Folder Portal Catalog

### Folder Portal Manifest

```json
{
  "id": "community.folder_portal",
  "displayName": "Folder Portal",
  "version": "1.0.0",
  "description": "Displays external folder contents inside fences without moving source files.",
  "author": "SimpleFences Community",
  "minHostVersion": "0.0.012",
  "maxHostVersion": "0.1.999",
  "minHostApiVersion": "0.0.012",
  "maxHostApiVersion": "0.1.999",
  "enabledByDefault": true,
  "supportsSettingsPage": true,
  "supportsMainContentPage": true,
  "icon": "",
  "updateChannelId": "stable",
  "capabilities": ["fence_content_provider", "commands", "tray_contributions", "settings_pages"]
}
```

### Folder Portal Commands

- `portal.new`
- `portal.reconnect_all`
- `portal.refresh_all`
- `portal.pause_updates_toggle`
- `portal.open_source`
- `portal.convert_to_static`

### Folder Portal Settings

- `portal.general.enabled` (`Bool`, default `true`)
- `portal.general.default_mode` (`Enum`: `read_only|copy_in|move_in`, default `read_only`)
- `portal.general.allow_open` (`Bool`, default `true`)
- `portal.general.allow_rename` (`Bool`, default `false`)
- `portal.general.allow_delete` (`Bool`, default `false`)
- `portal.general.show_hidden` (`Bool`, default `false`)
- `portal.general.show_system` (`Bool`, default `false`)
- `portal.watch.enabled_live_refresh` (`Bool`, default `true`)
- `portal.watch.debounce_ms` (`Int`, default `400`)
- `portal.watch.rescan_interval_seconds` (`Int`, default `60`)
- `portal.watch.pause_while_dragging` (`Bool`, default `true`)
- `portal.watch.retry_unavailable` (`Bool`, default `true`)
- `portal.watch.retry_interval_seconds` (`Int`, default `30`)
- `portal.safety.read_only_network_default` (`Bool`, default `true`)
- `portal.safety.warn_destructive` (`Bool`, default `true`)
- `portal.safety.block_drop_readonly` (`Bool`, default `true`)
- `portal.safety.keep_visible_when_missing` (`Bool`, default `true`)
- `portal.display.show_path_tooltip` (`Bool`, default `true`)
- `portal.display.show_health_badge` (`Bool`, default `true`)

### Folder Portal User Flow

1. User triggers `New Folder Portal`.
2. Plugin resolves source path and validates it.
3. Fence is created with `contentType=folder_portal`.
4. Plugin starts debounced watch plus periodic rescan.
5. UI updates health badge and empty states when source is unavailable.

### Folder Portal Failure Policy

- Keep portal visible if source is missing.
- Emit health states: `connected`, `unavailable`, `permission_denied`, `offline`, `slow`.
- Never perform destructive source operations by default.

## Sorting Clean Up Catalog

### Sorting Clean Up Manifest

```json
{
  "id": "community.fence_sort_cleanup",
  "displayName": "Fence Sort and Clean Up",
  "version": "1.0.0",
  "description": "Sorts and arranges fence items with optional automation and presets.",
  "author": "SimpleFences Community",
  "minHostVersion": "0.0.012",
  "maxHostVersion": "0.1.999",
  "minHostApiVersion": "0.0.012",
  "maxHostApiVersion": "0.1.999",
  "enabledByDefault": true,
  "supportsSettingsPage": true,
  "supportsMainContentPage": false,
  "icon": "",
  "updateChannelId": "stable",
  "capabilities": ["commands", "tray_contributions", "settings_pages", "desktop_context"]
}
```

### Sorting Clean Up Commands

- `sort.current.name`
- `sort.current.type`
- `sort.current.modified`
- `cleanup.current`
- `align.current.grid`
- `autosort.toggle`

### Sorting Clean Up Settings

- `sort.mode.default` (`Enum`: `name|type|extension|size|modified|created|manual`)
- `sort.mode.direction` (`Enum`: `asc|desc`)
- `sort.case_sensitive` (`Bool`, default `false`)
- `sort.locale_aware` (`Bool`, default `true`)
- `sort.folders_first` (`Bool`, default `true`)
- `layout.cleanup_after_sort` (`Bool`, default `true`)
- `layout.align_to_grid` (`Bool`, default `true`)
- `layout.grid_spacing_px` (`Int`, default `12`)
- `layout.preserve_pinned` (`Bool`, default `true`)
- `auto.on_item_add` (`Bool`, default `false`)
- `auto.debounce_ms` (`Int`, default `800`)

### Sorting Clean Up Failure Policy

- Stable sort and no-overlap guarantee.
- Rollback on layout compute failure.
- Debounce burst updates to prevent thrash.

## Rules Engine Catalog

### Rules Engine Manifest

```json
{
  "id": "community.rules_engine",
  "displayName": "Rules Engine",
  "version": "1.0.0",
  "description": "Routes and classifies items using ordered matching rules.",
  "author": "SimpleFences Community",
  "minHostVersion": "0.0.012",
  "maxHostVersion": "0.1.999",
  "minHostApiVersion": "0.0.012",
  "maxHostApiVersion": "0.1.999",
  "enabledByDefault": true,
  "supportsSettingsPage": true,
  "supportsMainContentPage": false,
  "icon": "",
  "updateChannelId": "stable",
  "capabilities": ["commands", "settings_pages", "tray_contributions"]
}
```

### Rules Engine Commands

- `rules.run_now`
- `rules.pause_toggle`
- `rules.editor.open`
- `rules.test_item`
- `rules.export`

### Rules Engine Settings

- `rules.enabled` (`Bool`, default `true`)
- `rules.eval_mode` (`Enum`: `first_match|all_matches|first_terminal_match`)
- `rules.log_matches` (`Bool`, default `true`)
- `rules.dry_run` (`Bool`, default `false`)
- `rules.debounce_ms` (`Int`, default `300`)
- `rules.max_rules_per_item` (`Int`, default `100`)
- `rules.prevent_loops` (`Bool`, default `true`)

### Rules Engine Failure Policy

- Deterministic ordering and conflict policy.
- Loop prevention and dry-run mode.
- Pattern validation with clear error messages.

## Appearance Visual Modes Catalog

### Appearance Visual Modes Manifest

```json
{
  "id": "community.visual_modes",
  "displayName": "Visual Modes",
  "version": "1.0.0",
  "description": "Applies global or per-fence themes and visual behavior presets.",
  "author": "SimpleFences Community",
  "minHostVersion": "0.0.012",
  "maxHostVersion": "0.1.999",
  "minHostApiVersion": "0.0.012",
  "maxHostApiVersion": "0.1.999",
  "enabledByDefault": true,
  "supportsSettingsPage": true,
  "supportsMainContentPage": false,
  "icon": "",
  "updateChannelId": "stable",
  "capabilities": ["appearance", "commands", "tray_contributions", "settings_pages"]
}
```

### Appearance Visual Modes Commands

- `theme.switch`
- `theme.compact_toggle`
- `theme.presentation_toggle`
- `theme.apply_current_to_fence`
- `theme.reset_fence`

### Appearance Visual Modes Settings

- `theme.preset` (`Enum`: `default|dark_glass|compact|minimal|high_contrast|presentation|custom`)
- `theme.apply_global` (`Bool`, default `true`)
- `theme.allow_per_fence_override` (`Bool`, default `true`)
- `theme.colors.background` (`String`)
- `theme.colors.header` (`String`)
- `theme.colors.border` (`String`)
- `theme.colors.text` (`String`)
- `theme.effects.transparency` (`Bool`, default `true`)
- `theme.effects.opacity_percent` (`Int`, default `88`)
- `theme.effects.blur` (`Bool`, default `true`)
- `theme.effects.corner_radius_px` (`Int`, default `8`)

### Appearance Visual Modes Failure Policy

- Fallback when blur or acrylic is unsupported.
- Readability guardrails and contrast enforcement.

## Productivity Actions Catalog

### Productivity Actions Manifest

```json
{
  "id": "community.productivity_actions",
  "displayName": "Productivity Actions",
  "version": "1.0.0",
  "description": "Adds practical multi-step commands for daily fence workflows.",
  "author": "SimpleFences Community",
  "minHostVersion": "0.0.012",
  "maxHostVersion": "0.1.999",
  "minHostApiVersion": "0.0.012",
  "maxHostApiVersion": "0.1.999",
  "enabledByDefault": true,
  "supportsSettingsPage": true,
  "supportsMainContentPage": false,
  "icon": "",
  "updateChannelId": "stable",
  "capabilities": ["commands", "tray_contributions", "settings_pages", "desktop_context"]
}
```

### Productivity Actions Commands

- `productivity.create_project_fence`
- `productivity.archive_old`
- `productivity.open_all`
- `productivity.batch_rename`
- `productivity.snapshot.save`

### Productivity Actions Settings

- `prod.enabled` (`Bool`, default `true`)
- `prod.confirm_batch` (`Bool`, default `true`)
- `prod.templates.default` (`Enum`: `dev|design|media|general`)
- `prod.archive.threshold_days` (`Int`, default `30`)
- `prod.archive.action` (`Enum`: `move|copy|prompt`)
- `prod.archive.destination` (`String`)
- `prod.rename.pattern` (`String`, default `{name}_{index}`)

### Productivity Actions Failure Policy

- Confirmation gates for destructive actions.
- Locked-file handling and partial success reporting.

## Widgets Catalog

### Widgets Manifest

```json
{
  "id": "community.widgets_plus",
  "displayName": "Widgets Plus",
  "version": "1.0.0",
  "description": "Provides embeddable utility widgets with bounded refresh and persisted state.",
  "author": "SimpleFences Community",
  "minHostVersion": "0.0.012",
  "maxHostVersion": "0.1.999",
  "minHostApiVersion": "0.0.012",
  "maxHostApiVersion": "0.1.999",
  "enabledByDefault": true,
  "supportsSettingsPage": true,
  "supportsMainContentPage": true,
  "icon": "",
  "updateChannelId": "stable",
  "capabilities": ["widgets", "commands", "settings_pages", "tray_contributions"]
}
```

### Widgets Commands

- `widgets.add.clock`
- `widgets.add.notes`
- `widgets.add.checklist`
- `widgets.refresh_all`
- `widgets.pause_toggle`

### Widgets Settings

- `widgets.enabled` (`Bool`, default `true`)
- `widgets.refresh_seconds_default` (`Int`, default `60`)
- `widgets.pause_on_battery_or_fullscreen` (`Bool`, default `true`)
- `widgets.clock.mode` (`Enum`: `digital|analog`)
- `widgets.clock.use_24h` (`Bool`, default `false`)
- `widgets.notes.auto_save` (`Bool`, default `true`)
- `widgets.notes.max_chars` (`Int`, default `5000`)

### Widgets Failure Policy

- Bounded refresh frequency.
- Safe fallback rendering on widget failure.

## External Provider Catalog

### External Provider Manifest

```json
{
  "id": "community.external_provider",
  "displayName": "External Provider Fences",
  "version": "1.0.0",
  "description": "Shows provider-backed virtual item lists from external and generated sources.",
  "author": "SimpleFences Community",
  "minHostVersion": "0.0.012",
  "maxHostVersion": "0.1.999",
  "minHostApiVersion": "0.0.012",
  "maxHostApiVersion": "0.1.999",
  "enabledByDefault": true,
  "supportsSettingsPage": true,
  "supportsMainContentPage": true,
  "icon": "",
  "updateChannelId": "stable",
  "capabilities": ["fence_content_provider", "commands", "settings_pages", "tray_contributions"]
}
```

### External Provider Commands

- `provider.new`
- `provider.refresh_current`
- `provider.refresh_all`
- `provider.reconnect_failed`

### External Provider Settings

- `provider.enabled` (`Bool`, default `true`)
- `provider.refresh_mode` (`Enum`: `manual|interval|on_startup|hybrid`)
- `provider.refresh_interval_seconds` (`Int`, default `300`)
- `provider.timeout_seconds` (`Int`, default `15`)
- `provider.retry_count` (`Int`, default `3`)
- `provider.cache_enabled` (`Bool`, default `true`)
- `provider.cache_ttl_seconds` (`Int`, default `600`)
- `provider.read_only_default` (`Bool`, default `true`)
- `provider.rss.url` (`String`)
- `provider.json.source` (`String`)
- `provider.network.root_path` (`String`)

### External Provider Failure Policy

- Timeout plus retry policies.
- Stale-data indicators.
- No blocking UI on provider operations.

## Context Actions Catalog

### Context Actions Manifest

```json
{
  "id": "community.context_actions",
  "displayName": "Context Actions",
  "version": "1.0.0",
  "description": "Adds context-aware right-click actions for desktop, fences, and items.",
  "author": "SimpleFences Community",
  "minHostVersion": "0.0.012",
  "maxHostVersion": "0.1.999",
  "minHostApiVersion": "0.0.012",
  "maxHostApiVersion": "0.1.999",
  "enabledByDefault": true,
  "supportsSettingsPage": true,
  "supportsMainContentPage": false,
  "icon": "",
  "updateChannelId": "stable",
  "capabilities": ["commands", "desktop_context", "settings_pages"]
}
```

### Context Actions Commands

- `context.new_folder_portal_here`
- `context.sort_this_fence`
- `context.cleanup_this_fence`
- `context.apply_theme_this_fence`
- `context.pin_selected`
- `context.refresh_provider`
- `context.copy_item_metadata`
- `context.open_plugin_settings`

### Context Actions Settings

- `context.enabled` (`Bool`, default `true`)
- `context.show_advanced` (`Bool`, default `false`)
- `context.compact_mode` (`Bool`, default `true`)
- `context.group_under_submenu` (`Bool`, default `true`)
- `context.show_desktop_background` (`Bool`, default `true`)
- `context.show_fence_background` (`Bool`, default `true`)
- `context.show_fence_items` (`Bool`, default `true`)

### Context Actions Failure Policy

- Context-aware visibility to prevent invalid actions.
- Non-blocking command handlers.

## Cross Plugin Baseline

Every production plugin should include at least:

- `plugin.enabled` (`Bool`)
- `plugin.log_actions` (`Bool`)
- `plugin.show_notifications` (`Bool`)
- `plugin.safe_mode` or `plugin.read_only_mode` (`Bool`, where relevant)
- `plugin.default_mode` (`Enum`)
- one or more source or config fields (`String`)
- refresh or update interval (`Int`, where relevant)

## Maturity Tiers

- Tier 1: Folder Portal, Sorting/Clean Up, Appearance/Visual Modes, Context Menu Extension
- Tier 2: Rules, Productivity Actions, Widgets
- Tier 3: External Content Provider

Tier 1 gives strongest visible value at lower risk. Tier 3 has higher complexity around connectivity, caching, and validation.

## Production Standards

All plugins should have:

- clear manifest and semantic versioning
- strong empty-state UX
- graceful failure states
- debounced or throttled updates
- no blocking UI work
- visible diagnostics hooks
- safe defaults
- conservative destructive behavior
- recoverable configuration
- contributor-friendly documentation
