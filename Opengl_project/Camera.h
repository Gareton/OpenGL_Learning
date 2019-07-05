#ifndef CAMERA_H
#define CAMERA_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace fps_cam
{

	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 7.0f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;
	const float ZOOM_SPEED = 2.0f;
	const glm::vec3 POS = glm::vec3(0.0f, 0.0f, 3.0f);
	const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);

	enum Movement {
		forward, back, right, left
	};

	class Camera
	{
	public:
		Camera(glm::vec3 pos = POS, glm::vec3 world_up = UP, float yaw = YAW, float pitch = PITCH, float fov = ZOOM);
		glm::mat4 getViewMat();
		void processKeyboard(unsigned int direction, float deltaTime, float foward_speed = SPEED);
		void processMouseMovement(float xoffset, float yoffset);
		void processMouseScroll(float offset);
		float getFov() { return _fov; }
		void addY(float delta) { _pos.y += SPEED * delta; }
		glm::vec3 getPos();
	private:
		glm::vec3 _world_up;
		glm::vec3 _up;
		glm::vec3 _pos;
		glm::vec3 _front;
		glm::vec3 _right;
		glm::vec3 front_with_fixed_y;
		float _yaw;
		float _pitch;
		float _fov;

		void updateVectors();
	};

	glm::vec3 Camera::getPos()
	{
		return _pos;
	}

	glm::mat4 Camera::getViewMat()
	{
		return glm::lookAt(_pos, _pos + _front, _up);
	}

	Camera::Camera(glm::vec3 pos, glm::vec3 world_up, float yaw, float pitch, float fov)
	{
		_world_up = world_up;
		_pos = pos;
		_yaw = yaw;
		_pitch = pitch;
		_fov = fov;
		updateVectors();
	}

	void Camera::updateVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(_pitch)) * cos(glm::radians(_yaw));
	    front.y = sin(glm::radians(_pitch));
		front.z = cos(glm::radians(_pitch)) * sin(glm::radians(_yaw));
		_front = glm::normalize(front);
		_right = glm::normalize(glm::cross(_front, _world_up));
		_up = glm::normalize(glm::cross(_right, _front));
		front_with_fixed_y = glm::normalize(glm::cross(_world_up, _right));
	}

	void Camera::processKeyboard(unsigned int direction, float deltaTime, float foward_speed)
	{
		float camSpeed = deltaTime * SPEED;

		switch (direction){

		case Movement::forward:
			_pos += foward_speed * deltaTime * front_with_fixed_y;
			break;

		case Movement::left:
			_pos -= camSpeed * _right;
			break;

		case Movement::back:
			_pos -= camSpeed * front_with_fixed_y;
			break;

		case Movement::right:
			_pos += camSpeed * _right;
			break;
		}

		
	}

	void Camera::processMouseMovement(float xoffset, float yoffset)
	{
		xoffset *= SENSITIVITY;
		yoffset *= SENSITIVITY;

		_yaw += xoffset;
		_pitch += yoffset;

		if (_pitch > 89.0f)
			_pitch = 89.0f;

		if (_pitch < -89.0f)
			_pitch = -89.0f;

		updateVectors();
	}


	void Camera::processMouseScroll(float offset)
	{
		if (_fov >= 1.0f && _fov <= 45.0f)
			_fov -= offset * ZOOM_SPEED;

		if (_fov <= 1.0f)
			_fov = 1.0f;

		if (_fov >= 45.0f)
			_fov = 45.0f;
	}


}


#endif
