#include "render_state.h"

namespace Eon
{
	void RenderState::Apply() const
	{
		static RenderState lastState;

		if (static bool firstCall = true; firstCall)
		{
			firstCall = false;

			if (depth_test)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
			glDepthFunc(depth_function);

			if (blend)
			{
				glEnable(GL_BLEND);
				glBlendFunc(blend_src, blend_dst);
			}
			else
			{
				glDisable(GL_BLEND);
			}

			if (cull_face)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(cull_face_mode);
			}
			else
			{
				glDisable(GL_CULL_FACE);
			}

			lastState = *this;
			return;
		}

		if (depth_test != lastState.depth_test)
		{
			if (depth_test)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}

		if (depth_function != lastState.depth_function)
		{
			glDepthFunc(depth_function);
		}

		if (blend != lastState.blend)
		{
			if (blend)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
		}

		if (blend && (blend_src != lastState.blend_src || blend_dst != lastState.blend_dst))
		{
			glBlendFunc(blend_src, blend_dst);
		}

		if (cull_face != lastState.cull_face)
		{
			if (cull_face)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}

		if (cull_face && cull_face_mode != lastState.cull_face_mode)
		{
			glCullFace(cull_face_mode);
		}

		lastState = *this;
	}

	void RenderState::Reset()
	{
		shader = nullptr;
		depth_function = GL_LESS;
		depth_test = true;
		blend = false;
		blend_src = GL_SRC_ALPHA;
		blend_dst = GL_ONE_MINUS_SRC_ALPHA;
		cull_face = true;
		cull_face_mode = GL_BACK;
	}
}