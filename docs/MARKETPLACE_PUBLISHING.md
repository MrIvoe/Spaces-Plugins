# Marketplace Publishing Flow

This document defines the marketplace artifact model for Spaces plugins.

## Goals

- No Git dependency for end users.
- Artifact-based plugin distribution (ZIP + catalog JSON).
- Host-compatible and API-compatible filtering in app.
- Deterministic CI publishing flow.

## Artifact Outputs

CI generates the following under `dist/marketplace/`:

- `catalog/catalog.json`
- `packages/<plugin-id>-<version>.zip`

Each plugin package is versioned and immutable.

## Catalog Contract

The marketplace catalog schema lives at:

- `marketplace-catalog.schema.json`

Each plugin entry includes:

- `id`
- `displayName`
- `author`
- `description`
- `category`
- `version`
- `channel`
- `downloadUrl`
- `hash`
- `compatibility.hostVersion.min/max`
- `compatibility.hostApiVersion.min/max`
- `capabilities`
- `supportsSettingsPage`
- `restartRequired`

Additional metadata (for UI/detail panels) can be included.

## Package Format

Plugin ZIP package should contain plugin files at package root:

- `plugin.json`
- binaries/assets/content required by plugin

The host extracts package content into:

- `%LOCALAPPDATA%/SimpleSpaces/plugins/<plugin-id>/`

## CI Workflow

Marketplace build workflow:

- `.github/workflows/publish-marketplace.yml`

On `master` push and `marketplace-v*` tags:

1. Validate manifests.
2. Build plugin ZIP artifacts.
3. Generate `catalog.json`.
4. Upload `dist/marketplace` as workflow artifact.
5. On tags, publish ZIP and catalog assets to GitHub Releases.

## Scripts

Script used by CI/local maintainers:

- `scripts/build-marketplace-artifacts.ps1`

Example local run:

```powershell
./scripts/build-marketplace-artifacts.ps1 `
  -BaseDownloadUrl "https://github.com/MrIvoe/Spaces-Plugins/releases/download/marketplace-v1.0.0"
```

## Compatibility Rules

Compatibility is sourced from plugin manifest:

- preferred: `compatibility.hostVersion` and `compatibility.hostApiVersion`
- fallback: legacy `minHostVersion/maxHostVersion` and API equivalents

Host decides install/update eligibility based on these ranges.

## Release Channels

Manifest channel source:

- `updateChannelId` (`stable` or `preview`)

Catalog generator can optionally exclude preview packages.

## Security and Safety Expectations

Host side must enforce:

- manifest validity
- host compatibility checks
- hash verification
- safe extraction to plugin folder
- capability policy guard

These checks are required before activation.
