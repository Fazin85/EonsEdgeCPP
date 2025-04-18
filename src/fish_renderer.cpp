#include "fish_renderer.h"

namespace Eon {
	FishRenderer::FishRenderer() : fish_front_texture(nullptr), fish_bottom_texture(nullptr), fish_front_side_texture(nullptr), fish_top_texture(nullptr)
	{
		Image fishFrontImage("fish_front.png");
		Image fishBottomImage("fish_bottom.png");
		Image fishFrontSideImage("fish_front_side.png");
		Image fishTopImage("fish_top.png");

		fish_front_texture = std::make_unique<Texture>(fishFrontImage, true);
		fish_bottom_texture = std::make_unique<Texture>(fishBottomImage, true);
		fish_front_side_texture = std::make_unique<Texture>(fishFrontSideImage, true);
		fish_top_texture = std::make_unique<Texture>(fishTopImage, true);

		top_fin_list.Begin(); {
			fish_front_texture->Bind();
			glBegin(GL_QUADS); {
				cubeRenderer.FrontFace();
				cubeRenderer.BackFace();
				cubeRenderer.LeftFace();
				cubeRenderer.RightFace();
			}
			glEnd();

			fish_top_texture->Bind();
			glBegin(GL_QUADS); {
				cubeRenderer.TopFace();
			}
			glEnd();
		}
		top_fin_list.End();

		generic_list.Begin(); {
			fish_front_texture->Bind();
			glBegin(GL_QUADS); {
				cubeRenderer.FrontFace();
				cubeRenderer.BackFace();
				cubeRenderer.LeftFace();
				cubeRenderer.RightFace();
			}
			glEnd();

			fish_top_texture->Bind();
			glBegin(GL_QUADS); {
				cubeRenderer.TopFace();
			}
			glEnd();

			fish_bottom_texture->Bind();
			glBegin(GL_QUADS); {
				cubeRenderer.BottomFace();
			}
			glEnd();
		}
		generic_list.End();

		head_list.Begin(); {
			fish_front_texture->Bind();
			glBegin(GL_QUADS); {
				cubeRenderer.FrontFace();
				cubeRenderer.BackFace();
			}
			glEnd();

			fish_front_side_texture->Bind();
			glBegin(GL_QUADS); {
				cubeRenderer.LeftFace();
				cubeRenderer.RightFace();
			}
			glEnd();

			fish_top_texture->Bind();
			glBegin(GL_QUADS); {
				cubeRenderer.TopFace();
			}
			glEnd();

			fish_bottom_texture->Bind();
			glBegin(GL_QUADS); {
				cubeRenderer.BottomFace();
			}
			glEnd();
		}
		head_list.End();
	}

	void FishRenderer::Render(Camera& camera, glm::vec3 playerPos, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, glm::vec3 color, float time)
	{
		glUseProgram(0);
		glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_PROJECTION);
		auto& proj = *camera.ProjectionMatrix();
		glLoadMatrixf(&proj[0][0]);

		camera.CalculateViewMatrix(playerPos);
		glMatrixMode(GL_MODELVIEW);
		auto& view = *camera.ViewMatrix();
		glLoadMatrixf(&view[0][0]);

		glTranslatef(pos.x, pos.y, pos.z);

		glRotatef(rot.x, 1.0f, 0.0f, 0.0f);
		glRotatef(rot.y, 0.0f, 1.0f, 0.0f);
		glRotatef(rot.z, 0.0f, 0.0f, 1.0f);

		glScalef(scale.x, scale.y, scale.z);

		glColor3f(color.x, color.y, color.z);

		const glm::vec3 bodySize = { 0.25f, 0.35f, 1.15f };

		//body
		glPushMatrix(); {
			glScalef(bodySize.x, bodySize.y, bodySize.z);

			generic_list.Call();
		}
		glPopMatrix();

		//top fin
		glPushMatrix(); {
			const glm::vec3 topFinSize = { 0.01f, 0.2f, 0.45f };
			glTranslatef(0.0f, bodySize.y, bodySize.z / 5.0f);
			glScalef(topFinSize.x, topFinSize.y, topFinSize.z);

			top_fin_list.Call();
		}
		glPopMatrix();

		//tail
		glPushMatrix(); {
			const glm::vec3 tailSize = { 0.05f, 0.3f, 0.45f };
			glTranslatef(0, 0, -bodySize.z);
			glRotatef(std::sinf(time) * 6.8f, 0.0f, 1.0f, 0.0f);
			glTranslatef(0, 0, -(tailSize.z / 2.0f));
			glScalef(tailSize.x, tailSize.y, tailSize.z);

			generic_list.Call();
		}
		glPopMatrix();

		const glm::vec3 finSize = { 0.35f, 0.05f, 0.15f };

		// Left front fin
		glPushMatrix(); {
			glTranslatef(-bodySize.x, -bodySize.y + bodySize.y / 5.0f, 0.5f);
			glRotatef(-std::sinf(time) * 6.8f, 0.0f, 0.0f, 1.0f);
			glTranslatef(-finSize.x / 2.0f, 0.0f, 0.0f);
			glScalef(finSize.x, finSize.y, finSize.z);

			generic_list.Call();
		}
		glPopMatrix();

		// Right front fin
		glPushMatrix(); {
			glTranslatef(bodySize.x, -bodySize.y + bodySize.y / 5.0f, 0.5f);
			glRotatef(std::sinf(time) * 6.8f, 0.0f, 0.0f, 1.0f);
			glTranslatef(finSize.x / 2.0f, 0.0f, 0.0f);
			glScalef(finSize.x, finSize.y, finSize.z);

			generic_list.Call();
		}
		glPopMatrix();

		//head
		glPushMatrix(); {
			const glm::vec3 headSize = { bodySize.x, 0.275f, 0.25f };
			glTranslatef(0, (bodySize.y / 2.0f) - (headSize.y / 2.0f), bodySize.z + (headSize.z / 2.0f));
			glScalef(headSize.x - 0.1f, headSize.y, headSize.z);

			head_list.Call();
		}
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
	}
}
