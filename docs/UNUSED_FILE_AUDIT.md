# Unused File Audit (Plugin Folders)

Audit date: 2026-04-07

## Scope

- `plugins/*`
- includes plugin source, manifests, README files, and shared helper headers

## Method

1. Enumerated plugin-folder files.
2. Verified helper/header references in plugin source.
3. Verified rules-engine schema/sample references in code and docs.
4. Confirmed each plugin has expected contract files (`plugin.json`, `README.md`, `src/*`).

## Result

No confirmed dead files were found in plugin folders.

## Confirmed In Use

- `plugins/shared/PluginUiPatterns.h`: referenced by plugin source files.
- `plugins/shared/SampleFsHelpers.h`: referenced by organizer and portal plugins.
- `plugins/rules-engine/rules.schema.json` and `plugins/rules-engine/rules.sample.json`: referenced by rules-engine plugin code and documentation.

## Action Taken

- No plugin-folder files were deleted in this audit because no file met the "confirmed dead" threshold.
