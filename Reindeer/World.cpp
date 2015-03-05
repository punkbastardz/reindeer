#include "World.h"

#ifndef USE_OPENGL
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            } break;
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}
#else
#endif

World* World::Global = NULL;

World::World(Container* container)
{
	mouseRightButtonDown = false;
	mouseMiddleButtonDown = false;
	btClock cl;
	lastTimeMiddleMousePressed = cl.getTimeMilliseconds();
	this->container = container;
	container->AddListener(this);
}

World::~World()
{
    //dtor
}

void World::Init()
{
	InitWindow();
	Init3d();
}

void World::InitWindow()
{
#ifndef USE_OPENGL
    WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(0);
	wc.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);
	hWnd = CreateWindowEx(NULL, L"WindowClass", L"Reindeer", WS_OVERLAPPEDWINDOW, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, GetModuleHandle(0), NULL);
	hDC = GetDC(hWnd);

	ShowWindow(hWnd, SW_SHOWDEFAULT);
#else
    container->Init();
#endif
}

void World::Render()
{
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	collisionConfiguration->setConvexConvexMultipointIterations(3);
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0.0f * PHYSICS_WORLD_SCALE, -10.0f * PHYSICS_WORLD_SCALE, 0.0f * PHYSICS_WORLD_SCALE));

	ground = new BoxObject();
	ground->SetMinMax(RDRVEC3(-25.0f, -25.0f, -25.0f), RDRVEC3(25.0f, 25.0f, 25.0f));
	ground->SetPosition(RDRVEC3(0.0f, -25.0f, 0.0f));
	ground->SetMass(0.0f);
	dynamicsWorld->addRigidBody(ground->rigidBody);

	for (size_t i = 0; i < PhysicalObjects.size(); i++)
	{
		PhysicalObjects[i]->SetDynamicsWorld(dynamicsWorld);
		dynamicsWorld->addRigidBody(PhysicalObjects[i]->rigidBody);
	}

	btClock cl;
	btScalar currentTime = (btScalar)cl.getTimeMicroseconds() / 1000000.f;

#ifndef USE_OPENGL
	MSG msg;
	while(TRUE)
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(msg.message == WM_QUIT)
            break;
#else
	while (!container->ShouldClose()) {
#endif
        container->ReadInput();

		btScalar newTime = (btScalar)cl.getTimeMicroseconds() / 1000000.f;
		btScalar frameTime = newTime - currentTime;
		currentTime = newTime;

		dynamicsWorld->stepSimulation(frameTime, 7);

		for (size_t i = 0; i < PhysicalObjects.size(); i++)
		{
			PhysicalObjects[i]->Update();
		}

		PreUpdate();

		Draw();

		PostUpdate();

#ifndef USE_OPENGL
		SwapBuffers(hDC);
#else
        container->PostUpdate();
#endif
	}

	for (size_t i = 0; i < PhysicalObjects.size(); i++)
	{
		dynamicsWorld->removeRigidBody(PhysicalObjects[i]->rigidBody);
	}
	dynamicsWorld->removeRigidBody(ground->rigidBody);

	DrawableObjects.clear();
	PhysicalObjects.clear();

    delete ground;

	delete dynamicsWorld;
	delete solver;
	delete collisionConfiguration;
	delete dispatcher;
	delete broadphase;

	for (unordered_map<string, btCollisionShape*>::iterator it = CollisionShapes.begin(); it != CollisionShapes.end(); it++)
	{
		delete it->second;
	}
	CollisionShapes.clear();

	PostRender();

#ifdef _MSC_VER
#else
	glfwTerminate();
#endif
}

void World::Draw()
{
	for (size_t i = 0; i < DrawableObjects.size(); i++)
    {
		DrawableObjects[i]->Draw(&camera, light);
    }
    ground->Draw(&camera, light);
}

void World::Scrolled(double xoffset, double yoffset)
{
    RDRVEC3 pos = *camera.GetPosition();
	RDRVEC3 delta = (float)yoffset * glm::normalize(*camera.GetRotation() * AXIS_Z);
	camera.SetPosition(pos + delta);
}

void World::MouseLeftButtonPressed  (double x, double y)
{
}

void World::MouseLeftButtonReleased (double x, double y)
{
}

void World::MouseRightButtonPressed (double x, double y)
{
    mouseRightButtonDown = true;
    mouseX = x;
    mouseY = y;
}

void World::MouseRightButtonReleased(double x, double y)
{
    mouseRightButtonDown = false;
}

void World::MouseMiddleButtonPressed (double x, double y)
{
    mouseMiddleButtonDown = true;
    mouseX = x;
    mouseY = y;
}

void World::MouseMiddleButtonReleased(double x, double y)
{
    mouseMiddleButtonDown = false;
}

void World::MouseMoved(double x, double y)
{
    if (mouseRightButtonDown) {
        camera.RotateLocalX(ToRadian((mouseY - y) * 0.1f), true);
        camera.RotateGlobalY(ToRadian((mouseX - x) * 0.1f), true);
        camera.BuildWorld();
        mouseX = x;
        mouseY = y;
    }

    if (mouseMiddleButtonDown) {
        RDRVEC3 pos = *camera.GetPosition();
        RDRVEC3 delta = (float)(mouseX - x) * 0.01f * glm::normalize(*camera.GetRotation() * AXIS_X);
        delta += (float)(y - mouseY) * 0.01f * glm::normalize(*camera.GetRotation() * AXIS_Y);
        camera.SetPosition(pos + delta);
        mouseX = x;
        mouseY = y;
    }
}

void World::KeyPressed(int keyCode)
{
    if (keyCode == GLFW_KEY_LEFT)
        camera.RotateGlobalY(ToRadian(0.01f));

    if (keyCode == GLFW_KEY_RIGHT)
        camera.RotateGlobalY(ToRadian(-0.01f));

    if (keyCode == GLFW_KEY_UP)
        camera.RotateLocalX(ToRadian(0.01f));

    if (keyCode == GLFW_KEY_DOWN)
        camera.RotateLocalX(ToRadian(-0.01f));
}

void World::KeyReleased(int keyCode)
{
}
