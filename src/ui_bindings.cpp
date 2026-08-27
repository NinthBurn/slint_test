#include "ui_bindings.h"

#include <iostream>
#include <memory>

#include "explorer.h"
#include "explorer/open_file.h"
#include "private/slint_models.h"
#include "private/slint_string.h"

namespace fs = std::filesystem;

void DisplayNotification(const slint::ComponentHandle<AppWindow>& ui,
                         const std::string& message) {
    std::cout << message << std::endl;
    ui->invoke_show_notification(slint::SharedString(message));
}

void UpdateFileModel(const FileExplorer& explorer,
                     std::shared_ptr<slint::VectorModel<FileEntry>>& model) {
    model->clear();

    for (const auto& entry : explorer.GetEntries()) {
        FileEntry file;
        file.name = entry.name;
        file.is_directory = entry.is_directory;

        model->push_back(file);
    }
}

std::shared_ptr<slint::VectorModel<slint::SharedString>> const
ToSlintStringVector(const std::vector<fs::path>& paths) {
    auto result = std::make_shared<slint::VectorModel<slint::SharedString>>();

    for (const auto& path : paths) {
        std::string name;

        if (path == path.root_path()) {
            name = path.root_name().string();
        } else {
            name = path.filename().string();
        }

        result->push_back(slint::SharedString(name));
    }

    return result;
}

void HandleNavigation(const slint::ComponentHandle<AppWindow>& ui,
                      const FileExplorer& explorer,
                      std::shared_ptr<slint::VectorModel<FileEntry>>& model,
                      ChangeDirResult result) {
    ui->set_search_path(
        slint::SharedString(explorer.GetCurrentPath().string()));
    ui->invoke_unfocus();

    switch (result) {
        case ChangeDirResult::Success:
            UpdateFileModel(explorer, model);
            ui->set_breadcrumb_steps(
                ToSlintStringVector(explorer.GetBreadcrumbPaths()));
            DisplayNotification(ui, "Changed directory");
            break;

        case ChangeDirResult::DoesNotExist:
            DisplayNotification(ui, "Directory does not exist");
            break;

        case ChangeDirResult::NotADirectory:
            DisplayNotification(ui, "Path is not a directory");
            break;

        case ChangeDirResult::PermissionDenied:
            DisplayNotification(ui, "Permission denied");
            break;

        case ChangeDirResult::NoParent:
            DisplayNotification(ui, "No parent");
            break;

        case ChangeDirResult::NoHistory:  // temporary?
            DisplayNotification(ui, "No existing history");
            break;

        case ChangeDirResult::NoChange:
            DisplayNotification(ui, "No change");
            break;

        default:
            DisplayNotification(ui, "Invalid operation");
            break;
    }
}

void AddHandlers(const slint::ComponentHandle<AppWindow>& ui,
                 FileExplorer& explorer,
                 std::shared_ptr<slint::VectorModel<FileEntry>>& model) {
    ui->on_entry_clicked([&](int id) {
        auto entries = explorer.GetEntries();
        const auto& entry = entries[id];

        if (entry.is_directory) {
            HandleNavigation(
                ui, explorer, model, explorer.ChangeDirectory(entry.path));
        } else {
            OpenFileResult result = OpenFile(entry.path);

            switch (result) {
                case OpenFileResult::Success:
                    break;

                case OpenFileResult::DoesNotExist:
                    DisplayNotification(ui, "File does not exist");
                    break;

                case OpenFileResult::NotAFile:
                    DisplayNotification(ui, "Not a file");
                    break;

                case OpenFileResult::Failed:
                    DisplayNotification(ui, "Could not open file");
                    break;
            }
        }
    });

    ui->on_go_back([&]() {
        HandleNavigation(ui, explorer, model, explorer.NavigateBack());
    });

    ui->on_go_forward([&]() {
        HandleNavigation(ui, explorer, model, explorer.NavigateForward());
    });

    ui->on_go_up([&]() {
        HandleNavigation(ui, explorer, model, explorer.NavigateUp());
    });

    ui->on_directory_changed([&](const slint::SharedString& path) {
        HandleNavigation(
            ui, explorer, model, explorer.ChangeDirectory(path.data()));
    });

    ui->on_breadcrumb_step_clicked([&](const int index) {
        HandleNavigation(
            ui, explorer, model, explorer.ChangeDirectoryToBreadcrumb(index));
    });

    ui->on_request_search_path([&]() {
        ui->set_search_path(
            slint::SharedString(explorer.GetCurrentPath().string()));
    });
}
