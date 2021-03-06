#ifdef USE_D3D9
#include "D3d9World.h"
#else
#include "OpenGLWorld.h"
#endif
#include "BoxObject.h"
#include "GlfwContainer.h"
#ifdef _WIN32
#include "WglContainer.h"
#endif

using namespace std;

int main()
{
    CollisionShapes["0"] = NULL;

	int nBox;
	//printf("nBox: ");
	nBox = 10;
	//scanf("%d", &nBox);

	//printf("scale: ");
	PHYSICS_WORLD_SCALE = 1;
	//scanf("%f", &PHYSICS_WORLD_SCALE);

#ifdef USE_D3D9
	Win32Container container;
	D3d9World world(&container);
#else
    GlfwContainer container;
	//WglContainer container;
	OpenGLWorld world(&container);
#endif
    World::Global = &world;
	world.Init();

	BoxObject* box = new BoxObject[nBox];

    RDRVEC3 boxmin(-0.1f, -0.1f, -0.1f);
	RDRVEC3 boxmax( 0.1f,  0.1f,  0.1f);

	for (int i = 0; i < nBox; i++)
	{
#ifdef USE_OPENGL
		//box[i].SetProgramId(world.programId);
#endif
		box[i].SetMinMax(boxmin, boxmax);
		box[i].SetPosition(RDRVEC3(-1.0f + i*0.1f, 10.0f + i*1.0f, 0.0f), true);
		box[i].RotateGlobalX(ToRadian(44.0f), true);
		box[i].RotateGlobalY(ToRadian(10.0f), true);
		box[i].RotateGlobalZ(ToRadian(30.0f), true);
		box[i].BuildWorld();
		world.DrawableObjects.push_back(&box[i]);
		world.PhysicalObjects.push_back(&box[i]);
	}


	ModelObject obj("../suzanne.obj");
#ifdef USE_OPENGL
	//obj.SetProgramId(world.programId);
#endif
	obj.SetPosition(RDRVEC3(-0.7f, 1.0f, 0.5f), true);
	//obj.RotateGlobalY(ToRadian(180.0f), true);
	obj.BuildWorld();
	world.DrawableObjects.push_back(&obj);
	world.PhysicalObjects.push_back(&obj);


	Camera& camera = world.camera;
	camera.SetPosition(RDRVEC3(0.0f, 3.0f, 7.0f), true);
	//camera.RotateLocalY(ToRadian(180.0f), true);
	//camera.RotateLocalX(ToRadian(10), true);
	camera.BuildWorld();

	world.Render();

	delete[] box;

	return 0;
}
