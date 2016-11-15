#include <iostream>
#include <vector>
#include <string.h>
#include "scene.h"
using namespace std;

Scene::Scene()
{
	mTotal_ = 0;
}

Scene::Scene(const char * sceneFile)
{
	mTotal_ = 0;
	loadScene(sceneFile);
}

Scene::~Scene()
{
}

void Scene::loadScene(const char * sceneFile)
{
	FILE* data;
	char token[100], buf[100];
	double v[3];
		
	data = fopen(sceneFile, "r");
	if (!data) {
		cout << "Can not open Scene File \"" << sceneFile << "\" !" << endl;
		return;
	}
	cout << endl << sceneFile << endl;

	while (!feof(data)) {
		token[0] = '\0';
		fscanf(data, "%s", token);
		if (strcmp(token, "model") == 0) {
			Model m;
			fscanf(data, "%s", buf);
			m.objFile_ = buf;
			fscanf(data, "%lf%lf%lf", &v[0], &v[1], &v[2]);
			m.scale_.set(v);
			fscanf(data, "%lf%lf%lf%lf", &m.angle_, &v[0], &v[1], &v[2]);
			m.rotate_.set(v);
			fscanf(data, "%lf%lf%lf", &v[0], &v[1], &v[2]);
			m.transfer_.set(v);
			cout << m.objFile_ << endl;
			mList_.push_back(m);
		}
	}

	if (data) fclose(data);
	mTotal_ = mList_.size();
	printf("total models: %lu\n", mTotal_);
}

Model Scene::getModel(const string& objFile)
{
	for (vector<Model>::iterator it=mList_.begin(); it!=mList_.end(); it++)
		if (it->objFile_ == objFile)
			return *it;
	Model null;
	return null;
}
