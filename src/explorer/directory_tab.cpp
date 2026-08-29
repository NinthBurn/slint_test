#include "directory_tab.h"

#include <algorithm>


namespace fs = std::filesystem;

DirectoryTab::DirectoryTab() : current_directory(fs::current_path()) {}
DirectoryTab::DirectoryTab(fs::path starting_path)
    : current_directory(starting_path) {}

ChangeDirResult DirectoryTab::ChangeDirectory(const fs::path& path) {
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

const fs::path& DirectoryTab::GetCurrentPath() const {
    return current_directory;
}

ChangeDirResult DirectoryTab::NavigateBack() {
    if (back_history.empty()) {
        return ChangeDirResult::NoHistory;
    }

    forward_history.push_back(current_directory);
    current_directory = back_history.back();
    back_history.pop_back();

    return ChangeDirResult::Success;
}

ChangeDirResult DirectoryTab::NavigateForward() {
    if (forward_history.empty()) {
        return ChangeDirResult::NoHistory;
    }

    back_history.push_back(current_directory);
    current_directory = forward_history.back();
    forward_history.pop_back();

    return ChangeDirResult::Success;
}

ChangeDirResult DirectoryTab::NavigateUp() {
    fs::path parent = current_directory.parent_path();

    if (parent == current_directory) {
        return ChangeDirResult::NoParent;
    }

    return ChangeDirectory(parent);
}

std::vector<fs::path> DirectoryTab::GetBreadcrumbPaths() const {
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

ChangeDirResult DirectoryTab::ChangeDirectoryToBreadcrumb(size_t index) {
    auto paths = GetBreadcrumbPaths();

    if (index >= paths.size()) {
        return ChangeDirResult::DoesNotExist;
    }

    return ChangeDirectory(paths[index]);
}

std::string DirectoryTab::GetTitle() const {
    const auto& path = current_directory;

    if (path == path.root_path()) {
        return path.root_path().string();
    }

    return path.filename().string();
}