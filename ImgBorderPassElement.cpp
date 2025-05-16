#include "ImgBorderPassElement.hpp"

CImgBorderPassElement::CImgBorderPassElement(
    const CImgBorderPassElement::SData &data_) {
  data = data_;
}

const char *CImgBorderPassElement::passName() {
  return "CImgBorderPassElement";
}

bool CImgBorderPassElement::needsLiveBlur() { return false; }

bool CImgBorderPassElement::needsPrecomputeBlur() { return false; }

void CImgBorderPassElement::draw(const CRegion &damage) {
  // TODO
  // data.deco->drawPass(g_pHyprOpenGL->m_renderData.pMonitor.lock(), data.a);
}
