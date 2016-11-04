#include "SpriteBatch.h"
#include <algorithm> // used for sorting

namespace GameEngine
{
		//initialize the glyph
		Glyph::Glyph(const glm::vec4& _destRect, const glm::vec4& _uvRect, GLuint _texture, float _depth, const ColorRGBA8& _color) :
				m_texture(_texture),
				m_depth(_depth)
		{
				//intialize the color, position and uv coordinate for the 4 vertices of the sprite
				m_topLeft.m_color = _color;
				m_topLeft.SetPosition(_destRect.x, _destRect.y + _destRect.w, 0.0f);
				m_topLeft.SetUV(_uvRect.x, _uvRect.y + _uvRect.w);

				m_bottomLeft.m_color = _color;
				m_bottomLeft.SetPosition(_destRect.x, _destRect.y, 0.0f);
				m_bottomLeft.SetUV(_uvRect.x, _uvRect.y);

				m_bottomRight.m_color = _color;
				m_bottomRight.SetPosition(_destRect.x + _destRect.z, _destRect.y, 0.0f);
				m_bottomRight.SetUV(_uvRect.x + _uvRect.z, _uvRect.y);

				m_topRight.m_color = _color;
				m_topRight.SetPosition(_destRect.x + _destRect.z, _destRect.y + _destRect.w, 0.0f);
				m_topRight.SetUV(_uvRect.x + _uvRect.z, _uvRect.y + _uvRect.w);

		}

		Glyph::Glyph(const glm::vec4& _destRect, const glm::vec4& _uvRect, GLuint _texture, float _depth, const ColorRGBA8& _color, float _angle) :
				m_texture(_texture),
				m_depth(_depth)
		{
				glm::vec2 halfDims(_destRect.z / 2.0f, _destRect.w / 2.0f);

				// Get points centered at origin
				glm::vec2 tL(-halfDims.x, halfDims.y);
				glm::vec2 bL(-halfDims.x, -halfDims.y);
				glm::vec2 bR(halfDims.x, -halfDims.y);
				glm::vec2 tR(halfDims.x, halfDims.y);

				// Rotate the points
				tL = RotatePoint(tL, _angle) + halfDims;
				bL = RotatePoint(bL, _angle) + halfDims;
				bR = RotatePoint(bR, _angle) + halfDims;
				tR = RotatePoint(tR, _angle) + halfDims;

				//intialize the color, position and uv coordinate for the 4 vertices of the sprite with the rotated points
				m_topLeft.m_color = _color;
				m_topLeft.SetPosition(_destRect.x + tL.x, _destRect.y + tL.y, 0.0f);
				m_topLeft.SetUV(_uvRect.x, _uvRect.y + _uvRect.w);

				m_bottomLeft.m_color = _color;
				m_bottomLeft.SetPosition(_destRect.x + bL.x, _destRect.y + bL.y, 0.0f);
				m_bottomLeft.SetUV(_uvRect.x, _uvRect.y);

				m_bottomRight.m_color = _color;
				m_bottomRight.SetPosition(_destRect.x + bR.x, _destRect.y + bR.y, 0.0f);
				m_bottomRight.SetUV(_uvRect.x + _uvRect.z, _uvRect.y);

				m_topRight.m_color = _color;
				m_topRight.SetPosition(_destRect.x + tR.x, _destRect.y + tR.y, 0.0f);
				m_topRight.SetUV(_uvRect.x + _uvRect.z, _uvRect.y + _uvRect.w);

		}

		glm::vec2 Glyph::RotatePoint(const glm::vec2& _pos, float _angle)
		{
				//rotate a point
				glm::vec2 newV;
				newV.x = _pos.x * cos(_angle) - _pos.y * sin(_angle);
				newV.y = _pos.x * sin(_angle) + _pos.y * cos(_angle);
				return newV;
		}

		//initialize the vbo and vao to 0 when the spritebatch variable is created
		SpriteBatch::SpriteBatch() : m_vbo(0), m_vao(0)
		{
		}


		SpriteBatch::~SpriteBatch()
		{
		}

		void SpriteBatch::Init()
		{
				CreateVertexArray();
		}

		void SpriteBatch::Dispose()
		{
				//delete the vao's and vbo's and reset them to 0
				if (m_vao != 0)
				{
						glDeleteVertexArrays(1, &m_vao);
						m_vao = 0;
				}

				if (m_vbo != 0)
				{
						glDeleteBuffers(1, &m_vbo);
						m_vbo = 0;
				}
		}

		void SpriteBatch::Begin(GlyphSortType _sortType/* = GlyphSortType::TEXTURE*/)
		{
				m_sortType = _sortType;
				m_renderBatches.clear();

				/* Make m_glpyhs.size() == 0, however it does not free internal memory,
						so when we later call emplace_back it doesn't need to internally call new. */
				m_glyphs.clear();
		}
		void SpriteBatch::End()
		{
				//set up pointers for fast sorting
				//resite the glyphPointers vectors to the number of glyphs so there is exactly as much space as it's needed
				m_glyphPointers.resize(m_glyphs.size());
				//set all the glyphPointer elements to the addresses of their respective glyph
				for (size_t i = 0; i < m_glyphs.size(); i++)
				{
						m_glyphPointers[i] = &m_glyphs[i];
				}
				//then sort the glyphs using the glyph pointers vector
				SortGlyphs();
				//create the render batches
				CreateRenderBatches();
		}

		void SpriteBatch::Draw(const glm::vec4& _destRect, const glm::vec4& _uvRect, GLuint _texture, float _depth, const ColorRGBA8& _color)
		{
				m_glyphs.emplace_back(_destRect, _uvRect, _texture, _depth, _color);
		}

		void SpriteBatch::Draw(const glm::vec4& _destRect, const glm::vec4& _uvRect, GLuint _texture, float _depth, const ColorRGBA8& _color, float _angle)
		{
				m_glyphs.emplace_back(_destRect, _uvRect, _texture, _depth, _color, _angle);
		}

		void SpriteBatch::Draw(const glm::vec4& _destRect, const glm::vec4& _uvRect, GLuint _texture, float _depth, const ColorRGBA8& _color, const glm::vec2& _dir)
		{
				const glm::vec2 right(1.0f, 0.0f);

				float angle = acos(glm::dot(right, _dir));

				if (_dir.y < 0.0f)
				{
						angle = -angle;
				}

				m_glyphs.emplace_back(_destRect, _uvRect, _texture, _depth, _color, angle);
		}

		void SpriteBatch::RenderBatch()
		{
				/* Bind the VAO. This sets up the opengl state we need, including the
							vertex attribute pointers and it binds the VBO */
				glBindVertexArray(m_vao);

				for (size_t i = 0; i < m_renderBatches.size(); i++)
				{
						glBindTexture(GL_TEXTURE_2D, m_renderBatches[i].m_texture);

						glDrawArrays(GL_TRIANGLES, m_renderBatches[i].m_offset, m_renderBatches[i].m_numVertices);
				}
				//unbind the vao
				glBindVertexArray(0);
		}

		void SpriteBatch::CreateRenderBatches()
		{
				// This will store all the vertices that we need to upload
				std::vector<Vertex> vertices;
				// Resize the buffer to the exact size we need so we can treat it like an array
				// Multiply the gP size by 6, because there are 6 vertices in a glyph (2 triangles)
				vertices.resize(m_glyphPointers.size() * 6);
				//check if the gP vector is empty
				if (m_glyphPointers.empty())
				{
						return;
				}
				int offset = 0; //current offset
				int currentVertex = 0;
				//Add the first batch
				m_renderBatches.emplace_back(offset, 6, m_glyphPointers[0]->m_texture);
				vertices[currentVertex++] = m_glyphPointers[0]->m_topLeft;
				vertices[currentVertex++] = m_glyphPointers[0]->m_bottomLeft;
				vertices[currentVertex++] = m_glyphPointers[0]->m_bottomRight;
				vertices[currentVertex++] = m_glyphPointers[0]->m_bottomRight;
				vertices[currentVertex++] = m_glyphPointers[0]->m_topRight;
				vertices[currentVertex++] = m_glyphPointers[0]->m_topLeft;
				offset += 6;
				//Add all the rest of the glyphs
				//currentglyph is set to 1 rather than 0, because we already added the first batch
				for (size_t currentGlyph = 1; currentGlyph < m_glyphPointers.size(); currentGlyph++)
				{
						/* Check if this glyph can be part of the current batch
									by comparing the current glyph's texture with the previous one */
						if (m_glyphPointers[currentGlyph]->m_texture != m_glyphPointers[currentGlyph - 1]->m_texture)
						{
								// Make a new batch
								m_renderBatches.emplace_back(offset, 6, m_glyphPointers[currentGlyph]->m_texture);
						}
						else
						{
								// If its part of the current batch, just increase numVertices
								m_renderBatches.back().m_numVertices += 6;
						}
						vertices[currentVertex++] = m_glyphPointers[currentGlyph]->m_topLeft;
						vertices[currentVertex++] = m_glyphPointers[currentGlyph]->m_bottomLeft;
						vertices[currentVertex++] = m_glyphPointers[currentGlyph]->m_bottomRight;
						vertices[currentVertex++] = m_glyphPointers[currentGlyph]->m_bottomRight;
						vertices[currentVertex++] = m_glyphPointers[currentGlyph]->m_topRight;
						vertices[currentVertex++] = m_glyphPointers[currentGlyph]->m_topLeft;
						offset += 6;
				}
				// Bind our VBO
				glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
				// Orphan the buffer (for speed)
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
				//upload the data
				glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
				// Unbind the VBO
				glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		void SpriteBatch::CreateVertexArray()
		{
				// Generate the VAO if it isn't already generated
				if (m_vao == 0)
				{
						glGenVertexArrays(1, &m_vao);
				}
				// Bind the VAO. All subsequent opengl calls will modify it's state.
				glBindVertexArray(m_vao);

				//Generate the VBO if it isn't already generated
				if (m_vbo == 0)
				{
						glGenBuffers(1, &m_vbo);
				}
				glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

				//Tell opengl what attribute arrays we need
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);

				//this is the position attribute pointer
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::m_position));
				//this is the color attribute pointer
				glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::m_color));
				//this is the UV attribute pointer
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::m_uv));
				//Unbind the vertex array
				glBindVertexArray(0);
		}

		void SpriteBatch::SortGlyphs()
		{
				//sort the glyphs with stable_sort taken from <algorithm>
				switch (m_sortType)
				{
				case GlyphSortType::FRONT_TO_BACK:
				{
						std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), [](Glyph* _a, Glyph* _b) { return (_a->m_depth < _b->m_depth); });
						break;
				}
				case GlyphSortType::BACK_TO_FRONT:
				{
						std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), [](Glyph* _a, Glyph* _b) { return (_a->m_depth > _b->m_depth); });
						break;
				}
				case GlyphSortType::TEXTURE:
				{
						std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), [](Glyph* _a, Glyph* _b) { return (_a->m_texture < _b->m_texture); });
						break;
				}
				default:
						break;
				}
		}
}

