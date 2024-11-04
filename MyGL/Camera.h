#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace MyGL
{
    // Default camera values
    // const glm::vec3 POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
    // const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
    constexpr float YAW = -90.0f;
    constexpr float PITCH = 0.0f;
    constexpr float SPEED = 2.5f;
    constexpr float MOUSE_SENSITIVITY_X = 0.1f;
    constexpr float MOUSE_SENSITIVITY_Y = 0.1f;
    constexpr float CONTROLLER_SENSITIVITY_X = 1.0f;
    constexpr float CONTROLLER_SENSITIVITY_Y = 1.0f;
    constexpr float ZOOM = 45.0f;

    class Camera
    {
    public:
        glm::mat4 get_view_matrix() const;
        glm::mat4 get_model_matrix() const;
        glm::mat4 get_projection_matrix(float aspect_ratio) const;

        glm::vec3 get_position() const { return position; }

        virtual void set_position(glm::vec3 position) = 0;
        virtual void look_at(glm::vec3 target) = 0;

        // Keyboard and mouse input
        enum class KeyboardMoveDirection
        {
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT,
            UP,
            DOWN
        };
        enum class MouseZoomDirection
        {
            IN,
            OUT
        };
        virtual void on_keyboard(KeyboardMoveDirection direction, float delta_time) {}
        virtual void on_mouse_movement(float x_offset, float y_offset,
                                       bool constrain_pitch = true) {}
        virtual void on_mouse_scroll(MouseZoomDirection zoom_direction) {}

        // Gamepad input
        virtual void on_lstick(float x, float y, float delta_time) {}
        virtual void on_rstick(float x, float y, float delta_time,
                               bool constrain_pitch = true) {}
        virtual void on_triggers(float left_trigger, float right_trigger, float delta_time) {}

    protected:
        glm::vec3 position;

        glm::vec3 world_up;

        glm::vec3 front;
        glm::vec3 right;
        glm::vec3 up;

        float movement_speed = SPEED;
        float mouse_sensitivity_x = MOUSE_SENSITIVITY_X;
        float mouse_sensitivity_y = MOUSE_SENSITIVITY_Y;
        float controller_sensitivity_x = CONTROLLER_SENSITIVITY_X;
        float controller_sensitivity_y = CONTROLLER_SENSITIVITY_Y;
        float zoom = ZOOM;

        void update_camera_vectors();
    };

    class FpsCamera : public Camera
    {
    public:
        FpsCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

        void set_position(glm::vec3 position) override;
        void look_at(glm::vec3 target) override;

        // Keyboard and mouse input
        void on_keyboard(KeyboardMoveDirection direction, float delta_time) override;
        void on_mouse_movement(float x_offset, float y_offset,
                               bool constrain_pitch = true) override;
        void on_mouse_scroll(MouseZoomDirection zoom_direction) override;

        // Gamepad input
        void on_lstick(float x, float y, float delta_time) override;
        void on_rstick(float x, float y, float delta_time,
                       bool constrain_pitch = true) override;

    protected:
        float yaw;
        float pitch;

        void update_camera_vectors();
    };

    class OrbitCamera : public Camera
    {
    public:
        OrbitCamera(glm::vec3 target, float distance = 3.0f, float theta = 0.0f, float phi = 0.0f);

        void set_position(glm::vec3 position) override;
        void look_at(glm::vec3 target) override;

        // Keyboard and mouse input
        void on_keyboard(KeyboardMoveDirection direction, float delta_time) override;

        // Gamepad input
        void on_lstick(float x, float y, float delta_time) override;
        void on_triggers(float left_trigger, float right_trigger, float delta_time) override;

    private:
        glm::vec3 target;
        float radius;
        float theta;
        float phi;

        void update_camera_vectors();
    };
}