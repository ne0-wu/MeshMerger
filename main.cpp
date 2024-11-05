#include <OpenMesh/Core/IO/MeshIO.hh>

#include "Mesh.h"
#include "MeshToGL.h"

#include "MyGL/LogConsole.h"
#include "MyGL/Mesh.h"
#include "MyGL/PickVertex.h"
#include "MyGL/Shader.h"
#include "MyGL/Utils.h"
#include "MyGL/Window.h"


struct Flags
{
    enum class InteractionMode
    {
        DEFAULT,
        SELECT_VERTEX
    } draw_mode = InteractionMode::DEFAULT;

    bool draw_wireframe = true;
} flags;

const char *InteractionModeItems[] = {"Default", "Select Vertex"};

MyGL::LogConsole logger;

int main()
{
    try
    {
        // Initialize window (and OpenGL context)
        MyGL::Window window;

        // Shaders
        MyGL::ShaderProgram basic_shader(MyGL::read_file_to_string("data/shaders/basic.vert"),
                                         MyGL::read_file_to_string("data/shaders/basic.frag"));
        MyGL::ShaderProgram phong_shader(MyGL::read_file_to_string("data/shaders/phong.vert"),
                                         MyGL::read_file_to_string("data/shaders/phong.frag"));
        MyGL::PickVertex pick_vertex;

        // Load mesh from file
        Mesh mesh;
        if (!OpenMesh::IO::read_mesh(mesh, "data/models/stanford-bunny.obj"))
            throw std::runtime_error("Failed to read mesh from file");

        // Move mesh to [-1, 1]^3
        {
            Eigen::Vector3d center{0.0, 0.0, 0.0},
                min{std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
                    std::numeric_limits<double>::max()},
                max{std::numeric_limits<double>::min(), std::numeric_limits<double>::min(),
                    std::numeric_limits<double>::min()};
            for (const auto &v : mesh.vertices())
            {
                const auto &point = mesh.point(v);
                min[0] = std::min(min[0], point[0]);
                min[1] = std::min(min[1], point[1]);
                min[2] = std::min(min[2], point[2]);
                max[0] = std::max(max[0], point[0]);
                max[1] = std::max(max[1], point[1]);
                max[2] = std::max(max[2], point[2]);
            }
            center = (min + max) / 2.0;
            const auto scale = 2.0 / (max - min).maxCoeff();
            for (auto &v : mesh.vertices())
                mesh.set_point(v, scale * (mesh.point(v) - center));
        }

        mesh.request_face_normals();
        mesh.request_vertex_normals();
        mesh.update_normals();

        // Convert mesh to MyGL::Mesh
        MeshToGL mesh2gl;
        MyGL::Mesh gl_mesh(mesh2gl.vertices(mesh), mesh2gl.indices(mesh));

        // Set up camera
        // the camera looks at the origin and is positioned at (0, 0, -2) in the beginning
        MyGL::OrbitCamera camera({0.0f, 0.0f, 0.0f});
        camera.set_position({0.0f, 0.0f, -2.0f});

        // Render loop
        float last_frame_time = 0.0f;
        float delta_time = 0.0f;
        auto io = ImGui::GetIO();

        while (!window.should_close())
        {
            // Per-frame time logic
            float current_frame_time = static_cast<float>(glfwGetTime());
            delta_time = current_frame_time - last_frame_time;
            last_frame_time = current_frame_time;

            // Process input
            // ==================================================
            window.poll_events();
            window.process_input();
            window.process_camera_input(camera, delta_time);

            glm::mat4 model =
                glm::mat4(1.0f); // for convenience, we represent the translation of models in the model matrix
            glm::mat4 view = camera.get_view_matrix() *
                             camera.get_model_matrix(); // and write the model matrix of the camera in the view matrix
            auto [width, height] = window.get_framebuffer_size();
            glm::mat4 projection = camera.get_projection_matrix(static_cast<float>(width) / height);

            // Select vertex
            // ==================================================
            if (window.is_mouse_inside() && !ImGui::GetIO().WantCaptureMouse)
            {
                ImVec2 mouse_pos = ImGui::GetMousePos();
                auto [width, height] = MyGL::get_viewport_size();
                pick_vertex.pick({mouse_pos.x, height - mouse_pos.y}, gl_mesh, {model, view, projection});
            }

            // ImGUI
            // ==================================================
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Settings");

            ImGui::Checkbox("Draw wireframe", &flags.draw_wireframe);

            int currentItem = static_cast<int>(flags.draw_mode);
            if (ImGui::Combo("Interaction Mode", &currentItem, InteractionModeItems,
                             IM_ARRAYSIZE(InteractionModeItems)))
                flags.draw_mode = static_cast<Flags::InteractionMode>(currentItem);

            ImGui::End();

            logger.draw();

            // Render
            // ==================================================
            glEnable(GL_MULTISAMPLE);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // draw the mesh
            if (flags.draw_wireframe)
            {
                basic_shader.use();
                basic_shader.set_MVP(model, view, projection);
                basic_shader.set_uniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                gl_mesh.draw(MyGL::Mesh::DrawMode::WIREFRAME);
            }

            phong_shader.use();
            phong_shader.set_MVP(model, view, projection);
            phong_shader.set_uniform("color", glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));
            phong_shader.set_uniform("light_pos", glm::vec3(2.2f, 1.0f, 2.0f));
            phong_shader.set_uniform("light_color", glm::vec3(1.0f, 1.0f, 1.0f));
            phong_shader.set_uniform("view_pos", camera.get_position());
            gl_mesh.draw();

            pick_vertex.highlight_hovered_vertex(gl_mesh, {model, view, projection});

            // render imgui and swap buffers
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            window.swap_buffers();
        }

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}