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

bool CImgBorderPassElement::needsLiveBlur() { return data.deco->shouldBlur(); }

bool CImgBorderPassElement::needsPrecomputeBlur() { return false; }

std::optional<CBox> CImgBorderPassElement::boundingBox() {
  return std::optional{data.deco->getGlobalBoundingBox(
      g_pHyprOpenGL->m_renderData.pMonitor.lock())};
}
