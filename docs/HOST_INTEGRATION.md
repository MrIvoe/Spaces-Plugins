# Host Integration Pack (Consolidated)

This page replaces the previous split copy-paste packs.

## Goal

Provide one host-facing entry for:
- contract validation implementation concepts
- updater/security gate concepts
- host-side test coverage targets

## Implementation Areas (Host Repo)

Recommended host implementation modules:
- plugin contract models
- manifest/feed validation
- updater gate checks
- theme token policy checks

## Required Behavior

- Validate plugin manifests before install.
- Validate update feed entries before stage/activate.
- Enforce compatibility ranges and channels.
- Enforce hash/signature policy.
- Keep plugin UI host-rendered with theme tokens.

## Host Test Coverage

Add tests for:
- manifest contract validity and failure cases
- hosted summary panel contract validity
- update feed channel and compatibility gates
- hash/signature policy behavior

## Reference Note

If you still need historical copy-paste blocks, retrieve them from git history.
This repository now keeps one consolidated host integration page for simpler maintenance.
