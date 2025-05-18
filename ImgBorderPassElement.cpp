#include "ImgBorderPassElement.hpp"
#include "ImgBorder.hpp"
#include <hyprland/src/render/OpenGL.hpp>

CImgBorderPassElement::CImgBorderPassElement(
    const CImgBorderPassElement::SData &data_) {
  data = data_;
}
CImgBorderPassElement::~CImgBorderPassElement() {}

void CImgBorderPassElement::draw(const CRegion &damage) {
  data.deco->drawPass(g_pHyprOpenGL->m_renderData.pMonitor.lock(), data.a);
}

bool CImgBorderPassElement::needsLiveBlur() { return false; }

bool CImgBorderPassElement::needsPrecomputeBlur() { return false; }
