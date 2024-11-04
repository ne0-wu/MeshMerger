#include "Camera.h"

#include <iostream>

// Base camera class
// =================

glm::mat4 MyGL::Camera::get_model_matrix() const
{
    return glm::translate(glm::mat4(1.0f), -position);
}

glm::mat4 MyGL::Camera::get_view_matrix() const
{
    return glm::lookAt(position, position + front, up);
}

glm::mat4 MyGL::Camera::get_projection_matrix(float aspect_ratio) const
{
    return glm::perspective(glm::radians(zoom), aspect_ratio, 0.1f, 100.0f);
}

// FPS camera class
// ================

MyGL::FpsCamera::FpsCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : yaw(yaw), pitch(pitch)
{
    this->position = position;
    this->world_up = up;
}

void MyGL::FpsCamera::set_position(glm::vec3 position)
{
    this->position = position;
}

void MyGL::FpsCamera::look_at(glm::vec3 target)
{
    front = glm::normalize(target - position);
    yaw = glm::degrees(atan2(front.z, front.x));
    pitch = glm::degrees(asin(front.y));

    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}

void MyGL::FpsCamera::on_keyboard(KeyboardMoveDirection direction, float delta_time)
{
    float velocity = movement_speed * delta_time;

    switch (direction)
    {
    case KeyboardMoveDirection::FORWARD:
        position += front * velocity;
        break;
    case KeyboardMoveDirection::BACKWARD:
        position -= front * velocity;
        break;
    case KeyboardMoveDirection::LEFT:
        position -= right * velocity;
        break;
    case KeyboardMoveDirection::RIGHT:
        position += right * velocity;
        break;
    }
}

void MyGL::FpsCamera::on_mouse_movement(float x_offset, float y_offset, bool constrain_pitch)
{
    x_offset *= mouse_sensitivity_x;
    y_offset *= mouse_sensitivity_y;

    yaw += x_offset;
    pitch += y_offset;

    if (constrain_pitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    update_camera_vectors();
}

void MyGL::FpsCamera::on_mouse_scroll(MouseZoomDirection zoom_direction)
{
    if (zoom_direction == MouseZoomDirection::IN)
        zoom -= 0.5f;
    if (zoom_direction == MouseZoomDirection::OUT)
        zoom += 0.5f;

    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

void MyGL::FpsCamera::on_lstick(float x, float y, float delta_time)
{
    float velocity = movement_speed * delta_time;

    position += front * y * velocity;
    position += right * x * velocity;
}

void MyGL::FpsCamera::on_rstick(float x, float y, float delta_time, bool constrain_pitch)
{
    x *= controller_sensitivity_x;
    y *= controller_sensitivity_y;

    yaw += x;
    pitch += y;

    if (constrain_pitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    update_camera_vectors();
}

void MyGL::FpsCamera::update_camera_vectors()
{
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}

// Orbit camera class
// ==================

MyGL::OrbitCamera::OrbitCamera(glm::vec3 target, float distance, float theta, float phi)
    : target(target), radius(distance), theta(theta), phi(phi)
{
    world_up = {0.0f, 1.0f, 0.0f};
    update_camera_vectors();
}

void MyGL::OrbitCamera::set_position(glm::vec3 position)
{
    glm::vec3 direction = glm::normalize(position - target);
    radius = glm::length(position - target);
    theta = -glm::degrees(atan2(direction.z, direction.x));
    phi = -glm::degrees(asin(direction.y));
    update_camera_vectors();
}

void MyGL::OrbitCamera::look_at(glm::vec3 target)
{
    this->target = target;
    update_camera_vectors();
}

void MyGL::OrbitCamera::on_keyboard(KeyboardMoveDirection direction, float delta_time)
{
    float velocity = movement_speed * delta_time;
    float rad2deg = 180.0f / glm::pi<float>();

    switch (direction)
    {
    case KeyboardMoveDirection::FORWARD:
        radius -= velocity;
        break;
    case KeyboardMoveDirection::BACKWARD:
        radius += velocity;
        break;
    case KeyboardMoveDirection::LEFT:
        theta -= velocity * rad2deg * radius;
        break;
    case KeyboardMoveDirection::RIGHT:
        theta += velocity * rad2deg * radius;
        break;
    case KeyboardMoveDirection::UP:
        phi += velocity * rad2deg * radius;
        break;
    case KeyboardMoveDirection::DOWN:
        phi -= velocity * rad2deg * radius;
        break;
    }

    update_camera_vectors();
}

void MyGL::OrbitCamera::on_lstick(float x, float y, float delta_time)
{
    // deadzone
    if (x * x + y * y < 0.1f)
        return;

    float velocity = movement_speed * delta_time;
    float rad2deg = 180.0f / glm::pi<float>();

    theta += x * controller_sensitivity_x * velocity * rad2deg * radius;
    phi += y * controller_sensitivity_y * velocity * rad2deg * radius;

    update_camera_vectors();
}

void MyGL::OrbitCamera::on_triggers(float left_trigger, float right_trigger, float delta_time)
{
    float velocity = movement_speed * delta_time;

    radius -= (right_trigger - left_trigger) * velocity * 0.2;
    update_camera_vectors();
}

void MyGL::OrbitCamera::update_camera_vectors()
{
    if (phi > 89.0f)
        phi = 89.0f;
    if (phi < -89.0f)
        phi = -89.0f;
    if (radius < 0.1f)
        radius = 0.1f;

    position.x = glm::cos(glm::radians(phi)) * glm::cos(glm::radians(theta));
    position.y = glm::sin(glm::radians(phi));
    position.z = glm::cos(glm::radians(phi)) * glm::sin(glm::radians(theta));

    position = target + radius * position;

    front = glm::normalize(target - position);
    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}
