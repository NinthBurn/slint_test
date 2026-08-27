#include <memory>

#include "app-window.h"
#include "explorer/explorer.h"
#include "private/slint_models.h"
#include "ui_bindings.h"

int main(int argc, char** argv) {
    auto ui = AppWindow::create();
    auto model = std::make_shared<slint::VectorModel<FileEntry>>();
    FileExplorer explorer;

    ui->set_files(model);
    HandleNavigation(ui, explorer, model, ChangeDirResult::Success);
    AddHandlers(ui, explorer, model);

    ui->run();
}
