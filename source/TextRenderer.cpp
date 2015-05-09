#include "AssetManager.h"
#include "Context.h"
#include "DynamicMesh.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "GeometricGraphicsComponent.h"
#include "IOUtils.h"
#include "LogHelper.h"
#include "Model.h"
#include "RenderData.h"
#include "ShaderProgram.h"
#include "TextRenderer.h"
#include "Texture.h"
#include "TextureMaterial.h"
#include "TextureUnitManager.h"
#include "TintMaterial.h"

#include <glm/gtc/matrix_transform.hpp>

#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBRP_ASSERT ASSERT
#include <stb/stb_rect_pack.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#define STBTT_assert ASSERT
#include <stb/stb_truetype.h>

#include <array>
#include <map>
#include <vector>

struct FontRange {
   float fontSize;
   int firstGlyph;
   int numGlyphs;
   std::vector<stbtt_packedchar> charData;
};

typedef std::map<FontType, FontRange> FontRangeMap;

class FontAtlas {
public:
   SPtr<Texture> texture;
   int bitmapSize;
   FontRangeMap fontRangeMap;

   FontAtlas(int bitmapSize, FontRangeMap &map, unsigned char *fontData);

   virtual ~FontAtlas();
};

namespace {

const std::string FONT_FILE = "fonts/Inconsolata-Regular.ttf";

const float FONT_SIZE_SMALL = 36.0f;
const float FONT_SIZE_MEDIUM = 72.0f;
const float FONT_SIZE_LARGE = 160.0f;

const int BITMAP_SIZE = 512;

const int FIRST_PRINTABLE_GLYPH = 32;
const int NUM_PRINTABLE_GLYPHS = 96;
const int FIRST_NUMERIC_GLYPH = 48;
const int NUM_NUMERIC_GLYPHS = 10;

float getStringWidth(FontRange &range, int bitmapSize, const std::string &text) {
   float x = 0.0f, y = 0.0f;

   for (char c : text) {
      if (c < range.firstGlyph || c >= range.firstGlyph + range.numGlyphs) {
         continue;
      }

      stbtt_aligned_quad q;
      stbtt_GetPackedQuad(range.charData.data(), bitmapSize, bitmapSize, c - range.firstGlyph, &x, &y, &q, 0);
   }

   return x;
}

void renderQuad(GameObject &gameObject, DynamicMesh &mesh, const stbtt_aligned_quad &q, float xOffset, float yOffset) {
   std::array<float, 12> vertices({{ q.x0 + xOffset, q.y1 + yOffset, 0.0f,
                                     q.x1 + xOffset, q.y1 + yOffset, 0.0f,
                                     q.x0 + xOffset, q.y0 + yOffset, 0.0f,
                                     q.x1 + xOffset, q.y0 + yOffset, 0.0f }});

   std::array<unsigned int, 6> indices({{ 0, 1, 2,
                                          2, 1, 3 }});

   std::array<float, 8> texCoords({{ q.s0, q.t1,
                                     q.s1, q.t1,
                                     q.s0, q.t0,
                                     q.s1, q.t0 }});

   mesh.setVertices(vertices.data(), vertices.size());
   mesh.setIndices(indices.data(), indices.size());
   mesh.setTexCoords(texCoords.data(), texCoords.size());

   RenderData renderData;
   gameObject.getGraphicsComponent().draw(renderData);
}

} // namespace

FontAtlas::FontAtlas(int bitmapSize, FontRangeMap &map, unsigned char *fontData)
   : bitmapSize(bitmapSize), fontRangeMap(std::move(map)) {
   ASSERT(bitmapSize > 0, "Invalid bitmap size");

   UPtr<unsigned char[]> bitmap(new unsigned char[bitmapSize * bitmapSize]);
   stbtt_pack_context packContext;

   int res = stbtt_PackBegin(&packContext, bitmap.get(), bitmapSize, bitmapSize, 0, 1, nullptr);
   ASSERT(res, "stbtt_PackBegin failed");

   std::vector<stbtt_pack_range> packRanges;
   for (std::pair<const FontType, FontRange> &item : fontRangeMap) {
      FontRange &fontRange = item.second;
      stbtt_pack_range packRange;
      fontRange.charData.resize(fontRange.numGlyphs);

      packRange.font_size = fontRange.fontSize;
      packRange.first_unicode_char_in_range = fontRange.firstGlyph;
      packRange.num_chars_in_range = fontRange.numGlyphs;
      packRange.chardata_for_range = fontRange.charData.data();

      packRanges.push_back(packRange);
   }

   res = stbtt_PackFontRanges(&packContext, fontData, 0, packRanges.data(), packRanges.size());
   ASSERT(res, "stbtt_PackFontRanges failed");

   stbtt_PackEnd(&packContext);

   texture = std::make_shared<Texture>(GL_TEXTURE_2D);
   texture->bind();

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmapSize, bitmapSize, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.get());
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   texture->unbind();
}

FontAtlas::~FontAtlas() {
}

TextRenderer::TextRenderer()
   : initialized(false) {
}

TextRenderer::~TextRenderer() {
}

void TextRenderer::loadFontAtlases(float pixelDensity) {
   UPtr<const unsigned char[]> fontData(IOUtils::readFromBinaryDataFile(FONT_FILE));
   if (!fontData) {
      LOG_WARNING("Unable to load font data from " << FONT_FILE);
      return;
   }

   FontRangeMap fontRangeMap;

   FontRange smallRange;
   smallRange.fontSize = FONT_SIZE_SMALL * pixelDensity;
   smallRange.firstGlyph = FIRST_PRINTABLE_GLYPH;
   smallRange.numGlyphs = NUM_PRINTABLE_GLYPHS;
   fontRangeMap[FontType::Small] = smallRange;

   FontRange mediumRange;
   mediumRange.fontSize = FONT_SIZE_MEDIUM * pixelDensity;
   mediumRange.firstGlyph = FIRST_PRINTABLE_GLYPH;
   mediumRange.numGlyphs = NUM_PRINTABLE_GLYPHS;
   fontRangeMap[FontType::Medium] = mediumRange;

   FontRange largeNumberRange;
   largeNumberRange.fontSize = FONT_SIZE_LARGE * pixelDensity;
   largeNumberRange.firstGlyph = FIRST_NUMERIC_GLYPH;
   largeNumberRange.numGlyphs = NUM_NUMERIC_GLYPHS;
   fontRangeMap[FontType::LargeNumber] = largeNumberRange;

   atlas = UPtr<FontAtlas>(new FontAtlas(BITMAP_SIZE * pixelDensity, fontRangeMap, const_cast<unsigned char*>(fontData.get())));
}

void TextRenderer::init(float pixelDensity) {
   mesh = std::make_shared<DynamicMesh>();
   gameObject = std::make_shared<GameObject>();
   gameObject->setGraphicsComponent(std::make_shared<GeometricGraphicsComponent>(*gameObject));

   SPtr<ShaderProgram> program(Context::getInstance().getAssetManager().loadShaderProgram("shaders/text"));
   SPtr<Model> model(std::make_shared<Model>(program, mesh));
   gameObject->getGraphicsComponent().setModel(model);

   textureMaterial = std::make_shared<TextureMaterial>(nullptr, "uTexture");
   SPtr<TintMaterial> tintMaterial(std::make_shared<TintMaterial>(1.0f, glm::vec3(1.0f)));
   model->attachMaterial(textureMaterial);
   model->attachMaterial(tintMaterial);

   loadFontAtlases(pixelDensity);

   initialized = true;
}

void TextRenderer::onPixelDensityChange(float pixelDensity) {
   if (!initialized) {
      return;
   }

   loadFontAtlases(pixelDensity);
}

void TextRenderer::render(int fbWidth, int fbHeight, float x, float y, const std::string &text, FontType fontType, HAlign hAlign, VAlign vAlign) {
   ASSERT(atlas, "Font atlas not loaded");
   if (!atlas) {
      return;
   }

   ASSERT(atlas->fontRangeMap.count(fontType) != 0, "Invalid font type");
   if (atlas->fontRangeMap.count(fontType) == 0) {
      return;
   }
   FontRange &fontRange = atlas->fontRangeMap.at(fontType);

   SPtr<ShaderProgram> shaderProgram = gameObject->getGraphicsComponent().getModel()->getShaderProgram();
   shaderProgram->setUniformValue("uProjMatrix", glm::ortho<float>(0.0f, (float)fbWidth, (float)fbHeight, 0.0f));
   shaderProgram->setUniformValue("uViewMatrix", glm::mat4(1.0f));
   shaderProgram->setUniformValue("uModelMatrix", glm::mat4(1.0f));

   textureMaterial->setTexture(atlas->texture);

   float width = getStringWidth(fontRange, atlas->bitmapSize, text);
   float fontSize = fontRange.fontSize;

   float xOffset = 0.0f;
   if (hAlign == HAlign::Center) {
      xOffset = -width / 2.0f;
   } else if (hAlign == HAlign::Right) {
      xOffset = -width;
   }

   float yOffset = 0.0f;
   if (vAlign == VAlign::Center) {
      yOffset = fontSize / 4.0f; // Approximate (offset from baseline)
   } else if (vAlign == VAlign::Top) {
      yOffset = fontSize;
   }

   glDisable(GL_DEPTH_TEST);

   float absX = fbWidth * x;
   float absY = fbHeight * y;
   for (char c : text) {
      if (c < fontRange.firstGlyph || c >= fontRange.firstGlyph + fontRange.numGlyphs) {
         continue;
      }

      stbtt_aligned_quad q;
      stbtt_GetPackedQuad(fontRange.charData.data(), atlas->bitmapSize, atlas->bitmapSize, c - fontRange.firstGlyph, &absX, &absY, &q, 0);

      renderQuad(*gameObject, *mesh, q, xOffset, yOffset);
   }

   glEnable(GL_DEPTH_TEST);
}
