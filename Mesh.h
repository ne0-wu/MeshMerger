#pragma once

#include <Eigen/Dense>

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Geometry/EigenVectorT.hh>

#include <OpenMesh/Core/Utils/PropertyManager.hh>

struct MyTraits : public OpenMesh::DefaultTraits
{
	typedef Eigen::Vector3d Point;
	typedef Eigen::Vector3d Normal;
	typedef Eigen::Vector2d TexCoord2D;
};

using Mesh = OpenMesh::TriMesh_ArrayKernelT<MyTraits>;