#include "bounding_box.h"

namespace Eon
{
	BoundingBox::BoundingBox(glm::dvec3 min, glm::dvec3 max)
	{
		this->min = min;
		this->max = max;
	}

	bool BoundingBox::InBounds3(glm::dvec3 position) const
	{
		return position.x < max.x && position.x >= min.x && position.y < max.y && position.y >= min.y && position.z < max.z && position.z >= min.z;
	}

	bool BoundingBox::InBounds2(glm::dvec2 position) const
	{
		return InBounds3(glm::dvec3(position.x, min.y, position.y));
	}

	BoundingBox BoundingBox::Expand(glm::dvec3 amount) const
	{
		double x0 = min.x;
		double y0 = min.y;
		double z0 = min.z;
		double x1 = max.x;
		double y1 = max.y;
		double z1 = max.z;

		if (amount.x < 0.0)
		{
			x0 += amount.x;
		}
		else if (amount.x < 0.0)
		{
			x1 += amount.x;
		}

		if (amount.y < 0.0)
		{
			y0 += amount.y;
		}
		else if (amount.y > 0.0)
		{
			y1 += amount.y;
		}

		if (amount.z < 0.0)
		{
			z0 += amount.z;
		}
		else if (amount.z > 0.0)
		{
			z1 += amount.z;
		}

		return BoundingBox(glm::dvec3(x0, y0, z0), glm::dvec3(x1, y1, z1));
	}

	double BoundingBox::ClipCollideX(BoundingBox& bb, double xv) const
	{
		if (bb.max.y > min.y && bb.min.y < max.y)
		{
			if (bb.max.z > min.z && bb.min.z < max.z)
			{
				double max;
				if (xv > 0.0 && bb.max.x <= min.x)
				{
					max = min.x - bb.max.x;
					if (max < xv)
					{
						xv = max;
					}
				}

				if (xv < 0.0 && bb.min.x >= this->max.x)
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

	double BoundingBox::ClipCollideY(BoundingBox& bb, double yv) const
	{
		if (bb.max.x > min.x && bb.min.x < max.x)
		{
			if (bb.max.z > min.z && bb.min.z < max.z)
			{
				double max;
				if (yv > 0.0 && bb.max.y <= min.y)
				{
					max = min.y - bb.max.y;
					if (max < yv)
					{
						yv = max;
					}
				}

				if (yv < 0.0 && bb.min.y >= this->max.y)
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

	double BoundingBox::ClipCollideZ(BoundingBox& bb, double zv) const
	{
		if (bb.max.x > min.x && bb.min.x < max.x)
		{
			if (bb.max.y > min.y && bb.min.y < max.y)
			{
				double max;
				if (zv > 0.0 && bb.max.z <= min.z)
				{
					max = min.z - bb.max.z;
					if (max < zv)
					{
						zv = max;
					}
				}

				if (zv < 0.0 && bb.min.z >= this->max.z)
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

	void BoundingBox::Move(glm::dvec3 amount)
	{
		min += amount;
		max += amount;
	}
}
