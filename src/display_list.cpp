#include "display_list.h"

namespace Eon {
	DisplayList::DisplayList()
	{
		id = glGenLists(1);
	}

	DisplayList::~DisplayList()
	{
		glDeleteLists(id, 1);
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
}
