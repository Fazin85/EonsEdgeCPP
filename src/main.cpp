#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "log.h"
#include "num.h"
#include "window.h"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 512
// levels have 32 chunks in x and z dimension
// levels are 512 blocks in all dimensions
#define LEVEL_CHUNKS_SIZE 32

int main()
{
	Eon::Log::Init();

	Eon::Window window(640, 480, "Eon's Edge");

	Eon::Camera cam(70, 8, window);

	while (!window.ShouldClose())
	{
		cam.Update(1);

		glClear(GL_COLOR_BUFFER_BIT);

		window.SwapBuffers();

		window.PollEvents();
	}

	return 0;
}
