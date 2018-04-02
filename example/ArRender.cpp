#include "Renderer.h"
#include "Model3D.h"
#include "Camera.h"
#include <iostream>

int main(int argc, char * argv[])
{
	if (argc < 3)
	{
		std::cout << "Too few parameters in call !" << std::endl;
		return 0;
	}
	Model3D model3d(argv[1]);
	if (model3d.isEmpty())
	{
		std::cout << "Fail to load 3d model !" << std::endl;
		return 0;
	}
	Camera camera(argv[2]);
	if (camera.isEmpty())
	{
		std::cout << "Fail to load camera !" << std::endl;
		return 0;
	}
	camera.setCamera(0);
	Renderer renderer(camera, model3d);
	renderer.setBackground(camera.getNextImage());
	renderer.initialize(argc, argv);
	renderer.startRendering();
	return 0;
}