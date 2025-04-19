#include "display_list.h"

namespace Eon {
	DisplayList::DisplayList()
	{
		id = glGenLists(1);
		destroyed = false;
	}

	DisplayList::~DisplayList()
	{
		Destroy();
	}

	void DisplayList::Begin() const
	{
		glNewList(id, GL_COMPILE);
	}

	void DisplayList::End()
	{
		glEndList();
	}

	void DisplayList::Call() const
	{
		glCallList(id);
	}

	void DisplayList::Destroy()
	{
		if (!destroyed) {
			glDeleteLists(id, 1);
			destroyed = true;
		}
	}
}
