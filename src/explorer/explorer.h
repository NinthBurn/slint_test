#pragma once
#include <algorithm>
#include <filesystem>
#include <vector>

struct DirectoryEntry {
    std::filesystem::path path;
    std::string name;
    bool is_directory;
};

enum class ChangeDirResult {
    Success,
    DoesNotExist,
    NotADirectory,
    PermissionDenied,
    NoParent,
    NoHistory,
    NoChange
};

class FileExplorer {
private:
    std::filesystem::path current_directory;
    std::vector<std::filesystem::path> back_history;
    std::vector<std::filesystem::path> forward_history;

public:
    FileExplorer();
    FileExplorer(std::filesystem::path starting_path);

    const std::filesystem::path& GetCurrentPath() const;
    std::vector<DirectoryEntry> GetEntries() const;

    ChangeDirResult ChangeDirectory(const std::filesystem::path& path);
    ChangeDirResult NavigateBack();
    ChangeDirResult NavigateForward();
    ChangeDirResult NavigateUp();

    std::vector<std::filesystem::path> GetBreadcrumbPaths() const;
    ChangeDirResult ChangeDirectoryToBreadcrumb(size_t index);
};