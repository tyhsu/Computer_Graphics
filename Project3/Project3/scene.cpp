#include <iostream>
#include <vector>
#include <string.h>
#include "scene.h"
using namespace std;

Scene::Scene()
{
	texTotal_ = 0;
}

Scene::Scene(const char * sceneFile)
{
	texTotal_ = 0;
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

			m.texIndex_ = texList_.size() - 1;	// the index in texList_
			modelList_.push_back(m);
		}
		else if (strcmp(token, "no-texture") == 0) {
			Textures t;
			t.technique_ = 0;
			t.imageTotal_ = 0;
			texList_.push_back(t);
		}
		else if (strcmp(token, "single-texture") == 0) {
			Textures t;
			t.technique_ = 1;
			t.imageTotal_ = 1;
			fscanf(data, "%s", buf);
			TexImage ti(buf);
			t.imageList_.push_back(ti);
			texList_.push_back(t);
		}
		else if (strcmp(token, "multi-texture") == 0) {
			Textures t;
			t.technique_ = 2;
			t.imageTotal_ = 2;
			for (size_t i = 0; i < t.imageTotal_; i++) {
				fscanf(data, "%s", buf);
				TexImage ti(buf);
				t.imageList_.push_back(ti);
			}
			texList_.push_back(t);
		}
		else if (strcmp(token, "cube-map") == 0) {
			Textures t;
			t.technique_ = 3;
			t.imageTotal_ = 6;
			for (size_t i = 0; i < t.imageTotal_; i++) {
				fscanf(data, "%s", buf);
				TexImage ti(buf);
				t.imageList_.push_back(ti);
			}
			texList_.push_back(t);
		}
	}

	if (data) fclose(data);
	texTotal_ = texList_.size();
	modelTotal_ = modelList_.size();

	printf("total models: %zu\n", texTotal_);
}

Model& Scene::searchModel(const string modelName)
{
	for (vector<Model>::iterator it = modelList_.begin(); it != modelList_.end(); it++)
		if (it->objFile_ == modelName)
			return *it;
	cout << "Don't have " << modelName << " in modelList in Scene" << endl;
	return Model();
}
