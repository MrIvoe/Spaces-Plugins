#pragma once

#include "extensions/PluginContracts.h"

#include <chrono>

// Community plugin: Context Actions
// Adds contextual command contributions for desktop, fence, and item workflows.
// Capability: commands, desktop_context, settings_pages
class ContextActionsPlugin final : public IPlugin
{
public:
    PluginManifest GetManifest() const override;
    bool Initialize(const PluginContext& context) override;
    void Shutdown() override;

private:
    PluginContext m_context{};
    mutable std::chrono::steady_clock::time_point m_lastRefreshAt{};

    void RegisterSettings() const;
    void RegisterMenus() const;
    void RegisterCommands() const;

    void HandleNewFolderPortalHere(const CommandContext& command) const;
    void HandleSortThisFence(const CommandContext& command) const;
    void HandleCleanupThisFence(const CommandContext& command) const;
    void HandleApplyThemeThisFence(const CommandContext& command) const;
    void HandlePinSelected(const CommandContext& command) const;
    void HandleRefreshProvider(const CommandContext& command) const;
    void HandleCopyItemMetadata(const CommandContext& command) const;
    void HandleOpenPluginSettings(const CommandContext& command) const;
    bool GetBool(const std::wstring& key, bool fallback) const;
    int GetInt(const std::wstring& key, int fallback) const;
    void Notify(const std::wstring& message) const;
    void RefreshFenceWithThrottle(const std::wstring& fenceId) const;
    void LogInfo(const std::wstring& message) const;
};
