#pragma once
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
    NoHistory
};

namespace fs = std::filesystem;

class FileExplorer {
private:
    fs::path current_directory;
    std::vector<fs::path> back_history;
    std::vector<fs::path> forward_history;

public:
    FileExplorer() : current_directory(fs::current_path()) {}

    ChangeDirResult ChangeDirectory(const fs::path& path) {
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

            back_history.push_back(current_directory);
            current_directory = new_directory;
            forward_history.clear();

            return ChangeDirResult::Success;
        } catch (const fs::filesystem_error&) {
            return ChangeDirResult::PermissionDenied;
        }
    }

    const fs::path& GetCurrentPath() const {
        return current_directory;
    }

    std::vector<DirectoryEntry> GetEntries() const {
        std::vector<DirectoryEntry> entries;

        for (const auto& entry : fs::directory_iterator(current_directory)) {
            entries.push_back({.path = entry.path(),
                               .name = entry.path().filename().string(),
                               .is_directory = entry.is_directory()});
        }

        return entries;
    }

    ChangeDirResult NavigateBack() {
        if (back_history.empty()) {
            return ChangeDirResult::NoHistory;
        }

        forward_history.push_back(current_directory);
        current_directory = back_history.back();
        back_history.pop_back();

        return ChangeDirResult::Success;
    }

    ChangeDirResult NavigateForward() {
        if (forward_history.empty()) {
            return ChangeDirResult::NoHistory;
        }

        back_history.push_back(current_directory);
        current_directory = forward_history.back();
        forward_history.pop_back();

        return ChangeDirResult::Success;
    }

    ChangeDirResult NavigateUp() {
        fs::path parent = current_directory.parent_path();

        if (parent == current_directory) {
            return ChangeDirResult::NoParent;
        }

        return ChangeDirectory(parent);
    }
};