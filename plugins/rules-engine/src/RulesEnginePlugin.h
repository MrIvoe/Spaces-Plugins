#pragma once

#include "extensions/PluginContracts.h"

#include <chrono>

class RulesEnginePlugin final : public IPlugin
{
public:
    PluginManifest GetManifest() const override;
    bool Initialize(const PluginContext& context) override;
    void Shutdown() override;

private:
    PluginContext m_context{};
    mutable bool m_paused = false;
    mutable std::chrono::steady_clock::time_point m_lastRunAt{};

    void RegisterSettings() const;
    void RegisterMenus() const;
    void RegisterCommands() const;

    void HandleRunNow(const CommandContext& command) const;
    void HandlePauseToggle(const CommandContext& command) const;
    void HandleEditorOpen(const CommandContext& command) const;
    void HandleTestItem(const CommandContext& command) const;
    void HandleExport(const CommandContext& command) const;

    bool GetBool(const std::wstring& key, bool fallback) const;
    int GetInt(const std::wstring& key, int fallback) const;
    std::wstring GetString(const std::wstring& key, const std::wstring& fallback) const;
    bool IsPluginEnabled() const;
    bool ShouldLogActions() const;
    bool IsSafeModeEnabled() const;
    std::wstring GetDefaultMode() const;
    int GetRefreshIntervalSeconds() const;
    void Notify(const std::wstring& message) const;
    bool CanRunNow() const;
    void LogInfo(const std::wstring& message) const;
    void LogWarn(const std::wstring& message) const;
};
