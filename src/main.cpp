#include <iostream>
#include <memory>

#include "app-window.h"
#include "explorer.cpp"
#include "private/slint_models.h"

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

int main(int argc, char** argv) {
    auto ui = AppWindow::create();
    auto model = std::make_shared<slint::VectorModel<FileEntry>>();
    FileExplorer explorer;

    UpdateFileModel(explorer, model);
    ui->set_files(model);

    ui->on_directory_changed([&](const slint::SharedString& path) {
        auto result = explorer.ChangeDirectory(path.data());

        switch (result) {
            case ChangeDirResult::Success:
                UpdateFileModel(explorer, model);
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
        }
    });

    ui->run();
}
