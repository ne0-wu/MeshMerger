#pragma once

#include "Mesh.h"
#include <OpenMesh/Core/Utils/PropertyManager.hh>

class Dijkstra
{
  public:
    // Constructor for non-targeted Dijkstra's algorithm
    Dijkstra(const Mesh &mesh, Mesh::VertexHandle source, const OpenMesh::EProp<double> &edge_weight);

    // Constructor for targeted Dijkstra's algorithm
    // would terminate when the target is reached
    Dijkstra(const Mesh &mesh, Mesh::VertexHandle source, Mesh::VertexHandle target,
             const OpenMesh::EProp<double> &edge_weight);

    void run();

    double get_distance(Mesh::VertexHandle vertex) const
    {
        return distance[vertex];
    }

    Mesh::VertexHandle get_previous(Mesh::VertexHandle vertex) const
    {
        return previous[vertex];
    }

  private:
    const Mesh &mesh;
    Mesh::VertexHandle source;
    Mesh::VertexHandle target = Mesh::VertexHandle();
    const OpenMesh::EProp<double> &edge_weight;

    OpenMesh::VProp<double> distance;
    OpenMesh::VProp<Mesh::VertexHandle> previous;
};