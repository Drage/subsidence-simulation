
#include "Geometry.h"

Mesh::Mesh()
{
	this->name = "";
}

Mesh::Mesh(const std::string &name)
{
	this->name = name;
}

void Mesh::Clear()
{
	vertices.clear();
}

void Mesh::AddTriangle(const Vertex &a, const Vertex &b, const Vertex &c)
{
	vertices.push_back(a);
	vertices.push_back(b);
	vertices.push_back(c);
}

void Mesh::AddQuad(const Vertex &a, const Vertex &b, const Vertex &c, const Vertex &d)
{
	vertices.push_back(a);
	vertices.push_back(b);
	vertices.push_back(c);
	
	vertices.push_back(c);
	vertices.push_back(d);
	vertices.push_back(a);
}

void Mesh::CalculateNormals()
{
	for (int i = 0; i < vertices.size(); i += 3)
	{
		Vector3 ab = vertices[i + 1].position - vertices[i].position;
		Vector3 ac = vertices[i + 2].position - vertices[i].position;
		
		ab.Normalize();
		ac.Normalize();
		
		Vector3 n = ab.Cross(ac).Normalize();
		
		vertices[i].normal = n;
		vertices[i + 1].normal = n;
		vertices[i + 2].normal = n;
	}
}
