#pragma once

#include "../shared_in.h"

//#define GLM_FORCE_SSE2
//#define GLM_FORCE_SSE3
//#define GLM_FORCE_AVX
//#define GLM_FORCE_AVX2
//#define GLM_FORCE_PRECISION_LOWP_BOOL
//#define GLM_FORCE_PRECISION_LOWP_INT
//#define GLM_FORCE_PRECISION_LOWP_UINT
//#define GLM_FORCE_PRECISION_LOWP_FLOAT
//#define GLM_FORCE_PRECISION_LOWP_DOUBLE
//#define GLM_FORCE_MESSAGES
#define GLM_FORCE_ALIGNED
#define GLM_ENABLE_EXPERIMENTAL

#include <vec3.hpp>
#include <vec4.hpp>
#include <mat4x4.hpp>

#include <gtx/transform.hpp>
#include <gtx/euler_angles.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>

inline glm::mat4 matProjection(float horFov, float aspect, float zNear, float zFar)
{
	return glm::perspective(glm::radians(horFov), aspect, zNear, zFar);
	//return glm::ortho(glm::radians(horFov), aspect, zNear, zFar);
}

inline glm::mat4 matView(CFLOAT3 pos, CFLOAT3 look, CFLOAT3 up = { 0, 1, 0 })
{
	return glm::lookAt(
		glm::vec3( pos.x,  pos.y,  pos.z),
		glm::vec3(look.x, look.y, look.z),
		glm::vec3(  up.x,   up.y,   up.z)
	);
}

inline glm::mat4 matModel(CFLOAT3 pos, CFLOAT3 root, CFLOAT3 size)
{
	glm::mat4 M = glm::mat4(1.0f);

	const auto rad_roots = glm::radians(glm::vec3(root.x, root.y, root.z));

	if (!math::equal(pos,  { 0.f, 0.f, 0.f })) M *= glm::translate(glm::vec3(pos.x, pos.y, pos.z));
	if (!math::equal(root, { 0.f, 0.f, 0.f })) M *= glm::toMat4(glm::quat(rad_roots)); //glm::yawPitchRoll(root.x, root.y, root.z);
	if (!math::equal(size, { 1.f, 1.f, 1.f })) M *= glm::scale(glm::vec3(size.x, size.y, size.z));
	//////////////////////////////////////////////////////////////////////////
	return M;
}