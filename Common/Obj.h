
/*
 * @file	Obj.h/.cpp
 * @brief	Imports and Exports Wavefront OBJ data.
 * @details	Only supports triangle data.
 *			Does not support materials.
 *			Mesh vertices should use counter-clockwise winding.
 * @author	Matt Drage
 * @date	25/12/2012
 */

#ifndef OBJ_H
#define OBJ_H

#include <fstream>
#include "Geometry.h"

class Obj
{
	public:
		// Constructors & destructors
		Obj() {}
		Obj(const std::string &filename);
		virtual ~Obj();

		// Import & export
		bool Load(const std::string &filename);
		bool Save(const std::string &filename);

		// Mesh management
		void Clear();
		void AddMesh(Mesh *mesh);

	private:
		void ParseVertexCoord(std::ifstream &file, std::vector<float> &vertexCoords);
		void ParseTextureCoord(std::ifstream &file, std::vector<float> &textureCoords);
		void ParseNormal(std::ifstream &file, std::vector<float> &normals);
		std::string Timestamp();
	
		MeshList m_meshList;
};

#endif
