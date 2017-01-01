// mesh.h: interface for the mesh class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_MESH_H_
#define AFX_MESH_H_

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>

class Material
{
public:
	float Ka[4];		//ambient coefficient
	float Kd[4];		//diffuse coefficient
	float Ks[4];		//specular coefficient
	float Ke[4];		//emissive coefficient
	float Tf[4];		//transmission filter
	float Ns;			//shiness, specular exponent
	float Ni;			//optical density

	float Tr;			//Transpant (or d)
	int illum;			//illumination models
	std::string map_Ka;	//ambient texture
	std::string map_Kd;	//diffuse texture
	std::string map_Ks;	//specular texture

						//This mtl loader is still incomplete
						//Please see http://en.wikipedia.org/wiki/Wavefront_.objFile#Material_template_library

	Material()
	{
		Ka[0] = Ka[1] = Ka[2] = 0.2, Ka[3] = 1.0;
		Kd[0] = Kd[1] = Kd[2] = 0.8, Kd[3] = 1.0;
		Ks[0] = Ks[1] = Ks[2] = 0.0, Ks[3] = 1.0;
		Ke[0] = Ke[1] = Ke[2] = 0.0, Ke[3] = 1.0;
		Ns = 0.0f;
		Tr = 0.0f;
	}
};

class Mesh
{
	class Vertex		// store the property of a basic vertex
	{
	public:
		size_t v;		// vertex (index of vList_)
		size_t n;		// normal (index of nList_)
		size_t t;		// texture (index of tList_)

		Vertex() {};
		Vertex(size_t v_index, size_t n_index, size_t t_index = 0)
		{
			v = v_index;
			n = n_index;
			t = t_index;
		}
	};

	class Vec3		// structure for vList_, nList_, tList_
	{
	public:
		float ptr[3];
		Vec3(float *v)
		{
			for (size_t i = 0; i<3; i++)
				ptr[i] = v[i];
		}
		inline float& operator[](size_t index)
		{
			return ptr[index];
		}
	};

	class Face			// structure for faceList_
	{
	public:
		Vertex v[3];	// 3 vertex for each face
		int    m;		// Material (index of Material)

		Face(Vertex &v1, Vertex &v2, Vertex &v3, int m_index)
		{
			v[0] = v1;
			v[1] = v2;
			v[2] = v3;
			m = m_index;
		}
		inline Vertex& operator[](size_t index)
		{
			return v[index];
		}
	};

	void	loadMtl(std::string tex_file);

public:
	/////////////////////////////////////////////////////////////////////////////
	// Loading Object
	/////////////////////////////////////////////////////////////////////////////

	std::string					objFile_;
	std::string					matFile_;

	size_t						matTotal_;	// total Material 
	std::map<std::string, size_t>matMap_;	// matMap_[Material_name] = Material_ID
	std::vector<Material>		matList_;	// Material ID (every Mesh has at most 100 Materials)	

	std::vector<Vec3>			vList_;		// Vertex List (Position) - world cord.
	std::vector<Vec3>			nList_;		// Normal List
	std::vector<Vec3>			tList_;		// Texture List
	std::vector<Face>			faceList_;	// Face List

	size_t	vTotal_, tTotal_, nTotal_, fTotal_; // number of total vertice, faces, texture coord., normal vecters, and faces

	void	loadMesh(std::string objFile);

	Mesh();
	Mesh(const char* objFile);
	virtual ~Mesh();

	void init(const char* objFile);

private:
	FILE		*mtlFilePtr_;
};

#endif
