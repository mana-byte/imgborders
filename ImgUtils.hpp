#pragma once

#include <hyprland/src/render/Texture.hpp>

namespace ImgUtils {
SP<CTexture> load(const std::string &filename);

SP<CTexture> sliceTexture(SP<CTexture> tex, const CBox &box);
} // namespace ImgUtils
