#include "glut.h"
void GL_display()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearAccum(0.0, 0.0, 0.0, 0.0);
	glClear(GL_ACCUM_BUFFER_BIT);
	for (int i = 0; i < 5; i++) {
		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();
		gluLookAt(i*0.4, 0.0, 8.0, i*0.4, 0.0, 0.0, 0.0, 1.0, 0.0);
		glColor3f(1, 0, 0);
		glutWireTeapot(2);
		glAccum(GL_ACCUM, (float)i);
	}
	glAccum(GL_RETURN, 0.1);
	glutSwapBuffers();
}

void GL_reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 50.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_ACCUM);
	glutCreateWindow("Accumulation");
	glutDisplayFunc(GL_display);
	glutReshapeFunc(GL_reshape);
	glutMainLoop();
	return 0;
}