#include "ImgBorder.hpp"
#include "ImgBorderPassElement.hpp"
#include "globals.hpp"
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/render/Renderer.hpp>

CImgBorder::CImgBorder(PHLWINDOW pWindow) : IHyprWindowDecoration(pWindow) {
  m_pWindow = pWindow;
}

CImgBorder::~CImgBorder() { damageEntire(); }

std::string CImgBorder::getDisplayName() { return "Image borders :)"; }

eDecorationType CImgBorder::getDecorationType() { return DECORATION_CUSTOM; }

uint64_t CImgBorder::getDecorationFlags() {
  return DECORATION_PART_OF_MAIN_WINDOW;
}

eDecorationLayer CImgBorder::getDecorationLayer() {
  return DECORATION_LAYER_OVER;
}

const auto HEIGHT = 10;
SDecorationPositioningInfo CImgBorder::getPositioningInfo() {
  return {
      .policy = DECORATION_POSITION_STICKY,
      .edges = DECORATION_EDGE_BOTTOM | DECORATION_EDGE_TOP,
      .priority = 9990,
      .desiredExtents =
          {
              .topLeft = {0, HEIGHT},
              .bottomRight = {0, 0},
          },
      .reserved = true,
  };
}

void CImgBorder::onPositioningReply(const SDecorationPositioningReply &reply) {
  m_bAssignedGeometry = reply.assignedGeometry;
}

void CImgBorder::draw(PHLMONITOR pMonitor, const float &a) {
  if (!validMapped(m_pWindow))
    return;

  const auto PWINDOW = m_pWindow.lock();

  if (!PWINDOW->m_sWindowData.decorate.valueOrDefault())
    return;

  CImgBorderPassElement::SData data;
  data.deco = this;

  g_pHyprRenderer->m_sRenderPass.add(makeShared<CImgBorderPassElement>(data));
}

void CImgBorder::updateWindow(PHLWINDOW pWindow) { damageEntire(); }

void CImgBorder::damageEntire() {
  // TODO
  // CBox dm = m_bLastRelativeBox.copy().translate(m_lastWindowPos).expand(2);
  // g_pHyprRenderer->damageBox(assignedBoxGlobal());
}
