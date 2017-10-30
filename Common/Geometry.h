
/*
 * @file	Geometry.h/.cpp
 * @brief	Mesh and vertex 3D geometry types.
 * @details	Used in Wavefront OBJ import/export.
 * @author	Matt Drage
 * @date	25/12/2012
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Vector2.h"
#include "Vector3.h"
#include <vector>
#include <string>

// A point in a mesh
struct Vertex
{
	Vector3 position;
	Vector2 texCoord;
	Vector3 normal;
};
typedef std::vector<Vertex> VertexList;

// A collection of vertices (stored as triangles)
struct Mesh
{
	std::string name;
	VertexList vertices;
	
	// Constructors
	Mesh();
	Mesh(const std::string &name);
	
	// Reset mesh
	void Clear();
	
	// Add primitive (Quad triangulates)
	void AddTriangle(const Vertex &a, const Vertex &b, const Vertex &c);
	void AddQuad(const Vertex &a, const Vertex &b, const Vertex &c, const Vertex &d);
	
	// Flat shading
	void CalculateNormals();
};
typedef std::vector<Mesh*> MeshList;

#endif
