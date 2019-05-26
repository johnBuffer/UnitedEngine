#pragma once

#include "constraint_solver.hpp"

namespace up
{

class SolidSegment
{
public:
	SolidSegment(BodyPtr b1, BodyPtr b2) :
		m_constraint(b1, b2)
	{}

	const Vec2 getVec() const
	{
		return (m_constraint.body2->position - m_constraint.body1->position);
	}

	const Vec2 getDir() const
	{
		Vec2 v(getVec());
		v.normalize();

		return v;
	}

	void update(float dt)
	{
		m_constraint.update(dt);
	}

	const Vec2& getClosestPointTo(const Vec2& pt) const
	{
		const float l1((pt - m_constraint.position1()).length());
		const float l2((pt - m_constraint.position2()).length());

		if (l1 < l2) return m_constraint.position1();

		return m_constraint.position2();
	}

	const Vec2& getPointProjection(const Vec2& point) const
	{
		Vec2 v(getVec());
		const float length(v.length());


	}

	void solveCollisions(SolidSegment& segment)
	{
		const Vec2& p1 = m_constraint.position1();
		const Vec2& p2 = m_constraint.position2();
		const Vec2& p3 = segment.m_constraint.position2();
		const Vec2& p4 = segment.m_constraint.position2();

		const Intersection inter(p1, p2, p3, p4);
		Vec2 inter_pt1;
		Vec2 inter_pt2;

		if (inter.cross)
		{
			Vec2 inter_pt1 = this->getClosestPointTo(inter.point);
			Vec2 inter_pt2 = segment.getClosestPointTo(inter.point);

			float d1((inter.point - inter_pt1).length());
			float d2((inter.point - inter_pt2).length());

			if (d1 < d2)
			{
				const Vec2 v = inter.point - inter_pt1;
				m_constraint.body1->move(v);
				m_constraint.body2->move(v);
			} else {
				const Vec2 v = inter.point - inter_pt2;
				segment.m_constraint.body1->move(v);
				segment.m_constraint.body2->move(v);
			}
		}
	}

private:
	Constraint m_constraint;
};

}
