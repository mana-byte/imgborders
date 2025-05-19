#pragma once

#include <hyprland/src/render/Texture.hpp>

namespace ImgLoader {
SP<CTexture> load(const std::string &filename);

SP<CTexture> sliceTexture(SP<CTexture> tex, const CBox &box);
} // namespace ImgLoader
