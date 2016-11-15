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
	TextureMap ttmp;
		
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

			m.texture_ = ttmp;
			modelList_.push_back(m);
		}
		else if (strcmp(token, "no-texture") == 0) {
			ttmp.reset();
			ttmp.technique_ = 0;
			ttmp.imageTotal_ = 0;
			texList_.push_back(ttmp);
		}
		else if (strcmp(token, "single-texture") == 0) {
			ttmp.reset();
			ttmp.technique_ = 1;
			ttmp.imageTotal_ = 1;
			fscanf(data, "%s", buf);
			ttmp.imageList_.push_back(buf);
			texList_.push_back(ttmp);
		}
		else if (strcmp(token, "multi-texture") == 0) {
			ttmp.reset();
			ttmp.technique_ = 2;
			ttmp.imageTotal_ = 2;
			for (size_t i = 0; i < ttmp.imageTotal_; i++) {
				fscanf(data, "%s", buf);
				ttmp.imageList_.push_back(buf);
			}
			texList_.push_back(ttmp);
		}
		else if (strcmp(token, "cube-map") == 0) {
			ttmp.reset();
			ttmp.technique_ = 3;
			ttmp.imageTotal_ = 6;
			for (size_t i = 0; i < ttmp.imageTotal_; i++) {
				fscanf(data, "%s", buf);
				ttmp.imageList_.push_back(buf);
			}
			texList_.push_back(ttmp);
		}
	}

	if (data) fclose(data);
	texTotal_ = texList_.size();
	modelTotal_ = modelList_.size();
	
	printf("total models: %lu\n", texTotal_);
}

void TextureMap::reset()
{
	technique_ = 0;
	imageTotal_ = 0;
	imageList_.clear();
	modelTotal_ = 0;
	modelList_.clear();
}