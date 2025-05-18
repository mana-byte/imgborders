#include "ImgBorder.hpp"
#include "ImgBorderPassElement.hpp"
#include "ImgLoader.hpp"
#include "globals.hpp"
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/render/Texture.hpp>
#include <hyprland/src/render/decorations/DecorationPositioner.hpp>

CImgBorder::CImgBorder(PHLWINDOW pWindow) : IHyprWindowDecoration(pWindow) {
  m_pWindow = pWindow;
}

CImgBorder::~CImgBorder() {
  // TODO
  // g_pGlobalState->borders.erase(m_self);
  // damageEntire();
}

SDecorationPositioningInfo CImgBorder::getPositioningInfo() {
  SDecorationPositioningInfo info;
  info.policy = DECORATION_POSITION_STICKY;
  info.edges = DECORATION_EDGE_TOP;
  info.priority = 9990;
  info.desiredExtents = {
      .topLeft = {100, 100},
      .bottomRight = {100, 100},
  };
  info.reserved = true;
  return info;
}

void CImgBorder::onPositioningReply(const SDecorationPositioningReply &reply) {
  m_bAssignedBox = reply.assignedGeometry;
}

CBox CImgBorder::assignedBoxGlobal() {
  if (!validMapped(m_pWindow))
    return {};

  CBox box = m_bAssignedBox;
  box.translate(g_pDecorationPositioner->getEdgeDefinedPoint(
      DECORATION_EDGE_TOP, m_pWindow.lock()));

  const auto PWORKSPACE = m_pWindow->m_workspace;
  const auto WORKSPACEOFFSET = PWORKSPACE && !m_pWindow->m_pinned
                                   ? PWORKSPACE->m_renderOffset->value()
                                   : Vector2D();

  return box.translate(WORKSPACEOFFSET);
}

void CImgBorder::draw(PHLMONITOR pMonitor, const float &a) {
  if (!validMapped(m_pWindow))
    return;

  const auto PWINDOW = m_pWindow.lock();

  if (!PWINDOW->m_windowData.decorate.valueOrDefault())
    return;

  CImgBorderPassElement::SData data = {
      .deco = this,
      .a = a,
  };
  g_pHyprRenderer->m_renderPass.add(makeShared<CImgBorderPassElement>(data));
}

// const SP<CTexture> texture =
// g_pHyprOpenGL->loadAsset("/home/zac/Untitled.png");
const SP<CTexture> texture = ImgLoader::load("/home/zac/Untitled.png");
void CImgBorder::drawPass(PHLMONITOR pMonitor, const float &a) {
  const auto DECOBOX = m_bAssignedBox;
  g_pHyprOpenGL->renderTexture(texture, DECOBOX, 1.F);
  g_pHyprOpenGL->renderTexture(texture, assignedBoxGlobal(), 1.F);
  g_pHyprOpenGL->renderTexture(texture, assignedBoxGlobal(), 1.F);
}

eDecorationType CImgBorder::getDecorationType() { return DECORATION_CUSTOM; }

void CImgBorder::updateWindow(PHLWINDOW pWindow) { damageEntire(); }

void CImgBorder::damageEntire() {
  g_pHyprRenderer->damageBox(assignedBoxGlobal());
}

eDecorationLayer CImgBorder::getDecorationLayer() {
  return DECORATION_LAYER_OVER;
}

uint64_t CImgBorder::getDecorationFlags() {
  return DECORATION_PART_OF_MAIN_WINDOW;
}
