#include "glut.h"
void GL_display()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glClearStencil(0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glColor3f(0.0, 1.0, 1.0);
	glutSolidCube(16.0);

	glClear(GL_DEPTH_BUFFER_BIT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
	glStencilFunc(GL_EQUAL, 1, 1);
	glColor3f(1.0, 1.0, 1.0);
	glutSolidTeapot(8);

	glStencilFunc(GL_NOTEQUAL, 1, 1);
	glColor3f(1.0, 1.0, 0.0);
	glPushMatrix();
	glTranslatef(10, 0, 0);
	glutSolidSphere(12, 10, 6);
	glPopMatrix();

	glFlush();
}

void GL_reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.5, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutCreateWindow("Stencil Buffer");
	glutDisplayFunc(GL_display);
	glutReshapeFunc(GL_reshape);
	glutMainLoop();
	return 0;
}