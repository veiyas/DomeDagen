#pragma once

#include<glm/glm.hpp>

//A class handling a ball joint constraint on quaternions
//The constraint is defined by ... which define a plane 

//          _.-""""-._
// _______.'____|_____`._____
//       /      |       \
//      |       |        |
//      |       x        |   (Pretend this is a circle)
//      |                |
//       \              /
//        `._        _.'
//           `-....-'
//Thus if (and only if) the ... is above the plane, the ... is in the allowed range,


//Assuming direction of a quat is a rotation of the vector (0,0,-1)

//Somewhat based on https://www.youtube.com/watch?v=nDKFnqUZ1BI
class BallJointConstraint
{
public:
	BallJointConstraint();

	BallJointConstraint(float fov, float tilt);

	//Sets the constraints from fov and tilt as
	//they are defined in SGCT fisheye config
	void setConstraint(float fov, float tilt);

	//
	void setConstraint(glm::vec3 direction, float distance);

	//??? would modifying ref directly be better?
	//Clamps q to the allowed range
	void clamp(glm::quat& q) const;

private:
	//TODO most of these are not used or needed probably

	bool isInRange(glm::vec3 direction) const;
	//glm::vec3 closestAllowedQuat(glm::vec3 direction) const;

	//3 axis defining plane
	glm::vec3 mX, mY, mZ;


	//
	glm::vec3 mPlanePos;
};

