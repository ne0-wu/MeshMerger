#include <stdexcept>

#include "Window.h"

MyGL::Window::Window(int width, int height, std::string title)
{
    setup_GLFW(width, height, title);
    setup_GLAD();
    setup_ImGui();
}

MyGL::Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

std::tuple<int, int> MyGL::Window::get_framebuffer_size() const
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return {width, height};
}

bool MyGL::Window::should_close() const
{
    return glfwWindowShouldClose(window);
}

void MyGL::Window::swap_buffers() const
{
    glfwSwapBuffers(window);
}

void MyGL::Window::poll_events() const
{
    glfwPollEvents();
}

void MyGL::Window::process_input() const
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void MyGL::Window::process_camera_input(Camera &camera, float delta_time) const
{
    // Keyboard input
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.on_keyboard(Camera::KeyboardMoveDirection::FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.on_keyboard(Camera::KeyboardMoveDirection::BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.on_keyboard(Camera::KeyboardMoveDirection::LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.on_keyboard(Camera::KeyboardMoveDirection::RIGHT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        camera.on_keyboard(Camera::KeyboardMoveDirection::UP, delta_time);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        camera.on_keyboard(Camera::KeyboardMoveDirection::DOWN, delta_time);

    // Mouse input
    // TODO: Implement mouse input

    // Gamepad input
    int axis_count;
    const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axis_count);
    if (axis_count >= 2)
        camera.on_lstick(axes[0], axes[1], delta_time);
    if (axis_count >= 4)
        camera.on_rstick(axes[2], axes[3], delta_time);
    if (axis_count >= 6)
        camera.on_triggers(axes[4], axes[5], delta_time);
}

bool MyGL::Window::is_mouse_inside() const
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    auto [width, height] = get_framebuffer_size();
    return x >= 0 && x < width && y >= 0 && y < height;
}

void MyGL::Window::setup_GLFW(int width, int height, std::string title)
{
    // Initialize GLFW
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    // Set OpenGL version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(width, height, title.data(), NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(window); // Make the window's context current

    glfwWindowHint(GLFW_SAMPLES, 4); // Enable multisampling
    glfwSwapInterval(1);             // Enable vsync

    // Set the callback function for window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void MyGL::Window::setup_GLAD() const
{
    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Enable polygon offset (for wireframe rendering)
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);

    glEnable(GL_POLYGON_OFFSET_POINT);
}

void MyGL::Window::setup_ImGui() const
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void MyGL::Window::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}