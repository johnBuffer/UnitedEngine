#pragma once

#include "constraint.hpp"
#include <iostream>

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

	void move(const Vec2& v)
	{
		m_constraint.body1->move(v);
		m_constraint.body2->move(v);
	}

	void collideWith(SolidSegment& segment)
	{
		const Vec2& p1 = m_constraint.position1();
		const Vec2& p2 = m_constraint.position2();
		const Vec2& p3 = segment.m_constraint.position1();
		const Vec2& p4 = segment.m_constraint.position2();

		const Intersection inter(p1, p2, p3, p4);
		if (inter.cross) {
			// Projections of segment on this
			const Vec2 proj1(getPointProjection(p3));
			const Vec2 proj2(getPointProjection(p4));
			// Projection of this on segment
			const Vec2 proj3(segment.getPointProjection(p1));
			const Vec2 proj4(segment.getPointProjection(p2));
			// Find min dist
			const Vec2 col_vec_1(proj1 - p3);
			const Vec2 col_vec_2(proj2 - p4);
			const Vec2 col_vec_3(p1 - proj3);
			const Vec2 col_vec_4(p2 - proj4);
			const float dist1(col_vec_1.length());
			float min_dist = dist1;
			const Vec2* min_vec(&col_vec_1);

			const float dist2(col_vec_2.length());
			if (dist2 < min_dist) {
				min_dist = dist2;
				min_vec = &col_vec_2;
			}
			const float dist3(col_vec_3.length());
			if (dist3 < min_dist) {
				min_dist = dist3;
				min_vec = &col_vec_3;
			}
			const float dist4(col_vec_4.length());
			if (dist4 < min_dist) {
				min_dist = dist4;
				min_vec = &col_vec_4;
			}

			if (min_dist > 0.001f) {
				segment.move(0.5f * (*min_vec));
				move(-0.5f * (*min_vec));
			}
		}
	}

private:
	Constraint m_constraint;
};

using SolidSegmentPtr = fva::Handle<SolidSegment>;

}
