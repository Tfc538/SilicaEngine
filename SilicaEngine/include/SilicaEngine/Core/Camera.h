/**
 * @file Camera.h
 * @brief Configurable camera system for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Flexible camera system with first-person, third-person, and orbital camera modes.
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace SilicaEngine {

    /// Camera projection types
    enum class CameraProjection {
        Perspective,
        Orthographic
    };

    /// Camera movement modes
    enum class CameraMode {
        FirstPerson,    // WASD + Mouse look
        ThirdPerson,    // Orbit around target with distance
        Orbital,        // Orbit around target with fixed distance
        Free,           // Full 6DOF movement
        Fixed           // No movement (static camera)
    };

    /// Camera configuration structure
    struct CameraConfig {
        // Projection settings
        CameraProjection projection = CameraProjection::Perspective;
        float fov = 45.0f;                          // Field of view in degrees (perspective only)
        float nearPlane = 0.1f;                     // Near clipping plane
        float farPlane = 1000.0f;                   // Far clipping plane
        float orthoSize = 10.0f;                    // Orthographic size (half height)
        
        // Movement settings
        CameraMode mode = CameraMode::FirstPerson;
        float movementSpeed = 5.0f;                 // Units per second
        float rotationSpeed = 0.1f;                 // Radians per pixel
        float zoomSpeed = 2.0f;                     // Zoom sensitivity
        float mouseWheelZoomSpeed = 1.0f;           // Mouse wheel zoom sensitivity
        
        // First person / Free mode settings
        float maxPitch = 89.0f;                     // Maximum pitch angle in degrees
        bool invertY = false;                       // Invert Y mouse axis
        bool smoothMovement = true;                 // Enable movement smoothing
        float smoothingFactor = 10.0f;              // Movement smoothing factor
        
        // Third person / Orbital mode settings
        glm::vec3 target = glm::vec3(0.0f);         // Target position to orbit around
        float distance = 10.0f;                     // Distance from target
        float minDistance = 1.0f;                   // Minimum orbit distance
        float maxDistance = 100.0f;                 // Maximum orbit distance
        
        // Constraints
        bool constrainPitch = true;                 // Constrain pitch rotation
        bool constrainYaw = false;                  // Constrain yaw rotation
        float minYaw = -180.0f;                     // Minimum yaw angle
        float maxYaw = 180.0f;                      // Maximum yaw angle
        
        // Input settings
        bool enableKeyboardMovement = true;
        bool enableMouseLook = true;
        bool enableMouseWheelZoom = true;
        bool enableGamepadMovement = true;
    };

    /// Camera class with configurable controls and projection
    class Camera {
    public:
        explicit Camera(const CameraConfig& config = CameraConfig{});
        ~Camera() = default;

        /// Update camera (call once per frame)
        void Update(float deltaTime);
        
        /// Enable/disable camera processing
        void SetEnabled(bool enabled) { m_Enabled = enabled; }
        bool IsEnabled() const { return m_Enabled; }

        /// Set camera configuration
        void SetConfig(const CameraConfig& config);
        const CameraConfig& GetConfig() const { return m_Config; }

        // === Position and Orientation ===
        
        /// Set camera position
        void SetPosition(const glm::vec3& position);
        const glm::vec3& GetPosition() const { return m_Position; }

        /// Set camera rotation (Euler angles in degrees)
        void SetRotation(float yaw, float pitch, float roll = 0.0f);
        void SetYaw(float yaw);
        void SetPitch(float pitch);
        void SetRoll(float roll);
        float GetYaw() const { return m_Yaw; }
        float GetPitch() const { return m_Pitch; }
        float GetRoll() const { return m_Roll; }

        /// Set camera orientation using direction vector
        void SetDirection(const glm::vec3& direction);
        glm::vec3 GetDirection() const { return m_Front; }

        /// Look at target position
        void LookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

        // === Projection ===
        
        /// Set perspective projection
        void SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
        
        /// Set orthographic projection
        void SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
        
        /// Set orthographic projection with size
        void SetOrthographic(float size, float aspectRatio, float nearPlane, float farPlane);
        
        /// Update aspect ratio
        void SetAspectRatio(float aspectRatio);
        float GetAspectRatio() const { return m_AspectRatio; }

        // === Matrices ===
        
        /// Get view matrix
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        
        /// Get projection matrix
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        
        /// Get view-projection matrix
        glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

        // === Movement (for programmatic control) ===
        
        /// Move camera in local space
        void Move(const glm::vec3& offset);
        
        /// Move camera in world space
        void MoveWorld(const glm::vec3& offset);
        
        /// Rotate camera
        void Rotate(float deltaYaw, float deltaPitch, float deltaRoll = 0.0f);
        
        /// Zoom camera (changes distance for orbital, FOV for perspective)
        void Zoom(float delta);

        // === Third Person / Orbital Mode ===
        
        /// Set orbit target
        void SetTarget(const glm::vec3& target);
        const glm::vec3& GetTarget() const { return m_Config.target; }
        
        /// Set orbit distance
        void SetDistance(float distance);
        float GetDistance() const { return m_Config.distance; }

        // === Input Integration ===
        
        /// Process keyboard input (automatically called in Update if enabled)
        void ProcessKeyboardInput(float deltaTime);
        
        /// Process mouse movement (automatically called in Update if enabled)
        void ProcessMouseMovement(float deltaX, float deltaY);
        
        /// Process mouse scroll (automatically called in Update if enabled)
        void ProcessMouseScroll(float deltaY);
        
        /// Process gamepad input (automatically called in Update if enabled)
        void ProcessGamepadInput(float deltaTime);

        // === Utility ===
        
        /// Get camera's right vector
        glm::vec3 GetRight() const { return m_Right; }
        
        /// Get camera's up vector
        glm::vec3 GetUp() const { return m_Up; }
        
        /// Get camera's forward vector
        glm::vec3 GetFront() const { return m_Front; }
        
        /// Screen to world ray casting
        glm::vec3 ScreenToWorldRay(const glm::vec2& screenPos, const glm::vec2& screenSize) const;

    private:
        CameraConfig m_Config;
        
        // Transform
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 3.0f);
        float m_Yaw = -90.0f;       // Euler angle around Y axis
        float m_Pitch = 0.0f;       // Euler angle around X axis
        float m_Roll = 0.0f;        // Euler angle around Z axis
        
        // Vectors
        glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        
        // Matrices
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        float m_AspectRatio = 16.0f / 9.0f;
        
        // Smoothing
        glm::vec3 m_VelocityTarget = glm::vec3(0.0f);
        glm::vec3 m_CurrentVelocity = glm::vec3(0.0f);
        
        // Control
        bool m_Enabled = true;
        
        // Internal methods
        void UpdateCameraVectors();
        void UpdateViewMatrix();
        void UpdateProjectionMatrix();
        void UpdateOrbitalPosition();
        void ApplyConstraints();
        glm::vec3 CalculateMovementInput(float deltaTime);
    };

} // namespace SilicaEngine