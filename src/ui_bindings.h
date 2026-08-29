#pragma once

#include <memory>

#include "app-window.h"
#include "explorer/explorer.h"
#include "private/slint_item_tree.h"
#include "private/slint_models.h"
#include "private/slint_string.h"

class UIController {
private:
    FileExplorer explorer;

    const slint::ComponentHandle<AppWindow>& ui;
    std::shared_ptr<slint::VectorModel<FileEntry>> fileListModel;
    std::shared_ptr<slint::VectorModel<slint::SharedString>> breadcrumbModel;
    std::shared_ptr<slint::VectorModel<TabInfo>> tabModel;

    void HandleNavigation(ChangeDirResult result);
    void AddHandlers();

    void DisplayNotification(const std::string& message);
    void UpdateFileModel() const;
    void UpdateBreadcrumbModel(
        const std::vector<std::filesystem::path>& paths) const;
    void UpdateTabModel() const;
    void RefreshExplorerState() const;

public:
    UIController(const slint::ComponentHandle<AppWindow>& ui);
};