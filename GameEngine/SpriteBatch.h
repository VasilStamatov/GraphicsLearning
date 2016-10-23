#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>

#include "Vertex.h"
#include "SpriteFont.h"

namespace GameEngine
{
  // Determines how we should sort the glyphs
  enum class GlyphSortType
  {
    NONE,
    FRONT_TO_BACK,
    BACK_TO_FRONT,
    TEXTURE
  };

  // A glyph is a single sprite (quad). They are added with SpriteBatch::Draw
  class Glyph
  {
  public:
    Glyph() {};
    Glyph(const glm::vec4& _destRect, const glm::vec4& _uvRect, GLuint _texture, float _depth, const ColorRGBA8& _color);

    Glyph(const glm::vec4& _destRect, const glm::vec4& _uvRect, GLuint _texture, float _depth, const ColorRGBA8& _color, float _angle);

    GLuint m_texture{ 0 };
    float m_depth{ 0.0f };

    Vertex m_topLeft;
    Vertex m_bottomLeft;
    Vertex m_topRight;
    Vertex m_bottomRight;

  private:
    glm::vec2 RotatePoint(const glm::vec2& _pos, float _angle);
  };

  // Each render batch is used for a single draw call
  class RenderBatches
  {
  public:
    RenderBatches(GLuint _offset, GLuint _numVertices, GLuint _texture) :
      m_offset(_offset), m_numVertices(_numVertices), m_texture(_texture) {
    }
    GLuint m_offset{ 0 };
    GLuint m_numVertices{ 0 };
    GLuint m_texture{ 0 };
  };

  // The SpriteBatch class is an efficient way of drawing sprites
  class SpriteBatch
  {
  public:
    SpriteBatch();
    ~SpriteBatch();

    // Initializes the spritebatch
    void Init();
    //disposes of the spritebatch
    void Dispose();

    // Begins the spritebatch and chooses the sort type (by texture is default)
    void Begin(GlyphSortType _sortType = GlyphSortType::TEXTURE);

    // Ends the spritebatch
    void End();

    // Adds a glyph to the vector of glyphs
    void Draw(const glm::vec4& _destRect, const glm::vec4& _uvRect, GLuint _texture, float _depth, const ColorRGBA8& _color);
    // Adds a glyph to the vector of glyphs with rotation
    void Draw(const glm::vec4& _destRect, const glm::vec4& _uvRect, GLuint _texture, float _depth, const ColorRGBA8& _color, float _angle);
    // Adds a glyph to the vector of glyphs with rotation
    void Draw(const glm::vec4& _destRect, const glm::vec4& _uvRect, GLuint _texture, float _depth, const ColorRGBA8& _color, const glm::vec2& _dir);

    // Renders the entire SpriteBatch to the screen
    void RenderBatch();
  private:
    // Creates all the needed RenderBatches
    void CreateRenderBatches();
    // Generates the VAO and VBO
    void CreateVertexArray();
    // Sorts glyphs according to _sortType
    void SortGlyphs();

    GLuint m_vbo{ 0 };
    GLuint m_vao{ 0 };

    GlyphSortType m_sortType;

    std::vector<Glyph*> m_glyphPointers; ///< this is for sorting
    std::vector<Glyph> m_glyphs; ///<these are the actual glyphs
    std::vector<RenderBatches> m_renderBatches;
  };
}


