# Rules Engine

Rules Engine is a catalog-aligned sample that demonstrates routed item classification commands and safe rule execution controls.

## Capabilities

- commands
- settings_pages
- tray_contributions

## Commands

- rules.run_now
- rules.pause_toggle
- rules.editor.open
- rules.test_item
- rules.export

## Settings

- plugin.enabled
- plugin.log_actions
- plugin.show_notifications
- plugin.safe_mode
- plugin.default_mode
- plugin.config_source
- plugin.refresh_interval_seconds
- rules.enabled
- rules.eval_mode
- rules.log_matches
- rules.dry_run
- rules.debounce_ms
- rules.max_rules_per_item
- rules.prevent_loops

## Rule schema files

- rules.schema.json: JSON Schema contract for rule files (version 1.0).
- rules.sample.json: validated starter rule set that follows the schema.

## Behavior summary

- Supports pause/resume and dry-run guardrails.
- Uses item payload from CommandContext for rules.test_item classification.
- Exports UTF-8 JSON to rules-engine-export.json with schemaVersion, schemaRef, sampleRuleFile, and normalized settings.
- Keeps actions non-blocking and diagnostics-first.

## Host registration snippet

```cpp
#include "plugins/community/rules_engine/RulesEnginePlugin.h"

plugins.push_back(std::make_unique<RulesEnginePlugin>());
```
