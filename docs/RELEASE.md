# Public Release Guide

This guide defines how to publish plugin-hub updates safely.

## Release Scope

A release may include:
- new plugins
- plugin updates
- schema/docs updates
- tooling updates

## Pre-Release Checklist

1. Validate all plugin manifests.
2. Verify plugin readme files are present and accurate.
3. Update `docs/CHANGELOG.md`.
4. Confirm compatibility ranges in manifests.
5. Confirm update channel values are supported (`stable` or `preview`).

## Quality Checklist

1. Build and run host app with changed plugins.
2. Verify settings page behavior and persistence.
3. Verify no plugin breaks startup stability.
4. Verify no plugin requires undeclared capabilities.

## Versioning

- Use semantic versioning in plugin manifests.
- Increase minor version for new backward-compatible features.
- Increase patch version for fixes only.
- Increase major version for contract-breaking changes.

## GitHub Release Steps

1. Merge approved changes into `master`.
2. Create a tag: `vX.Y.Z`.
3. Create GitHub release notes from changelog.
4. Attach update feed artifacts if used.

## Post-Release

1. Verify repository docs links.
2. Track issues/regressions.
3. Plan patch release if required.
