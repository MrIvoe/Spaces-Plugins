# Create A New Plugin

## 1. Copy the template

Copy `plugin-template/` to a new folder in `plugins/` using kebab-case.

## 2. Update the manifest

Edit `plugin.json`:
- choose a unique `community.*` plugin id
- set host version/API compatibility
- set capabilities accurately
- add hosted summary panel metadata when needed

## 3. Implement plugin source

Rename and edit template source files:
- `TemplatePlugin.h`
- `TemplatePlugin.cpp`

## 4. Add settings page fields

Use host-compatible field types:
- `Bool`
- `Int`
- `String`
- `Enum`

Prefer shared UI baseline conventions for consistency.

## 5. Validate

Run manifest validation:

```powershell
./scripts/validate-plugin-manifests.ps1
```

## 6. Integrate and test in host

In IVOESimpleFences:
1. Register plugin source in host build.
2. Register plugin in `BuiltinPlugins.cpp`.
3. Build and run host.
4. Verify settings page appears.
5. Verify settings persist across restart.

## 7. Prepare PR

Include:
- plugin purpose
- capability usage
- any host API dependencies
- test notes
