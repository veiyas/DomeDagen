#include"balljointconstraint.hpp"

#include<glm/gtc/quaternion.hpp>
#include<glm/gtx/quaternion.hpp>
#include<sgct/log.h>
#include<glm/gtx/string_cast.hpp>
#include<iostream>
#include<cassert>

BallJointConstraint::BallJointConstraint()
{
	//TEMPORARY STUFF/////////////////
	//mPlanePos 
	mPlanePos = glm::vec3(0.f, 0.f, -0.1f);
	mZ = glm::vec3(0.f, 0.f, 1.f); // ???? should be negated in calculations probbaly

	/////////////////////////////////
}

BallJointConstraint::BallJointConstraint(float fov, float tilt)
	: BallJointConstraint{ }
{
	setConstraint(fov, tilt);
}

void BallJointConstraint::setConstraint(float fov, float tilt)
{
	//The distance from origin to the plane (at the closest point)
	float distance = cos(glm::radians(fov / 2)); //TODO funkar ej för störa vinklar nog eller kankse?

	float tiltRad = glm::radians(tilt);

	//Unit vector specifying direction of the vector defining the plane
	glm::vec3 direction(0.f, cos(tiltRad), -sin(tiltRad));

	mPlanePos = direction * distance;

	//?
	mZ = direction; //Is needed to avoid problems if distance=0 and also fov>180 (i think)

	//mY = ...

	std::cout << "mPlanePos=" << glm::to_string(mPlanePos)
	          << " | mZ=" << glm::to_string(mZ) << '\n';
}

void BallJointConstraint::setConstraint(glm::vec3 direction, float distance)
{
	//TODO fix all this, cause its probably not right for negative directions?
	mZ = glm::normalize(direction);
	mPlanePos = direction * mZ;
}

void BallJointConstraint::clamp(glm::quat& q) const
{
	using namespace glm;

	//Unit vector pointing at the Object
	const vec3 direction = q * vec3(0.f, 0.f, -1.f);

	if (!isInRange(direction))
	{
		//Calculate the closest 

		sgct::Log::Info("OUTSIDE ALLOWED AREA");

		//Calculate the closest allowed position
		float scale = sqrt(1.0f - length(mPlanePos) * length(mPlanePos));

		vec3 hej = direction - (dot(direction, mPlanePos) / (length(mPlanePos) * length(mPlanePos))) * mPlanePos;

		vec3 newPos = (scale * normalize(hej)) + mPlanePos;

		vec3 up = q * vec3(0.f, 1.f, 0.f); //this should be close enough hopefully

		//Set q to the calculated position
		q = quatLookAt(newPos, up);
	}
	else
	{
		//std::cout << to_string(direction) << ' ';
		sgct::Log::Info("inside ALLOWED AREA");
	}
}

bool BallJointConstraint::isInRange(glm::vec3 direction) const
{
	glm::vec3 relation = direction - mPlanePos;
	float dotProd = glm::dot(relation, mZ);

	return !(dotProd < 0);
}


// Useful debugging code:

//std::cout << to_string(newPos) << " with length: " << length(newPos) << '\n';

//std::cout << scale << '\n';
