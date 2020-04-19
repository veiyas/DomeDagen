#pragma once

#include<glm/glm.hpp>

//A class handling a ball joint constraint on quaternions
//Assuming direction of a quat is a rotation of the vector (0,0,-1)
class BallJointConstraint
{
public:
	BallJointConstraint();

	//Sets the constraints from fov and tilt as
	//they are defined in SGCT fisheye config
	BallJointConstraint(float fov, float tilt);

	//Clamps q to the allowed range
	void apply(glm::quat& q) const;

	//Test if q is in the allowed range
	bool isInRange(const glm::quat& q) const;

private:
	//Returns the closest quat that is in the allowed range, assuming q is outside,
	//i.e. returns the closest quat on the edge of the allowed range
	glm::quat closestAllowedQuat(const glm::quat& q) const;

	//Unit vector pointing in the direction of the center of the
	//allowed range
	glm::vec3 mCenter;

	//A vector going from origin to the nearest point on a orthogonal plane
	//whose intersection with the unit sphere defines the edge of the allowed range
	glm::vec3 mPlanePos;
};

