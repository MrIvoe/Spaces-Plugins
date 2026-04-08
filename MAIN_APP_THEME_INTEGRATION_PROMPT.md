# Prompt: Main App Theme Integration (Single Theme System, Public and Safe)

Use this prompt in the main app repository (Spaces).

You are implementing a production theme pipeline that is:

- single-system
- non-breaking for existing users
- universal for public theme authors
- safe for long-term host evolution

Core outcome:

- one active theme system in host runtime: Win32ThemeSystem
- one selector plugin path for built-in switching: community.visual_modes
- host-owned rendering with token-driven surfaces

Hard requirements:

1. Single source of truth

- Accept only theme.source = win32_theme_system for active theme application.
- Treat theme.win32.theme_id as canonical lookup key.
- Use theme.win32.display_name for UI only.
- Never use display names as internal IDs.

1. Appearance tab theme source replacement

- The host appearance tab must source its theme list exclusively from the Win32ThemeSystem catalog.
- Remove or gate off any existing external marketplace, online catalog, or hardcoded theme list (e.g. Discord theme, GitHub theme, or similar named presets) from the built-in appearance tab.
- The canonical built-in theme list is exactly the 20 Win32ThemeSystem families:
  - amber-terminal, arctic-glass, aurora-light, brass-steampunk, copper-foundry
  - emerald-ledger, forest-organic, graphite-office, harbor-blue, ivory-bureau
  - mono-minimal, neon-cyberpunk, nocturne-dark, nova-futuristic, olive-terminal
  - pop-colorburst, rose-paper, storm-steel, sunset-retro, tape-lo-fi
- Third-party theme packages installed by users appear alongside built-in themes in the same list, not from a separate marketplace pane.
- If a prior theme source (marketplace or online catalog) must remain reachable, put it behind a separate "Browse external themes" action, never mixed into the primary appearance tab dropdown.
- Do not fetch or display any theme names from the network in the primary appearance tab at runtime.

1. Backward compatibility and migration

- On startup, run idempotent migration before first render.
- Normalize legacy IDs from underscore to kebab-case.
- If theme.win32.theme_id is missing, derive from theme.preset.
- If theme.source is missing or invalid, set it to win32_theme_system.
- Populate missing bridge keys:
  - theme.win32.theme_id
  - theme.win32.display_name
  - theme.win32.catalog_version
- Persist migrated values once.
- Do not alter valid current values on subsequent launches.

1. Non-breaking behavior policy

- Unknown theme IDs must fall back to graphite-office.
- On fallback, log a warning with original value for diagnostics.
- No crash, no blank UI, no undefined rendering path.
- If theme resources fail to load, keep previous valid theme applied.

1. Plugin ownership and conflict prevention

- Allow only one active appearance selector path: community.visual_modes.
- If additional appearance selector plugins are present, disable their theme-write path and log why.
- Keep compatibility aliases for older command routes, but normalize to canonical IDs before persistence.

1. Public theme authoring contract

- Publish a stable theme package contract for external authors:
  - unique theme ID in kebab-case
  - display name
  - semantic version
  - token map only (no host internals)
  - optional metadata: author, website, preview image
- Validate third-party theme packages before load.
- Reject invalid or incomplete packages with actionable diagnostics.
- Support side-by-side third-party themes without changing host code.
- Keep host API for theme discovery versioned and documented.

1. Rendering and security rules

- Host renderer resolves tokens and owns final visual output.
- Plugins and theme packages provide token data, not direct compositor writes.
- Validate token namespace and token schema before apply.
- Disallow raw executable/script payloads in theme packages.
- Cache resolved tokens safely and invalidate on theme change.

1. Performance and stability

- Apply theme changes atomically to avoid partial repaint states.
- Debounce rapid theme switch requests.
- Keep UI responsive during theme load.
- Add telemetry/log events for: migration, fallback, apply success, apply failure.

1. Required automated tests

- Migration test: underscore IDs become kebab-case.
- Migration test: missing keys are backfilled correctly.
- Idempotency test: second startup does not rewrite correct values.
- Selector test: only community.visual_modes controls active selection.
- Fallback test: unknown ID falls back to graphite-office.
- Failure test: invalid package is rejected without app instability.
- Persistence test: chosen theme survives restart unchanged.
- Conflict test: extra appearance plugins cannot override active host theme.

1. Manual verification checklist

- Upgrade from older settings and verify current theme is preserved.
- Restart twice and verify settings remain stable.
- Switch among built-in themes and verify host-rendered consistency.
- Load a valid third-party theme package and apply successfully.
- Load an invalid third-party package and verify clean rejection.
- Confirm no secondary plugin path overrides applied theme.

1. Deliverables

- host code updates in theme manager, migration layer, plugin conflict guard, and package validation
- tests covering migration, compatibility, fallback, conflict, and package validation
- documentation page for public theme authoring
- release note line:
  - Theme system consolidated to Win32ThemeSystem with backward-compatible migration and public theme package support.
