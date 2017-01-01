#ifndef AFX_VIEWING_H_
#define AFX_VIEWING_H_

#include <stdio.h>

class View
{
	class Vec3		// structure for eye, vat, vup
	{
	public:
		double ptr[3];
		void set(double *v)
		{
			for (size_t i=0; i<3; i++)
				ptr[i] = v[i];
		}
		double* add(double* v, double mul)
		{
			double result[3];
			for (size_t i=0; i<3; i++)
				result[i] = ptr[i] + v[i]*mul;
			return result;
		}
		double* cross(double* v)
		{
			double result[3];
			result[0] = ptr[1] * v[2] - ptr[2] * v[1];
			result[1] = ptr[2] * v[0] - ptr[0] * v[2];
			result[2] = ptr[0] * v[1] - ptr[1] * v[0];
			return result;
		}
		inline double& operator[](size_t index)
		{
			return ptr[index];
		}
	};
public:
	Vec3 eye_, vat_, vup_;
	double fovy_;
	double dnear_;
	double dfar_;
	int x_, y_;
	size_t width_, height_;

	View();
	View(const char* viewFile);
	~View();
	void loadView(const char* viewFile);
};

#endif
