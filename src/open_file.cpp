#pragma once
#include <filesystem>

namespace fs = std::filesystem;

enum class OpenFileResult {
    Success,
    DoesNotExist,
    NotAFile,
    Failed
};

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <cstdlib>
#endif

OpenFileResult OpenFile(const fs::path& path)
{
    if (!fs::exists(path)) {
        return OpenFileResult::DoesNotExist;
    }

    if (!fs::is_regular_file(path)) {
        return OpenFileResult::NotAFile;
    }

#ifdef _WIN32
    auto result = ShellExecuteW(
        nullptr,
        L"open",
        path.c_str(),
        nullptr,
        nullptr,
        SW_SHOWNORMAL
    );

    if (reinterpret_cast<std::intptr_t>(result) <= 32) {
        return OpenFileResult::Failed;
    }

    return OpenFileResult::Success;

#elif defined(__linux__)
    std::string command = "xdg-open " + std::quoted(path.string());
    int result = std::system(command.c_str());

    if (result != 0) {
        return OpenFileResult::Failed;
    }

    return OpenFileResult::Success;
#else
    return OpenFileResult::Failed;
#endif
}