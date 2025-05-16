#pragma once

#include "globals.hpp"
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>

class CImgBorder : public IHyprWindowDecoration {
public:
  CImgBorder(PHLWINDOW pWindow);

  virtual ~CImgBorder();

  virtual std::string getDisplayName();

  virtual eDecorationType getDecorationType();

  virtual uint64_t getDecorationFlags();

  virtual eDecorationLayer getDecorationLayer();

  virtual SDecorationPositioningInfo getPositioningInfo();

  virtual void onPositioningReply(const SDecorationPositioningReply &reply);

  virtual void draw(PHLMONITOR pMonitor, float const &a);

  virtual void updateWindow(PHLWINDOW pWindow);

  virtual void damageEntire();

private:
  PHLWINDOWREF m_pWindow;

  CBox m_bAssignedGeometry;

  SBoxExtents m_seExtents;

  friend class CImgBorderPassElement;
};
