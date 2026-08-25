#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

struct DirectoryEntry {
    fs::path path;
    std::string name;
    bool is_directory;
};

enum class ChangeDirResult {
    Success,
    DoesNotExist,
    NotADirectory,
    PermissionDenied
};

class FileExplorer {
private:
    fs::path current_directory;

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

            current_directory = new_directory;
            return ChangeDirResult::Success;
        } catch (const fs::filesystem_error&) {
            return ChangeDirResult::PermissionDenied;
        }
    }

    const fs::path& GetCurrentDirectory() const {
        return current_directory;
    }

    std::vector<DirectoryEntry> GetEntries() const {
        std::vector<DirectoryEntry> entries;

        for (const auto& entry : fs::directory_iterator(current_directory)) {
            DirectoryEntry file;

            file.path = entry.path();
            file.name = entry.path().filename().string();
            file.is_directory = entry.is_directory();

            entries.push_back(file);
        }

        return entries;
    }
};