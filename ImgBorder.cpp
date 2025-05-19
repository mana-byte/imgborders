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

const SP<CTexture> tex = ImgLoader::load("/home/zac/stone-border.png");
const SP<CTexture> tl_tex = ImgLoader::sliceTexture(tex, {{0, 0}, {50, 50}});
// const SP<CTexture> tl_tex = ImgLoader::sliceTexture(tex, {{0, 0}, {70, 70}});

CImgBorder::CImgBorder(PHLWINDOW pWindow) : IHyprWindowDecoration(pWindow) {
  m_pWindow = pWindow;
}

CImgBorder::~CImgBorder() {
  // std::erase(g_pGlobalState->borders, m_self);
  // damageEntire();
}

SDecorationPositioningInfo CImgBorder::getPositioningInfo() {
  SDecorationPositioningInfo info;
  info.policy = DECORATION_POSITION_STICKY;
  info.edges = DECORATION_EDGE_LEFT | DECORATION_EDGE_RIGHT |
               DECORATION_EDGE_TOP | DECORATION_EDGE_BOTTOM;
  info.priority = 9990;
  info.desiredExtents = {
      .topLeft = {70, 70},
      .bottomRight = {70, 70},
  };
  info.reserved = true;
  return info;
}

void CImgBorder::onPositioningReply(const SDecorationPositioningReply &reply) {
  m_bAssignedBox = reply.assignedGeometry;
}

void CImgBorder::draw(PHLMONITOR pMonitor, const float &a) {
  if (!validMapped(m_pWindow))
    return;

  const auto PWINDOW = m_pWindow.lock();

  if (!PWINDOW->m_windowData.decorate.valueOrDefault())
    return;

  CImgBorderPassElement::SData data = {
      .deco = this,
      .a = 1.F,
  };
  g_pHyprRenderer->m_renderPass.add(makeShared<CImgBorderPassElement>(data));
}

void CImgBorder::drawPass(PHLMONITOR pMonitor, const float &a) {
  // Get the global bounding box
  // ------------

  CBox box = m_bAssignedBox;

  const auto PWINDOW = m_pWindow.lock();

  const auto PWORKSPACE = PWINDOW->m_workspace;
  const auto WORKSPACEOFFSET = PWORKSPACE && !m_pWindow->m_pinned
                                   ? PWORKSPACE->m_renderOffset->value()
                                   : Vector2D();
  box.translate(PWINDOW->m_floatingOffset - pMonitor->m_position +
                WORKSPACEOFFSET);

  box.translate(g_pDecorationPositioner->getEdgeDefinedPoint(
      DECORATION_EDGE_LEFT | DECORATION_EDGE_RIGHT | DECORATION_EDGE_TOP |
          DECORATION_EDGE_BOTTOM,
      PWINDOW));

  m_bLastRelativeBox = box;

  // Render the textures
  // ------------

  g_pHyprOpenGL->renderTexture(tl_tex, box, a);
}

eDecorationType CImgBorder::getDecorationType() { return DECORATION_CUSTOM; }

void CImgBorder::updateWindow(PHLWINDOW pWindow) { damageEntire(); }

void CImgBorder::damageEntire() {
  g_pHyprRenderer->damageBox(m_bLastRelativeBox);
}

eDecorationLayer CImgBorder::getDecorationLayer() {
  return DECORATION_LAYER_OVER;
}

uint64_t CImgBorder::getDecorationFlags() {
  return DECORATION_PART_OF_MAIN_WINDOW;
}
