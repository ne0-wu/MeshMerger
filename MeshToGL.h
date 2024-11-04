#pragma once

#include "Mesh.h"
#include "MyGL/Mesh.h"

class MeshToGL
{
public:
    static std::vector<MyGL::Vertex> vertices(const Mesh &mesh)
    {
        std::vector<MyGL::Vertex> vertices;
        vertices.reserve(mesh.n_vertices());

        const auto zero3d = Eigen::Vector3d(0.0f, 0.0f, 0.0f);
        const auto zero2d = Eigen::Vector2d(0.0f, 0.0f);

        for (const auto &v : mesh.vertices())
        {
            const auto &point = mesh.point(v);
            const auto &normal = mesh.has_vertex_normals() ? mesh.normal(v) : zero3d;
            const auto &tex_coord = mesh.has_vertex_texcoords2D() ? mesh.texcoord2D(v) : zero2d;
            vertices.push_back({{point[0], point[1], point[2]},
                                {normal[0], normal[1], normal[2]},
                                {tex_coord[0], tex_coord[1]}});
        }
        return vertices;
    }

    static std::vector<unsigned int> indices(const Mesh &mesh)
    {
        std::vector<unsigned int> indices;
        indices.reserve(mesh.n_faces() * 3);

        for (const auto &f : mesh.faces())
            for (const auto &v : mesh.fv_range(f))
                indices.push_back(v.idx());
        return indices;
    }
};