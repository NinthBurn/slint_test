#include "app-window.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void get_files_in_directory(
    const fs::path& path,
    const std::shared_ptr<slint::VectorModel<FileEntry>>& model
) {
    for (const auto& entry : fs::directory_iterator(path)) {
        FileEntry file;
        auto filename = entry.path().filename().string();
        std::cout << filename << " is_dir: " << entry.is_directory() << '\n';
        
        file.name = filename;
        file.is_directory = entry.is_directory();

        model->push_back(file);
    }
}

void display_notification(
    const slint::ComponentHandle<AppWindow>& ui,
    const std::string& message
) {
    std::cout << message << std::endl;
    ui->invoke_show_notification(slint::SharedString(message));
}

void change_directory(
    const slint::SharedString& path,
    const std::shared_ptr<slint::VectorModel<FileEntry>>& model,
    const slint::ComponentHandle<AppWindow>& ui
) {
    fs::path directory(path.data());
    std::cout << "Changing directory to: " << directory << std::endl;

    if (!fs::exists(directory)) {
        display_notification(ui, "Directory does not exist");
        return;
    }

    if (!fs::is_directory(directory)) {
        display_notification(ui, "Path is not a directory");
        return;
    }

    model->clear();
    get_files_in_directory(directory, model);
    display_notification(ui, "Changed directory");
}

int main(int argc, char **argv)
{
    auto ui = AppWindow::create();
    auto model = std::make_shared<slint::VectorModel<FileEntry>>();
    
    get_files_in_directory(".", model);
    ui->on_directory_changed([&](const slint::SharedString& path) {
            change_directory(path, model, ui);
        }
    );

    ui->set_files(model);
    ui->run();
}
