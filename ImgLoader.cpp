#include "ImgLoader.hpp"
#include <cairo/cairo.h>
#include <hyprland/src/render/Texture.hpp>
#include <src/render/OpenGL.hpp>

static SP<CTexture> invalidImageTexture = nullptr;

void initInvalidImageTexture() {
  SP<CTexture> tex = makeShared<CTexture>();
  tex->allocate();

  const auto CAIROSURFACE =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 512, 512);
  const auto CAIRO = cairo_create(CAIROSURFACE);

  cairo_set_antialias(CAIRO, CAIRO_ANTIALIAS_NONE);
  cairo_save(CAIRO);
  cairo_set_source_rgba(CAIRO, 0, 0, 0, 1);
  cairo_set_operator(CAIRO, CAIRO_OPERATOR_SOURCE);
  cairo_paint(CAIRO);
  cairo_set_source_rgba(CAIRO, 1, 0, 1, 1);
  cairo_rectangle(CAIRO, 256, 0, 256, 256);
  cairo_fill(CAIRO);
  cairo_rectangle(CAIRO, 0, 256, 256, 256);
  cairo_fill(CAIRO);
  cairo_restore(CAIRO);

  cairo_surface_flush(CAIROSURFACE);

  tex->m_size = {512, 512};

  // copy the data to an OpenGL texture we have
  const GLint glFormat = GL_RGBA;
  const GLint glType = GL_UNSIGNED_BYTE;

  const auto DATA = cairo_image_surface_get_data(CAIROSURFACE);
  glBindTexture(GL_TEXTURE_2D, tex->m_texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
  glTexImage2D(GL_TEXTURE_2D, 0, glFormat, tex->m_size.x, tex->m_size.y, 0,
               glFormat, glType, DATA);

  cairo_surface_destroy(CAIROSURFACE);
  cairo_destroy(CAIRO);

  invalidImageTexture = tex;
}

SP<CTexture> ImgLoader::load(const std::string &fullPath) {
  const auto CAIROSURFACE =
      cairo_image_surface_create_from_png(fullPath.c_str());

  if (!CAIROSURFACE) {
    Debug::log(
        ERR, "loadAsset: failed to load {} (corrupt / inaccessible / not png)",
        fullPath);
    if (invalidImageTexture == nullptr) {
      initInvalidImageTexture();
    }
    return invalidImageTexture;
  }

  const auto CAIROFORMAT = cairo_image_surface_get_format(CAIROSURFACE);
  auto tex = makeShared<CTexture>();

  tex->allocate();
  tex->m_size = {cairo_image_surface_get_width(CAIROSURFACE),
                 cairo_image_surface_get_height(CAIROSURFACE)};

  const GLint glIFormat =
      CAIROFORMAT == CAIRO_FORMAT_RGB96F ? GL_RGB32F : GL_RGBA;
  const GLint glFormat = CAIROFORMAT == CAIRO_FORMAT_RGB96F ? GL_RGB : GL_RGBA;
  const GLint glType =
      CAIROFORMAT == CAIRO_FORMAT_RGB96F ? GL_FLOAT : GL_UNSIGNED_BYTE;

  const auto DATA = cairo_image_surface_get_data(CAIROSURFACE);
  glBindTexture(GL_TEXTURE_2D, tex->m_texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  if (CAIROFORMAT != CAIRO_FORMAT_RGB96F) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, glIFormat, tex->m_size.x, tex->m_size.y, 0,
               glFormat, glType, DATA);

  cairo_surface_destroy(CAIROSURFACE);

  return tex;
}
