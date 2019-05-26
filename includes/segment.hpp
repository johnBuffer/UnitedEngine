#pragma once

#include "constraint.hpp"

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
		return (m_constraint.body2->position() - m_constraint.body1->position());
	}

	const Vec2 getDir() const
	{
		Vec2 v(getVec());
		v.normalize();

		return v;
	}

	const Vec2& getBody1Position() const
	{
		return m_constraint.position1();
	}

	const Vec2& getBody2Position() const
	{
		return m_constraint.position2();
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

	const Vec2 getPointProjection(const Vec2& point) const
	{
		// B1 to B2
		Vec2 segment_vec(getVec());
		const float length(segment_vec.length());
		segment_vec.normalize();
		// B1 to P
		const Vec2& b1p(getBody1Position());
		Vec2 point_vec(point - b1p);
		// Projection of B1->P on B1->B2
		float dot(segment_vec.dot(point_vec));

		if (dot < 0.0f) {
			return b1p;
		} else if (dot > length) {
			return getBody2Position();
		}

		return b1p + (dot * segment_vec);
	}

	void collideWith(up::Body& body)
	{
		if (&body == &(*m_constraint.body1) || &body == &(*m_constraint.body2)) {
			return;
		}

		const Vec2& body_position(body.position());
		const Vec2& segment_closest_point(getPointProjection(body_position));

		Vec2 col_vec(body_position - segment_closest_point);
		const float distance(col_vec.length());

		const float body_radius(body.radius());
		if (distance < 2 * body_radius) {
			const float delta(0.5f * (2.0f * body_radius - distance));

			const float distFromB1((segment_closest_point - getBody1Position()).length());
			col_vec.normalize();
			body.move(delta * col_vec);
			moveAt(-delta * col_vec, distFromB1);
		}
	}

	void moveAt(const Vec2& v, float distFromB1)
	{
		const float ratio1(distFromB1 / m_constraint.length());
		const float ratio2(1.0f - ratio1);

		m_constraint.body1->move(ratio2 * v);
		m_constraint.body2->move(ratio1 * v);
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

using SolidSegmentPtr = fva::Handle<SolidSegment>;

}
