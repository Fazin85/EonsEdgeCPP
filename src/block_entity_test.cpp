#include "block_entity_test.h"

#include <glad/glad.h>

namespace Eon
{
	void BlockEntityTest::Render(const Camera& camera)
	{
		glUseProgram(0);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&camera.ProjectionMatrix()[0][0]);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&camera.ViewMatrix()[0][0]);
		glDisable(GL_CULL_FACE);

		glTranslatef(static_cast<float>(world_position.x), static_cast<float>(world_position.y), static_cast<float>(world_position.z));

		glBegin(GL_QUADS);
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			// FRONT
			glVertex3f(-0.5f, -0.5f, 0.5f);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.5f, -0.5f, 0.5f);
			glVertex3f(0.5f, 0.5f, 0.5f);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);
			// BACK
			glVertex3f(-0.5f, -0.5f, -0.5f);
			glVertex3f(-0.5f, 0.5f, -0.5f);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, -0.5f);
			glVertex3f(0.5f, -0.5f, -0.5f);

			glColor3f(0.0f, 1.0f, 0.0f);
			// LEFT
			glVertex3f(-0.5f, -0.5f, 0.5f);
			glVertex3f(-0.5f, 0.5f, 0.5f);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(-0.5f, 0.5f, -0.5f);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(-0.5f, -0.5f, -0.5f);
			// RIGHT
			glVertex3f(0.5f, -0.5f, -0.5f);
			glVertex3f(0.5f, 0.5f, -0.5f);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, 0.5f);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(0.5f, -0.5f, 0.5f);

			glColor3f(0.0f, 0.0f, 1.0f);
			// TOP
			glVertex3f(-0.5f, 0.5f, 0.5f);
			glVertex3f(0.5f, 0.5f, 0.5f);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, -0.5f);
			glVertex3f(-0.5f, 0.5f, -0.5f);
			glColor3f(1.0f, 0.0f, 0.0f);
			// BOTTOM
			glVertex3f(-0.5f, -0.5f, 0.5f);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(-0.5f, -0.5f, -0.5f);
			glVertex3f(0.5f, -0.5f, -0.5f);
			glVertex3f(0.5f, -0.5f, 0.5f);
		}
		glEnd();

		glEnable(GL_CULL_FACE);
	}
}
