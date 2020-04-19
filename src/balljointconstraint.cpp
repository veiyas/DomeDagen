#include"balljointconstraint.hpp"

#include<glm/gtc/quaternion.hpp>
#include<glm/gtx/quaternion.hpp>
#include<sgct/log.h>
#include<glm/gtx/string_cast.hpp>
#include<iostream>
#include<cassert>

BallJointConstraint::BallJointConstraint()
	: mPlanePos{ 0.f, 0.f, -1.0f }, mCenter{ 0.f, 0.f, -0.7f } { }

BallJointConstraint::BallJointConstraint(float fov, float tilt)
{
	float distance = cos(glm::radians(fov / 2));

	float tiltRad = glm::radians(tilt);
	glm::vec3 direction(0.f, cos(tiltRad), -sin(tiltRad));

	mPlanePos = direction * distance;
	mCenter = direction;

	//std::cout << "mPlanePos=" << glm::to_string(mPlanePos)
	//          << " | mZ=" << glm::to_string(mZ) << '\n';
}

void BallJointConstraint::apply(glm::quat& q) const
{
	if (!isInRange(q))
		q = closestAllowedQuat(q);
}

bool BallJointConstraint::isInRange(const glm::quat& q) const
{
	const glm::vec3 direction = q * glm::vec3(0.f, 0.f, -1.f);
	const glm::vec3 relation = direction - mPlanePos;
	float dotProd = glm::dot(relation, mCenter);

	return !(dotProd < 0);
}

glm::quat BallJointConstraint::closestAllowedQuat(const glm::quat& q) const
{
	using namespace glm;

	//Direction pointing to the object
	const vec3 dir = q * vec3(0.f, 0.f, -1.f);
	
	//The distance between the tip of mPlanePos and the surface of a unit sphere along a path
	//that is orthogonal to mPlanePos
	float scale = sqrt(1.0f - pow(length(mPlanePos), 2));

	//The projection of dir unto a plane with normal mPlanePos
	const vec3 proj = dir - (dot(dir, mPlanePos) / pow(length(mPlanePos), 2)) * mPlanePos;

	//Unit vector pointing at the new position
	const vec3 newPos = (scale * normalize(proj)) + mPlanePos;
	const vec3 up = q * vec3(0.f, 1.f, 0.f); //this should be close enough hopefully

	return quatLookAt(newPos, up);
}
