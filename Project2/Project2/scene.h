#ifndef AFX_SCENE_H_
#define AFX_SCENE_H_

#include <stdio.h>
#include <vector>
#include <string>
using namespace std;

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
};

class Scene
{
public:
	size_t mTotal_;
	vector<Model> mList_;
	Model getModel(const string& objFile);

	Scene();
	Scene(const char* sceneFile);
	~Scene();
	void loadScene(const char* sceneFile);
};

#endif
