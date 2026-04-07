# Plugin README Template

Use this structure for all plugin README files.

## Template

```markdown
# <Plugin Display Name>

<One-sentence plugin description>

## Plugin Snapshot

- Folder: `<plugin-folder>`
- Plugin ID: `<community.plugin_id>`
- Version: `<version>`
- Capabilities: `<comma-separated capabilities>`

## Files

- `plugin.json`
- `src/<PluginClass>.h`
- `src/<PluginClass>.cpp`

## Host Integration

1. Copy this plugin folder into the host plugin source location.
2. Register plugin source in host build configuration.
3. Register plugin in host plugin bootstrap (`BuiltinPlugins.cpp`).
4. Build and run host app.

## Validation

1. Run manifest validator from this repo root:
   - `./scripts/validate-plugin-manifests.ps1`
2. In host app, verify:
   - settings page visibility (if applicable)
   - command/menu behavior (if applicable)
   - startup stability and settings persistence

## Related Docs

- `../../docs/CREATE_A_PLUGIN.md`
- `../../docs/HOW_IT_WORKS.md`
- `../../docs/PLUGINS.md`
- `../../docs/RELEASE.md`
```

## Notes

- Keep plugin-specific custom sections short.
- Prefer shared docs for long explanations.
- Do not duplicate repository-wide policy in every plugin README.
