#include "glut.h"
void GL_display()
{
	// clear the buffer
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearAccum(0.0, 0.0, 0.0, 0.0);
	glClear(GL_ACCUM_BUFFER_BIT);
	for (int i = 0; i < 360; i++) {
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1.0, 1 - (float)i / 180, 1 - (float)i / 360);
		glPushMatrix();
		glTranslatef(0, -6, -10);
		glRotatef(i, 0, 1, 0);
		glutWireTeapot(5);
		glPopMatrix();
		glAccum(GL_ACCUM, 0.01);
	}
	glAccum(GL_RETURN, 1.0);
	glutSwapBuffers();
}

void GL_reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 3.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
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