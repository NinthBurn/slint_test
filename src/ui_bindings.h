#pragma once

#include "app-window.h"
#include "explorer/explorer.h"
#include "private/slint_models.h"

void display_notification(const slint::ComponentHandle<AppWindow>& ui,
                          const std::string& message);

void UpdateFileModel(const FileExplorer& explorer,
                     std::shared_ptr<slint::VectorModel<FileEntry>>& model);

void HandleNavigation(const slint::ComponentHandle<AppWindow>& ui,
                      const FileExplorer& explorer,
                      std::shared_ptr<slint::VectorModel<FileEntry>>& model,
                      ChangeDirResult result);

void AddHandlers(const slint::ComponentHandle<AppWindow>& ui,
                 FileExplorer& explorer,
                 std::shared_ptr<slint::VectorModel<FileEntry>>& model);
