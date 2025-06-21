#pragma once

#include <hyprland/src/render/Texture.hpp>
#include <hyprland/src/render/pass/PassElement.hpp>

// Class defined elsewhere
class CImgBorder;

static const std::string PASS_NAME = "CImgBorderPassElement";

class CImgBorderPassElement : public IPassElement {
public:
  struct SData {
    CImgBorder *deco = nullptr;
    float a = 1.F;
  };

  CImgBorderPassElement(const SData &data_);
  virtual ~CImgBorderPassElement();

  virtual void draw(const CRegion &damage);

  virtual bool needsLiveBlur();

  virtual bool needsPrecomputeBlur();

  virtual const char *passName() { return PASS_NAME.c_str(); }

  virtual std::optional<CBox> boundingBox();

private:
  SData data;
};
