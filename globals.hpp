#pragma once

#include <hyprland/src/plugins/PluginAPI.hpp>

// Plugin API handle
inline HANDLE PHANDLE = nullptr;

// Class defined elsewhere
class CImgBorder;

struct SGlobalState {
  std::vector<WP<CImgBorder>> borders;
};
inline UP<SGlobalState> g_pGlobalState;
