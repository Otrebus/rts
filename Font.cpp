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


std::tuple<Buffer2d<real>, std::map<char, GlyphCoords>> makeSdfMap(FT_Face& face)
{
    int mapWidth = 2096, mapHeight = 100;
    std::vector<real> sdfMap(mapWidth*mapHeight, 50);

    int posX = 0, posY = 0;
    const int margin = 50;

    std::map<char, GlyphCoords> glyphMap;

    int rowMaxHeight = 0;
    int emBoxHeight = 50;
    FT_Set_Pixel_Sizes(face, 0, emBoxHeight);

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

        int charWidth = buf.width;
        int charHeight = buf.height;

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

        for (int y = 0; y < charHeight; y++)
        {
            for (int x = 0; x < charWidth; x++)
            {
                sdfMap[(posY + y) * mapWidth + posX + x] = sBuf.get(x, y, 50.f);
            }
        }

        glyphMap[ch] = GlyphCoords{
            real(posX + margin),
            real(posX + charWidth - margin),
            real(posY + charHeight - margin),
            real(posY + margin)
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
    }

    return { Buffer2d<real>(sdfMap, mapWidth, mapHeight), glyphMap };
}


void saveSdfMap(std::string fileName, const std::vector<GlyphCoords>& glyphInfos, const Buffer2d<real>& sdfMap)
{
    Bytestream b;
    b << glyphInfos.size();
    for(auto glyphInfo : glyphInfos)
        b << glyphInfo.x1 << glyphInfo.x2 << glyphInfo.y1 << glyphInfo.y2;
    b << sdfMap.width << sdfMap.height;
    for(auto x : sdfMap.data)
        b << x;
    b.saveToFile(fileName);
}


std::pair<Buffer2d<real>, std::map<char, GlyphCoords>> loadSdfMap(std::string fileName)
{
    std::map<char, GlyphCoords> glyphMap;
    Bytestream b;
    b.loadFromFile(fileName);
    size_t size;
    b >> size;
    unsigned char c = 32;
    for(int i = 0; i < size; i++)
    {
        GlyphCoords glyphInfo;
        b >> glyphInfo.x1 >> glyphInfo.x2 >> glyphInfo.y1 >> glyphInfo.y2;
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


Glyph::Glyph(const FT_Face& face, char ch, GlyphCoords glyphCoord)
{
    if(!vertexShader)
        vertexShader = new Shader("vertexShader.vert", GL_VERTEX_SHADER);
    if(!geometryShader)
        geometryShader = new Shader("geometryShader.geom", GL_GEOMETRY_SHADER);
    if(!fragmentShader)
        fragmentShader = new Shader("signShader.frag", GL_FRAGMENT_SHADER);

    auto glyph_index = FT_Get_Char_Index(face, ch);
    FT_Load_Glyph(face, glyph_index, 0);

    const real width = 0.1;
    real widthA = face->glyph->metrics.width;
    real ratio = real(face->glyph->metrics.height)/real(face->glyph->metrics.width);
    real height = width*ratio;

    real bear = width*real(face->glyph->metrics.width)/widthA;

    Vector3 pos(0.0, 0.0, 0.0);
    Vector3 vb(bear, 0.0, 0.0);

    std::vector<Vertex3> vs = {
        {pos + vb, {0.0, 0.0, 1.0}, {glyphCoord.x1, glyphCoord.y1}},
        {pos + vb + Vector3(width, 0, 0), {0.0, 0.0, 1.0}, {glyphCoord.x2, glyphCoord.y1}},
        {pos + vb + Vector3(width, height, 0), {0.0, 0.0, 1.0}, {glyphCoord.x2, glyphCoord.y2}},
        {pos + vb + Vector3(0, height, 0), {0.0, 0.0, 1.0}, {glyphCoord.x1, glyphCoord.y2}}
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


void Glyph::draw(Scene& scene)
{
    auto s = scene.getShaderProgramManager();
    auto program = s->getProgram(fragmentShader, geometryShader, vertexShader);
    scene.setShaderProgram(program);

    program->use();
    glBindVertexArray(VAO);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "modelViewMatrix"), 1, GL_TRUE, (float*)identityMatrix.m_val);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "projectionMatrix"), 1, GL_TRUE, (float*)identityMatrix.m_val);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "normalMatrix"), 1, GL_TRUE, (float*)identityMatrix.m_val);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


Font::Font(Scene& scene, std::string fileName)
{
    FT_Library library;
    FT_Face face;
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
    std::map<char, GlyphCoords> coordMap;

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

    std::vector<Vertex3> vs = {
        {{ -1, -1, 0 }, {0.0, 0.0, 1.0}, { 0, 0 }},
        {Vector3(1, -1, 0.0 ), {0.0, 0.0, 1.0}, {1, 0 }},
        {Vector3(1, 1, 0), {0.0, 0.0, 1.0}, {1, 1}},
        {Vector3(-1, 1, 0), {0.0, 0.0, 1.0}, {0, 1}}
    };



    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, sdfMap.width, sdfMap.height, 0, GL_RED, GL_FLOAT, sdfMap.data.data());

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(unsigned char ch = 32; ch < 255; ch++)
    {
        Glyph* glyph = new Glyph(face, ch, coordMap[ch]);
        glyphMap[ch] = glyph;
    }
};


void Font::draw(Scene& scene, std::string str)
{
    for(int i = 0; i < str.size(); i++)
    {
        auto c = glyphMap[str[i]];
        c->draw(scene);
    }
}

Shader* Glyph::vertexShader = nullptr;
Shader* Glyph::fragmentShader = nullptr;
Shader* Glyph::geometryShader = nullptr;
