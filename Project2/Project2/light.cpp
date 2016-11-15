#include <iostream>
#include <vector>
#include <string.h>
#include "light.h"
using namespace std;

Source::Source()
{
	for (int i=0; i<4; i++) {
		pos_[i] = 1.0;
		ambient_[i] = 1.0;
		diffuse_[i] = 1.0;
		specular_[i] = 1.0;
	}
}

Light::Light()
{
	lTotal_ = 0;
	for (int i=0; i<4; i++)
		enAmbient_[i] = 1.0;
}

Light::Light(const char * lightFile)
{
	lTotal_ = 0;
	loadLight(lightFile);
}

Light::~Light()
{
}

void Light::loadLight(const char * lightFile)
{
	FILE* data;
	char token[100];
		
	data = fopen(lightFile, "r");
	if (!data) {
		cout << "Can not open Light File \"" << lightFile << "\" !" << endl;
		return;
	}
	cout << endl << lightFile << endl;

	while (!feof(data)) {
		token[0] = '\0';
		fscanf(data, "%s", token);
		if (strcmp(token, "light") == 0) {
			Source l;
			fscanf(data, "%f%f%f", &l.pos_[0], &l.pos_[1], &l.pos_[2]);
			fscanf(data, "%f%f%f", &l.ambient_[0], &l.ambient_[1], &l.ambient_[2]);
			fscanf(data, "%f%f%f", &l.diffuse_[0], &l.diffuse_[1], &l.diffuse_[2]);
			fscanf(data, "%f%f%f", &l.specular_[0], &l.specular_[1], &l.specular_[2]);
			lList_.push_back(l);
		}
		else if (strcmp(token, "ambient") == 0) {
			fscanf(data, "%f%f%f", &enAmbient_[0], &enAmbient_[1], &enAmbient_[2]);
		}
	}

	if (data) fclose(data);
	lTotal_ = lList_.size();
	printf("total lights: %lu\n", lTotal_);
	printf("environment ambient: %f %f %f\n", enAmbient_[0], enAmbient_[1], enAmbient_[2]);
}
