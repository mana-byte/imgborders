#pragma once

#include <hyprland/src/render/pass/PassElement.hpp>

// Class defined elsewhere
class CImgBorder;

class CImgBorderPassElement : public IPassElement {
public:
  struct SData {
    CImgBorder *deco = nullptr;
  };

  CImgBorderPassElement(const SData &data_);
  virtual ~CImgBorderPassElement() = default;

  virtual const char *passName();
  virtual bool needsLiveBlur();
  virtual bool needsPrecomputeBlur();
  virtual void draw(const CRegion &damage);

private:
  SData data;
};
