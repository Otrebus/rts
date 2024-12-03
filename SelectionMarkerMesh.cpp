#include "LambertianMaterial.h"
#include "Scene.h"
#include "SelectionDecalMaterial.h"
#include "SelectionDecalMaterial.h"
#include "SelectionMarkerMesh.h"
#include "Terrain.h"


SelectionMarkerMesh::SelectionMarkerMesh(Entity* entity, int length, int width) : length(length), width(width)
{
    this->entity = entity;
    int pass = 0;
    circular = false;
}


void SelectionMarkerMesh::draw(Material* mat)
{
    auto material = mat ? mat : this->material;
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(material->getShader(), getGeometryShader(), getVertexShader());
    scene->setShaderProgram(program);
    program->use();
    
    float radius = 1.0;
    if(!circular)
        radius = 1.0; // In this case it's the fraction of the full figure

    if(pass == 0)
    {
        ((SelectionDecalMaterial*)material)->alpha = 0.89;
        SelectionDecalMaterial::radius = radius - 0.1;

        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ZERO);
    }
    else if(pass == 1)
    {
        ((SelectionDecalMaterial*)material)->alpha = 1.0;
        SelectionDecalMaterial::radius = pre ? (radius - 0.05) : radius;

        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_OFFSET_FILL);

        glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    }
    else if(pass == 2)
    {
        ((SelectionDecalMaterial*)material)->alpha = 1.0;
        SelectionDecalMaterial::radius = radius - 0.1;

        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_OFFSET_FILL);

        glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA, GL_ONE, GL_ZERO);
    }
    updateUniforms();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_BLEND);
    glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ZERO, GL_ONE);
}


std::pair<std::vector<Vertex3>, std::vector<int>> SelectionMarkerMesh::calcVertices(Scene* scene)
{
    auto entityPos = entity->getPosition();

    int xc = entityPos.x, yc = entityPos.y;

    std::vector<Vertex3> vs;
    for(int y = 0; y <= width; y++)
    {
        for(int x = 0; x <= length; x++)
        {
            int Y = y + yc-1;
            int X = x + xc - 1;

            auto pos = scene->getTerrain()->getPoint(X, Y);
            pos.z += 0.01;
            if(circular)
                vs.push_back({ pos.x, pos.y, pos.z, 0, 0, 1, real(X)-entityPos.x, real(Y)-entityPos.y });
            else
                vs.push_back({ pos.x, pos.y, pos.z, 0, 0, 1, real(x), real(y) });
        }
    }

    std::vector<int> triangles;

    for(int y = 0; y < width; y++)
    {
        for(int x = 0; x < length; x++)
        {
            int i1 = y*(length+1)+x, i2 = y*(length+1)+x+1, i3 = (y+1)*(length+1)+x+1, i4 = (y+1)*(length+1) + x;
            triangles.insert(triangles.end(), { i1, i2, i3, i1, i3, i4 });
        }
    }
    return { vs, triangles };
}


void SelectionMarkerMesh::init()
{
    auto [vs, triangles] = calcVertices(scene);

    auto material = new SelectionDecalMaterial({ 0, 0.8, 0.1 }, length, width, circular);

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


void SelectionMarkerMesh::update()
{
    auto [vs, triangles] = calcVertices(scene);
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
