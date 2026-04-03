#pragma once

#include "extensions/SettingsSchema.h"

#include <string>
#include <vector>

namespace PluginUiPatterns
{
    inline std::wstring BoolDefault(bool value)
    {
        return value ? L"true" : L"false";
    }

    inline void AppendBaselineSettingsFields(std::vector<SettingsFieldDescriptor>& fields,
                                             int firstOrder,
                                             int refreshIntervalSeconds,
                                             bool showNotificationsDefault)
    {
        if (refreshIntervalSeconds < 1)
        {
            refreshIntervalSeconds = 1;
        }

        fields.push_back(SettingsFieldDescriptor{
            L"plugin.show_notifications",
            L"Show notifications",
            L"Emit user-facing notification events to diagnostics.",
            SettingsFieldType::Bool,
            BoolDefault(showNotificationsDefault),
            {},
            firstOrder});

        fields.push_back(SettingsFieldDescriptor{
            L"plugin.refresh_interval_seconds",
            L"Refresh interval (s)",
            L"Minimum interval between host refresh operations triggered by this plugin.",
            SettingsFieldType::Int,
            std::to_wstring(refreshIntervalSeconds),
            {},
            firstOrder + 1});

        fields.push_back(SettingsFieldDescriptor{
            L"plugin.ui.color_source",
            L"Color source",
            L"Use host resources for plugin UI colors by default. Choose custom only when a plugin explicitly supports custom palettes.",
            SettingsFieldType::Enum,
            L"host_resources",
            {
                {L"host_resources", L"Host resources (recommended)"},
                {L"custom", L"Custom plugin colors"},
            },
            firstOrder + 2});

        fields.push_back(SettingsFieldDescriptor{
            L"plugin.ui.use_host_icons",
            L"Use host icons",
            L"Prefer host-provided icon and glyph resources for visual consistency.",
            SettingsFieldType::Bool,
            L"true",
            {},
            firstOrder + 3});

        fields.push_back(SettingsFieldDescriptor{
            L"plugin.ui.section_style",
            L"Section style",
            L"Preferred settings section rendering style.",
            SettingsFieldType::Enum,
            L"cards",
            {
                {L"cards", L"Cards"},
                {L"headers", L"Section headers"},
                {L"compact", L"Compact"},
            },
            firstOrder + 4});

        fields.push_back(SettingsFieldDescriptor{
            L"plugin.ui.standard_margins",
            L"Use standard margins",
            L"Apply host standard outer margins for plugin settings content.",
            SettingsFieldType::Bool,
            L"true",
            {},
            firstOrder + 5});

        fields.push_back(SettingsFieldDescriptor{
            L"plugin.ui.standard_padding",
            L"Use standard padding",
            L"Apply host standard inner padding for plugin settings cards and groups.",
            SettingsFieldType::Bool,
            L"true",
            {},
            firstOrder + 6});

        fields.push_back(SettingsFieldDescriptor{
            L"plugin.ui.show_section_headers",
            L"Show section headers",
            L"Render standardized section headers above settings groups.",
            SettingsFieldType::Bool,
            L"true",
            {},
            firstOrder + 7});
    }
}
