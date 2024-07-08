#include "Entity.h"

class TankWreck : public Entity
{
public:
    TankWreck(Vector3 pos, Vector3 dir, Vector3 up, Terrain* terrain);
    void updateUniforms();
	static void loadModels();
    void draw(Material* mat = nullptr);
    void drawTurret(Material* mat = nullptr);
    void update(real dt);
    void init(Scene* scene);

    void setPosition(Vector3 pos);
    void setDirection(Vector3 dir, Vector3 up);
    
protected:
    friend class Tank;
    Model3d* body;
    Model3d* turret;
    Model3d* gun;

	static real gunLength;
    real gunRecoilPos;
    Vector3 turretDir; // Relative to forward/up of the tank
    Vector3 turretPos;
    Vector3 gunPos;
    Vector3 absGunDir;
    Vector3 absGunUp;
    Vector3 absGunPos;
    Vector3 absTurUp;
    Vector3 absTurDir;
    Vector3 absTurPos;
    Vector3 absMuzzlePos;
    static Model3d* tankWreckBoundingBoxModel;
    static BoundingBox tankWreckBoundingBox;
};

static Model3d* tankWreckBoundingBoxModel;
static BoundingBox tankWreckBoundingBox;
