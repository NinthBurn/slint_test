#pragma once

#include <filesystem>
#include <vector>

enum class ChangeDirResult {
    Success,
    DoesNotExist,
    NotADirectory,
    PermissionDenied,
    NoParent,
    NoHistory,
    NoChange
};

class DirectoryTab {
private:
    std::filesystem::path current_directory;
    std::vector<std::filesystem::path> back_history;
    std::vector<std::filesystem::path> forward_history;

public:
    DirectoryTab();
    DirectoryTab(std::filesystem::path starting_path);

    const std::filesystem::path& GetCurrentPath() const;

    ChangeDirResult ChangeDirectory(const std::filesystem::path& path);
    ChangeDirResult NavigateBack();
    ChangeDirResult NavigateForward();
    ChangeDirResult NavigateUp();

    std::vector<std::filesystem::path> GetBreadcrumbPaths() const;
    ChangeDirResult ChangeDirectoryToBreadcrumb(size_t index);

    std::string GetTitle() const;
};