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
	private:
		unsigned int id;
	};
}