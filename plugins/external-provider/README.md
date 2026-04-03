# External Provider Fences

External Provider is a catalog-aligned sample for provider-backed virtual item lists sourced from file paths, directories, and generated inputs.

## Capabilities

- fence_content_provider
- commands
- settings_pages
- tray_contributions

## Commands

- provider.new
- provider.refresh_current
- provider.refresh_all
- provider.reconnect_failed

## Settings

- plugin.show_notifications
- plugin.refresh_interval_seconds
- provider.enabled
- provider.refresh_mode
- provider.refresh_interval_seconds
- provider.timeout_seconds
- provider.retry_count
- provider.cache_enabled
- provider.cache_ttl_seconds
- provider.read_only_default
- provider.rss.url
- provider.json.source
- provider.network.root_path

## Behavior summary

- Registers an external_provider content provider with enumerate/drop/delete callbacks.
- Supports cache-backed enumeration with TTL.
- Uses fence content state transitions: connected, offline, unavailable, permission_denied.
- New provider command creates an external_provider fence using configured source defaults.

## Host registration snippet

```cpp
#include "plugins/community/external_provider/ExternalProviderPlugin.h"

plugins.push_back(std::make_unique<ExternalProviderPlugin>());
```
