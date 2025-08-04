/**
 * @file Camera.cpp
 * @brief Implementation of configurable camera system
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 */

#include "SilicaEngine/Core/Camera.h"
#include "SilicaEngine/Core/Input.h"
#include "SilicaEngine/Core/Logger.h"
#include <algorithm>
#include <cmath>

namespace SilicaEngine {

    Camera::Camera(const CameraConfig& config) : m_Config(config) {
        UpdateCameraVectors();
        UpdateViewMatrix();
        UpdateProjectionMatrix();
    }

    void Camera::Update(float deltaTime) {
        if (!m_Enabled) return;
        
        // Validate deltaTime to prevent erratic behavior
        if (deltaTime < 0.0f) {
            SE_WARN("Negative deltaTime provided to Camera::Update: {}", deltaTime);
            return;
        }
        
        // Cap deltaTime to prevent large jumps (e.g., if frame rate drops significantly)
        const float MAX_DELTA_TIME = 1.0f; // 1 second max
        if (deltaTime > MAX_DELTA_TIME) {
            SE_WARN("Excessive deltaTime provided to Camera::Update: {} (capped to {})", deltaTime, MAX_DELTA_TIME);
            deltaTime = MAX_DELTA_TIME;
        }
        
        // Process input if enabled
        if (m_Config.enableKeyboardMovement) {
            ProcessKeyboardInput(deltaTime);
        }
        
        if (m_Config.enableMouseLook) {
            double mouseX, mouseY;
            Input::GetMouseDelta(mouseX, mouseY);
            if (mouseX != 0.0 || mouseY != 0.0) {
                ProcessMouseMovement(static_cast<float>(mouseX), static_cast<float>(mouseY));
            }
        }
        
        if (m_Config.enableMouseWheelZoom) {
            double scrollX, scrollY;
            Input::GetScrollDelta(scrollX, scrollY);
            if (scrollY != 0.0) {
                ProcessMouseScroll(static_cast<float>(scrollY));
            }
        }
        
        if (m_Config.enableGamepadMovement) {
            ProcessGamepadInput(deltaTime);
        }

        // Update orbital position if in orbital or third person mode
        if (m_Config.mode == CameraMode::ThirdPerson || m_Config.mode == CameraMode::Orbital) {
            UpdateOrbitalPosition();
        }

        // Apply constraints
        ApplyConstraints();

        // Update matrices
        UpdateCameraVectors();
        UpdateViewMatrix();
    }

    void Camera::SetConfig(const CameraConfig& config) {
        m_Config = config;
        UpdateProjectionMatrix();
    }

    void Camera::SetPosition(const glm::vec3& position) {
        m_Position = position;
        UpdateViewMatrix();
    }

    void Camera::SetRotation(float yaw, float pitch, float roll) {
        m_Yaw = yaw;
        m_Pitch = pitch;
        m_Roll = roll;
        ApplyConstraints();
        UpdateCameraVectors();
        UpdateViewMatrix();
    }

    void Camera::SetYaw(float yaw) {
        m_Yaw = yaw;
        ApplyConstraints();
        UpdateCameraVectors();
        UpdateViewMatrix();
    }

    void Camera::SetPitch(float pitch) {
        m_Pitch = pitch;
        ApplyConstraints();
        UpdateCameraVectors();
        UpdateViewMatrix();
    }

    void Camera::SetRoll(float roll) {
        m_Roll = roll;
        UpdateCameraVectors();
        UpdateViewMatrix();
    }

    void Camera::SetDirection(const glm::vec3& direction) {
        // Check if direction vector is zero or near-zero to prevent NaN values
        float length = glm::length(direction);
        if (length < 1e-6f) {
            SE_WARN("Attempted to set camera direction with zero or near-zero vector");
            return; // Don't update yaw and pitch with invalid direction
        }
        
        glm::vec3 normalizedDir = glm::normalize(direction);
        
        // Calculate yaw and pitch from direction vector
        m_Yaw = glm::degrees(atan2(normalizedDir.z, normalizedDir.x));
        m_Pitch = glm::degrees(asin(-normalizedDir.y));
        
        ApplyConstraints();
        UpdateCameraVectors();
        UpdateViewMatrix();
    }

    void Camera::LookAt(const glm::vec3& target, const glm::vec3& up) {
        glm::vec3 direction = glm::normalize(target - m_Position);
        SetDirection(direction);
        m_WorldUp = up;
        UpdateCameraVectors();
        UpdateViewMatrix();
    }

    void Camera::SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
        m_Config.projection = CameraProjection::Perspective;
        m_Config.fov = fov;
        m_Config.nearPlane = nearPlane;
        m_Config.farPlane = farPlane;
        m_AspectRatio = aspectRatio;
        UpdateProjectionMatrix();
    }

    void Camera::SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
        m_Config.projection = CameraProjection::Orthographic;
        m_Config.nearPlane = nearPlane;
        m_Config.farPlane = farPlane;
        m_AspectRatio = (right - left) / (top - bottom);
        
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
    }

    void Camera::SetOrthographic(float size, float aspectRatio, float nearPlane, float farPlane) {
        m_Config.projection = CameraProjection::Orthographic;
        m_Config.orthoSize = size;
        m_Config.nearPlane = nearPlane;
        m_Config.farPlane = farPlane;
        m_AspectRatio = aspectRatio;
        UpdateProjectionMatrix();
    }

    void Camera::SetAspectRatio(float aspectRatio) {
        m_AspectRatio = aspectRatio;
        UpdateProjectionMatrix();
    }

    void Camera::Move(const glm::vec3& offset) {
        glm::vec3 movement = m_Right * offset.x + m_Up * offset.y + m_Front * offset.z;
        m_Position += movement;
        UpdateViewMatrix();
    }

    void Camera::MoveWorld(const glm::vec3& offset) {
        m_Position += offset;
        UpdateViewMatrix();
    }

    void Camera::Rotate(float deltaYaw, float deltaPitch, float deltaRoll) {
        m_Yaw += deltaYaw;
        m_Pitch += deltaPitch;
        m_Roll += deltaRoll;
        
        ApplyConstraints();
        UpdateCameraVectors();
        UpdateViewMatrix();
    }

    void Camera::Zoom(float delta) {
        if (m_Config.projection == CameraProjection::Perspective) {
            if (m_Config.mode == CameraMode::ThirdPerson || m_Config.mode == CameraMode::Orbital) {
                // Adjust distance for orbital cameras
                m_Config.distance -= delta * m_Config.zoomSpeed;
                m_Config.distance = std::clamp(m_Config.distance, m_Config.minDistance, m_Config.maxDistance);
                UpdateOrbitalPosition();
            } else {
                // Adjust FOV for other cameras
                m_Config.fov -= delta * m_Config.zoomSpeed;
                m_Config.fov = std::clamp(m_Config.fov, 1.0f, 120.0f);
                UpdateProjectionMatrix();
            }
        } else {
            // Adjust ortho size
            m_Config.orthoSize -= delta * m_Config.zoomSpeed * 0.1f;
            m_Config.orthoSize = std::max(m_Config.orthoSize, 0.1f);
            UpdateProjectionMatrix();
        }
    }

    void Camera::SetTarget(const glm::vec3& target) {
        m_Config.target = target;
        if (m_Config.mode == CameraMode::ThirdPerson || m_Config.mode == CameraMode::Orbital) {
            UpdateOrbitalPosition();
        }
    }

    void Camera::SetDistance(float distance) {
        m_Config.distance = std::clamp(distance, m_Config.minDistance, m_Config.maxDistance);
        if (m_Config.mode == CameraMode::ThirdPerson || m_Config.mode == CameraMode::Orbital) {
            UpdateOrbitalPosition();
        }
    }

    void Camera::ProcessKeyboardInput(float deltaTime) {
        if (m_Config.mode == CameraMode::Fixed) return;

        glm::vec3 movement = CalculateMovementInput(deltaTime);

        // Apply movement based on camera mode
        if (m_Config.mode == CameraMode::ThirdPerson || m_Config.mode == CameraMode::Orbital) {
            // For orbital cameras, movement affects the orbit
            const float ORBITAL_ROTATION_MULTIPLIER = 10.0f; // Configurable orbital rotation speed
            m_Yaw += movement.x * m_Config.rotationSpeed * ORBITAL_ROTATION_MULTIPLIER;
            m_Pitch += movement.y * m_Config.rotationSpeed * ORBITAL_ROTATION_MULTIPLIER;
            m_Config.distance -= movement.z * m_Config.movementSpeed * deltaTime;
            m_Config.distance = std::clamp(m_Config.distance, m_Config.minDistance, m_Config.maxDistance);
        } else {
            // For first person and free cameras, move in local space
            Move(movement);
        }
    }

    void Camera::ProcessMouseMovement(float deltaX, float deltaY) {
        if (m_Config.mode == CameraMode::Fixed) return;

        // Apply mouse sensitivity
        deltaX *= m_Config.rotationSpeed;
        deltaY *= m_Config.rotationSpeed;

        // Invert Y axis if requested
        if (m_Config.invertY) {
            deltaY = -deltaY;
        }

        m_Yaw += deltaX;
        m_Pitch -= deltaY; // Subtract because screen Y is inverted

        ApplyConstraints();
        UpdateCameraVectors();
        UpdateViewMatrix();
    }

    void Camera::ProcessMouseScroll(float deltaY) {
        Zoom(deltaY * m_Config.mouseWheelZoomSpeed);
    }

    void Camera::ProcessGamepadInput(float deltaTime) {
        if (m_Config.mode == CameraMode::Fixed) return;

        // Check for connected gamepad
        if (!Input::IsGamepadConnected(0)) return;

        // Gamepad configuration constants
        const float GAMEPAD_DEADZONE = 0.1f;
        const float GAMEPAD_SENSITIVITY_MULTIPLIER = 50.0f;

        // Movement with left stick
        float leftX = Input::GetGamepadAxis(0, GamepadAxis::LeftX);
        float leftY = Input::GetGamepadAxis(0, GamepadAxis::LeftY);

        if (std::abs(leftX) > GAMEPAD_DEADZONE || std::abs(leftY) > GAMEPAD_DEADZONE) {
            glm::vec3 movement(leftX, 0.0f, -leftY); // -Y because gamepad Y is inverted
            movement *= m_Config.movementSpeed * deltaTime;

            if (m_Config.mode == CameraMode::ThirdPerson || m_Config.mode == CameraMode::Orbital) {
                const float ORBITAL_ROTATION_MULTIPLIER = 10.0f;
                m_Yaw += movement.x * ORBITAL_ROTATION_MULTIPLIER;
                m_Config.distance += movement.z;
                m_Config.distance = std::clamp(m_Config.distance, m_Config.minDistance, m_Config.maxDistance);
            } else {
                Move(movement);
            }
        }

        // Look with right stick
        float rightX = Input::GetGamepadAxis(0, GamepadAxis::RightX);
        float rightY = Input::GetGamepadAxis(0, GamepadAxis::RightY);

        if (std::abs(rightX) > GAMEPAD_DEADZONE || std::abs(rightY) > GAMEPAD_DEADZONE) {
            float sensitivity = m_Config.rotationSpeed * GAMEPAD_SENSITIVITY_MULTIPLIER * deltaTime;
            ProcessMouseMovement(rightX * sensitivity, rightY * sensitivity);
        }

        // Zoom with triggers
        float leftTrigger = Input::GetGamepadAxis(0, GamepadAxis::LeftTrigger);
        float rightTrigger = Input::GetGamepadAxis(0, GamepadAxis::RightTrigger);

        if (leftTrigger > GAMEPAD_DEADZONE || rightTrigger > GAMEPAD_DEADZONE) {
            float zoomDelta = (rightTrigger - leftTrigger) * m_Config.zoomSpeed * deltaTime;
            Zoom(zoomDelta);
        }
    }

    glm::vec3 Camera::ScreenToWorldRay(const glm::vec2& screenPos, const glm::vec2& screenSize) const {
        // Convert screen position to normalized device coordinates
        float x = (2.0f * screenPos.x) / screenSize.x - 1.0f;
        float y = 1.0f - (2.0f * screenPos.y) / screenSize.y;

        // Create ray in clip space
        glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

        // Convert to eye space
        glm::vec4 rayEye = glm::inverse(m_ProjectionMatrix) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        // Convert to world space
        glm::vec4 rayWorld = glm::inverse(m_ViewMatrix) * rayEye;
        glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

        return rayDirection;
    }

    void Camera::UpdateCameraVectors() {
        // Calculate front vector
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);

        // Calculate right and up vectors
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

    void Camera::UpdateViewMatrix() {
        m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    }

    void Camera::UpdateProjectionMatrix() {
        if (m_Config.projection == CameraProjection::Perspective) {
            m_ProjectionMatrix = glm::perspective(glm::radians(m_Config.fov), m_AspectRatio, 
                                                m_Config.nearPlane, m_Config.farPlane);
        } else {
            float halfWidth = m_Config.orthoSize * m_AspectRatio;
            float halfHeight = m_Config.orthoSize;
            m_ProjectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight,
                                          m_Config.nearPlane, m_Config.farPlane);
        }
    }

    void Camera::UpdateOrbitalPosition() {
        // Calculate position based on spherical coordinates around target
        float yawRad = glm::radians(m_Yaw);
        float pitchRad = glm::radians(m_Pitch);

        glm::vec3 offset;
        offset.x = m_Config.distance * cos(pitchRad) * cos(yawRad);
        offset.y = m_Config.distance * sin(pitchRad);
        offset.z = m_Config.distance * cos(pitchRad) * sin(yawRad);

        m_Position = m_Config.target + offset;

        // Always look at the target
        LookAt(m_Config.target, m_WorldUp);
    }

    void Camera::ApplyConstraints() {
        // Constrain pitch
        if (m_Config.constrainPitch) {
            m_Pitch = std::clamp(m_Pitch, -m_Config.maxPitch, m_Config.maxPitch);
        }

        // Constrain yaw
        if (m_Config.constrainYaw) {
            m_Yaw = std::clamp(m_Yaw, m_Config.minYaw, m_Config.maxYaw);
        }

        // Keep yaw in range [-180, 180]
        while (m_Yaw > 180.0f) m_Yaw -= 360.0f;
        while (m_Yaw < -180.0f) m_Yaw += 360.0f;
    }

    glm::vec3 Camera::CalculateMovementInput(float deltaTime) {
        glm::vec3 movement(0.0f);

        // Get movement input from Input system
        if (Input::IsActionActive("MoveForward")) {
            movement.z += 1.0f;
        }
        if (Input::IsActionActive("MoveBackward")) {
            movement.z -= 1.0f;
        }
        if (Input::IsActionActive("MoveLeft")) {
            movement.x -= 1.0f;
        }
        if (Input::IsActionActive("MoveRight")) {
            movement.x += 1.0f;
        }

        // Additional movement for free camera
        if (m_Config.mode == CameraMode::Free) {
            if (Input::IsKeyPressed(GLFW_KEY_Q)) {
                movement.y -= 1.0f;
            }
            if (Input::IsKeyPressed(GLFW_KEY_E)) {
                movement.y += 1.0f;
            }
        }

        // Apply speed and normalize
        if (glm::length(movement) > 0.0f) {
            movement = glm::normalize(movement) * m_Config.movementSpeed * deltaTime;
        }

        return movement;
    }

    // CameraController removed - use Camera::SetEnabled() for control instead

} // namespace SilicaEngine