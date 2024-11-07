#pragma once

#include <functional>

#include "Mesh.h"
#include <OpenMesh/Core/Utils/PropertyManager.hh>

class Dijkstra
{
  public:
    using EdgeWeightFunc = std::function<double(Mesh::EdgeHandle)>;

    // Default constructor
    Dijkstra(const Mesh &mesh, EdgeWeightFunc edge_weight, Mesh::VertexHandle source,
             Mesh::VertexHandle target = Mesh::VertexHandle());

    // Use Euclidean distance as edge weight by default
    Dijkstra(const Mesh &mesh, Mesh::VertexHandle source, Mesh::VertexHandle target = Mesh::VertexHandle());

    // Use edge weights from the given property
    Dijkstra(const Mesh &mesh, const OpenMesh::EProp<double> &weights, Mesh::VertexHandle source,
             Mesh::VertexHandle target = Mesh::VertexHandle());

    template <typename... Args> static Dijkstra compute(Args &&...args)
    {
        static_assert(std::is_constructible_v<Dijkstra, Args...>, "Invalid arguments for Dijkstra construction");
        Dijkstra dijkstra(std::forward<Args>(args)...);
        dijkstra.run();
        return dijkstra;
    }

    void run();

    bool has_path(Mesh::VertexHandle vertex) const
    {
        return distance[vertex] != std::numeric_limits<double>::infinity();
    }

    double get_distance(Mesh::VertexHandle vertex) const
    {
        return distance[vertex];
    }

    std::vector<Mesh::VertexHandle> get_path(Mesh::VertexHandle vertex) const
    {
        if (!has_path(vertex))
            return {};

        std::vector<Mesh::VertexHandle> path;
        for (auto v = vertex; v.is_valid(); v = previous[v])
            path.push_back(v);
        std::reverse(path.begin(), path.end());
        return path;
    }

    Mesh::VertexHandle get_previous(Mesh::VertexHandle vertex) const
    {
        return previous[vertex];
    }

  private:
    const Mesh &mesh;
    const EdgeWeightFunc edge_weight;

    Mesh::VertexHandle source;
    Mesh::VertexHandle target;

    OpenMesh::VProp<double> distance;
    OpenMesh::VProp<Mesh::VertexHandle> previous;
};