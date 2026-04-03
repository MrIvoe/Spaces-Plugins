#pragma once

#include <filesystem>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace SampleFsHelpers
{
    inline std::filesystem::path BuildUniquePath(const std::filesystem::path& target)
    {
        if (!std::filesystem::exists(target))
        {
            return target;
        }

        const std::filesystem::path stem = target.stem();
        const std::filesystem::path ext = target.extension();
        const std::filesystem::path dir = target.parent_path();
        for (int i = 1; i < 1000; ++i)
        {
            const std::filesystem::path candidate = dir /
                (stem.wstring() + L" (" + std::to_wstring(i) + L")" + ext.wstring());
            if (!std::filesystem::exists(candidate))
            {
                return candidate;
            }
        }

        return target;
    }

    inline bool IsHiddenPath(const std::filesystem::path& path)
    {
#ifdef _WIN32
        const auto attrs = GetFileAttributesW(path.c_str());
        return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_HIDDEN) != 0;
#else
        (void)path;
        return false;
#endif
    }
}
