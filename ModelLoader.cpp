#include "ModelLoader.h"
#include <fstream>
#include "Parser.h"
#include <string>


class ObjVertex
{
public:
    ObjVertex(Vector3 position, Vector3 normal, Vector2 texture) : position(position), normal(normal), texture(texture) {}

    Vector3 position;
    Vector3 normal;
    Vector2 texture;

    std::vector<ObjTriangle*> triangles;
};

class ObjTriangle
{
public:
    ObjTriangle(ObjVertex* v0, ObjVertex* v1, ObjVertex* v2) : v0(v0), v1(v1), v2(v2) {}
    ObjVertex *v0, *v1, *v2;

    Vector3 getNormal() const
    {
        Vector3 normal = (v1->position-v0->position)%(v2->position-v0->position);
        if(!normal)
            return Vector3(0, 0, 1);
        else
        {
            normal.normalize();
            return normal;
        }
    }
};


/**
 * Reads a Wavefront .mtl file.
 *
 * @throws ParseException If the file was badly formed.
 * @param file The name of the obj file.
 * @param matefilestr The name of the materials file.
 * @returns A map from the name of the material to the material object.
 */
std::map<std::string, Material*> readMaterialFile(Model3d* model, const std::string& matfilestr)
{
    std::ifstream matfile;
    matfile.open(matfilestr.c_str(), std::ios::in);
    std::map<std::string, Material*> materials;

    if(matfile.fail())
    {
        matfile.close();
        throw ParseException("Couldn't open the file \"" + matfilestr + "\"");
    }

    std::string str;
    auto parser = Parser(tokenize(matfile, str));

    LambertianMaterial* curmat = nullptr;
    bool phong = false, emissive = false;
    while(!parser.accept(Token::Eof))
    {
        if(parser.accept(Token::Newline))
            while(parser.accept(Token::Newline));

        else if(acceptAnyCaseStr(parser, "newmtl"))
        {
            std::string matName = std::string(expectStr(parser));
            curmat = new LambertianMaterial;
            materials[matName] = curmat;
            phong = true;
        }

        else if(acceptAnyCaseStr(parser, "ka"))
        {
            // We ignore any ambient term
            for(int i = 0; i < 3; i++)
                expectReal(parser);

            if(!curmat)
                throw ParseException("No current material specified"); // TODO: not really a parse exception
        }
        else if(acceptAnyCaseStr(parser, "tf"))
        {
            // We ignore any transmission filter
            expectReal(parser);

            if(!curmat)
                throw ParseException("No current material specified"); // TODO: not really a parse exception
        }
        else if(acceptAnyCaseStr(parser, "d"))
        {
            // We ignore any optical density
            expectReal(parser);

            if(!curmat)
                throw ParseException("No current material specified"); // TODO: not really a parse exception
        }
        else if(acceptAnyCaseStr(parser, "tr"))
        {
            // We ignore any transparency
            expectReal(parser);

            if(!curmat)
                throw ParseException("No current material specified"); // TODO: not really a parse exception
        }
        else if(acceptAnyCaseStr(parser, "kd"))
        {
            if(!curmat)
                throw ParseException("No current material specified"); // TODO: not really a parse exception
            if(phong)
                curmat->Kd = expectVector3d(parser);
            else if(!emissive)
                throw ParseException("Kd specified for custom material");
        }
        else if(acceptAnyCaseStr(parser, "ks"))
        {
            if(!curmat)
                throw ParseException("No current material specified"); // TODO: not really a parse exception
            if(phong)
                expectVector3d(parser);
            else if(!emissive)
                throw ParseException("Ks specified for custom material");
        }
        else if(acceptAnyCaseStr(parser, "ns"))
        {
            if(!curmat)
                throw ParseException("No current material specified"); // TODO: not really a parse exception
            if(phong)
                expectReal(parser);
            else if(!emissive)
                throw ParseException("Ns specified for custom material");
        }
        else if(acceptAnyCaseStr(parser, "ke"))
            expectVector3d(parser);
        else if(acceptAnyCaseStr(parser, "ni"))
            expectReal(parser);
        else if(acceptAnyCaseStr(parser, "illum"))
            expectInt(parser);
        else
        {
            auto token = parser.peek();
            throw ParseException("Unknown token \"" + std::string(token.str) + "\"", token);
        }
    }
    return materials;
}


void readFromFile(Model3d* model, const std::string& file)
{
    Material* curmat = nullptr;
    std::ifstream myfile;
    myfile.open(file.c_str(), std::ios::in);

    std::map<std::string, Material*> materials;

    std::string str;
    int currentSmoothingGroup = 0;

    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2> textureCoords;

    std::unordered_map<int, std::vector<ObjVertex*>> smoothingVertices[33]; // The vertices that have been added to the current smoothing group so far
    std::vector<ObjTriangle*> smoothingTriangles[33];

    std::vector<Mesh3d> mesh;

    auto getOrMakeVertex = [&smoothingVertices, &positions, &normals, &textureCoords](int group, int position, int normal, int tex)
        {
            for(auto v : smoothingVertices[group][position])
                if((v->texture == (tex ? textureCoords[tex-1] : Vector2(0, 0))) && v->normal == (normal ? normals[normal-1] : Vector3(0, 0, 0)))
                    return v;

            auto v = new ObjVertex(positions[position-1], normal ? normals[normal-1] : Vector3(0, 0, 0), tex ? textureCoords[tex-1] : Vector2(0, 0));
            smoothingVertices[group][position].push_back(v);
            return v;
        };

    auto addMesh = [&smoothingTriangles, &smoothingVertices, model, &curmat]()
        {
            std::vector<ObjVertex*> vertices[33];

            for(int i = 0; i < 33; i++)
            {
                Mesh3d* mesh = new Mesh3d;
                if(!curmat)
                    curmat = new LambertianMaterial(Vector3(0.7f, 0.7f, 0.7f));
                mesh->material = curmat;
                std::vector<Vertex3> vertices;
                std::vector<int> indices;
                std::unordered_map<ObjVertex*, int> vertMap;

                for(auto& t : smoothingTriangles[i])
                {
                    for(auto& v :{ t->v0, t->v1, t->v2 })
                    {
                        auto normal = v->normal;
                        if(!normal)
                            normal = ((t->v1->position-t->v0->position)%(t->v2->position-t->v0->position)).normalized();

                        if(vertMap.find(v) == vertMap.end())
                        {
                            vertMap[v] = int(vertices.size());
                            vertices.emplace_back(v->position, normal, v->texture);
                        }
                        indices.push_back(vertMap[v]);
                    }
                    delete t;
                }

                if(indices.size())
                {
                    for(auto& p : smoothingVertices[i])
                    {
                        for(auto& v : p.second)
                            delete v;
                        p.second.clear();
                    }
                    smoothingVertices[i].clear();
                    smoothingTriangles[i].clear();

                    mesh->v = vertices;
                    mesh->triangles = indices;

                    if(mesh->triangles.size())
                        model->addMesh(*mesh);
                }
                mesh->nTriangles = int(mesh->triangles.size());
            }
        };

    try
    {
        if(myfile.fail())
        {
            myfile.close();
            throw ParseException("Can't open the given .obj file \"" + file + "\"");
        }

        Parser parser(tokenize(myfile, str));

        while(!parser.accept(Token::Eof))
        {
            if(parser.accept(Token::Newline))
                while(parser.accept(Token::Newline));

            if(parser.accept("f"))
            {
                std::vector<ObjVertex*> faceVertices;

                while(true)
                {
                    auto [success, v, t, n] = acceptVertex(parser);
                    if(!success)
                        break;

                    if(v < 0)
                        v = (int)positions.size() + v + 1;

                    auto vertex = getOrMakeVertex(currentSmoothingGroup, v, n, t);
                    faceVertices.push_back(vertex);
                }

                for(int i = 0; i < (int)faceVertices.size()-2; i++)
                {
                    auto pv0 = faceVertices[0], pv1 = faceVertices[i+1], pv2 = faceVertices[i+2];

                    ObjTriangle* tri = new ObjTriangle(pv0, pv1, pv2);

                    smoothingTriangles[currentSmoothingGroup].push_back(tri);
                    for(auto& p :{ pv0, pv1, pv2 })
                        p->triangles.push_back(tri);
                }
            }
            else if(parser.accept("g") || parser.peek() == Token::Eof)
            {
                // We don't care about the name of the group
                for(auto p = acceptStr(parser); std::get<0>(p); p = acceptStr(parser));

                for(int i = 1; i < 33; i++)
                {
                    for(auto& [_, vs] : smoothingVertices[i])
                    {
                        for(auto& v : vs)
                        {
                            v->normal = Vector3(0, 0, 0);
                            for(auto& tri : v->triangles)
                                v->normal += tri->getNormal();

                            v->normal /= real(v->triangles.size());
                        }
                    }
                }
                for(auto& t : smoothingTriangles[0])
                {
                    if(!t->v0->normal)
                        t->v0->normal = t->getNormal();
                    if(!t->v1->normal)
                        t->v1->normal = t->getNormal();
                    if(!t->v2->normal)
                        t->v2->normal = t->getNormal();
                }

                addMesh();
            } // if(a == "g" ..
            else if(parser.accept("mtllib"))
            {
                // Check if there's an associated materials file, and parse it
                auto matfilestr = expectStr(parser);
                materials = readMaterialFile(model, std::string(matfilestr));
                continue;
            }
            else if(parser.accept("vn"))
                normals.push_back(expectVector3d(parser));
            else if(parser.accept("o"))
                auto str = expectStr(parser);
            else if(parser.accept("vt"))
                textureCoords.push_back(expectVtCoordinate(parser));
            else if(parser.accept("s")) // Smoothing group ending or starting
            {
                auto [success, s1] = acceptInt(parser);
                if(success)
                    currentSmoothingGroup = s1;
                else
                {
                    auto s1 = expectStr(parser);
                    if(s1 != "off")
                        throw ParseException("Expected 'off'");
                    currentSmoothingGroup = 0;
                }
            }
            else if(parser.accept("v"))
                positions.push_back(expectVector3d(parser));
            else if(parser.accept("usemtl"))
            {
                addMesh();
                auto mtl = std::string(expectStr(parser));
                auto it = materials.find(mtl);
                if(it == materials.end())
                    curmat = 0;
                else
                    curmat = materials[mtl];
            }
            else if(parser.accept(Token::Newline));
            else if(parser.accept("l"))
                while(parser.next().type != Token::Newline);
            else
            {
                auto token = parser.peek();
                throw ParseException("Unknown token \"" + std::string(token.str) + "\"", token.line, token.column);
            }
        }
    }
    catch(const ParseException& p)
    {
        logger.box(p.message);
        __debugbreak();
    }
}