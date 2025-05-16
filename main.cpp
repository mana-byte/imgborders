#include <src/debug/Log.hpp>
#include <string>
#define WLR_USE_UNSTABLE
#include "ImgBorder.hpp"
#include "globals.hpp"
#include <any>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprutils/memory/UniquePtr.hpp>
#include <unistd.h>

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

static void onOpenWindow(void *self, std::any data) {
  Debug::log(INFO, "letsgo");

  // Data is guaranteed
  const auto PWINDOW = std::any_cast<PHLWINDOW>(data);

  Debug::log(INFO, "letsgo2");

  // Create border and apply it
  auto border = makeUnique<CImgBorder>(PWINDOW);
  // g_pGlobalState->borders.emplace_back(border);
  HyprlandAPI::addWindowDecoration(PHANDLE, PWINDOW, std::move(border));

  HyprlandAPI::addNotification(PHANDLE, "[imgborders] added border",
                               CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
  Debug::log(INFO, "letsgo3");
}

static void onCloseWindow(void *self, std::any data) {
  // Data is guaranteed
  const auto PWINDOW = std::any_cast<PHLWINDOW>(data);

  HyprlandAPI::addNotification(PHANDLE, "[imgborders] window closed",
                               CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;

  // Make sure we're running against the correct Hyprland version
  const std::string HASH = __hyprland_api_get_hash();
  if (HASH != GIT_COMMIT_HASH) {
    HyprlandAPI::addNotification(PHANDLE,
                                 "[imgborders] Failure in initialization: "
                                 "Version mismatch (headers ver "
                                 "is not equal to running hyprland ver)",
                                 CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
    throw std::runtime_error("[imgborders] Version mismatch");
  }

  // Register callbacks
  static auto openWindow = HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "openWindow",
      [&](void *self, SCallbackInfo &info, std::any data) {
        Debug::log(INFO, "letsgo0");
        onOpenWindow(self, data);
      });
  static auto closeWindow = HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "closeWindow",
      [&](void *self, SCallbackInfo &info, std::any data) {
        Debug::log(INFO, "closewin");
        onCloseWindow(self, data);
      });

  // Yay let's go
  Debug::log(INFO, "init'd");
  HyprlandAPI::addNotification(PHANDLE,
                               "[imgborders] Initialized successfully!",
                               CHyprColor{0.2, 1.0, 0.2, 1.0}, 5000);
  return {"imgborders", DESCRIPTION, "zacoons", VERSION};
}

APICALL EXPORT void PLUGIN_EXIT() {
  // ...
}
