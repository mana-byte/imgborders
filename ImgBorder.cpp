#include "ImgBorder.hpp"
#include "ImgBorderPassElement.hpp"
#include "ImgUtils.hpp"
#include "globals.hpp"
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/render/Texture.hpp>
#include <hyprland/src/render/decorations/DecorationPositioner.hpp>

const SP<CTexture> tex = ImgUtils::load("/home/zac/assets/border.png");
const double BORDER_LEFT = 7;
const double BORDER_RIGHT = 7;
const double BORDER_TOP = 7;
const double BORDER_BOTTOM = 7;
const SP<CTexture> tex_tl =
    ImgUtils::sliceTexture(tex, {{0., 0.}, {BORDER_LEFT, BORDER_TOP}});
const SP<CTexture> tex_t = ImgUtils::sliceTexture(
    tex, {{BORDER_LEFT, 0.}, {BORDER_RIGHT, BORDER_TOP}});
const SP<CTexture> tex_tr = ImgUtils::sliceTexture(
    tex, {{tex->m_size.x - BORDER_RIGHT, 0.}, {BORDER_RIGHT, BORDER_TOP}});
const SP<CTexture> tex_r = ImgUtils::sliceTexture(
    tex, {{BORDER_RIGHT, BORDER_TOP}, {tex->m_size.x, BORDER_BOTTOM}});
const SP<CTexture> tex_br = ImgUtils::sliceTexture(
    tex, {{tex->m_size.x - BORDER_RIGHT, tex->m_size.y - BORDER_BOTTOM},
          {BORDER_RIGHT, BORDER_BOTTOM}});
const SP<CTexture> tex_b = ImgUtils::sliceTexture(
    tex, {{BORDER_LEFT, BORDER_BOTTOM}, {BORDER_RIGHT, tex->m_size.y}});
const SP<CTexture> tex_bl = ImgUtils::sliceTexture(
    tex, {{0., tex->m_size.y - BORDER_BOTTOM}, {BORDER_LEFT, BORDER_BOTTOM}});
const SP<CTexture> tex_l = ImgUtils::sliceTexture(
    tex, {{0., BORDER_TOP}, {BORDER_LEFT, BORDER_BOTTOM}});
const auto TEX_SCALE = 5.;

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
      .topLeft = {12, 12},
      .bottomRight = {12, 12},
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

  // Save previous values

  const auto wasUsingNearestNeighbour =
      g_pHyprOpenGL->m_renderData.useNearestNeighbor;
  const auto prevUVTL = g_pHyprOpenGL->m_renderData.primarySurfaceUVTopLeft;
  const auto prevUVBR = g_pHyprOpenGL->m_renderData.primarySurfaceUVBottomRight;

  // Do the thing

  g_pHyprOpenGL->m_renderData.useNearestNeighbor = true;

  g_pHyprOpenGL->m_renderData.primarySurfaceUVTopLeft = {0, 0};
  // g_pHyprOpenGL->m_renderData.primarySurfaceUVBottomRight = {
  //     box.width / tex_tl->m_size.x / TEX_SCALE,
  //     box.height / tex_tl->m_size.y / TEX_SCALE};
  g_pHyprOpenGL->m_renderData.primarySurfaceUVBottomRight = {1, 1};
  const CBox box_tl = {box.pos(), tex_tl->m_size * TEX_SCALE};
  g_pHyprOpenGL->renderTexture(tex_tl, box_tl, a, 0, 2.F, false, false,
                               GL_REPEAT, GL_REPEAT);

  const CBox box_tr = {
      {box.x + box.width - tex_tr->m_size.x * TEX_SCALE, box.y},
      tex_tr->m_size * TEX_SCALE};
  g_pHyprOpenGL->renderTexture(tex_tr, box_tr, a, 0, 2.F, false, false,
                               GL_REPEAT, GL_REPEAT);

  const CBox box_br = {{box.x + box.width - tex_br->m_size.x * TEX_SCALE,
                        box.y + box.height - tex_br->m_size.y * TEX_SCALE},
                       tex_br->m_size * TEX_SCALE};
  g_pHyprOpenGL->renderTexture(tex_br, box_br, a, 0, 2.F, false, false,
                               GL_REPEAT, GL_REPEAT);

  const CBox box_bl = {
      {box.x, box.y + box.height - tex_bl->m_size.y * TEX_SCALE},
      tex_bl->m_size * TEX_SCALE};
  g_pHyprOpenGL->renderTexture(tex_bl, box_bl, a, 0, 2.F, false, false,
                               GL_REPEAT, GL_REPEAT);

  // Restore previous values

  g_pHyprOpenGL->m_renderData.useNearestNeighbor = wasUsingNearestNeighbour;

  g_pHyprOpenGL->m_renderData.primarySurfaceUVTopLeft = prevUVTL;
  g_pHyprOpenGL->m_renderData.primarySurfaceUVBottomRight = prevUVBR;
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
