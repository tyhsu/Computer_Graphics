#ifndef AFX_SCENE_H_
#define AFX_SCENE_H_

#include <stdio.h>
#include <vector>
#include <string>
using namespace std;

class TextureMap;

class Model
{
	class Vec3
	{
	public:
		double ptr[3];
		void set(double *v)
		{
			for (size_t i = 0; i<3; i++)
				ptr[i] = v[i];
		}
		inline double& operator[](size_t index)
		{
			return ptr[index];
		}
	};

public:
	string objFile_;
	double angle_;
	Vec3 scale_;
	Vec3 rotate_;
	Vec3 transfer_;

	TextureMap texture_;
};

class TextureMap
{
public:
	int technique_;
	size_t imageTotal_;
	vector<string> imageList_;	//list of image file name

	void reset();
};

class Scene
{
public:
	size_t texTotal_;
	vector<TextureMap> texList_;
	size_t modelTotal_;
	vector<Model> modelList_;	//list of models

	Scene();
	Scene(const char* sceneFile);
	~Scene();
	void loadScene(const char* sceneFile);
};

#endif
