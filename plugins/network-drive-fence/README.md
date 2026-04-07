# Network Drive Fence

A fence content provider that maps a UNC path or mapped network drive as a browseable folder fence.

## Plugin Snapshot

- Folder: `network-drive-fence`
- Plugin ID: `community.network_drive_fence`
- Version: `1.2.2`
- Capabilities: `fence_content_provider`, `settings_pages`

## Files

- `plugin.json`
- `src/NetworkDriveFencePlugin.h`
- `src/NetworkDriveFencePlugin.cpp`

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

- [Create A Plugin](../../docs/CREATE_A_PLUGIN.md)
- [How It Works](../../docs/HOW_IT_WORKS.md)
- [Plugins Offered](../../docs/PLUGINS.md)
- [Release Guide](../../docs/RELEASE.md)
