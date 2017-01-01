#ifndef AFX_LIGHT_H_
#define AFX_LIGHT_H_

#include <stdio.h>
#include <vector>
using namespace std;

class Source
{
public:
	float pos_[4];
	float ambient_[3];
	float diffuse_[3];
	float specular_[3];

	Source();
};

class Light
{	
public:
	size_t lTotal_;
	vector<Source> lList_;
	float enAmbient_[3];

	Light();
	Light(const char* lightFile);
	~Light();
	void loadLight(const char* lightFile);
};

#endif
