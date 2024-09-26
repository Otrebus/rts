#pragma once
#include <string>
#include <map>
#include "Utils.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Scene.h"
#include <ft2build.h>
#include FT_FREETYPE_H


struct GlyphCoords
{
    real x1, x2, y1, y2, marginX, marginY;
};


struct Glyph
{
    GLuint VAO, VBO, EBO;
    unsigned char ch;

    Glyph(const FT_Face& face, unsigned char ch, GlyphCoords glyphCoord, GLuint texture);
    void draw(Scene& scene, FT_Face face, Vector2, real);

    static Shader* vertexShader;
    static Shader* geometryShader;
    static Shader* fragmentShader;
    GLuint texture;
    real marginX, marginY;
    real height, width;
};


class Font
{
public:
    Font(Scene& scene, std::string fileName);
    void draw(Scene& scene, std::string str, Vector2 pos, real size);

private:
    FT_Library library; // Might want to make this one static
    FT_Face face;

    Vector2 getAdvance(unsigned char a, unsigned char b, real size);

    GLuint texture;
    std::map<unsigned char, Glyph*> glyphMap;
};
