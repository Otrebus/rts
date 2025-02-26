#include "Entity.h"

class Rock : public Entity
{
public:
    Rock(Vector3 pos, Vector3 dir, Vector3 up, Terrain* terrain);
    void updateUniforms();
	static void loadModels();
    void draw(Material* mat = nullptr);
    void update(real dt);
    void init(Scene* scene);

    void setPosition(Vector3 pos);
    void setDirection(Vector3 dir, Vector3 up);

    Model3d* model;

    static BoundingBox rockBoundingBox;
};
