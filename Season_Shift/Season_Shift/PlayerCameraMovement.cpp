#include "pch.h"
#include "PlayerCameraMovement.h"
#include "Input.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include <imgui_impl_dx11.h>

using namespace DirectX::SimpleMath;

void PlayerCameraMovement::start()
{
	m_yaw = DirectX::XM_2PI / 2;
	m_pitch = 0.0f;
	m_roll = 0.0f;
	m_goToRoll = 0.0f;
	m_landShake = false;
	m_stop = true;
	m_secondTime = false;
	m_goToY = 0;

	m_playerCamera = m_gameObject->getComponentType<CameraComponent>(Component::ComponentEnum::CAMERA);
	m_baseCamPosY = 2.0f;
	m_camPosY = m_baseCamPosY;

	m_baseCameraFov = m_playerCamera->getCamera()->getFieldOfView();
	m_cameraFov = m_baseCameraFov;

	m_playerCamera->setRotation(m_roll, m_pitch, m_yaw);
}

void PlayerCameraMovement::update()
{
	Input::getInput().mouseMovement(m_pitch, m_yaw);
	m_playerCamera->setRotation(m_roll, m_pitch, m_yaw);
	landShake();

	ImGui::Begin("Player Camera");
	{
		ImGui::Text("GoToRoll %f", m_goToRoll);
		ImGui::Text("Roll %f", m_roll);
		ImGui::Text("RunRoll %f", m_runRoll);
		ImGui::Text("MoveY %f", m_camPosY);
		ImGui::Text("FOV %f", m_cameraFov);
		ImGui::SliderFloat("Modifier", &temp, 0.01f, 1000.0f);
	}
	ImGui::End();
}

void PlayerCameraMovement::resetCamera()
{
	m_roll = 0.0f;
	m_pitch = 0.0f;
	m_yaw = DirectX::XM_2PI / 2;
	m_goToRoll = 0.0f;
	m_runMoveY = 0;
	m_runRoll = 0;
	m_runShake = false;
	m_landShake = false;
	m_secondTime = false;
}

void PlayerCameraMovement::updateFrameTime(const float& frameTime)
{
	m_frameTime = frameTime;
}

void PlayerCameraMovement::landShake()
{
	constexpr float rollSpeed = DirectX::XM_PI * 5.f / 7.f;

	if (!m_stop)
	{
		if (m_roll < m_goToRoll)
		{
			m_roll += m_frameTime * rollSpeed;
		}
		else if (m_roll > m_goToRoll)
		{
			m_roll -= m_frameTime * rollSpeed;
		}

		if ((m_goToRoll < 0 && m_goToRoll >= m_roll) || (m_goToRoll > 0 && m_goToRoll <= m_roll))
			m_goToRoll = 0;

		if ((m_direction > 0 && m_roll <= 0) || (m_direction < 0 && m_roll >= 0) || m_direction == 0)
		{
			if (!m_secondTime)
			{
				setGoToRoll(m_secondTime);
			}
			if (m_landShake)
				m_stop = m_secondTime;
			m_roll = 0;
			m_landShake = !m_secondTime;
			m_secondTime = !m_secondTime;
		}
	}
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void PlayerCameraMovement::changeFOV(const Vector3& velocity, const float& maxSpeedXZ, const float& maxSpeedY)
{
	Vector3 velocityXZ = velocity;
	velocityXZ.y = 0;
	Vector3 velocityY = velocity;
	velocityY.x = velocityY.z = 0;
	float diff = std::clamp(velocityXZ.Length() * 0.95f / maxSpeedXZ + velocityY.Length() * 0.7f / maxSpeedY, 0.0f, 1.0f);
	float delta = m_frameTime * 3.0f;
	if (diff < m_cameraFov - m_baseCameraFov)
		delta = m_frameTime * 6.0f;
	m_cameraFov = lerp(m_cameraFov, m_baseCameraFov + diff * 0.5f, delta);
	//m_cameraFov = m_baseCameraFov + diff;
	m_playerCamera->setFieldOfView(m_cameraFov);
}

void PlayerCameraMovement::wallRunning(const bool& wallRunning, const Vector3& normal)
{
	constexpr float minRollOff = 0.01f;
	constexpr float rollWallCheck = 0.3f;
	constexpr float rollModifier = DirectX::XM_PI * 5.f / 7.f;

	if (m_landShake || m_runShake)
		return;

	if (!wallRunning)
	{
		if (m_roll > minRollOff)
		{
			m_roll -= m_frameTime;
		}
		else if (m_roll < -minRollOff)
		{
			m_roll += m_frameTime;
		}
		else
		{
			m_roll = 0.0;
		}
	}
	else
	{
		if (m_roll > -rollWallCheck && normal.x > 0)
		{
			m_roll -= normal.x * m_frameTime * rollModifier;
		}
		else if (m_roll < rollWallCheck && normal.x < 0)
		{
			m_roll -= normal.x * m_frameTime * rollModifier;
		}
		if (m_roll > -rollWallCheck && normal.z > 0)
		{
			m_roll -= normal.z * m_frameTime * rollModifier;
		}
		else if (m_roll < rollWallCheck && normal.z < 0)
		{
			m_roll -= normal.z * m_frameTime * rollModifier;
		}
	}
}

void PlayerCameraMovement::shake(Vector3 velocity, const Vector3& normal)
{
	constexpr float minVelocity = -50.0f;
	velocity = normal * velocity;
	if (velocity.y < minVelocity)
	{
		m_stop = false;
		m_landShake = true;
		m_secondTime = false;
		m_runShake = false;
		setGoToRoll(!m_secondTime);
		m_velocityY = velocity.y;
	}
}

void PlayerCameraMovement::setDirection(const float& roll, const bool& moveCam)
{
	if (!moveCam)
	{
		if (roll < 0)
			m_direction = -1;
		else if (roll > 0)
			m_direction = 1;
		else
			m_direction = 0;
	}
	else
	{
		if (roll < 0)
			m_camMoveDirection = -1;
		else if (roll > 0)
			m_camMoveDirection = 1;
		else
			m_camMoveDirection = 0;
	}
}

void PlayerCameraMovement::setGoToRoll(const bool& firstTime)
{
	constexpr float rollLandShake = DirectX::XM_PI / 26.0f;//36.0f;
	constexpr float modifier = 100.0f;

	if (firstTime)
	{
		if (m_direction != 0)
			m_goToRoll = rollLandShake * m_direction * fabs(m_velocityY) / modifier;
		else
			m_goToRoll = -rollLandShake * fabs(m_velocityY) / modifier;
	}
	else
		m_goToRoll = rollLandShake * -1 * m_direction * fabs(m_velocityY) / (modifier * 2.0f);

	setDirection(m_goToRoll);
}

void PlayerCameraMovement::setRunRoll(const bool& firstTime)
{
	constexpr float rollRunShake = DirectX::XM_PI / 512.0f;//256.0f;
	if (firstTime)
	{
		if (m_direction != 0)
			m_runRoll = rollRunShake * m_direction;
		else
			m_runRoll = -rollRunShake;
	}
	else
	{
		if (m_direction != 0)
			m_runRoll = rollRunShake * -1 * m_direction;
		else
			m_runRoll = rollRunShake;
	}

	setDirection(m_runRoll);
}

void PlayerCameraMovement::runShake(const Vector3& moveDirection, const bool& onGround, const bool& wallRunning, const float& speed, const float& maxSpeed)
{
	if (m_landShake)
		return;

	float rollSpeed = DirectX::XM_PI / 50.f * speed / maxSpeed;
	if (moveDirection != Vector3::Zero && onGround && !wallRunning && !m_landShake)
	{
		if (m_runRoll == 0)
			setRunRoll(m_secondTime);

		if (m_roll < m_runRoll)
		{
			m_roll += m_frameTime * rollSpeed;
		}
		else if (m_roll > m_runRoll)
		{
			m_roll -= m_frameTime * rollSpeed;
		}

		if ((m_runRoll < 0 && m_runRoll >= m_roll) || (m_runRoll > 0 && m_runRoll <= m_roll))
		{
			setRunRoll(!m_secondTime);
			m_secondTime = !m_secondTime;
		}
		m_runShake = true;
		runMoveY(moveDirection, onGround, wallRunning, speed, maxSpeed);
	}
	else
		m_runShake = false;

}

void PlayerCameraMovement::runMoveY(const Vector3& moveDirection, const bool& onGround, const bool& wallRunning, const float& speed, const float& maxSpeed)
{
	constexpr float moveYSpeed = 1.0f;

	if (moveDirection != Vector3::Zero && onGround && !wallRunning && !m_landShake)
	{
		if (m_goToY == 0)
			setRunMoveY(m_secondTime);

		if (m_camPosY < m_goToY)
		{
			m_camPosY += m_frameTime * moveYSpeed;
		}
		else if (m_camPosY > m_goToY)
		{
			m_camPosY -= m_frameTime * moveYSpeed;
		}

		if ((m_camPosY < m_goToY && m_camMoveDirection < 0) || (m_camPosY > m_goToY && m_camMoveDirection > 0))
		{
			setRunMoveY(m_secondTime);
			m_secondTime = !m_secondTime;
		}
	}
	else
		m_camPosY = m_baseCamPosY;

	m_playerCamera->setOffset(0.0f, m_camPosY, 0.0f);
}

void PlayerCameraMovement::setRunMoveY(const bool& firstTime)
{
	float changeYPos = 0.2f;
	if (firstTime)
	{
		if (m_camMoveDirection < 0)
			changeYPos /= 2;
		if (m_camMoveDirection != 0)
			m_goToY = m_baseCamPosY + changeYPos * m_camMoveDirection;
		else
			m_goToY = m_baseCamPosY + -changeYPos;
	}
	else
	{
		if (m_camMoveDirection > 0)
			changeYPos /= 2;
		if (m_camMoveDirection != 0)
			m_goToY = m_baseCamPosY + changeYPos * -1 * m_camMoveDirection;
		else
			m_goToY = m_baseCamPosY + changeYPos;
	}

	setDirection(m_goToY - m_baseCamPosY, true);
}

//char msgbuf[1000];
//sprintf_s(msgbuf, "My variable is %f\n", velocitySkipY.Length());
//OutputDebugStringA(msgbuf);