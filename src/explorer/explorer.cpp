#include "explorer.h"
namespace fs = std::filesystem;

FileExplorer::FileExplorer() : current_tab_idx(0) {
    tabs.emplace_back();
}

FileExplorer::FileExplorer(fs::path starting_path) : current_tab_idx(0) {
    tabs.emplace_back(std::move(starting_path));
}

// = = = Tab Management = = = //
const DirectoryTab& FileExplorer::GetCurrentTab() const {
    return tabs[current_tab_idx];
}

DirectoryTab& FileExplorer::GetCurrentTab() {
    return tabs[current_tab_idx];
}

size_t FileExplorer::GetCurrentTabIndex() const {
    return current_tab_idx;
}

size_t FileExplorer::GetTabCount() const {
    return tabs.size();
}

const std::vector<DirectoryTab>& FileExplorer::GetTabs() const {
    return tabs;
}

void FileExplorer::CreateTab() {
    tabs.emplace_back();
    current_tab_idx = tabs.size() - 1;
}

void FileExplorer::CreateTab(const fs::path& starting_path) {
    tabs.emplace_back(starting_path);
    current_tab_idx = tabs.size() - 1;
}

bool FileExplorer::SelectTab(size_t index) {
    if (index >= tabs.size()) {
        return false;
    }

    current_tab_idx = index;

    return true;
}

bool FileExplorer::CloseTab(size_t index) {
    if (index >= tabs.size() || tabs.size() == 1) {
        return false;
    }

    tabs.erase(tabs.begin() + index);

    if (current_tab_idx > index) {
        --current_tab_idx;
    } else if (current_tab_idx >= tabs.size()) {
        current_tab_idx = tabs.size() - 1;
    }

    return true;
}

// = = = = Navigation = = = = //

ChangeDirResult FileExplorer::ChangeDirectory(const fs::path& path) {
    return tabs[current_tab_idx].ChangeDirectory(path);
}

const fs::path& FileExplorer::GetCurrentPath() const {
    return tabs[current_tab_idx].GetCurrentPath();
}

ChangeDirResult FileExplorer::NavigateBack() {
    return tabs[current_tab_idx].NavigateBack();
}

ChangeDirResult FileExplorer::NavigateForward() {
    return tabs[current_tab_idx].NavigateForward();
}

ChangeDirResult FileExplorer::NavigateUp() {
    return tabs[current_tab_idx].NavigateUp();
}

std::vector<fs::path> FileExplorer::GetBreadcrumbPaths() const {
    return tabs[current_tab_idx].GetBreadcrumbPaths();
}

ChangeDirResult FileExplorer::ChangeDirectoryToBreadcrumb(size_t index) {
    return tabs[current_tab_idx].ChangeDirectoryToBreadcrumb(index);
}

std::vector<DirectoryEntry> FileExplorer::GetEntries() const {
    std::vector<DirectoryEntry> entries;

    for (const auto& entry : fs::directory_iterator(GetCurrentPath())) {
        entries.push_back({.path = entry.path(),
                           .name = entry.path().filename().string(),
                           .is_directory = entry.is_directory()});
    }

    return entries;
}