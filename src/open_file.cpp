#pragma once
#include <filesystem>

namespace fs = std::filesystem;

enum class OpenFileResult { Success, DoesNotExist, NotAFile, Failed };

#ifdef _WIN32
#include <windows.h>
OpenFileResult OpenFilePlatf(const fs::path& path) {
    auto result = ShellExecuteW(
        nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

    if (reinterpret_cast<std::intptr_t>(result) <= 32) {
        return OpenFileResult::Failed;
    }

    return OpenFileResult::Success;
}
#elif defined(__linux__)
#include <cstdlib>
OpenFileResult OpenFilePlatf(const fs::path& path) {
    std::ostringstream command;
    command << "xdg-open " << std::quoted(path.string());
    int result = std::system(command.str().c_str());
    if (result == 2) {
        return OpenFileResult::NotAFile;
    } else {
        return result == 0 ? OpenFileResult::Success : OpenFileResult::Failed;
    }
}
#else
OpenFileResult OpenFilePlatf(const fs::path& path) {
    return OpenFileResult::Failed;
}
#endif

OpenFileResult OpenFile(const fs::path& path) {
    if (!fs::exists(path)) {
        return OpenFileResult::DoesNotExist;
    }

    if (!fs::is_regular_file(path)) {
        return OpenFileResult::NotAFile;
    }

    return OpenFilePlatf(path);
}