#include "ImgBorder.hpp"
#include "ImgBorderPassElement.hpp"
#include "ImgUtils.hpp"
#include "globals.hpp"
#include <filesystem>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/render/Texture.hpp>
#include <hyprland/src/render/decorations/DecorationPositioner.hpp>
#include <iterator>
#include <src/SharedDefs.hpp>
#include <src/debug/Log.hpp>
#include <src/plugins/PluginAPI.hpp>
#include <wordexp.h>

CImgBorder::CImgBorder(PHLWINDOW pWindow) : IHyprWindowDecoration(pWindow) {
  m_pWindow = pWindow;
  updateConfig();
}

CImgBorder::~CImgBorder() {
  std::erase(g_pGlobalState->borders, m_self);
  // damageEntire();
}

SDecorationPositioningInfo CImgBorder::getPositioningInfo() {
  SDecorationPositioningInfo info;
  info.policy = DECORATION_POSITION_STICKY;
  info.edges = DECORATION_EDGE_LEFT | DECORATION_EDGE_RIGHT |
               DECORATION_EDGE_TOP | DECORATION_EDGE_BOTTOM;
  info.priority = 9990;
  if (!m_isHidden) {
    info.desiredExtents = {
        .topLeft = {12, 12},
        .bottomRight = {12, 12},
    };
  }
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
  if (m_isHidden)
    return;

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

  g_pHyprOpenGL->m_renderData.useNearestNeighbor = true;

  // Corners

  const CBox box_tl = {box.pos(), m_tex_tl->m_size * m_scale};
  g_pHyprOpenGL->renderTexture(m_tex_tl, box_tl, a, 0, 2.F, false, false,
                               GL_REPEAT, GL_REPEAT);

  const CBox box_tr = {
      {box.x + box.width - m_tex_tr->m_size.x * m_scale, box.y},
      m_tex_tr->m_size * m_scale};
  g_pHyprOpenGL->renderTexture(m_tex_tr, box_tr, a, 0, 2.F, false, false,
                               GL_REPEAT, GL_REPEAT);

  const CBox box_br = {{box.x + box.width - m_tex_br->m_size.x * m_scale,
                        box.y + box.height - m_tex_br->m_size.y * m_scale},
                       m_tex_br->m_size * m_scale};
  g_pHyprOpenGL->renderTexture(m_tex_br, box_br, a, 0, 2.F, false, false,
                               GL_REPEAT, GL_REPEAT);

  const CBox box_bl = {
      {box.x, box.y + box.height - m_tex_bl->m_size.y * m_scale},
      m_tex_bl->m_size * m_scale};
  g_pHyprOpenGL->renderTexture(m_tex_bl, box_bl, a, 0, 2.F, false, false,
                               GL_REPEAT, GL_REPEAT);

  // Edges

  g_pHyprOpenGL->m_renderData.primarySurfaceUVTopLeft = {0, 0};

  const CBox box_t = {
      {box_tl.x + box_tl.width, box.y},
      {box.width - box_tl.width - box_tr.width, m_tex_t->m_size.y * m_scale}};
  g_pHyprOpenGL->m_renderData.primarySurfaceUVBottomRight = {
      box_t.width / (m_tex_t->m_size.x * m_scale), 1.};
  g_pHyprOpenGL->renderTexture(m_tex_t, box_t, a, 0, 2.F, false, true,
                               GL_REPEAT, GL_REPEAT);

  const CBox box_b = {
      {box_bl.x + box_bl.width, box_bl.y},
      {box.width - box_bl.width - box_br.width, m_tex_b->m_size.y * m_scale}};
  g_pHyprOpenGL->renderTexture(m_tex_b, box_b, a, 0, 2.F, false, true,
                               GL_REPEAT, GL_REPEAT);

  const CBox box_l = {{box_tl.x, box.y + box_tl.height},
                      {box_tl.width, box_bl.y - box_tl.y - box_tl.height}};
  g_pHyprOpenGL->m_renderData.primarySurfaceUVBottomRight = {
      1., box_l.height / (m_tex_l->m_size.y * m_scale)};
  g_pHyprOpenGL->renderTexture(m_tex_l, box_l, a, 0, 2.F, false, true,
                               GL_REPEAT, GL_REPEAT);

  const CBox box_r = {{box_tr.x, box.y + box_tl.height},
                      {box_tr.width, box_bl.y - box_tl.y - box_tl.height}};
  g_pHyprOpenGL->renderTexture(m_tex_r, box_r, a, 0, 2.F, false, true,
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

// TODO better error handling
void CImgBorder::updateConfig() {
  // Read config
  // ------------

  // hidden
  m_isHidden = (Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
                   PHANDLE, "plugin:imgborders:enabled")
                   ->getDataStaticPtr();

  // image
  const auto texSrc = (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(
                          PHANDLE, "plugin:imgborders:image")
                          ->getDataStaticPtr();
  wordexp_t p;
  wordexp(*texSrc, &p, 0);
  std::string texSrcExpanded;
  for (size_t i = 0; i < p.we_wordc; i++)
    texSrcExpanded.append(p.we_wordv[i]);
  wordfree(&p);
  if (!std::filesystem::exists(texSrcExpanded)) {
    HyprlandAPI::addNotification(
        PHANDLE,
        std::format("[imgborders] {} image at doesn't exist", texSrcExpanded),
        CHyprColor{1.0, 0.1, 0.1, 1.0}, 5000);
    return;
  }
  m_texSrc = texSrcExpanded;

  // sizes
  const auto sizesStr = (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(
                            PHANDLE, "plugin:imgborders:sizes")
                            ->getDataStaticPtr();
  if (!sizesStr || std::string(*sizesStr).empty()) {
    HyprlandAPI::addNotification(
        PHANDLE, "[imgborders] missing or invalid sizes in config",
        CHyprColor{1.0, 0.1, 0.1, 1.0}, 5000);
    return;
  }
  int i = 0;
  for (; i < 4; i++) {
    try {
      std::string sizeStr;
      std::getline(std::stringstream(*sizesStr), sizeStr, ',');
      m_sizes[i] = std::stoi(sizeStr);
    } catch (...) {
      HyprlandAPI::addNotification(
          PHANDLE, "[imgborders] missing or invalid sizes in config",
          CHyprColor{1.0, 0.1, 0.1, 1.0}, 5000);
      return;
    }
  }

  // scale
  m_scale = **(Hyprlang::FLOAT *const *)HyprlandAPI::getConfigValue(
                  PHANDLE, "plugin:imgborders:scale")
                  ->getDataStaticPtr();

  // smooth
  m_shouldSmooth = (Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
                       PHANDLE, "plugin:imgborders:smooth")
                       ->getDataStaticPtr();

  // Create textures
  // ------------

  // But first delete old textures
  if (m_tex_tl)
    m_tex_tl->destroyTexture();
  if (m_tex_tr)
    m_tex_tr->destroyTexture();
  if (m_tex_br)
    m_tex_br->destroyTexture();
  if (m_tex_bl)
    m_tex_bl->destroyTexture();
  if (m_tex_l)
    m_tex_l->destroyTexture();
  if (m_tex_t)
    m_tex_t->destroyTexture();
  if (m_tex_b)
    m_tex_b->destroyTexture();
  if (m_tex_r)
    m_tex_r->destroyTexture();
  m_tex_tl = nullptr;
  m_tex_tr = nullptr;
  m_tex_br = nullptr;
  m_tex_bl = nullptr;
  m_tex_l = nullptr;
  m_tex_t = nullptr;
  m_tex_b = nullptr;
  m_tex_r = nullptr;

  auto tex = ImgUtils::load("/home/zac/assets/border.png");

  const auto BORDER_LEFT = (float)m_sizes[0];
  const auto BORDER_RIGHT = (float)m_sizes[1];
  const auto BORDER_TOP = (float)m_sizes[2];
  const auto BORDER_BOTTOM = (float)m_sizes[3];

  m_tex_tl = ImgUtils::sliceTexture(tex, {{0., 0.}, {BORDER_LEFT, BORDER_TOP}});

  m_tex_t = ImgUtils::sliceTexture(
      tex, {{BORDER_LEFT, 0.},
            {tex->m_size.x - BORDER_LEFT - BORDER_RIGHT, BORDER_TOP}});

  m_tex_tr = ImgUtils::sliceTexture(
      tex, {{tex->m_size.x - BORDER_RIGHT, 0.}, {BORDER_RIGHT, BORDER_TOP}});

  m_tex_r = ImgUtils::sliceTexture(
      tex, {{tex->m_size.x - BORDER_RIGHT, BORDER_TOP},
            {BORDER_RIGHT, tex->m_size.y - BORDER_TOP - BORDER_BOTTOM}});

  m_tex_br = ImgUtils::sliceTexture(
      tex, {{tex->m_size.x - BORDER_RIGHT, tex->m_size.y - BORDER_BOTTOM},
            {BORDER_RIGHT, BORDER_BOTTOM}});

  m_tex_b = ImgUtils::sliceTexture(
      tex, {{BORDER_LEFT, tex->m_size.y - BORDER_BOTTOM},
            {tex->m_size.x - BORDER_LEFT - BORDER_RIGHT, BORDER_BOTTOM}});

  m_tex_bl = ImgUtils::sliceTexture(
      tex, {{0., tex->m_size.y - BORDER_BOTTOM}, {BORDER_LEFT, BORDER_BOTTOM}});

  m_tex_l = ImgUtils::sliceTexture(
      tex, {{0., BORDER_TOP},
            {BORDER_LEFT, tex->m_size.y - BORDER_TOP - BORDER_BOTTOM}});

  tex->destroyTexture();
}

void CImgBorder::updateRules() {
  const auto PWINDOW = m_pWindow.lock();
  auto rules = PWINDOW->m_matchedRules;
  auto prevIsHidden = m_isHidden;

  m_isHidden = false;

  for (auto &r : rules) {
    if (r->m_rule == "plugin:imgborders:noimgborders")
      m_isHidden = true;
  }

  if (prevIsHidden != m_isHidden)
    g_pDecorationPositioner->repositionDeco(this);
}
