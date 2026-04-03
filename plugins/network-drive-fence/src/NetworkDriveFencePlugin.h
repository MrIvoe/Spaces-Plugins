#pragma once

#include "extensions/PluginContracts.h"

#include <chrono>

// Community plugin: Network Drive Fence
// Registers a fence_content_provider that browses a UNC / mapped drive path.
class NetworkDriveFencePlugin final : public IPlugin
{
public:
    PluginManifest GetManifest() const override;
    bool Initialize(const PluginContext& context) override;
    void Shutdown() override;

private:
    PluginContext m_context{};
    mutable std::chrono::steady_clock::time_point m_lastSyncAt{};

    bool GetBool(const std::wstring& key, bool fallback) const;
    int GetInt(const std::wstring& key, int fallback) const;
    void Notify(const std::wstring& message) const;
    void RefreshNetworkFencesWithThrottle() const;
};
