#include "bounding_box.h"

namespace Eon
{
	BoundingBox::BoundingBox(glm::vec3 min, glm::vec3 max)
	{
		this->min = min;
		this->max = max;
	}

	bool BoundingBox::InBounds3(glm::vec3 position) const
	{
		return position.x < max.x && position.x >= min.x && position.y < max.y && position.y >= min.y && position.z < max.z && position.z >= min.z;
	}

	bool BoundingBox::InBounds2(glm::vec2 position) const
	{
		return InBounds3(glm::vec3(position.x, min.y, position.y));
	}

	BoundingBox BoundingBox::Expand(glm::vec3 amount) const
	{
		float x0 = min.x;
		float y0 = min.y;
		float z0 = min.z;
		float x1 = max.x;
		float y1 = max.y;
		float z1 = max.z;

		if (amount.x < 0.0f)
		{
			x0 += amount.x;
		}
		else if (amount.x < 0.0f)
		{
			x1 += amount.x;
		}

		if (amount.y < 0.0f)
		{
			y0 += amount.y;
		}
		else if (amount.y > 0.0f)
		{
			y1 += amount.y;
		}

		if (amount.z < 0.0f)
		{
			z0 += amount.z;
		}
		else if (amount.z > 0.0f)
		{
			z1 += amount.z;
		}

		return BoundingBox(glm::vec3(x0, y0, z0), glm::vec3(x1, y1, z1));
	}

	float BoundingBox::ClipCollideX(BoundingBox& bb, float xv) const
	{
		if (bb.max.y > min.y && bb.min.y < max.y)
		{
			if (bb.max.z > min.z && bb.min.z < max.z)
			{
				float max;
				if (xv > 0.0f && bb.max.x <= min.x)
				{
					max = min.x - bb.max.x;
					if (max < xv)
					{
						xv = max;
					}
				}

				if (xv < 0.0F && bb.min.x >= this->max.x)
				{
					max = this->max.x - bb.min.x;
					if (max > xv)
					{
						xv = max;
					}
				}
			}
		}

		return xv;
	}

	float BoundingBox::ClipCollideY(BoundingBox& bb, float yv) const
	{
		if (bb.max.x > min.x && bb.min.x < max.x)
		{
			if (bb.max.z > min.z && bb.min.z < max.z)
			{
				float max;
				if (yv > 0.0F && bb.max.y <= min.y)
				{
					max = min.y - bb.max.y;
					if (max < yv)
					{
						yv = max;
					}
				}

				if (yv < 0.0F && bb.min.y >= this->max.y)
				{
					max = this->max.y - bb.min.y;
					if (max > yv)
					{
						yv = max;
					}
				}
			}
		}

		return yv;
	}

	float BoundingBox::ClipCollideZ(BoundingBox& bb, float zv) const
	{
		if (bb.max.x > min.x && bb.min.x < max.x)
		{
			if (bb.max.y > min.y && bb.min.y < max.y)
			{
				float max;
				if (zv > 0.0F && bb.max.z <= min.z)
				{
					max = min.z - bb.max.z;
					if (max < zv)
					{
						zv = max;
					}
				}

				if (zv < 0.0F && bb.min.z >= this->max.z)
				{
					max = this->max.z - bb.min.z;
					if (max > zv)
					{
						zv = max;
					}
				}
			}
		}

		return zv;
	}

	void BoundingBox::Move(glm::vec3 amount)
	{
		min += amount;
		max += amount;
	}
}
