#pragma once
#include <filesystem>


enum class OpenFileResult { Success, DoesNotExist, NotAFile, Failed };

OpenFileResult OpenFile(const std::filesystem::path& path);