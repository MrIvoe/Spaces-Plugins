# Network Drive Fence

A fence content provider that enumerates and manages UNC paths and mapped network drives.

## Current state

This plugin registers a fence content provider capability with 15+ persistent settings across three pages (Network Drive, Availability, and Access). The host can use this to populate fence content with network drive file listings and operations.

## What it does now

- registers the `network_drive_fence` content type for fence providers
- declares three settings pages: **Network Drive** (connection, display), **Availability** (offline handling, caching), and **Access** (auth, credentials)
- provides stable settings keys for connection parameters and behavior
- supports UNC paths, mapped drives, and reconnection logic

## Key settings

**Baseline plugin settings:**

- `plugin.show_notifications`: emits notification events to diagnostics when enabled
- `plugin.refresh_interval_seconds`: throttles network-drive sync refresh operations

**Network Drive page:**

- `default_path`: root UNC path or drive letter
- `reconnect_on_load`, `offline_label`: robustness
- `refresh_seconds`, `show_connection_status`: visibility
- `prefer_drive_label`, `display_alias`: labeling
- `open_action`: interaction mode (explorer, command prompt, custom)

**Availability page:**

- `offline_cache_mode`: fallback strategy
- `warn_on_slow_share`, `retry_count`: reliability
- `connect_timeout_seconds`, `background_refresh_on_metered`: performance

**Access page:**

- `auth_mode`: credential strategy
- `remember_successful_credentials`: persistence
- `read_only_preference`: safety default

## What the host still needs

- Win32 WinAPI calls for UNC/mapped drive enumeration (WNetOpenEnum, WNetEnumResource)
- credential caching and SecureStore integration
- file browser UI (context menu, properties, file operations)
- offline caching implementation (local SQLite or file store)
- slow/metered network detection (NETWORK_COST_MANAGER)

## Suggested host direction

Start with a folder browser view that:

1. Enumerates the default_path recursively
2. Applies reconnection logic on display
3. Caches results in-memory or on-disk
4. Shows connection status (green/offline icon)

Consider adding per-drive bookmarks and favorite paths.
