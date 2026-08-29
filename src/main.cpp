#include "app-window.h"
#include "ui_bindings.h"

int main(int argc, char** argv) {
    auto ui = AppWindow::create();
    UIController controller(ui);
    ui->run();
}
