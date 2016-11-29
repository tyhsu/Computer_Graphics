#include <iostream>
#include <stdio.h>
#include <string.h>
#include "viewing.h"
using namespace std;

View::View()
{
	eye_[0] = eye_[1] = eye_[2] = 0.0;
	vat_[0] = vat_[1] = 0.0;	vat_[2] = -1.0;
	vup_[0] = vup_[2] = 0.0;	vup_[1] = 1.0;
	x_ = 0;	y_ = 0;
}

View::View(const char * viewFile)
{
	loadView(viewFile);
}

View::~View()
{
}

void View::loadView(const char * viewFile)
{
	FILE* data;
	char token[100];
	double v[3];

	data = fopen(viewFile, "r");
	if (!data) {
		cout << "Can not open Viewing File \"" << viewFile << "\" !" << endl;
		return;
	}
	cout << endl << viewFile << endl;

	while (!feof(data)) {
		token[0] = '\0';
		fscanf(data, "%s", token);
		if (strcmp(token, "eye") == 0) {
			fscanf(data, "%lf%lf%lf", &v[0], &v[1], &v[2]);
			eye_.set(v);
		}
		else if (strcmp(token, "vat") == 0) {
			fscanf(data, "%lf%lf%lf", &v[0], &v[1], &v[2]);
			vat_.set(v);
		}
		else if (strcmp(token, "vup") == 0) {
			fscanf(data, "%lf%lf%lf", &v[0], &v[1], &v[2]);
			vup_.set(v);
		}
		else if (strcmp(token, "fovy") == 0) {
			fscanf(data, "%lf", &fovy_);
		}
		else if (strcmp(token, "dnear") == 0) {
			fscanf(data, "%lf", &dnear_);
		}
		else if (strcmp(token, "dfar") == 0) {
			fscanf(data, "%lf", &dfar_);
		}
		else if (strcmp(token, "viewport") == 0) {
			fscanf(data, "%d%d%zu%zu", &x_, &y_, &width_, &height_);
		}
	}

	if (data) fclose(data);
	printf("eye: %lf %lf %lf\n", eye_[0], eye_[1], eye_[2]);
	printf("vat: %lf %lf %lf\n", vat_[0], vat_[1], vat_[2]);
	printf("vup: %lf %lf %lf\n", vup_[0], vup_[1], vup_[2]);
	printf("fovy: %lf, dnear: %lf, dfar: %lf\n", fovy_, dnear_, dfar_);
	printf("x: %d, y: %d, w: %zu, h: %zu\n", x_, y_, width_, height_);
	printf("viewing file fclose\n\n");
}

