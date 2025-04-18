#pragma once

#include "display_list.h"

namespace Eon {
	class CubeRenderer {
	public:
		CubeRenderer();

		void FrontFace();
		void BackFace();
		void LeftFace();
		void RightFace();
		void TopFace();
		void BottomFace();
		
	private:
		DisplayList front_face_list;
		DisplayList back_face_list;
		DisplayList left_face_list;
		DisplayList right_face_list;
		DisplayList top_face_list;
		DisplayList bottom_face_list;
	};
}