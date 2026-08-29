#include "ui_bindings.h"

#include <iostream>
#include <memory>

#include "app-window.h"
#include "explorer.h"
#include "explorer/open_file.h"
#include "private/slint_models.h"
#include "private/slint_string.h"

namespace fs = std::filesystem;
using slint::SharedString;
using slint::VectorModel;
using std::make_shared;

UIController::UIController(const slint::ComponentHandle<AppWindow>& ui)
    : ui(ui) {
    fileListModel = make_shared<VectorModel<FileEntry>>();
    breadcrumbModel = make_shared<VectorModel<SharedString>>();
    tabModel = make_shared<VectorModel<TabInfo>>();

    ui->set_files(fileListModel);
    ui->set_breadcrumb_steps(breadcrumbModel);
    ui->set_tabs(tabModel);

    HandleNavigation(ChangeDirResult::Success);
    AddHandlers();
}
void UIController::DisplayNotification(const std::string& message) {
    std::cout << message << std::endl;
    ui->invoke_show_notification(SharedString(message));
}

void UIController::UpdateFileModel() const {
    fileListModel->clear();

    for (const auto& entry : explorer.GetEntries()) {
        FileEntry file;
        file.name = entry.name;
        file.is_directory = entry.is_directory;

        fileListModel->push_back(file);
    }
}

void UIController::UpdateBreadcrumbModel(
    const std::vector<fs::path>& paths) const {
    breadcrumbModel->clear();

    for (const auto& path : paths) {
        std::string name;

        if (path == path.root_path()) {
            name = path.root_name().string();
        } else {
            name = path.filename().string();
        }

        breadcrumbModel->push_back(SharedString(name));
    }
}

void UIController::UpdateTabModel() const {
    tabModel->clear();

    const auto& tabs = explorer.GetTabs();
    const auto current = explorer.GetCurrentTabIndex();

    for (size_t i = 0; i < tabs.size(); ++i) {
        TabInfo info;
        info.title = tabs[i].GetTitle();
        info.active = i == current;

        tabModel->push_back(info);
    }
}

void UIController::HandleNavigation(ChangeDirResult result) {
    ui->set_search_path(SharedString(explorer.GetCurrentPath().string()));
    ui->invoke_unfocus_path_edit();

    switch (result) {
        case ChangeDirResult::Success:
            RefreshExplorerState();
            DisplayNotification("Changed directory");
            break;

        case ChangeDirResult::DoesNotExist:
            DisplayNotification("Directory does not exist");
            break;

        case ChangeDirResult::NotADirectory:
            DisplayNotification("Path is not a directory");
            break;

        case ChangeDirResult::PermissionDenied:
            DisplayNotification("Permission denied");
            break;

        case ChangeDirResult::NoParent:
            DisplayNotification("No parent");
            break;

        case ChangeDirResult::NoHistory:  // temporary?
            DisplayNotification("No existing history");
            break;

        case ChangeDirResult::NoChange:
            DisplayNotification("No change");
            break;

        default:
            DisplayNotification("Invalid operation");
            break;
    }
}

void UIController::RefreshExplorerState() const {
    UpdateFileModel();
    UpdateBreadcrumbModel(explorer.GetBreadcrumbPaths());
    UpdateTabModel();
}

void UIController::AddHandlers() {
    ui->on_entry_clicked([&](int id) {
        auto entries = explorer.GetEntries();
        const auto& entry = entries[id];

        if (entry.is_directory) {
            HandleNavigation(explorer.ChangeDirectory(entry.path));
        } else {
            OpenFileResult result = OpenFile(entry.path);

            switch (result) {
                case OpenFileResult::Success:
                    break;

                case OpenFileResult::DoesNotExist:
                    DisplayNotification("File does not exist");
                    break;

                case OpenFileResult::NotAFile:
                    DisplayNotification("Not a file");
                    break;

                case OpenFileResult::Failed:
                    DisplayNotification("Could not open file");
                    break;
            }
        }
    });

    //// = = = = Navigation Controls = = = = ////
    ui->on_go_back([&]() { HandleNavigation(explorer.NavigateBack()); });

    ui->on_go_forward([&]() { HandleNavigation(explorer.NavigateForward()); });

    ui->on_go_up([&]() { HandleNavigation(explorer.NavigateUp()); });

    ui->on_directory_changed([&](const SharedString& path) {
        HandleNavigation(explorer.ChangeDirectory(path.data()));
    });

    ui->on_breadcrumb_step_clicked([&](const int index) {
        HandleNavigation(explorer.ChangeDirectoryToBreadcrumb(index));
    });

    ui->on_request_search_path([&]() {
        ui->set_search_path(SharedString(explorer.GetCurrentPath().string()));
    });

    //// = = = = Tab Controls = = = = ////
    ui->on_tab_clicked([&](const int index) {
        explorer.SelectTab(index);
        RefreshExplorerState();
    });

    ui->on_new_tab([&]() {
        explorer.CreateTab(explorer.GetCurrentPath());
        RefreshExplorerState();
    });

    ui->on_close_tab([&](const int index) {
        explorer.CloseTab(index);
        RefreshExplorerState();
    });
}
