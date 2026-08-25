#include <iostream>
#include <memory>

#include "app-window.h"
#include "explorer.cpp"
#include "open_file.cpp"
#include "private/slint_models.h"
#include "private/slint_string.h"

void display_notification(const slint::ComponentHandle<AppWindow>& ui,
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

void HandleNavigation(const slint::ComponentHandle<AppWindow>& ui,
                      const FileExplorer& explorer,
                      std::shared_ptr<slint::VectorModel<FileEntry>>& model,
                      ChangeDirResult result) {
    switch (result) {
        case ChangeDirResult::Success:
            UpdateFileModel(explorer, model);
            ui->set_search_path(
                slint::SharedString(explorer.GetCurrentPath().string()));
            display_notification(ui, "Changed directory");
            break;

        case ChangeDirResult::DoesNotExist:
            display_notification(ui, "Directory does not exist");
            break;

        case ChangeDirResult::NotADirectory:
            display_notification(ui, "Path is not a directory");
            break;

        case ChangeDirResult::PermissionDenied:
            display_notification(ui, "Permission denied");
            break;

        case ChangeDirResult::NoParent:
            display_notification(ui, "No parent");
            break;

        case ChangeDirResult::NoHistory:  // temporary?
            display_notification(ui, "No existing history");
            break;

        default:
            display_notification(ui, "Invalid operation");
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
                    display_notification(ui, "File does not exist");
                    break;

                case OpenFileResult::NotAFile:
                    display_notification(ui, "Not a file");
                    break;

                case OpenFileResult::Failed:
                    display_notification(ui, "Could not open file");
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
}

int main(int argc, char** argv) {
    auto ui = AppWindow::create();
    auto model = std::make_shared<slint::VectorModel<FileEntry>>();
    FileExplorer explorer;

    UpdateFileModel(explorer, model);
    ui->set_files(model);
    AddHandlers(ui, explorer, model);

    ui->run();
}
