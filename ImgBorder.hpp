#pragma once

#include "globals.hpp"
#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/desktop/WindowRule.hpp>
#include <hyprland/src/render/Texture.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>

static const auto DISPLAY_NAME = "Image borders";

class CImgBorder : public IHyprWindowDecoration {
public:
  CImgBorder(PHLWINDOW);
  virtual ~CImgBorder();

  virtual SDecorationPositioningInfo getPositioningInfo();

  virtual void onPositioningReply(const SDecorationPositioningReply &reply);

  virtual void draw(PHLMONITOR, float const &a);

  bool shouldBlur();

  CBox getGlobalBoundingBox(PHLMONITOR pMonitor);

  void drawPass(PHLMONITOR, float const &a);

  virtual eDecorationType getDecorationType();

  virtual void updateWindow(PHLWINDOW);

  virtual void damageEntire();

  virtual eDecorationLayer getDecorationLayer();

  virtual uint64_t getDecorationFlags();

  virtual std::string getDisplayName() { return DISPLAY_NAME; }

  PHLWINDOW getWindow() { return m_pWindow.lock(); }

  void updateConfig();

  void updateRules();

  WP<CImgBorder> m_self;

private:
  PHLWINDOWREF m_pWindow;

  bool m_isEnabled;
  bool m_isHidden;
  int m_sizes[4];
  int m_insets[4];
  float m_scale;
  bool m_shouldSmooth;
  bool m_shouldBlurGlobal;
  bool m_shouldBlur;

  SP<CTexture> m_tex_tl;
  SP<CTexture> m_tex_tr;
  SP<CTexture> m_tex_br;
  SP<CTexture> m_tex_bl;
  SP<CTexture> m_tex_t;
  SP<CTexture> m_tex_r;
  SP<CTexture> m_tex_b;
  SP<CTexture> m_tex_l;

  CBox m_bLastRelativeBox;
};
