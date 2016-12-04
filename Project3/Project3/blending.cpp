#include <math.h>
#include <time.h>
#include "glut.h"

GLfloat alpha = 0.0;
GLfloat pos[4] = { 0, 10, 10, 0 };
GLfloat dif_l[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat dif_t[4] = { 1.0, 0.5, 0.8, 1.0 };
GLfloat dif_m[4] = { 0.5, 0.8, 0.8, cos(alpha) };

int time1, time2;

void display();
void reshape(GLsizei, GLsizei);
void idle();

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Blending");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, 500, 500);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1 / 1, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(-10, 10, 30, 0, 0, 0, 0, 1, 0);
}

void idle()
{
	time2 = clock();

	alpha += 3.14 * (time2 - time1) / CLK_TCK / 6;
	dif_m[3] = (cos(alpha) + 1) / 2;

	time1 = time2;
	glutPostRedisplay();
}

void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif_l);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif_t);
	glutSolidTeapot(5);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif_m);
	glBegin(GL_POLYGON);
	glNormal3f(0, 0, 1);
	glVertex3f(-5, -5, 10);
	glVertex3f(5, -5, 10);
	glVertex3f(5, 5, 10);
	glVertex3f(-5, 5, 10);
	glEnd();
	glutSwapBuffers();
}