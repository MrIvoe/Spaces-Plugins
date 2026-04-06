# Host-side unit test pack for IVOESimpleFences

This document is a copy-paste starter for host PR test coverage.

Covers:

- Install gate
- Stage gate
- Activate gate
- Manifest contract validation
- Hosted summary panel contract validation
- Channel policy
- Theme token policy
- Hash and signature policy paths

## 1) Suggested test files in host repo

- tests/plugins/PluginContractValidationTests.cpp
- tests/plugins/PluginUpdaterGatesTests.cpp
- tests/plugins/ThemeTokenPolicyTests.cpp
- tests/plugins/PluginSecurityPolicyTests.cpp

## 2) Shared test helpers

File: tests/plugins/PluginTestFixtures.h

```cpp
#pragma once

#include "plugins/host/PluginContractModels.h"

namespace HostPlugins::TestData
{
    inline HostedSummaryPanel MakeValidHostedPanel()
    {
        HostedSummaryPanel panel;
        panel.panelId = L"community.example.summary";
        panel.title = L"Example Overview";
        panel.schemaVersion = L"1";
        panel.layout = L"cards";
        panel.themeTokenNamespace = L"win32_theme_system";
        panel.sections = {
            HostedSummarySection{
                L"status",
                L"Status",
                L"Host-rendered status section",
                L"host.icon.status",
                L"host.surface.card.status"
            }
        };
        return panel;
    }

    inline PluginManifestContract MakeValidManifest()
    {
        PluginManifestContract m;
        m.id = L"community.example_plugin";
        m.displayName = L"Example Plugin";
        m.version = L"1.0.0";
        m.description = L"Example";
        m.author = L"Test";

        m.minHostVersion = L"0.0.012";
        m.maxHostVersion = L"0.1.999";
        m.minHostApiVersion = L"0.0.012";
        m.maxHostApiVersion = L"0.1.999";

        m.enabledByDefault = true;
        m.supportsSettingsPage = true;
        m.supportsMainContentPage = false;
        m.supportsHostedSummaryPanel = true;

        m.icon = L"";
        m.updateChannelId = L"stable";
        m.capabilities = {L"settings_pages"};
        m.repository = L"https://example.invalid/repo";

        m.hostedSummaryPanel = MakeValidHostedPanel();
        return m;
    }

    inline UpdateFeedEntry MakeValidFeedEntry()
    {
        UpdateFeedEntry e;
        e.pluginId = L"community.example_plugin";
        e.displayName = L"Example Plugin";
        e.version = L"1.0.1";
        e.author = L"Test";
        e.description = L"Example update";
        e.minHostVersion = L"0.0.012";
        e.maxHostVersion = L"0.1.999";
        e.minHostApiVersion = L"0.0.012";
        e.maxHostApiVersion = L"0.1.999";
        e.packageUrl = L"https://example.invalid/pkg.sfplugin";
        e.sha256 = L"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
        e.packageSizeBytes = 2048;
        e.updateChannelId = L"stable";
        e.signature.algorithm = L"ecdsa-p256-sha256";
        e.signature.signatureBase64 = L"VALID_BASE64_SIG_PLACEHOLDER";
        e.signature.signingCertChainPem = {L"-----BEGIN CERTIFICATE-----...-----END CERTIFICATE-----"};
        e.signature.leafThumbprintSha256 = L"abcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcd";
        e.releaseNotesUrl = L"https://example.invalid/release-notes";
        return e;
    }
}
```

## 3) Manifest + hosted panel validation tests

File: tests/plugins/PluginContractValidationTests.cpp

```cpp
#include <gtest/gtest.h>

#include "plugins/host/PluginContractValidation.h"
#include "PluginTestFixtures.h"

using namespace HostPlugins;

TEST(PluginContractValidationTests, ValidateManifestContract_AcceptsValidManifest)
{
    const auto manifest = TestData::MakeValidManifest();
    const auto result = ValidateManifestContract(manifest);
    EXPECT_TRUE(result.ok) << std::string(result.message.begin(), result.message.end());
}

TEST(PluginContractValidationTests, ValidateManifestContract_RejectsInvalidChannel)
{
    auto manifest = TestData::MakeValidManifest();
    manifest.updateChannelId = L"nightly";

    const auto result = ValidateManifestContract(manifest);
    EXPECT_FALSE(result.ok);
}

TEST(PluginContractValidationTests, ValidateManifestContract_RejectsMissingHostedPanel)
{
    auto manifest = TestData::MakeValidManifest();
    manifest.supportsHostedSummaryPanel = false;

    const auto result = ValidateManifestContract(manifest);
    EXPECT_FALSE(result.ok);
}

TEST(PluginContractValidationTests, ValidateManifestContract_RejectsCapabilityMismatch_Settings)
{
    auto manifest = TestData::MakeValidManifest();
    manifest.supportsSettingsPage = false; // but capability still has settings_pages

    const auto result = ValidateManifestContract(manifest);
    EXPECT_FALSE(result.ok);
}

TEST(PluginContractValidationTests, ValidateManifestContract_RejectsCapabilityMismatch_MainPage)
{
    auto manifest = TestData::MakeValidManifest();
    manifest.capabilities = {L"settings_pages", L"widgets"};
    manifest.supportsMainContentPage = false;

    const auto result = ValidateManifestContract(manifest);
    EXPECT_FALSE(result.ok);
}

TEST(PluginContractValidationTests, ValidateHostedSummaryPanel_RejectsWrongThemeNamespace)
{
    auto panel = TestData::MakeValidHostedPanel();
    panel.themeTokenNamespace = L"custom_theme";

    const auto result = ValidateHostedSummaryPanel(panel);
    EXPECT_FALSE(result.ok);
}

TEST(PluginContractValidationTests, ValidateHostedSummaryPanel_RejectsEmptySections)
{
    auto panel = TestData::MakeValidHostedPanel();
    panel.sections.clear();

    const auto result = ValidateHostedSummaryPanel(panel);
    EXPECT_FALSE(result.ok);
}

TEST(PluginContractValidationTests, ValidateUpdateFeedEntry_AcceptsValidEntry)
{
    const auto entry = TestData::MakeValidFeedEntry();
    const auto result = ValidateUpdateFeedEntry(entry);
    EXPECT_TRUE(result.ok) << std::string(result.message.begin(), result.message.end());
}

TEST(PluginContractValidationTests, ValidateUpdateFeedEntry_RejectsBadSignatureAlgorithm)
{
    auto entry = TestData::MakeValidFeedEntry();
    entry.signature.algorithm = L"sha1-rsa";

    const auto result = ValidateUpdateFeedEntry(entry);
    EXPECT_FALSE(result.ok);
}
```

## 4) Updater gate tests

File: tests/plugins/PluginUpdaterGatesTests.cpp

```cpp
#include <gtest/gtest.h>

#include "plugins/host/PluginUpdaterGates.h"
#include "PluginTestFixtures.h"

using namespace HostPlugins;

namespace
{
    HostVersionContext MakeHost()
    {
        HostVersionContext host;
        host.hostVersion = L"0.0.050";
        host.hostApiVersion = L"0.0.012";
        host.updatePolicyChannel = L"stable";
        host.trustedRootStoreName = L"ROOT";
        return host;
    }
}

TEST(PluginUpdaterGatesTests, InstallGate_AllowsValidManifest)
{
    const auto result = InstallGate(TestData::MakeValidManifest(), MakeHost());
    EXPECT_TRUE(result.ok) << std::string(result.message.begin(), result.message.end());
}

TEST(PluginUpdaterGatesTests, InstallGate_RejectsPreviewWhenHostPolicyStable)
{
    auto manifest = TestData::MakeValidManifest();
    manifest.updateChannelId = L"preview";

    const auto result = InstallGate(manifest, MakeHost());
    EXPECT_FALSE(result.ok);
}

TEST(PluginUpdaterGatesTests, ActivateGate_AllowsCompatibleManifest)
{
    const auto result = ActivateGate(TestData::MakeValidManifest(), MakeHost());
    EXPECT_TRUE(result.ok);
}

TEST(PluginUpdaterGatesTests, StageGate_RejectsInvalidFeedEntry)
{
    auto entry = TestData::MakeValidFeedEntry();
    entry.updateChannelId = L"invalid";

    const auto result = StageGate(entry, L"C:\\temp\\pkg.sfplugin", MakeHost());
    EXPECT_FALSE(result.ok);
}

TEST(PluginUpdaterGatesTests, StageGate_HashMismatchFails)
{
    // Requires test package file + known bad hash.
    auto entry = TestData::MakeValidFeedEntry();
    entry.sha256 = L"ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";

    const auto result = StageGate(entry, L"C:\\temp\\pkg.sfplugin", MakeHost());
    EXPECT_FALSE(result.ok);
}

TEST(PluginUpdaterGatesTests, StageGate_SignatureFailureFails)
{
    // Keep this test once VerifySignaturePolicy is fully wired.
    auto entry = TestData::MakeValidFeedEntry();
    entry.signature.signatureBase64 = L"";

    const auto result = StageGate(entry, L"C:\\temp\\pkg.sfplugin", MakeHost());
    EXPECT_FALSE(result.ok);
}
```

## 5) Channel policy tests

File: tests/plugins/PluginSecurityPolicyTests.cpp

```cpp
#include <gtest/gtest.h>

#include "plugins/host/PluginContractValidation.h"

using namespace HostPlugins;

TEST(PluginSecurityPolicyTests, IsChannelAllowed_StableHostStableRequest_True)
{
    EXPECT_TRUE(IsChannelAllowed(L"stable", L"stable"));
}

TEST(PluginSecurityPolicyTests, IsChannelAllowed_StableHostPreviewRequest_False)
{
    EXPECT_FALSE(IsChannelAllowed(L"preview", L"stable"));
}

TEST(PluginSecurityPolicyTests, IsChannelAllowed_PreviewHostStableRequest_True)
{
    EXPECT_TRUE(IsChannelAllowed(L"stable", L"preview"));
}

TEST(PluginSecurityPolicyTests, IsChannelAllowed_PreviewHostPreviewRequest_True)
{
    EXPECT_TRUE(IsChannelAllowed(L"preview", L"preview"));
}

TEST(PluginSecurityPolicyTests, IsChannelAllowed_UnknownHostPolicy_False)
{
    EXPECT_FALSE(IsChannelAllowed(L"stable", L"unknown"));
}
```

## 6) Theme token policy tests

File: tests/plugins/ThemeTokenPolicyTests.cpp

```cpp
#include <gtest/gtest.h>

#include "plugins/host/ThemeTokenPolicy.h"

using namespace HostPlugins;

TEST(ThemeTokenPolicyTests, Namespace_Valid)
{
    EXPECT_TRUE(IsValidThemeTokenNamespace(L"win32_theme_system"));
}

TEST(ThemeTokenPolicyTests, Namespace_Invalid)
{
    EXPECT_FALSE(IsValidThemeTokenNamespace(L"custom_theme"));
}

TEST(ThemeTokenPolicyTests, TokenPath_HostPrefix_Valid)
{
    EXPECT_TRUE(IsValidThemeTokenPath(L"host.surface.card.status"));
}

TEST(ThemeTokenPolicyTests, TokenPath_ThemePrefix_Valid)
{
    EXPECT_TRUE(IsValidThemeTokenPath(L"theme.text.primary"));
}

TEST(ThemeTokenPolicyTests, TokenPath_RawHexColor_Invalid)
{
    EXPECT_FALSE(IsValidThemeTokenPath(L"#1F2530"));
}

TEST(ThemeTokenPolicyTests, TokenPath_CustomPrefix_Invalid)
{
    EXPECT_FALSE(IsValidThemeTokenPath(L"plugin.custom.color"));
}
```

## 7) Test matrix checklist for PR

Minimum expected passing tests:

- Manifest contract: valid + invalid channel + capability mismatch + missing hosted panel
- Hosted panel contract: invalid namespace + empty sections
- Update feed contract: valid + invalid signature algorithm
- Install gate: valid + blocked preview on stable host
- Stage gate: invalid entry + hash mismatch + signature failure
- Activate gate: compatible success
- Channel policy: all stable/preview combinations
- Theme token policy: namespace + token path allowlist

## 8) Integration tips

- Replace C:\\temp\\pkg.sfplugin with test fixture package paths.
- Add a small known file fixture for deterministic SHA-256 tests.
- Once signature verification is fully wired, add positive signature tests using a local test certificate chain.
- Keep tests host-owned and avoid plugin-managed update behavior in test setup.
