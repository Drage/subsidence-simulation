
#include "Obj.h"
#include <sstream>
#include <iostream>
#include <map>

// Used to test if a vector already exists in a std::map
// Overrides default compare (<operator) which compares by magnitude
struct VectorMapCompare
{
	bool operator() (const Vector3 &a, const Vector3 &b) const
	{
		if (a.x < b.x)
			return true;
		else if (a.x > b.x)
			return false;
		else
		{
			if (a.y < b.y)
				return true;
			else if (a.y > b.y)
				return false;
			else
			{
				if (a.z < b.z)
					return true;
				else
					return false;
			}
		}
	}
};

Obj::Obj(const std::string &filename)
{
	Load(filename);
}

Obj::~Obj()
{
	for (std::vector<Mesh*>::iterator i = m_meshList.begin(); i != m_meshList.end(); i++)
		delete (*i);
	m_meshList.clear();
}

bool Obj::Load(const std::string &filename)
{
	std::ifstream file;
	file.open(filename.c_str(), std::ios::in);

	// Check file is open
	if (!file.is_open())
	{
		std::cerr << "Could not load .obj file '" + filename + "'\n";
        return false;
	}

	// Input buffers
	std::string input;
	char buffer[256] = {0};
	char slash;

	// Temporary buffers to store vertex data
    std::vector<float> vertexCoords;
	std::vector<float> textureCoords;
	std::vector<float> normals;

	// Temp variables to store index data
	int v[3];
    int vt[3];
    int vn[3];

	Mesh *currentMesh = NULL;

	while (file.good())
	{
		file >> input;

		switch (input.at(0))
        {
			case '#': // Comment
				file.getline(buffer, sizeof(buffer));
				break;

			case 'g': // New mesh
			case 'o': // (g = group, o = object)
					if (currentMesh)
						m_meshList.push_back(currentMesh);
					currentMesh = new Mesh();
					file >> currentMesh->name;
				break;

			case 'v': // Verts
				if (input.length() == 1)
					ParseVertexCoord(file, vertexCoords);
				else if (input.at(1) == 'n')
					ParseNormal(file, normals);
				else if (input.at(1) == 't')
					ParseTextureCoord(file, textureCoords);
				break;

			case 'f': // Faces
				file >> v[0] >> slash >> vt[0] >> slash >> vn[0]
					 >> v[1] >> slash >> vt[1] >> slash >> vn[1]
					 >> v[2] >> slash >> vt[2] >> slash >> vn[2];
				
				// Offset since .obj index's start at 1
				for (int i = 0; i < 3; i++)
				{
					v[i]--;
					vt[i]--;
					vn[i]--;
				}

				// Add the triangle to the index buffer
				for (int i = 0; i < 3; i++)
				{
					Vertex vertex;
					vertex.position[0] = vertexCoords[v[i] * 3];
					vertex.position[1] = vertexCoords[v[i] * 3 + 1];
					vertex.position[2] = vertexCoords[v[i] * 3 + 2];			
					vertex.normal[0] = normals[vn[i] * 3];
					vertex.normal[1] = normals[vn[i] * 3 + 1];
					vertex.normal[2] = normals[vn[i] * 3 + 2];
					vertex.texCoord[0] = textureCoords[vt[i] * 2];
					vertex.texCoord[1] = textureCoords[vt[i] * 2 + 1];
					currentMesh->vertices.push_back(vertex);
				}
				break;

			default: // Ignore
				file.getline(buffer, sizeof(buffer));
				break;
		}
	}

	if (currentMesh)
		m_meshList.push_back(currentMesh);

	file.close();
	return true;
}

void Obj::ParseVertexCoord(std::ifstream &file, std::vector<float> &vertexCoords)
{
	float number;
	for (int i = 0; i < 3; i++)
	{
		file >> number;
		vertexCoords.push_back(number);
	}
}

void Obj::ParseTextureCoord(std::ifstream &file, std::vector<float> &textureCoords)
{
	float number;
	for (int i = 0; i < 2; i++)
	{
		file >> number;
		textureCoords.push_back(number);
	}
}

void Obj::ParseNormal(std::ifstream &file, std::vector<float> &normals)
{
	float number;
	for (int i = 0; i < 3; i++)
	{
		file >> number;
		normals.push_back(number);
	}
}

bool Obj::Save(const std::string &filename)
{
	std::ofstream file;
	file.open(filename.c_str(), std::ios::out);
	
	// Check file is open
	if (!file.is_open())
	{
		std::cerr << "Could not save .obj file '" + filename + "'\n";
        return false;
	}
	
	file << "# Drage Wavefront OBJ Exporter v1\n";
	file << "# File Created: " << Timestamp() << "\n";
	
	//file.setf(std::ios::fixed, std::ios::floatfield);
	file.precision(4);
	
	typedef std::map<Vector3, int, VectorMapCompare> CacheType;
	CacheType vCache, nCache, tCache;
	
	for (int m = 0; m < m_meshList.size(); m++)
	{
		VertexList *vertices = &m_meshList[m]->vertices;
		VertexList::iterator v;
		
		// Output vertices
		for (v = vertices->begin(); v != vertices->end(); v++)
		{
			CacheType::iterator i = vCache.find(v->position);
			if (i == vCache.end())
			{
				vCache[v->position] = vCache.size();
				file << "v " << v->position.x << " " << v->position.y << " " << v->position.z << "\n";
			}
		}
		file << "# " << vCache.size() << " vertices\n\n";
		
		// Output texture coords
		for (v = vertices->begin(); v != vertices->end(); v++)
		{
			CacheType::iterator i = tCache.find(v->texCoord);
			if (i == tCache.end())
			{
				tCache[v->texCoord] = tCache.size();
				file << "vt " << v->texCoord.x << " " << v->texCoord.y << "\n";
			}
		}
		file << "# " << tCache.size() << " texture coords\n\n";
		
		// Output normals
		for (v = vertices->begin(); v != vertices->end(); v++)
		{
			CacheType::iterator i = nCache.find(v->normal);
			if (i == nCache.end())
			{
				nCache[v->normal] = nCache.size();
				file << "vn " << v->normal.x << " " << v->normal.y << " " << v->normal.z << "\n";
			}
		}
		file << "# " << nCache.size() << " vertex normals\n\n";
		
		// Output faces
		if (m_meshList[m]->name == "")
			file << "g Mesh" << m << "\n";
		else 
			file << "g " << m_meshList[m]->name << "\n";
		int i = 0;
		for (v = vertices->begin(); v != vertices->end(); v++, i++)
		{
			if (i % 3 == 0) file << "f ";
			file << vCache[v->position] << "/" << tCache[v->texCoord] << "/" << nCache[v->normal];
			file << ((i % 3 == 2) ? "\n" : " ");
		}
		file << "# " << i/3 << " faces\n\n";
	}
	
	file.close();
	return true;
}
	
std::string Obj::Timestamp()
{
    time_t tTime = time(NULL); 
	std::stringstream ss;
	return asctime(localtime(&tTime));
}

void Obj::Clear()
{
	m_meshList.clear();
}

void Obj::AddMesh(Mesh *mesh)
{
	m_meshList.push_back(mesh);
}
