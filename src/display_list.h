#pragma once

#include <glad/glad.h>

namespace Eon {
	class DisplayList {
	public:
		DisplayList();
		~DisplayList();
		
		void Begin() const;
		void End();

		void Call() const;

		void Destroy();
	private:
		unsigned int id;
		bool destroyed;
	};
}