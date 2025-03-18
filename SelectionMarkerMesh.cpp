#include "LambertianMaterial.h"
#include "Scene.h"
#include "SelectionDecalMaterial.h"
#include "SelectionDecalMaterial.h"
#include "SelectionMarkerMesh.h"
#include "Terrain.h"


SelectionMarkerMesh::SelectionMarkerMesh(real length, real width, bool circular = false) : length(length), width(width), circular(circular)
{
    radius = 1.0f;
}


void SelectionMarkerMesh::setRadius(real radius)
{
    this->radius = radius;
}


void SelectionMarkerMesh::draw(Material* mat)
{
    auto material = mat ? mat : this->material;
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(material->getShader(), getGeometryShader(), getVertexShader());
    scene->setShaderProgram(program);
    program->use();
    
    float radiusA = radius, radiusB = radius;
    if(!circular)
    {
        radiusA = 0.2f + real(length)/2;
        radiusB = 0.2f + real(width)/2;
    }

    if(pass == 0)
    {
        ((SelectionDecalMaterial*)material)->alpha = 0.89f;
        SelectionDecalMaterial::radiusA = radiusA - 0.1f;
        SelectionDecalMaterial::radiusB = radiusB - 0.1f;

        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ZERO);
    }
    else if(pass == 1)
    {
        ((SelectionDecalMaterial*)material)->alpha = 1.0f;
        SelectionDecalMaterial::radiusA = pre ? (radiusA - 0.05f) : radiusA;
        SelectionDecalMaterial::radiusB = pre ? (radiusB - 0.05f) : radiusB;

        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_OFFSET_FILL);

        glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    }
    else if(pass == 2)
    {
        ((SelectionDecalMaterial*)material)->alpha = 1.0f;
        SelectionDecalMaterial::radiusA = radiusA - 0.1f;
        SelectionDecalMaterial::radiusB = radiusB - 0.1f;

        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_OFFSET_FILL);

        glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA, GL_ONE, GL_ZERO);
    }
    updateUniforms();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, GLsizei(triangles.size()), GL_UNSIGNED_INT, 0);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_BLEND);
    glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ZERO, GL_ONE);
}


std::pair<std::vector<Vertex3>, std::vector<int>> SelectionMarkerMesh::calcVertices(Vector2 pos)
{
    auto entityPos = pos;

    std::vector<Vertex3> vs;

    int W, L;
    if(circular)
    {
        W = int(radius*2 + 3);
        L = int(radius*2 + 3);
        width = real(W);
        length = real(L);
    }
    else
    {
        W = int(width + 2);
        L = int(length + 2);
    }

    auto terrain = scene->getTerrain();
    auto W_t = terrain->getWidth();
    auto H_t = terrain->getHeight();

    for(int y = 0; y <= W; y++)
    {
        for(int x = 0; x <= L; x++)
        {
            int Y = int(std::floor(y + entityPos.y - real(width)/2));
            int X = int(std::floor(x + entityPos.x - real(length)/2));

            int Xc = std::max(0, std::min(W_t-1, X));
            int Yc = std::max(0, std::min(H_t-1, Y));
    
            auto pos = terrain->getPoint(Xc, Yc);
            pos.x = real(X);
            pos.y = real(Y);
            pos.z += 0.01f;
            vs.push_back({ pos.x, pos.y, pos.z, 0.f, 0.f, 1.f, real(X)-entityPos.x, real(Y)-entityPos.y });
        }
    }

    std::vector<int> triangles;

    for(int y = 0; y < W; y++)
    {
        for(int x = 0; x < L; x++)
        {
            int i1 = y*(L+1)+x, i2 = y*(L+1)+x+1, i3 = (y+1)*(L+1)+x+1, i4 = (y+1)*(L+1) + x;
            triangles.insert(triangles.end(), { i1, i2, i3, i1, i3, i4 });
        }
    }
    return { vs, triangles };
}


void SelectionMarkerMesh::init(Vector2 pos)
{
    auto [vs, triangles] = calcVertices(pos);

    auto material = new SelectionDecalMaterial({ 0.f, 0.8f, 0.1f }, circular);

    v = vs;
    this->triangles = triangles;
    this->material = material;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3)*v.size(), v.data(), GL_STREAM_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*triangles.size(), triangles.data(), GL_STREAM_DRAW);
}


void SelectionMarkerMesh::update(Vector2 pos)
{
    auto [vs, triangles] = calcVertices(pos);
    v = vs;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3)*v.size(), v.data(), GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*triangles.size(), triangles.data(), GL_STREAM_DRAW);
}

void SelectionMarkerMesh::setSelectionType(bool pre)
{
    this->pre = pre;
}
