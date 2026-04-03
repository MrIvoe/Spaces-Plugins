#include "VisualModesPlugin.h"

#include "extensions/MenuContributionRegistry.h"
#include "extensions/PluginSettingsRegistry.h"
#include "extensions/SettingsSchema.h"

#include <array>

PluginManifest VisualModesPlugin::GetManifest() const
{
    PluginManifest m;
    m.id = L"community.visual_modes";
    m.displayName = L"Visual Modes";
    m.version = L"1.0.0";
    m.description = L"Applies global or per-fence visual behavior presets.";
    m.minHostApiVersion = SimpleFencesVersion::kPluginApiVersion;
    m.maxHostApiVersion = SimpleFencesVersion::kPluginApiVersion;
    m.capabilities = {L"appearance", L"commands", L"tray_contributions", L"settings_pages"};
    return m;
}

bool VisualModesPlugin::Initialize(const PluginContext& context)
{
    m_context = context;
    if (!m_context.settingsRegistry || !m_context.commandDispatcher || !m_context.appCommands)
    {
        if (m_context.diagnostics)
        {
            m_context.diagnostics->Error(L"VisualModesPlugin: Missing required host services");
        }
        return false;
    }

    RegisterSettings();
    RegisterMenus();
    RegisterCommands();
    LogInfo(L"Initialized");
    return true;
}

void VisualModesPlugin::Shutdown()
{
    LogInfo(L"Shutdown");
}

void VisualModesPlugin::RegisterSettings() const
{
    PluginSettingsPage page;
    page.pluginId = L"community.visual_modes";
    page.pageId = L"theme.visual_modes";
    page.title = L"Visual Modes";
    page.order = 40;

    page.fields.push_back(SettingsFieldDescriptor{L"plugin.enabled", L"Enable plugin", L"Master toggle for Visual Modes behavior.", SettingsFieldType::Bool, L"true", {}, 1});
    page.fields.push_back(SettingsFieldDescriptor{L"plugin.log_actions", L"Log actions", L"Write visual mode actions to diagnostics.", SettingsFieldType::Bool, L"true", {}, 2});
    page.fields.push_back(SettingsFieldDescriptor{L"plugin.show_notifications", L"Show notifications", L"Show user-facing notifications for visual mode changes when supported.", SettingsFieldType::Bool, L"false", {}, 3});
    page.fields.push_back(SettingsFieldDescriptor{L"plugin.safe_mode", L"Safe mode", L"Prefer conservative visual changes for compatibility.", SettingsFieldType::Bool, L"true", {}, 4});
    page.fields.push_back(SettingsFieldDescriptor{L"plugin.default_mode", L"Default mode", L"Default visual behavior mode.", SettingsFieldType::Enum, L"balanced", {{L"balanced", L"Balanced"}, {L"showcase", L"Showcase"}, {L"minimal", L"Minimal"}}, 5});
    page.fields.push_back(SettingsFieldDescriptor{L"plugin.config_source", L"Config source", L"Configuration source identifier for this plugin.", SettingsFieldType::String, L"local", {}, 6});
    page.fields.push_back(SettingsFieldDescriptor{L"plugin.refresh_interval_seconds", L"Refresh interval (s)", L"Preferred interval for visual refresh tasks.", SettingsFieldType::Int, L"300", {}, 7});

    page.fields.push_back(SettingsFieldDescriptor{
        L"theme.preset",
        L"Preset",
        L"Choose the active visual preset.",
        SettingsFieldType::Enum,
        L"default",
        {
            {L"default", L"Default"},
            {L"dark_glass", L"Dark Glass"},
            {L"compact", L"Compact"},
            {L"minimal", L"Minimal"},
            {L"high_contrast", L"High Contrast"},
            {L"presentation", L"Presentation"},
            {L"custom", L"Custom"},
        },
        10});

    page.fields.push_back(SettingsFieldDescriptor{L"theme.apply_global", L"Apply globally", L"Apply preset actions to all fences by default.", SettingsFieldType::Bool, L"true", {}, 20});
    page.fields.push_back(SettingsFieldDescriptor{L"theme.allow_per_fence_override", L"Allow per-fence override", L"Allow apply/reset commands to target one fence.", SettingsFieldType::Bool, L"true", {}, 30});
    page.fields.push_back(SettingsFieldDescriptor{L"theme.colors.background", L"Background color", L"Custom preset background color (#RRGGBB).", SettingsFieldType::String, L"#1F2530", {}, 40});
    page.fields.push_back(SettingsFieldDescriptor{L"theme.colors.header", L"Header color", L"Custom preset header color (#RRGGBB).", SettingsFieldType::String, L"#223247", {}, 50});
    page.fields.push_back(SettingsFieldDescriptor{L"theme.colors.border", L"Border color", L"Custom preset border color (#RRGGBB).", SettingsFieldType::String, L"#3E4A5F", {}, 60});
    page.fields.push_back(SettingsFieldDescriptor{L"theme.colors.text", L"Text color", L"Custom preset text color (#RRGGBB).", SettingsFieldType::String, L"#E4ECF7", {}, 70});
    page.fields.push_back(SettingsFieldDescriptor{L"theme.effects.transparency", L"Enable transparency", L"Enable transparent visual treatment when supported.", SettingsFieldType::Bool, L"true", {}, 80});
    page.fields.push_back(SettingsFieldDescriptor{L"theme.effects.opacity_percent", L"Opacity percent", L"Window opacity percent for custom and glass presets.", SettingsFieldType::Int, L"88", {}, 90});
    page.fields.push_back(SettingsFieldDescriptor{L"theme.effects.blur", L"Enable blur", L"Enable blur when host compositor supports it.", SettingsFieldType::Bool, L"true", {}, 100});
    page.fields.push_back(SettingsFieldDescriptor{L"theme.effects.corner_radius_px", L"Corner radius (px)", L"Corner radius used by supported presets.", SettingsFieldType::Int, L"8", {}, 110});

    m_context.settingsRegistry->RegisterPage(std::move(page));
}

void VisualModesPlugin::RegisterMenus() const
{
    if (!m_context.menuRegistry)
    {
        return;
    }

    m_context.menuRegistry->Register(MenuContribution{MenuSurface::Tray, L"Switch Visual Preset", L"theme.switch", 210, false});
    m_context.menuRegistry->Register(MenuContribution{MenuSurface::Tray, L"Toggle Compact Mode", L"theme.compact_toggle", 220, false});
    m_context.menuRegistry->Register(MenuContribution{MenuSurface::Tray, L"Toggle Presentation Mode", L"theme.presentation_toggle", 230, false});
}

void VisualModesPlugin::RegisterCommands() const
{
    m_context.commandDispatcher->RegisterCommand(L"theme.switch", [this](const CommandContext& command) { HandleThemeSwitch(command); });
    m_context.commandDispatcher->RegisterCommand(L"theme.compact_toggle", [this](const CommandContext& command) { HandleCompactToggle(command); });
    m_context.commandDispatcher->RegisterCommand(L"theme.presentation_toggle", [this](const CommandContext& command) { HandlePresentationToggle(command); });
    m_context.commandDispatcher->RegisterCommand(L"theme.apply_current_to_fence", [this](const CommandContext& command) { HandleApplyCurrentToFence(command); });
    m_context.commandDispatcher->RegisterCommand(L"theme.reset_fence", [this](const CommandContext& command) { HandleResetFence(command); });
}

void VisualModesPlugin::HandleThemeSwitch(const CommandContext&) const
{
    if (!IsPluginEnabled())
    {
        return;
    }

    static const std::array<std::wstring, 7> presets = {
        L"default", L"dark_glass", L"compact", L"minimal", L"high_contrast", L"presentation", L"custom"
    };

    const std::wstring current = GetSetting(L"theme.preset", L"default");
    size_t index = 0;
    while (index < presets.size() && presets[index] != current)
    {
        ++index;
    }

    const std::wstring nextPreset = presets[(index + 1) % presets.size()];
    SetPreset(nextPreset);

    if (GetSettingBool(L"theme.apply_global", true) && !IsSafeModeEnabled())
    {
        const FenceMetadata active = m_context.appCommands->GetActiveFenceMetadata();
        if (!active.id.empty())
        {
            ApplyPresetToFence(active.id, nextPreset, true);
        }
    }
}

void VisualModesPlugin::HandleCompactToggle(const CommandContext& command) const
{
    if (!IsPluginEnabled())
    {
        return;
    }

    const std::wstring current = GetSetting(L"theme.preset", L"default");
    const std::wstring next = (current == L"compact") ? L"default" : L"compact";
    SetPreset(next);

    const std::wstring fenceId = ResolveTargetFenceId(command);
    if (!fenceId.empty())
    {
        const bool applyToAll = GetSettingBool(L"theme.apply_global", true);
        ApplyPresetToFence(fenceId, next, applyToAll);
    }
}

void VisualModesPlugin::HandlePresentationToggle(const CommandContext& command) const
{
    if (!IsPluginEnabled())
    {
        return;
    }

    const std::wstring current = GetSetting(L"theme.preset", L"default");
    const std::wstring next = (current == L"presentation") ? L"default" : L"presentation";
    SetPreset(next);

    const std::wstring fenceId = ResolveTargetFenceId(command);
    if (!fenceId.empty())
    {
        const bool applyToAll = GetSettingBool(L"theme.apply_global", true);
        ApplyPresetToFence(fenceId, next, applyToAll);
    }
}

void VisualModesPlugin::HandleApplyCurrentToFence(const CommandContext& command) const
{
    if (!IsPluginEnabled())
    {
        return;
    }

    if (!GetSettingBool(L"theme.allow_per_fence_override", true))
    {
        return;
    }

    const std::wstring fenceId = ResolveTargetFenceId(command);
    if (fenceId.empty())
    {
        return;
    }

    ApplyPresetToFence(fenceId, GetSetting(L"theme.preset", L"default"), false);
}

void VisualModesPlugin::HandleResetFence(const CommandContext& command) const
{
    if (!IsPluginEnabled())
    {
        return;
    }

    if (!GetSettingBool(L"theme.allow_per_fence_override", true))
    {
        return;
    }

    const std::wstring fenceId = ResolveTargetFenceId(command);
    if (fenceId.empty())
    {
        return;
    }

    ApplyPresetToFence(fenceId, L"default", false);
}

std::wstring VisualModesPlugin::GetSetting(const std::wstring& key, const std::wstring& fallback) const
{
    return m_context.settingsRegistry ? m_context.settingsRegistry->GetValue(key, fallback) : fallback;
}

bool VisualModesPlugin::GetSettingBool(const std::wstring& key, bool fallback) const
{
    return GetSetting(key, fallback ? L"true" : L"false") == L"true";
}

bool VisualModesPlugin::IsPluginEnabled() const
{
    return GetSettingBool(L"plugin.enabled", true);
}

bool VisualModesPlugin::ShouldLogActions() const
{
    return GetSettingBool(L"plugin.log_actions", true);
}

bool VisualModesPlugin::IsSafeModeEnabled() const
{
    return GetSettingBool(L"plugin.safe_mode", true);
}

int VisualModesPlugin::GetRefreshIntervalSeconds() const
{
    int seconds = 300;
    try
    {
        seconds = std::stoi(GetSetting(L"plugin.refresh_interval_seconds", L"300"));
    }
    catch (...)
    {
        seconds = 300;
    }

    if (seconds < 1)
    {
        seconds = 1;
    }

    return seconds;
}

void VisualModesPlugin::Notify(const std::wstring& message) const
{
    if (!GetSettingBool(L"plugin.show_notifications", false) || !m_context.diagnostics)
    {
        return;
    }

    m_context.diagnostics->Info(L"[VisualModes][Notification] " + message);
}

bool VisualModesPlugin::CanApplyNow() const
{
    const auto now = std::chrono::steady_clock::now();
    const auto interval = std::chrono::seconds(GetRefreshIntervalSeconds());
    if (m_lastApplyAt.time_since_epoch().count() != 0 && (now - m_lastApplyAt) < interval)
    {
        LogInfo(L"Apply throttled by plugin.refresh_interval_seconds");
        return false;
    }

    m_lastApplyAt = now;
    return true;
}

void VisualModesPlugin::SetPreset(const std::wstring& preset) const
{
    if (!m_context.settingsRegistry)
    {
        return;
    }

    m_context.settingsRegistry->SetValue(L"theme.preset", preset);
    LogInfo(L"Preset switched to " + preset);
}

std::wstring VisualModesPlugin::ResolveTargetFenceId(const CommandContext& command) const
{
    if (!command.fence.id.empty())
    {
        return command.fence.id;
    }

    const CommandContext activeCommand = m_context.appCommands->GetCurrentCommandContext();
    if (!activeCommand.fence.id.empty())
    {
        return activeCommand.fence.id;
    }

    return m_context.appCommands->GetActiveFenceMetadata().id;
}

FencePresentationSettings VisualModesPlugin::BuildPresentationFromPreset(const std::wstring& preset) const
{
    FencePresentationSettings settings;

    if (preset == L"compact")
    {
        settings.textOnlyMode = true;
        settings.rollupWhenNotHovered = true;
        settings.transparentWhenNotHovered = true;
        settings.labelsOnHover = true;
        settings.iconSpacingPreset = L"compact";
    }
    else if (preset == L"presentation")
    {
        settings.textOnlyMode = false;
        settings.rollupWhenNotHovered = false;
        settings.transparentWhenNotHovered = false;
        settings.labelsOnHover = false;
        settings.iconSpacingPreset = L"spacious";
    }
    else if (preset == L"minimal")
    {
        settings.textOnlyMode = true;
        settings.rollupWhenNotHovered = false;
        settings.transparentWhenNotHovered = true;
        settings.labelsOnHover = true;
        settings.iconSpacingPreset = L"compact";
    }
    else if (preset == L"high_contrast")
    {
        settings.textOnlyMode = false;
        settings.rollupWhenNotHovered = false;
        settings.transparentWhenNotHovered = false;
        settings.labelsOnHover = true;
        settings.iconSpacingPreset = L"comfortable";
    }
    else if (preset == L"dark_glass")
    {
        settings.textOnlyMode = false;
        settings.rollupWhenNotHovered = false;
        settings.transparentWhenNotHovered = true;
        settings.labelsOnHover = true;
        settings.iconSpacingPreset = L"comfortable";
    }
    else if (preset == L"custom")
    {
        settings.textOnlyMode = false;
        settings.rollupWhenNotHovered = false;
        settings.transparentWhenNotHovered = GetSettingBool(L"theme.effects.transparency", true);
        settings.labelsOnHover = true;
        settings.iconSpacingPreset = L"comfortable";
    }
    else
    {
        settings.textOnlyMode = false;
        settings.rollupWhenNotHovered = false;
        settings.transparentWhenNotHovered = false;
        settings.labelsOnHover = true;
        settings.iconSpacingPreset = L"comfortable";
    }

    return settings;
}

void VisualModesPlugin::ApplyPresetToFence(const std::wstring& fenceId, const std::wstring& preset, bool applyToAll) const
{
    if (!m_context.appCommands || fenceId.empty())
    {
        return;
    }

    if (!CanApplyNow())
    {
        return;
    }

    FencePresentationSettings settings = BuildPresentationFromPreset(preset);
    settings.applyToAll = applyToAll && !IsSafeModeEnabled();
    m_context.appCommands->UpdateFencePresentation(fenceId, settings);
    Notify(L"Visual preset applied.");
}

void VisualModesPlugin::LogInfo(const std::wstring& message) const
{
    if (m_context.diagnostics && ShouldLogActions())
    {
        m_context.diagnostics->Info(L"[VisualModes] " + message);
    }
}
