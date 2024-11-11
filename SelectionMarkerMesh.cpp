#include "LambertianMaterial.h"
#include "Scene.h"
#include "SelectionDecalMaterial.h"
#include "SelectionDecalMaterial.h"
#include "SelectionMarkerMesh.h"
#include "Terrain.h"


SelectionMarkerMesh::SelectionMarkerMesh(Entity* tank)
{
    this->tank = tank;
    int pass = 0;
}


void SelectionMarkerMesh::draw(Material* mat)
{
    auto material = mat ? mat : this->material;
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(material->getShader(), getGeometryShader(), getVertexShader());
    scene->setShaderProgram(program);
    program->use();

    if(pass == 0)
    {
        ((SelectionDecalMaterial*)material)->alpha = 0.89;
        SelectionDecalMaterial::radius = 0.90;

        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ZERO);
    }
    else if(pass == 1)
    {
        ((SelectionDecalMaterial*)material)->alpha = 1.0;
        SelectionDecalMaterial::radius = pre ? 0.95 : 1;

        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_OFFSET_FILL);

        glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    }
    else if(pass == 2)
    {
        ((SelectionDecalMaterial*)material)->alpha = 1.0;
        SelectionDecalMaterial::radius = 0.90;

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
    auto tankPos = tank->getPosition();

    int xc = tankPos.x, yc = tankPos.y;

    std::vector<Vertex3> vs;
    for(int y = yc-1; y < yc+3; y++)
    {
        for(int x = xc-1; x < xc+3; x++)
        {
            auto pos = scene->getTerrain()->getPoint(x, y);
            pos.z += 0.01;
            vs.push_back({ pos.x, pos.y, pos.z, 0, 0, 1, real(x)-tankPos.x, real(y)-tankPos.y });
        }
    }

    std::vector<int> triangles;

    for(int y = 0; y < 3; y++)
    {
        for(int x = 0; x < 3; x++)
        {
            int i1 = y*4+x, i2 = y*4+x+1, i3 = (y+1)*4+x+1, i4 = (y+1)*4 + x;
            triangles.insert(triangles.end(), { i1, i2, i3, i1, i3, i4 });
        }
    }
    return { vs, triangles };
}


void SelectionMarkerMesh::init()
{
    auto [vs, triangles] = calcVertices(scene);

    auto material = new SelectionDecalMaterial({ 0, 0.8, 0.1 });

    v = vs;
    this->triangles = triangles;
    this->material = material;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3)*v.size(), v.data(), GL_STREAM_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
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
