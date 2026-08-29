#pragma once

#include <filesystem>
#include <vector>

#include "directory_tab.h"

struct DirectoryEntry {
    std::filesystem::path path;
    std::string name;
    bool is_directory;
};

class FileExplorer {
private:
    std::vector<DirectoryTab> tabs;
    size_t current_tab_idx;

public:
    FileExplorer();
    FileExplorer(std::filesystem::path starting_path);

    // = = = Tab Management = = = //
    const DirectoryTab& GetCurrentTab() const;
    DirectoryTab& GetCurrentTab();
    size_t GetCurrentTabIndex() const;
    size_t GetTabCount() const;
    const std::vector<DirectoryTab>& GetTabs() const;

    void CreateTab();
    void CreateTab(const std::filesystem::path& starting_path);
    bool CloseTab(size_t index);
    bool SelectTab(size_t index);

    // = = = = Navigation = = = = //
    const std::filesystem::path& GetCurrentPath() const;
    std::vector<DirectoryEntry> GetEntries() const;

    ChangeDirResult ChangeDirectory(const std::filesystem::path& path);
    ChangeDirResult NavigateBack();
    ChangeDirResult NavigateForward();
    ChangeDirResult NavigateUp();

    std::vector<std::filesystem::path> GetBreadcrumbPaths() const;
    ChangeDirResult ChangeDirectoryToBreadcrumb(size_t index);
};