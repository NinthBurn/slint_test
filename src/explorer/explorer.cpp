#include "explorer.h"
namespace fs = std::filesystem;

FileExplorer::FileExplorer() : current_directory(fs::current_path()) {}
FileExplorer::FileExplorer(fs::path starting_path) : current_directory(starting_path) {}

ChangeDirResult FileExplorer::ChangeDirectory(const fs::path& path) {
    try {
        fs::path new_directory =
            path.is_absolute() ? path : current_directory / path;

        new_directory = fs::weakly_canonical(new_directory);

        if (!fs::exists(new_directory)) {
            return ChangeDirResult::DoesNotExist;
        }

        if (!fs::is_directory(new_directory)) {
            return ChangeDirResult::NotADirectory;
        }

        if (current_directory == new_directory || new_directory.empty()) {
            return ChangeDirResult::NoChange;
        }

        back_history.push_back(current_directory);
        current_directory = new_directory;
        forward_history.clear();

        return ChangeDirResult::Success;
    } catch (const fs::filesystem_error&) {
        return ChangeDirResult::PermissionDenied;
    }
}

const fs::path& FileExplorer::GetCurrentPath() const {
    return current_directory;
}

std::vector<DirectoryEntry> FileExplorer::GetEntries() const {
    std::vector<DirectoryEntry> entries;

    for (const auto& entry : fs::directory_iterator(current_directory)) {
        entries.push_back({.path = entry.path(),
                           .name = entry.path().filename().string(),
                           .is_directory = entry.is_directory()});
    }

    return entries;
}

ChangeDirResult FileExplorer::NavigateBack() {
    if (back_history.empty()) {
        return ChangeDirResult::NoHistory;
    }

    forward_history.push_back(current_directory);
    current_directory = back_history.back();
    back_history.pop_back();

    return ChangeDirResult::Success;
}

ChangeDirResult FileExplorer::NavigateForward() {
    if (forward_history.empty()) {
        return ChangeDirResult::NoHistory;
    }

    back_history.push_back(current_directory);
    current_directory = forward_history.back();
    forward_history.pop_back();

    return ChangeDirResult::Success;
}

ChangeDirResult FileExplorer::NavigateUp() {
    fs::path parent = current_directory.parent_path();

    if (parent == current_directory) {
        return ChangeDirResult::NoParent;
    }

    return ChangeDirectory(parent);
}

std::vector<fs::path> FileExplorer::GetBreadcrumbPaths() const {
    std::vector<fs::path> paths;

    fs::path current = current_directory;

    while (current.has_parent_path() && current != current.parent_path()) {
        paths.push_back(current);
        current = current.parent_path();
    }

    paths.push_back(current);

    std::reverse(paths.begin(), paths.end());

    return paths;
}

ChangeDirResult FileExplorer::ChangeDirectoryToBreadcrumb(size_t index) {
    auto paths = GetBreadcrumbPaths();

    if (index >= paths.size()) {
        return ChangeDirResult::DoesNotExist;
    }

    return ChangeDirectory(paths[index]);
}