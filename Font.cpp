#include "Font.h"
#include "Vertex3d.h"
#include "Shader.h"
#include "Buffer.h"
#include "Math.h"
#include "Bytestream.h"
#include <filesystem>
#include <set>


std::vector<real> calcSigned(std::vector<char> data, int xres, int yres)
{
    struct vecr { real x, y; bool operator<(const vecr& v) const { return std::make_pair(x, y) < std::make_pair(v.x, v.y); } };
    struct vec { int x, y; bool operator<(const vec& v) const { return std::make_pair(x, y) < std::make_pair(v.x, v.y); } };

    std::vector<real> cost(xres * yres, inf);
    std::vector<vecr> vecs(xres * yres);
    std::vector<char> vis(xres * yres, false);

    auto get = [xres, &data](int x, int y) { return data[y * xres + x]; };
    auto getVis = [xres, &vis](int x, int y) -> char& { return vis[y * xres + x]; };
    auto getCost = [xres, &cost](int x, int y) -> real& { return cost[y * xres + x]; };
    auto getVec = [xres, &vecs](int x, int y) -> vecr& { return vecs[y * xres + x]; };
    auto within = [xres, yres](int x, int y) { return x >= 0 && y >= 0 && x < xres && y < yres; };
    auto dist = [](real x, real y) { return std::sqrt(x * x + y * y); };

    std::set<std::pair<real, vec>> q;

    for(int y = 0; y < yres; y++)
    {
        for(int x = 0; x < xres; x++)
        {
            if(!get(x, y))
                continue;

            for(int dx = -1; dx <= 1; dx++)
            {
                for(int dy = -1; dy <= 1; dy++)
                {
                    if((dx || dy) && within(x+dx, y+dy) && !get(x+dx, y+dy))
                    {
                        auto c = dist(dx, dy)/2;
                        if(getCost(x+dx, y+dy) > c)
                        {
                            q.insert({ c, { x+dx, y+dy } });
                            getVec(x+dx, y+dy) = { real(dx)/2, real(dy)/2 };
                            getCost(x+dx, y+dy) = c;
                        }
                    }
                }
            }
        }
    }
    while(!q.empty())
    {
        auto it = q.begin();
        auto c = it->first;
        auto p = it->second;
        q.erase(it);

        
        if(getVis(p.x, p.y))
            continue;
        getVis(p.x, p.y) = true;

        for(int dx = -1; dx <= 1; dx++)
        {
            for(int dy = -1; dy <= 1; dy++)
            {
                if(within(p.x+dx, p.y+dy) && !getVis(p.x+dx, p.y+dy) && !get(p.x+dx, p.y+dy))
                {
                    auto v = getVec(p.x, p.y);
                    vecr v2 { v.x+dx, v.y+dy };
                    if(auto cost = dist(v2.x, v2.y); cost < getCost(p.x+dx, p.y+dy))
                    {
                        getCost(p.x+dx, p.y+dy) = cost;
                        getVec(p.x+dx, p.y+dy) = v2;
                        q.insert(std::make_pair(cost, vec { p.x+dx, p.y+dy }));
                    }
                }
            }
        }
    }

    std::fill(vis.begin(), vis.end(), false);

    for(int y = 0; y < yres; y++)
    {
        for(int x = 0; x < xres; x++)
        {
            if(get(x, y))
                continue;

            for(int dx = -1; dx <= 1; dx++)
            {
                for(int dy = -1; dy <= 1; dy++)
                {
                    if((dx || dy) && within(x+dx, y+dy) && get(x+dx, y+dy))
                    {
                        auto c = dist(dx, dy)/2;
                        if(getCost(x+dx, y+dy) > c)
                        {
                            q.insert({ c, { x+dx, y+dy } });
                            getVec(x+dx, y+dy) = { real(dx)/2, real(dy)/2 };
                            getCost(x+dx, y+dy) = c;
                        }
                    }
                }
            }
        }
    }
    while(!q.empty())
    {
        auto it = q.begin();
        auto c = it->first;
        auto p = it->second;
        q.erase(it);
        
        if(getVis(p.x, p.y))
            continue;
        getVis(p.x, p.y) = true;

        for(int dx = -1; dx <= 1; dx++)
        {
            for(int dy = -1; dy <= 1; dy++)
            {
                if(within(p.x+dx, p.y+dy) && !getVis(p.x+dx, p.y+dy) && get(p.x+dx, p.y+dy))
                {
                    auto v = getVec(p.x, p.y);
                    vecr v2 { v.x+dx, v.y+dy };
                    if(auto cost = dist(v2.x, v2.y); cost < getCost(p.x+dx, p.y+dy))
                    {
                        getCost(p.x+dx, p.y+dy) = cost;
                        getVec(p.x+dx, p.y+dy) = v2;
                        q.insert(std::make_pair(cost, vec { p.x+dx, p.y+dy }));
                    }
                }
            }
        }
    }

    for(int y = 0; y < yres; y++)
    {
        for(int x = 0; x < xres; x++)
        {
            if(getVis(x, y))
                getCost(x, y) = - getCost(x, y);
        }
    }

    real min = inf;
    real max = -inf;
    for(auto& c : cost)
    {
        min = std::min(c, min);
        max = std::max(c, max);
    }

    for(auto& c : cost)
    {
        if(c < 0)
            c = (50.f + std::max(c, -50.f))/100.f;
        else
            c = (50.f + std::min(50.f, c))/100.f;
    }

    return cost;
}


std::tuple<Buffer2d<real>, std::map<unsigned char, GlyphCoords>> makeSdfMap(FT_Face& face)
{
    int mapWidth = 2096, mapHeight = 100;
    std::vector<real> sdfMap(mapWidth*mapHeight, 50);

    int posX = 0, posY = 0;
    const int margin = 50;

    std::map<unsigned char, GlyphCoords> glyphMap;

    int rowMaxHeight = 0;
    FT_Set_Pixel_Sizes(face, 0, 2000);

    for (unsigned char ch = 32; ch <= 254; ch += 1)
    {
        auto glyph_index = FT_Get_Char_Index(face, ch);

        FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);

        FT_GlyphSlot slot = face->glyph;
        FT_Bitmap bitmap = slot->bitmap;

        Buffer2d<char> buf(bitmap.width + margin * 2, bitmap.rows + margin * 2, false);
        for(int y = 0; y < bitmap.rows; y++)
        {
            for(int x = 0; x < bitmap.width; x++)
            {
                auto b = x % 8;
                auto X = x/8;
                buf.get(margin+x, margin+y) = (((bitmap.buffer[y*bitmap.pitch+X] >> (7-b)) & 1));
            }
        }
        
        std::cout << "(" << posX << ", " << posY << ")" << std::endl;

        auto v = calcSigned(buf.data, buf.width, buf.height);
        auto sBuf = Buffer2d<real>(v, buf.width, buf.height);
        
        int charHeight = 128;
        auto dw = real(buf.height)/charHeight;
        real charWidth = (buf.width/real(buf.height))*charHeight;

        if (posX + charWidth > mapWidth)
        {
            posY += rowMaxHeight;
            posX = 0;
            rowMaxHeight = 0;
        }
        if(posY + charHeight > mapHeight)
        {
            mapHeight *= 2;
            sdfMap.resize(mapWidth*mapHeight, 50);
        }

        for(int y = 0; y < charHeight; y++)
        {
            for(int x = 0; x < int(1+charWidth); x++)
            {
                real X = dw*x + dw/2, Y = dw*y + dw/2;
                real xf = X-int(X), yf = Y-int(Y);
                real s = 0;
                s += yf*(xf*sBuf.get(X, Y, 50.f) + (1-xf)*sBuf.get(X+1, Y, 50.f));
                s += (1-yf)*(xf*sBuf.get(X, Y+1, 50.f) + (1-xf)*sBuf.get(X+1, Y+1, 50.f));
                sdfMap[(posY+y)*mapWidth+x+posX] = s;
            }
        }

        glyphMap[ch] = GlyphCoords{
            real(posX),
            real(posX + charWidth),
            real(posY),
            real(posY + charHeight),
            charWidth/charHeight,
            margin/real(bitmap.width),
            margin/real(bitmap.rows)
        };

        posX += charWidth;
        rowMaxHeight = std::max(rowMaxHeight, charHeight);
    }

    for (auto& it : glyphMap)
    {
        it.second.x1 /= mapWidth;
        it.second.x2 /= mapWidth;
        it.second.y1 = (mapHeight - it.second.y1) / (real)mapHeight;
        it.second.y2 = (mapHeight - it.second.y2) / (real)mapHeight;
        std::swap(it.second.y1, it.second.y2);
    }

    return { Buffer2d<real>(sdfMap, mapWidth, mapHeight), glyphMap };
}


void saveSdfMap(std::string fileName, const std::vector<GlyphCoords>& glyphInfos, const Buffer2d<real>& sdfMap)
{
    Bytestream b;
    b << glyphInfos.size();
    for(auto& glyphInfo : glyphInfos)
        b << glyphInfo.x1 << glyphInfo.x2 << glyphInfo.y1 << glyphInfo.y2 << glyphInfo.w << glyphInfo.marginX << glyphInfo.marginY;
    b << sdfMap.width << sdfMap.height;
    for(auto x : sdfMap.data)
        b << x;
    b.saveToFile(fileName);
}


std::pair<Buffer2d<real>, std::map<unsigned char, GlyphCoords>> loadSdfMap(std::string fileName)
{
    std::map<unsigned char, GlyphCoords> glyphMap;
    Bytestream b;
    b.loadFromFile(fileName);
    size_t size;
    b >> size;
    unsigned char c = 32;
    for(int i = 0; i < size; i++)
    {
        GlyphCoords glyphInfo;
        b >> glyphInfo.x1 >> glyphInfo.x2 >> glyphInfo.y1 >> glyphInfo.y2 >> glyphInfo.w >> glyphInfo.marginX >> glyphInfo.marginY;
        glyphMap[c++] = glyphInfo;
    }
    int width, height;
    b >> width >> height;
    Buffer2d<real> buf(width, height);
    for(int i = 0; i < width*height; i++)
    {
        real r;
        b >> r;
        buf.data[i] = r;
    }
    return { buf, glyphMap };
}


Glyph::Glyph(const FT_Face& face, unsigned char ch, GlyphCoords glyphCoord, GLuint texture)
{
    marginX = glyphCoord.marginX;
    marginY = glyphCoord.marginY;
    this->texture = texture;
    this->ch = ch;
    if(!vertexShader)
        vertexShader = new Shader("vertexShader.vert", GL_VERTEX_SHADER);
    if(!geometryShader)
        geometryShader = new Shader("geometryShader.geom", GL_GEOMETRY_SHADER);
    if(!fragmentShader)
        fragmentShader = new Shader("signShader.frag", GL_FRAGMENT_SHADER);

    auto glyph_index = FT_Get_Char_Index(face, ch);
    FT_Load_Glyph(face, glyph_index, 0);
    //auto w = real(face->glyph->metrics.width)/real(face->glyph->metrics.height);

    std::vector<Vertex3> vs = {
        { { 0.0, 0.0, 0.0 }, {0.0, 0.0, 1.0}, {glyphCoord.x1, glyphCoord.y1}},
        { { glyphCoord.w, 0.0, 0.0 }, {0.0, 0.0, 1.0}, {glyphCoord.x2, glyphCoord.y1}},
        { { glyphCoord.w, 1.0, 0.0 }, {0.0, 0.0, 1.0}, {glyphCoord.x2, glyphCoord.y2}},
        { { 0.0, 1.0, 0.0 }, {0.0, 0.0, 1.0}, {glyphCoord.x1, glyphCoord.y2}}
    };

    std::vector<int> indices = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3)*vs.size(), vs.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indices.size(), indices.data(), GL_STATIC_DRAW);
}


void Glyph::draw(Scene& scene, FT_Face face, Vector2 pos, real size)
{
    auto glyph_index = FT_Get_Char_Index(face, ch);
    FT_Set_Char_Size(face, 1000, 0, 1000, 1000);
    FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

    real H = face->size->metrics.ascender - face->size->metrics.descender;
    real b = face->glyph->metrics.horiBearingY;
    real h = face->glyph->metrics.height;
    real w = face->glyph->metrics.width;
    real l = face->glyph->metrics.horiBearingX;

    // H should be size tall
    // X/Y scaling: size/H

    auto translationMatrix = getTranslationMatrix(
        Vector3(
            pos.x + size*l/H - marginX*face->glyph->metrics.width*size/H,
            pos.y -size*h/H -size*(face->size->metrics.ascender - b)/H - h*marginY*size/H,
            0
        )
    );
    auto scalingMatrix = getScalingMatrix(Vector3(size*(1.0+marginY*2)*h/H, size*(1.0+marginY*2)*h/H, 1));

    auto modelViewMatrix = translationMatrix*scalingMatrix;

    auto s = scene.getShaderProgramManager();
    auto program = s->getProgram(fragmentShader, geometryShader, vertexShader);
    scene.setShaderProgram(program);

    auto matrix = getScalingMatrix(Vector3(0, 0, 0));

    program->use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(VAO);
    glUniform1i(glGetUniformLocation(program->getId(), "texture1"), 0);
    glUniform1f(glGetUniformLocation(program->getId(), "time"), (float)glfwGetTime());
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "modelViewMatrix"), 1, GL_TRUE, (float*)modelViewMatrix.m_val);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "projectionMatrix"), 1, GL_TRUE, (float*)identityMatrix.m_val);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "normalMatrix"), 1, GL_TRUE, (float*)identityMatrix.m_val);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


Font::Font(Scene& scene, std::string fileName)
{
    FT_Init_FreeType(&library);
    FT_New_Face(library, "OpenSans-regular.ttf", 0, &face);

    glGenTextures(1, &texture);

    std::vector<int> indices = { 0, 1, 2, 2, 3, 0 };

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Buffer2d<real> sdfMap;
    std::map<unsigned char, GlyphCoords> coordMap;

    fileName = "glyphmap.glm"; // TODO: use actual filename
    if(!std::filesystem::exists(fileName))
    {
        auto [map, gm] = makeSdfMap(face);

        std::vector<GlyphCoords> v;
        for(auto& it : gm)
        {
            v.push_back(it.second);
        }

        auto b = flipVertically(map.data, map.width);
        map.data = b;

        std::cout << "saving map" << std::endl;
        saveSdfMap(fileName, v, map);

        sdfMap = map;
        coordMap = gm;
    }
    else
    {
        std::cout << "loading map" << std::endl;
        auto [map, gm] = loadSdfMap(fileName);
        sdfMap = map;
        coordMap = gm;
    }


    //    auto glyph_index = FT_Get_Char_Index(face, 'a');
    //error = FT_Load_Glyph(
    //        face,          /* handle to face object */
    //        glyph_index,   /* glyph index           */
    //0 );  /* load flags, see below */

    //face->


    // tmp added

        //auto kv = Vector3(real(width)*kerning.x/widthA, real(width)*kerning.y/widthA, 0.f);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, sdfMap.width, sdfMap.height, 0, GL_RED, GL_FLOAT, sdfMap.data.data());

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(unsigned char ch = 32; ch < 255; ch++)
    {
        Glyph* glyph = new Glyph(face, ch, coordMap[ch], texture);
        glyphMap[ch] = glyph;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
};


Vector2 Font::getAdvance(unsigned char a, unsigned char b, real size)
{
    // Assumes sizing is set and there's a face loaded
    FT_Vector kerning;

    FT_Get_Kerning(
        face,
        FT_Get_Char_Index(face, a),
        FT_Get_Char_Index(face, b),
        FT_KERNING_DEFAULT,
        &kerning
    );

    real H = face->size->metrics.ascender - face->size->metrics.descender;
    real B = size*face->glyph->metrics.horiAdvance/H;

    return Vector2(B + size*kerning.x/H, 0);
}


void Font::draw(Scene& scene, std::string str, Vector2 pos, real size)
{
    for(int i = 0; i < str.size(); i++)
    {
        if(i > 0)
            pos += getAdvance(str[i-1], str[i], size);
        auto c = glyphMap[str[i]];
        c->draw(scene, face, pos, size);
    }
}

Shader* Glyph::vertexShader = nullptr;
Shader* Glyph::fragmentShader = nullptr;
Shader* Glyph::geometryShader = nullptr;
