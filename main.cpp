#include <OpenMesh/Core/IO/MeshIO.hh>

#include "Dijkstra.h"
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

class StatusBar
{
  public:
    StatusBar(const std::string &text) : text(text)
    {
    }

    void set_text(const std::string &text)
    {
        this->text = text;
    }

    void draw()
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - 30));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 30));
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
                                        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration;
        if (ImGui::Begin("##StatusBar", nullptr, window_flags))
        {
            float textHeight = ImGui::GetTextLineHeight();
            ImGui::SetCursorPosY((30 - textHeight) * 0.5f);
            ImGui::Text("%s", text.c_str());
        }
        ImGui::End();
    }

  private:
    std::string text;
} status_bar("no message");

MyGL::LogConsole logger;

// ==================================================

class SelectSeam
{
  public:
    SelectSeam(const Mesh &mesh) : mesh(mesh), gl_selected_vertices({glm::vec3(0.0f)})
    {
    }

    void add_vertex(Mesh::VertexHandle new_vertex)
    {
        if (is_closed())
        {
            // The path is already closed
            ; // Do nothing
        }
        else if (selected_vertices.empty())
        {
            // Start of the path
            if (mesh.is_boundary(new_vertex))
                // The first vertex should be on the boundary
                selected_vertices.push_back(new_vertex);
            update_gl_selected_vertices();
        }
        else
        {
            // Add the new vertex to the path
            auto last_vertex = selected_vertices.back();
            Dijkstra dijkstra = Dijkstra::compute(mesh, last_vertex, new_vertex);
            if (dijkstra.has_path(new_vertex))
            {
                auto path = dijkstra.get_path(new_vertex);
                selected_vertices.insert(selected_vertices.end(), path.begin(), path.end());
            }
            update_gl_selected_vertices();
        }
    }

    bool is_closed() const
    {
        return selected_vertices.size() > 1 && mesh.is_boundary(selected_vertices.back());
    }

    void draw(const std::tuple<glm::mat4, glm::mat4, glm::mat4> &mvp) const
    {
        if (selected_vertices.size() > 0)
        {
            basic_shader.use();
            basic_shader.set_MVP(mvp);
            basic_shader.set_uniform("color", color);
            gl_selected_vertices.draw();
        }
    }

  private:
    void update_gl_selected_vertices()
    {
        std::vector<glm::vec3> vertices;
        vertices.reserve(selected_vertices.size());
        for (const auto &v : selected_vertices)
        {
            auto point = mesh.point(v);
            vertices.emplace_back(point[0], point[1], point[2]);
        }
        gl_selected_vertices.update(vertices);
    }

    const Mesh &mesh;
    std::vector<Mesh::VertexHandle> selected_vertices;
    MyGL::PointCloud gl_selected_vertices;

    MyGL::ShaderProgram basic_shader{MyGL::read_file_to_string("data/shaders/basic.vert"),
                                     MyGL::read_file_to_string("data/shaders/round_point.frag")};

    glm::vec4 color{0.7f, 0.2f, 0.6f, 1.0f};
};

// ==================================================

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
        if (!OpenMesh::IO::read_mesh(mesh, "data/models/camelhead.obj"))
            throw std::runtime_error("Failed to read mesh from file");

        SelectSeam select_seam_0(mesh);

        // Move mesh to [-1, 1]^3
        glm::mat4 model; // for convenience, we represent translation of models in the model matrix
        Eigen::Vector3d center{0.0, 0.0, 0.0},
            min{std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
                std::numeric_limits<double>::max()},
            max{std::numeric_limits<double>::min(), std::numeric_limits<double>::min(),
                std::numeric_limits<double>::min()};
        for (const auto &v : mesh.vertices())
        {
            const auto &point = mesh.point(v);
            min = min.cwiseMin(point);
            max = max.cwiseMax(point);
        }
        center = (min + max) / 2.0;
        const auto scale = 2.0 / (max - min).maxCoeff();

        model = glm::scale(glm::mat4(1.0f), glm::vec3(scale)) *
                glm::translate(glm::mat4(1.0f), glm::vec3(-center[0], -center[1], -center[2]));

        // Compute normals (for Phong shading)
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

            // write the model matrix of camera in the view matrix
            glm::mat4 view = camera.get_view_matrix() * camera.get_model_matrix();
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

            auto selected_vertex = Mesh::VertexHandle(pick_vertex.get_picked_vertex());
            if (ImGui::IsMouseClicked(0) && selected_vertex.is_valid())
            {
                select_seam_0.add_vertex(selected_vertex);
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

            status_bar.draw();

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

            select_seam_0.draw({model, view, projection});

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