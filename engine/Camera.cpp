#include "Camera.h"
#include "Tools.h"
#include "Input.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

glm::vec3 Camera::GetPosition()
{
	return m_Position;
}

void Camera::LookAt(glm::vec3 pos, glm::vec3 target)
{
	m_Position = pos;
	m_Look = target - pos;
}

void Camera::SetLens(float fovY, float aspect, float nearZ, float farZ)
{
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = nearZ;
	m_FarZ = farZ;
}

void Camera::SetAspect(float aspect)
{
	m_Aspect = aspect;
}

void Camera::SetSpeed(float moveSpeed, float rotateSpeed)
{
	m_MoveSpeed = moveSpeed;
	m_RotateSpeed = rotateSpeed;
}

void Camera::Update(float deltaTime)
{
#ifdef _WIN32

	glm::vec3 dir = glm::vec3(0.0f, 0.0f, 0.0f);

	auto right = glm::cross(m_WorldUp, m_Look);

	if (input.key_W) dir += m_Look;
	if (input.key_S) dir -= m_Look;
	if (input.key_A) dir -= right;
	if (input.key_D) dir += right;
	if (input.key_Q) dir -= m_WorldUp;
	if (input.key_E) dir += m_WorldUp;

	if (glm::length(dir) > 0) {
		m_Position += glm::normalize(dir) * m_MoveSpeed * deltaTime;
	}

	if (input.key_MouseRight) {
		// 这里不要乘以deltaTime，要根据移动的距离来控制旋转的角度
		RotateY((input.pos.x - input.oldPos.x) * m_RotateSpeed);
		Pitch((input.pos.y - input.oldPos.y) * m_RotateSpeed);
	}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

	if (input.count > 0) {

		float deltaX = input.pos0.x - input.oldPos0.x;
		float deltaY = input.pos0.y - input.oldPos0.y;
		
		if (input.pos0.x < global::windowWidth / 2) {

			// 触摸屏幕左边，移动位置
			auto right = glm::cross(m_WorldUp, m_Look);

			glm::vec3 dir = deltaX * right - deltaY * m_Look;

			if (glm::length(dir) > 0) {
				// 这里不要乘以deltaTime，要根据移动的距离来控制移动的距离
				m_Position += dir * m_MoveSpeed;
			}
		}
		else {

			// 触摸屏幕右边，移动视角
			RotateY(deltaX * m_RotateSpeed);
			Pitch(deltaY * m_RotateSpeed);
		}

		deltaX = input.pos1.x - input.oldPos1.x;
		deltaY = input.pos1.y - input.oldPos1.y;

		if (input.pos1.x < global::windowWidth / 2) {

			// 触摸屏幕左边，移动位置
			auto right = glm::cross(m_WorldUp, m_Look);

			glm::vec3 dir = deltaX * right - deltaY * m_Look;

			if (glm::length(dir) > 0) {
				// 这里不要乘以deltaTime，要根据移动的距离来控制移动的距离
				m_Position += dir * m_MoveSpeed;
			}
		}
		else {

			// 触摸屏幕右边，移动视角
			RotateY(deltaX * m_RotateSpeed);
			Pitch(deltaY * m_RotateSpeed);
		}
	}

#endif
}

void Camera::Strafe(float d)
{
	auto right = glm::cross(m_WorldUp, m_Look);
	m_Position += right * d;
}

void Camera::Walk(float d)
{
	m_Position += m_Look * d;
}

void Camera::Fly(float d)
{
	m_Position += m_WorldUp * d;
}

void Camera::Pitch(float angle)
{
	auto right = glm::cross(m_WorldUp, m_Look);
	auto mat = glm::rotate(glm::mat4(1.0f), angle, right);
	m_Look = mat * glm::vec4(m_Look, 0.0f);
}

void Camera::RotateY(float angle)
{
	auto mat = glm::rotate(glm::mat4(1.0f), angle, m_WorldUp);
	m_Look = mat * glm::vec4(m_Look, 0.0f);
}

glm::mat4 Camera::GetView()
{
	auto right = glm::cross(m_WorldUp, m_Look);
	auto up = glm::cross(m_Look, right);
	return glm::lookAt(m_Position, m_Position + m_Look, up);
}

glm::mat4 Camera::GetProj()
{
	auto proj = glm::perspective(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	// GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted.
	// The easiest way to compensate for that is to flip the sign on the scaling factor of the Y axis in the projection matrix.
	// If you don't do this, then the image will be rendered upside down.
	proj[1][1] *= -1;
	return proj;
}