# Fence Organizer

A sorting and cleanup plugin that performs real filesystem organization actions for the selected fence.

## Features

### Sorting and Cleanup Commands

- **Organize by File Type**: Move files into `_type_<ext>` folders
- **Flatten Organized Folders**: Move files back to fence root
- **Remove Empty Subfolders**: Delete empty child folders
- **Archive Old Files**: Move files older than configured age into `_archive`
- **Move Large Files**: Move files larger than configured size into `_large_files`

## How It Works

This plugin demonstrates several key SimpleFences architecture patterns:

### 1. Menu Contributions

The plugin registers commands on the **FenceContext** menu surface - shown when right-clicking on a fence window. This allows users to trigger organization actions without leaving the fence UI.

**Example Menu Items:**

```text
├─ Sort by Name
├─ Sort by Date Modified  
├─ Sort by Size
├─ Sort by Type
├─ Group by File Type [separator]
├─ Find Duplicates
└─ Show Large Files
```

### 2. Settings Page

The plugin provides one action-oriented settings page with:

- plugin.enabled
- plugin.log_actions
- plugin.show_notifications
- plugin.safe_mode
- plugin.default_mode
- plugin.config_source
- plugin.refresh_interval_seconds
- Type bucket prefix
- Hidden and shortcut handling
- Flatten behavior scope
- Archive and large-file folder names
- Large file threshold (MB)
- Old file threshold (days)

### 3. Command Routing

Menu items dispatch commands to the `CommandDispatcher`:

- `organizer.by_type`
- `organizer.flatten`
- `organizer.cleanup_empty`
- `organizer.archive_old`
- `organizer.move_large`

Each command is registered with a handler lambda that captures `this`. The handler:

1. Reads selected fence payload from `CommandContext`
2. Falls back to `IApplicationCommands::GetCurrentCommandContext()` when needed
3. Performs filesystem operations on the resolved fence path
4. Uses diagnostics to log results

### 4. Fence Query API

The organizer uses host fence query methods in `IApplicationCommands`:

```cpp
// Get currently routed command context
CommandContext GetCurrentCommandContext() const;

// List all fence IDs in the system
std::vector<std::wstring> GetAllFenceIds() const;

// Get metadata for a specific fence
FenceMetadata GetFenceMetadata(const std::wstring& fenceId) const;

// Refresh a fence after plugin-initiated file operations
void RefreshFence(const std::wstring& fenceId);
```

Where `FenceMetadata` contains:

```cpp
struct FenceMetadata
{
    std::wstring id;                  // unique fence ID
    std::wstring title;               // display name
    std::wstring backingFolderPath;   // file system path to fence's contents
    std::wstring contentType;
    std::wstring contentPluginId;
    std::wstring contentSource;
    std::wstring contentState;
    std::wstring contentStateDetail;
};
```

### 5. File System Operations

All sort/organization logic operates directly on the fence's backing folder:

- No special host support needed for basic operations
- Uses standard C++ `<filesystem>` library
- Safe, graceful error handling

## Code Structure

```text
fence-organizer/
├─ plugin.json                  # Manifest: ID, version, capabilities, metadata
├─ src/
│  ├─ FenceOrganizerPlugin.h   # Class definition, command handlers
│  └─ FenceOrganizerPlugin.cpp # Implementation: handlers, sort logic, FS ops
└─ README.md                    # This file
```

## Extension Points Used

| Extension Point | Purpose | Example |
| --- | --- | --- |
| `MenuContributionRegistry` | Register fence context menu items | "Sort by Name" button |
| `PluginSettingsRegistry` | Declare persistent settings pages | "Behavior" and "Analysis" settings |
| `CommandDispatcher` | Register command handlers | `organizer.sort_by_name` handler |
| `IApplicationCommands` | Query active fence metadata | Get folder path to organize |
| `Diagnostics` | Log plugin activity | Info and error messages |

## Host Requirements

**Minimum Host Version**: 0.0.012+

- Requires `FenceMetadata` and active-fence query methods on `IApplicationCommands`

**No other special host support needed** - all sorting and file analysis happens in the plugin using standard filesystem APIs.

## Future Enhancements

1. Duplicate finder with hash-based reports
2. Per-operation dry-run preview mode
3. Undo transactions for bulk operations
4. Conflict strategy options (skip, overwrite, rename)

## Plugin Architecture Lessons

This plugin demonstrates several best practices for SimpleFences plugins:

✅ **Context-aware commands**

- Commands target the selected fence through routed payloads.
- No host-side organizer special cases are required.

✅ **Clean Separation**

- All organization logic lives in the plugin
- Host doesn't need to know about sorting, duplicates, or file analysis
- Plugin fails gracefully if fence is inaccessible

✅ **Multiple Registry Types**

- Menu contributions: user-facing commands
- Settings: persistent user preferences
- Command dispatch: command routing and execution
- Diagnostics: observability and debugging

✅ **Lazy Binding**

- Plugin captures `PluginContext` pointers in `Initialize()`
- Uses them later when commands are dispatched with payload context
- No global state or tight coupling

✅ **Error Handling**

- Checks for null pointers before use
- Catches filesystem exceptions
- Logs informative diagnostic messages
- Gracefully handles missing/inaccessible fences

## Contributing

To extend this plugin:

1. Add new sort modes in `SortFenceContents()`
2. Add new analysis methods (duplicate detection, etc.)
3. Register new menu items and command handlers
4. Add settings to control behavior
5. Test with fences containing various file types

## Testing Notes

To test the organizer plugin:

1. Build with the updated host (0.0.011+)
2. Create a test fence with mixed file types
3. Right-click the fence to see organizer menu items
4. Run each action and verify folder/file movement in the backing folder
5. Modify settings and retry to verify persistence
6. Check `%LOCALAPPDATA%\SimpleFences\debug.log` for diagnostic messages
