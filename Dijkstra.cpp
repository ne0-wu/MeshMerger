#include "Dijkstra.h"

#include <queue>

Dijkstra::Dijkstra(const Mesh &mesh, EdgeWeightFunc edge_weight, Mesh::VertexHandle source, Mesh::VertexHandle target)
    : mesh(mesh), edge_weight(edge_weight), source(source), target(target),
      distance(std::numeric_limits<double>::infinity(), mesh), previous(Mesh::VertexHandle(), mesh)
{
}

Dijkstra::Dijkstra(const Mesh &mesh, Mesh::VertexHandle source, Mesh::VertexHandle target)
    : Dijkstra(
          mesh,
          [&mesh](Mesh::EdgeHandle e) -> double {
              auto he = mesh.halfedge_handle(e, 0);
              return (mesh.point(mesh.to_vertex_handle(he)) - mesh.point(mesh.from_vertex_handle(he))).norm();
          },
          source, target)
{
}

Dijkstra::Dijkstra(const Mesh &mesh, const OpenMesh::EProp<double> &weights, Mesh::VertexHandle source,
                   Mesh::VertexHandle target)
    : Dijkstra(mesh, [&weights](Mesh::EdgeHandle e) -> double { return weights[e]; }, source, target)
{
}

void Dijkstra::run()
{
    using QueueElem = std::pair<double, Mesh::VertexHandle>;
    std::priority_queue<QueueElem, std::vector<QueueElem>, std::greater<>> queue;
    std::vector<bool> visited(mesh.n_vertices(), false);

    distance[source] = 0.0;
    queue.push({0.0, source});

    while (!queue.empty())
    {
        auto [dist, current] = queue.top();
        queue.pop();

        if (visited[current.idx()])
            continue;
        visited[current.idx()] = true;

        if (current == target)
            break;

        for (const auto &half_edge : mesh.voh_range(current))
        {
            auto neighbor = mesh.to_vertex_handle(half_edge);

            if (visited[neighbor.idx()])
                continue;

            auto new_dist = dist + edge_weight(half_edge.edge());
            if (new_dist < distance[neighbor])
            {
                distance[neighbor] = new_dist;
                previous[neighbor] = current;
                queue.push({new_dist, neighbor});
            }
        }
    }
}
