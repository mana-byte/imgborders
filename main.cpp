#include "ImgBorder.hpp"
// #include "ImgBorderPassElement.hpp"
#include "globals.hpp"
#include <any>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprutils/memory/UniquePtr.hpp>
#include <string>

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

static void onOpenWindow(void *self, std::any data) {
  try {
    // Data is guaranteed
    const auto PWINDOW = std::any_cast<PHLWINDOW>(data);

    if (!PWINDOW->m_X11DoesntWantBorders) {
      // Make sure we haven't added the border already
      if (std::ranges::any_of(PWINDOW->m_windowDecorations, [](const auto &d) {
            return d->getDisplayName() == DISPLAY_NAME;
          }))
        return;

      // Create border and apply it
      auto border = makeUnique<CImgBorder>(PWINDOW);
      border->m_self = border;
      g_pGlobalState->borders.emplace_back(border);
      HyprlandAPI::addWindowDecoration(PHANDLE, PWINDOW, std::move(border));

      HyprlandAPI::addNotification(PHANDLE, "[imgborders] added border",
                                   CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
    }

  } catch (std::exception &e) {
    HyprlandAPI::addNotification(PHANDLE, "[imgborders] broked",
                                 CHyprColor{1.0, 0.2, 0.2, 1.0}, 1000);
  }
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
  // ALWAYS add this to your plugins. It will prevent random crashes coming from
  // mismatched header versions.
  const std::string HASH = __hyprland_api_get_hash();
  if (HASH != GIT_COMMIT_HASH) {
    HyprlandAPI::addNotification(PHANDLE,
                                 "[imgborders] Failure in initialization: "
                                 "Version mismatch (headers ver "
                                 "is not equal to running hyprland ver)",
                                 CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
    throw std::runtime_error("[imgborders] Version mismatch");
  }

  g_pGlobalState = makeUnique<SGlobalState>();

  // Register callbacks
  static auto openWindow = HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "openWindow",
      [&](void *self, SCallbackInfo &info, std::any data) {
        onOpenWindow(self, data);
      });
  static auto closeWindow = HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "closeWindow",
      [&](void *self, SCallbackInfo &info, std::any data) {
        onCloseWindow(self, data);
      });

  // Yay let's go
  HyprlandAPI::addNotification(PHANDLE,
                               "[imgborders] Initialized successfully!",
                               CHyprColor{0.2, 1.0, 0.2, 1.0}, 5000);
  return {"imgborders", "Add image borders to windows.", "zacoons", VERSION};
}

APICALL EXPORT void PLUGIN_EXIT() {
  for (auto &m : g_pCompositor->m_monitors)
    m->m_scheduledRecalc = true;

  // g_pHyprRenderer->m_renderPass.removeAllOfType(PASS_NAME);
}
