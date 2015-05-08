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
#include "TextureMaterial.h"
#include "TextureUnitManager.h"
#include "TintMaterial.h"

#include <glm/gtc/matrix_transform.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include <stb/stb_truetype.h>

#include <array>
#include <vector>

class FontAtlas {
public:
   const int firstGlyph;
   const int numGlyphs;
   const int bitmapSize;
   const float fontSize;
   std::vector<stbtt_bakedchar> cdata;
   GLuint textureID;

   FontAtlas(const int firstGlyph, const int numGlyphs, const int bitmapSize, const float fontSize, const unsigned char *fontData);

   virtual ~FontAtlas();
};

namespace {

const std::string FONT_FILE = "fonts/Inconsolata-Regular.ttf";

const float FONT_SIZE_SMALL = 36.0f;
const float FONT_SIZE_MEDIUM = 72.0f;
const float FONT_SIZE_LARGE = 160.0f;

const int BITMAP_SIZE_SMALL = 256;
const int BITMAP_SIZE_LARGE = 512;

const int FIRST_PRINTABLE_GLYPH = 32;
const int NUM_PRINTABLE_GLYPHS = 96;
const int FIRST_NUMERIC_GLYPH = 48;
const int NUM_NUMERIC_GLYPHS = 10;

float getStringWidth(FontAtlas &atlas, const std::string &text) {
   float x = 0.0f, y = 0.0f;

   for (char c : text) {
      if (c < atlas.firstGlyph || c > atlas.firstGlyph + atlas.numGlyphs) {
         continue;
      }

      stbtt_aligned_quad q;
      stbtt_GetBakedQuad(atlas.cdata.data(), atlas.bitmapSize, atlas.bitmapSize, c - atlas.firstGlyph, &x, &y, &q, 1);
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

FontAtlas::FontAtlas(const int firstGlyph, const int numGlyphs, const int bitmapSize, const float fontSize, const unsigned char *fontData)
   : firstGlyph(firstGlyph), numGlyphs(numGlyphs), bitmapSize(bitmapSize), fontSize(fontSize) {
   ASSERT(firstGlyph >= 0 && numGlyphs > 0, "Invalid glyph initialization values");
   ASSERT(bitmapSize > 0, "Invalid bitmap size");
   ASSERT(fontSize > 0.0f, "Invalid font size");
   ASSERT(fontData, "null font data");

   cdata.resize(numGlyphs);

   UPtr<unsigned char[]> bitmap(new unsigned char[bitmapSize * bitmapSize]);
   stbtt_BakeFontBitmap(fontData, 0, fontSize, bitmap.get(), bitmapSize, bitmapSize, firstGlyph, numGlyphs, cdata.data());

   glGenTextures(1, &textureID);
   glBindTexture(GL_TEXTURE_2D, textureID);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmapSize, bitmapSize, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.get());
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glBindTexture(GL_TEXTURE_2D, 0);
}

FontAtlas::~FontAtlas() {
   glDeleteTextures(1, &textureID);
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

   smallAtlas = UPtr<FontAtlas>(new FontAtlas(FIRST_PRINTABLE_GLYPH, NUM_PRINTABLE_GLYPHS, BITMAP_SIZE_SMALL * pixelDensity, FONT_SIZE_SMALL * pixelDensity, fontData.get()));
   largeAtlas = UPtr<FontAtlas>(new FontAtlas(FIRST_PRINTABLE_GLYPH, NUM_PRINTABLE_GLYPHS, BITMAP_SIZE_LARGE * pixelDensity, FONT_SIZE_MEDIUM * pixelDensity, fontData.get()));
   numberAtlas = UPtr<FontAtlas>(new FontAtlas(FIRST_NUMERIC_GLYPH, NUM_NUMERIC_GLYPHS, BITMAP_SIZE_LARGE * pixelDensity, FONT_SIZE_LARGE * pixelDensity, fontData.get()));
}

void TextRenderer::init(float pixelDensity) {
   mesh = std::make_shared<DynamicMesh>();
   gameObject = std::make_shared<GameObject>();
   gameObject->setGraphicsComponent(std::make_shared<GeometricGraphicsComponent>(*gameObject));

   SPtr<ShaderProgram> program(Context::getInstance().getAssetManager().loadShaderProgram("shaders/text"));
   SPtr<Model> model(std::make_shared<Model>(program, mesh));
   gameObject->getGraphicsComponent().setModel(model);

   textureMaterial = std::make_shared<TextureMaterial>(0, "uTexture");
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
   ASSERT(smallAtlas && largeAtlas && numberAtlas, "Font atlases not loaded");
   if (!smallAtlas || !largeAtlas || !numberAtlas) {
      return;
   }

   FontAtlas &atlas = fontType == FontType::Small ? *smallAtlas : (fontType == FontType::Large ? *largeAtlas : *numberAtlas);

   SPtr<ShaderProgram> shaderProgram = gameObject->getGraphicsComponent().getModel()->getShaderProgram();
   shaderProgram->setUniformValue("uProjMatrix", glm::ortho<float>(0.0f, (float)fbWidth, (float)fbHeight, 0.0f));
   shaderProgram->setUniformValue("uViewMatrix", glm::mat4(1.0f));
   shaderProgram->setUniformValue("uModelMatrix", glm::mat4(1.0f));

   textureMaterial->setTextureID(atlas.textureID);

   float width = getStringWidth(atlas, text);
   float fontSize = atlas.fontSize;

   float xOffset = 0.0f;
   if (hAlign == HAlign::Center) {
      xOffset = -width / 2.0f;
   } else if (hAlign == HAlign::Right) {
      xOffset = -width;
   }

   float yOffset = 0.0f;
   if (vAlign == VAlign::Center) {
      yOffset = fontSize / 4.0f; // Approximate (offset from baseline)
   } else if (vAlign == VAlign::Bottom) {
      yOffset = fontSize;
   }

   glDisable(GL_DEPTH_TEST);

   float absX = fbWidth * x;
   float absY = fbHeight * y;
   for (char c : text) {
      if (c < atlas.firstGlyph || c > atlas.firstGlyph + atlas.numGlyphs) {
         continue;
      }

      stbtt_aligned_quad q;
      stbtt_GetBakedQuad(atlas.cdata.data(), atlas.bitmapSize, atlas.bitmapSize, c - atlas.firstGlyph, &absX, &absY, &q, 1);

      renderQuad(*gameObject, *mesh, q, xOffset, yOffset);
   }

   glEnable(GL_DEPTH_TEST);
}
