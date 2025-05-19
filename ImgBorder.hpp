#pragma once

#include "globals.hpp"
#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>

static const auto DISPLAY_NAME = "Image borders";

class CImgBorder : public IHyprWindowDecoration {
public:
  CImgBorder(PHLWINDOW);
  virtual ~CImgBorder();

  virtual SDecorationPositioningInfo getPositioningInfo();

  virtual void onPositioningReply(const SDecorationPositioningReply &reply);

  virtual void draw(PHLMONITOR, float const &a);

  void drawPass(PHLMONITOR, float const &a);

  virtual eDecorationType getDecorationType();

  virtual void updateWindow(PHLWINDOW);

  virtual void damageEntire();

  virtual eDecorationLayer getDecorationLayer();

  virtual uint64_t getDecorationFlags();

  virtual std::string getDisplayName() { return DISPLAY_NAME; }

  WP<CImgBorder> m_self;

private:
  PHLWINDOWREF m_pWindow;

  CBox m_bAssignedBox;
  CBox m_bLastRelativeBox;

  SBoxExtents m_seExtents;
};
