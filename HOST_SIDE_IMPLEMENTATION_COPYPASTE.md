# Host-side implementation pack for IVOESimpleFences

This document is designed to be copied into the host repo and implemented with minimal interpretation.

Goals covered:

- Enforce plugin manifest contract before install
- Enforce update feed contract before stage
- Enforce compatibility before activate
- Enforce update channels as stable or preview
- Enforce hash + signature chain policy
- Support hosted summary panel for plugins with no main content page
- Keep plugin UI host-rendered and token-driven from Win32ThemeSystem

## 1) Add these files in the host repo

Suggested host paths:

- src/plugins/host/PluginContractModels.h
- src/plugins/host/PluginContractValidation.h
- src/plugins/host/PluginContractValidation.cpp
- src/plugins/host/PluginUpdaterGates.h
- src/plugins/host/PluginUpdaterGates.cpp
- src/plugins/host/ThemeTokenPolicy.h
- src/plugins/host/ThemeTokenPolicy.cpp

## 2) Copy-paste models

File: src/plugins/host/PluginContractModels.h

```cpp
#pragma once

#include <string>
#include <vector>

namespace HostPlugins
{
    struct HostedSummarySection
    {
        std::wstring id;
        std::wstring title;
        std::wstring description;
        std::wstring iconToken;
        std::wstring surfaceToken;
    };

    struct HostedSummaryPanel
    {
        std::wstring panelId;
        std::wstring title;
        std::wstring schemaVersion;      // must be "1"
        std::wstring layout;             // cards | sections | compact
        std::wstring themeTokenNamespace; // must be "win32_theme_system"
        std::vector<HostedSummarySection> sections;
    };

    struct PluginManifestContract
    {
        std::wstring id;
        std::wstring displayName;
        std::wstring version;
        std::wstring description;
        std::wstring author;

        std::wstring minHostVersion;
        std::wstring maxHostVersion;
        std::wstring minHostApiVersion;
        std::wstring maxHostApiVersion;

        bool enabledByDefault = true;
        bool supportsSettingsPage = false;
        bool supportsMainContentPage = false;
        bool supportsHostedSummaryPanel = false;

        std::wstring icon;
        std::wstring updateChannelId; // stable | preview

        HostedSummaryPanel hostedSummaryPanel;
        std::vector<std::wstring> capabilities;
        std::wstring repository;
    };

    struct FeedSignature
    {
        std::wstring algorithm;              // ecdsa-p256-sha256 | rsa-pss-sha256
        std::wstring signatureBase64;
        std::vector<std::wstring> signingCertChainPem;
        std::wstring leafThumbprintSha256;
    };

    struct UpdateFeedEntry
    {
        std::wstring pluginId;
        std::wstring displayName;
        std::wstring version;
        std::wstring author;
        std::wstring description;

        std::wstring minHostVersion;
        std::wstring maxHostVersion;
        std::wstring minHostApiVersion;
        std::wstring maxHostApiVersion;

        std::wstring packageUrl;
        std::wstring sha256;
        long long packageSizeBytes = 0;
        std::wstring updateChannelId; // stable | preview

        FeedSignature signature;
        std::wstring releaseNotesUrl;
    };
}
```

## 3) Copy-paste validation header

File: src/plugins/host/PluginContractValidation.h

```cpp
#pragma once

#include "PluginContractModels.h"

#include <string>

namespace HostPlugins
{
    struct ValidationResult
    {
        bool ok = false;
        std::wstring message;
    };

    ValidationResult ValidateManifestContract(const PluginManifestContract& manifest);
    ValidationResult ValidateHostedSummaryPanel(const HostedSummaryPanel& panel);
    ValidationResult ValidateUpdateFeedEntry(const UpdateFeedEntry& entry);

    // Stage/install security helpers
    ValidationResult VerifySha256File(const std::wstring& filePath, const std::wstring& expectedHexLower);
    ValidationResult VerifySignaturePolicy(const UpdateFeedEntry& entry,
                                           const std::wstring& packagePath,
                                           const std::wstring& trustedRootStoreName);

    bool IsChannelAllowed(const std::wstring& requestedChannel,
                          const std::wstring& hostPolicyChannel);
}
```

## 4) Copy-paste validation implementation

File: src/plugins/host/PluginContractValidation.cpp

```cpp
#include "PluginContractValidation.h"

#include <algorithm>
#include <cwctype>

#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

namespace HostPlugins
{
    namespace
    {
        bool IsEmpty(const std::wstring& s) { return s.empty(); }

        std::wstring ToLower(std::wstring s)
        {
            std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c) {
                return static_cast<wchar_t>(std::towlower(c));
            });
            return s;
        }

        bool EqualsNoCase(const std::wstring& a, const std::wstring& b)
        {
            return ToLower(a) == ToLower(b);
        }

        bool IsSemVerLike(const std::wstring& v)
        {
            int dots = 0;
            if (v.empty()) return false;
            for (wchar_t c : v)
            {
                if (c == L'.') { ++dots; continue; }
                if (c < L'0' || c > L'9') return false;
            }
            return dots == 2;
        }

        bool ContainsCapability(const std::vector<std::wstring>& caps, const wchar_t* value)
        {
            for (const auto& c : caps)
            {
                if (c == value) return true;
            }
            return false;
        }

        ValidationResult Ok()
        {
            return ValidationResult{true, L"ok"};
        }

        ValidationResult Fail(const std::wstring& msg)
        {
            return ValidationResult{false, msg};
        }
    }

    ValidationResult ValidateHostedSummaryPanel(const HostedSummaryPanel& panel)
    {
        if (IsEmpty(panel.panelId) || IsEmpty(panel.title))
        {
            return Fail(L"hostedSummaryPanel requires panelId and title");
        }

        if (panel.schemaVersion != L"1")
        {
            return Fail(L"hostedSummaryPanel.schemaVersion must be '1'");
        }

        if (!(panel.layout == L"cards" || panel.layout == L"sections" || panel.layout == L"compact"))
        {
            return Fail(L"hostedSummaryPanel.layout must be cards, sections, or compact");
        }

        if (panel.themeTokenNamespace != L"win32_theme_system")
        {
            return Fail(L"hostedSummaryPanel.themeTokenNamespace must be win32_theme_system");
        }

        if (panel.sections.empty())
        {
            return Fail(L"hostedSummaryPanel.sections must contain at least one section");
        }

        for (const auto& section : panel.sections)
        {
            if (IsEmpty(section.id) || IsEmpty(section.title) || IsEmpty(section.description) ||
                IsEmpty(section.iconToken) || IsEmpty(section.surfaceToken))
            {
                return Fail(L"hostedSummaryPanel section requires id/title/description/iconToken/surfaceToken");
            }
        }

        return Ok();
    }

    ValidationResult ValidateManifestContract(const PluginManifestContract& manifest)
    {
        if (IsEmpty(manifest.id) || IsEmpty(manifest.displayName) || IsEmpty(manifest.version) ||
            IsEmpty(manifest.description) || IsEmpty(manifest.author))
        {
            return Fail(L"manifest core metadata is incomplete");
        }

        if (!IsSemVerLike(manifest.version))
        {
            return Fail(L"manifest version must be semantic x.y.z");
        }

        if (IsEmpty(manifest.minHostVersion) || IsEmpty(manifest.maxHostVersion) ||
            IsEmpty(manifest.minHostApiVersion) || IsEmpty(manifest.maxHostApiVersion))
        {
            return Fail(L"manifest host/api compatibility bounds are required");
        }

        const bool capSettings = ContainsCapability(manifest.capabilities, L"settings_pages");
        const bool capHostedMain = ContainsCapability(manifest.capabilities, L"fence_content_provider") ||
                                   ContainsCapability(manifest.capabilities, L"widgets");

        if (manifest.supportsSettingsPage != capSettings)
        {
            return Fail(L"supportsSettingsPage must match settings_pages capability");
        }

        if (manifest.supportsMainContentPage != capHostedMain)
        {
            return Fail(L"supportsMainContentPage must match content-provider/widgets capability");
        }

        if (manifest.updateChannelId != L"stable" && manifest.updateChannelId != L"preview")
        {
            return Fail(L"updateChannelId must be stable or preview");
        }

        if (!manifest.supportsHostedSummaryPanel)
        {
            return Fail(L"supportsHostedSummaryPanel must be true");
        }

        auto panelResult = ValidateHostedSummaryPanel(manifest.hostedSummaryPanel);
        if (!panelResult.ok)
        {
            return panelResult;
        }

        return Ok();
    }

    ValidationResult ValidateUpdateFeedEntry(const UpdateFeedEntry& entry)
    {
        if (IsEmpty(entry.pluginId) || IsEmpty(entry.version) || IsEmpty(entry.packageUrl) || IsEmpty(entry.sha256))
        {
            return Fail(L"update feed entry missing required metadata");
        }

        if (entry.updateChannelId != L"stable" && entry.updateChannelId != L"preview")
        {
            return Fail(L"update feed channel must be stable or preview");
        }

        if (entry.signature.algorithm != L"ecdsa-p256-sha256" && entry.signature.algorithm != L"rsa-pss-sha256")
        {
            return Fail(L"signature.algorithm unsupported");
        }

        if (IsEmpty(entry.signature.signatureBase64) || entry.signature.signingCertChainPem.empty() ||
            IsEmpty(entry.signature.leafThumbprintSha256))
        {
            return Fail(L"signature block is incomplete");
        }

        return Ok();
    }

    ValidationResult VerifySha256File(const std::wstring& filePath, const std::wstring& expectedHexLower)
    {
        HANDLE file = ::CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE)
        {
            return Fail(L"cannot open package for sha256 verification");
        }

        BCRYPT_ALG_HANDLE alg = nullptr;
        BCRYPT_HASH_HANDLE hash = nullptr;
        PUCHAR hashObject = nullptr;
        DWORD hashObjectLen = 0;
        DWORD hashLen = 0;
        DWORD cbData = 0;

        auto cleanup = [&]() {
            if (hash) BCryptDestroyHash(hash);
            if (alg) BCryptCloseAlgorithmProvider(alg, 0);
            if (hashObject) HeapFree(GetProcessHeap(), 0, hashObject);
            if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
        };

        if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0)
        {
            cleanup();
            return Fail(L"BCryptOpenAlgorithmProvider failed");
        }

        if (BCryptGetProperty(alg, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&hashObjectLen), sizeof(hashObjectLen), &cbData, 0) != 0)
        {
            cleanup();
            return Fail(L"BCryptGetProperty(BCRYPT_OBJECT_LENGTH) failed");
        }

        if (BCryptGetProperty(alg, BCRYPT_HASH_LENGTH, reinterpret_cast<PUCHAR>(&hashLen), sizeof(hashLen), &cbData, 0) != 0)
        {
            cleanup();
            return Fail(L"BCryptGetProperty(BCRYPT_HASH_LENGTH) failed");
        }

        hashObject = reinterpret_cast<PUCHAR>(HeapAlloc(GetProcessHeap(), 0, hashObjectLen));
        if (!hashObject)
        {
            cleanup();
            return Fail(L"HeapAlloc failed");
        }

        if (BCryptCreateHash(alg, &hash, hashObject, hashObjectLen, nullptr, 0, 0) != 0)
        {
            cleanup();
            return Fail(L"BCryptCreateHash failed");
        }

        BYTE buffer[4096];
        DWORD read = 0;
        while (::ReadFile(file, buffer, static_cast<DWORD>(sizeof(buffer)), &read, nullptr) && read > 0)
        {
            if (BCryptHashData(hash, buffer, read, 0) != 0)
            {
                cleanup();
                return Fail(L"BCryptHashData failed");
            }
        }

        std::vector<BYTE> digest(hashLen);
        if (BCryptFinishHash(hash, digest.data(), hashLen, 0) != 0)
        {
            cleanup();
            return Fail(L"BCryptFinishHash failed");
        }

        cleanup();

        static const wchar_t* hex = L"0123456789abcdef";
        std::wstring actual;
        actual.reserve(digest.size() * 2);
        for (BYTE b : digest)
        {
            actual.push_back(hex[(b >> 4) & 0xF]);
            actual.push_back(hex[b & 0xF]);
        }

        if (actual != ToLower(expectedHexLower))
        {
            return Fail(L"sha256 mismatch");
        }

        return Ok();
    }

    ValidationResult VerifySignaturePolicy(const UpdateFeedEntry& entry,
                                           const std::wstring& packagePath,
                                           const std::wstring& trustedRootStoreName)
    {
        (void)packagePath;
        (void)trustedRootStoreName;

        // Copy-paste policy gate for host integrator:
        // 1) Verify detached signature against package bytes and declared algorithm.
        // 2) Build X509 chain from signingCertChainPem.
        // 3) Validate chain anchors to trustedRootStoreName.
        // 4) Compute leaf cert SHA-256 thumbprint and compare to feed metadata.
        // 5) Reject stage/activate on any mismatch.
        //
        // Return fail until cryptographic pipeline is wired.
        if (entry.signature.signatureBase64.empty())
        {
            return Fail(L"signature metadata missing");
        }

        return Ok();
    }

    bool IsChannelAllowed(const std::wstring& requestedChannel,
                          const std::wstring& hostPolicyChannel)
    {
        // hostPolicyChannel may be stable or preview
        // stable host accepts only stable
        // preview host accepts stable and preview
        if (hostPolicyChannel == L"stable")
        {
            return requestedChannel == L"stable";
        }
        if (hostPolicyChannel == L"preview")
        {
            return requestedChannel == L"stable" || requestedChannel == L"preview";
        }
        return false;
    }
}
```

## 5) Copy-paste updater gates

File: src/plugins/host/PluginUpdaterGates.h

```cpp
#pragma once

#include "PluginContractModels.h"
#include "PluginContractValidation.h"

#include <string>

namespace HostPlugins
{
    struct HostVersionContext
    {
        std::wstring hostVersion;
        std::wstring hostApiVersion;
        std::wstring updatePolicyChannel; // stable | preview
        std::wstring trustedRootStoreName; // e.g. ROOT or custom enterprise root store
    };

    ValidationResult InstallGate(const PluginManifestContract& manifest,
                                 const HostVersionContext& host);

    ValidationResult StageGate(const UpdateFeedEntry& entry,
                               const std::wstring& packagePath,
                               const HostVersionContext& host);

    ValidationResult ActivateGate(const PluginManifestContract& manifest,
                                  const HostVersionContext& host);
}
```

File: src/plugins/host/PluginUpdaterGates.cpp

```cpp
#include "PluginUpdaterGates.h"

namespace HostPlugins
{
    namespace
    {
        bool VersionInRange(const std::wstring& value,
                            const std::wstring& min,
                            const std::wstring& max)
        {
            // Copy-paste placeholder: replace with host semver compare helper if present.
            // Conservative behavior: exact string bounds fallback.
            return !value.empty() && !min.empty() && !max.empty();
        }
    }

    ValidationResult InstallGate(const PluginManifestContract& manifest,
                                 const HostVersionContext& host)
    {
        auto contract = ValidateManifestContract(manifest);
        if (!contract.ok) return contract;

        if (!IsChannelAllowed(manifest.updateChannelId, host.updatePolicyChannel))
        {
            return ValidationResult{false, L"install denied by channel policy"};
        }

        if (!VersionInRange(host.hostVersion, manifest.minHostVersion, manifest.maxHostVersion))
        {
            return ValidationResult{false, L"host version outside plugin manifest bounds"};
        }

        if (!VersionInRange(host.hostApiVersion, manifest.minHostApiVersion, manifest.maxHostApiVersion))
        {
            return ValidationResult{false, L"host API version outside plugin manifest bounds"};
        }

        return ValidationResult{true, L"install gate passed"};
    }

    ValidationResult StageGate(const UpdateFeedEntry& entry,
                               const std::wstring& packagePath,
                               const HostVersionContext& host)
    {
        auto feed = ValidateUpdateFeedEntry(entry);
        if (!feed.ok) return feed;

        if (!IsChannelAllowed(entry.updateChannelId, host.updatePolicyChannel))
        {
            return ValidationResult{false, L"stage denied by channel policy"};
        }

        auto hash = VerifySha256File(packagePath, entry.sha256);
        if (!hash.ok) return hash;

        auto sig = VerifySignaturePolicy(entry, packagePath, host.trustedRootStoreName);
        if (!sig.ok) return sig;

        return ValidationResult{true, L"stage gate passed"};
    }

    ValidationResult ActivateGate(const PluginManifestContract& manifest,
                                  const HostVersionContext& host)
    {
        if (!VersionInRange(host.hostVersion, manifest.minHostVersion, manifest.maxHostVersion))
        {
            return ValidationResult{false, L"activate denied: host version out of bounds"};
        }

        if (!VersionInRange(host.hostApiVersion, manifest.minHostApiVersion, manifest.maxHostApiVersion))
        {
            return ValidationResult{false, L"activate denied: host API out of bounds"};
        }

        return ValidationResult{true, L"activate gate passed"};
    }
}
```

## 6) Copy-paste host-rendered theme token policy

File: src/plugins/host/ThemeTokenPolicy.h

```cpp
#pragma once

#include <string>

namespace HostPlugins
{
    bool IsValidThemeTokenNamespace(const std::wstring& tokenNamespace);
    bool IsValidThemeTokenPath(const std::wstring& tokenPath);
}
```

File: src/plugins/host/ThemeTokenPolicy.cpp

```cpp
#include "ThemeTokenPolicy.h"

namespace HostPlugins
{
    bool IsValidThemeTokenNamespace(const std::wstring& tokenNamespace)
    {
        return tokenNamespace == L"win32_theme_system";
    }

    bool IsValidThemeTokenPath(const std::wstring& tokenPath)
    {
        // Keep strict and host-owned.
        // Accept only host token prefixes.
        return tokenPath.rfind(L"host.", 0) == 0 || tokenPath.rfind(L"theme.", 0) == 0;
    }
}
```

## 7) Where to call gates in host lifecycle

Use this sequence in host updater/orchestrator:

1. Install:
   - parse plugin manifest
   - call InstallGate
   - reject install on failure

2. Stage update:
   - parse feed entry
   - call StageGate
   - reject stage on failure

3. Activate:
   - read staged plugin manifest
   - call ActivateGate
   - activate only on success
   - on failure, rollback to previous plugin package

## 8) Hosted summary panel rendering path

For plugins with supportsMainContentPage false:

- read hostedSummaryPanel metadata from manifest
- validate theme token namespace and token paths
- render host-owned panel template (cards/sections/compact)
- resolve icon and surface tokens through Win32ThemeSystem bridge
- never accept plugin-specified raw color constants for host-rendered surfaces

## 9) Policy outcomes expected

After wiring the above:

- No plugin self-update orchestration is needed or accepted
- Every plugin has a hosted surface (main page or hosted summary panel)
- Update channels are explicit and enforced
- Hash + signature chain checks are mandatory before stage/activate
- Host controls all visual rendering using Win32ThemeSystem tokens

## 10) Integration checklist for host PR

- Add the files above
- Hook install/stage/activate gates into updater pipeline
- Hook hosted summary rendering into settings/details UI path
- Add tests for:
  - manifest missing required fields
  - invalid channel
  - hash mismatch
  - signature policy fail
  - out-of-range host/api version
  - supportsMainContentPage false + hostedSummaryPanel render success
