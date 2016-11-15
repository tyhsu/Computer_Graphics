#include <iostream>
#include <vector>
#include <string.h>
#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "mesh.h"
#include "viewing.h"
#include "light.h"
#include "scene.h"
using namespace std;

#define NUM_OBJECT 3
#define NUM_TEXTURE 1
unsigned int texObject[NUM_TEXTURE];

vector<Mesh> objects;
View *view;
Light *light;
Scene *scene;

int winWidth, winHeight;
double rSide[3], forth[3];
size_t selectObj = 0;
int preMouseX = 250, preMouseY = 250;
double movCamUnit = 5.0, movObjUnit = 25;

void lighting();
void loadTexture(char* textureFile, size_t k);
void display();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);
void drag(int x, int y);

int main(int argc, char** argv)
{
	char objFiles[NUM_OBJECT][100] = { "box.obj", "bunny.obj", "venus.obj" };
	//char objFiles[NUM_OBJECT][100] = { "bluebox.obj", "redbox.obj", "yellowbox.obj" };
	char texFiles[NUM_TEXTURE][100] = { "chek_old.bmp" };

	for (size_t i=0; i<NUM_OBJECT; i++) {
		Mesh obj(objFiles[i]);
		objects.push_back(obj);
	}
	view = new View("view.view");
	light = new Light("light.light");
	scene = new Scene("scene.scene");
	/*
	view = new View("scene2.view");
	light = new Light("scene2.light");
	scene = new Scene("scene2.scene");*/
	cout << endl << "--------------------- finish loading files ---------------------" << endl;

	glutInit(&argc, argv);
	glutInitWindowSize(view->width_, view->height_);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("OpenGL Project 2");

	glewInit();
	FreeImage_Initialise();
	glGenTextures(NUM_TEXTURE, texObject);
	for (size_t i=0; i<NUM_TEXTURE; i++)
		loadTexture(texFiles[i], i);
	cout << endl << "-------------------- finish loading textures --------------------" << endl;
	FreeImage_DeInitialise();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(drag);
	glutMainLoop();

	return 0;
}

void lighting()
{
	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	GLenum lightNo = GL_LIGHT0;
	for (vector<Source>::iterator it=light->lList_.begin(); it!=light->lList_.end(); it++) {
		glEnable(lightNo);
		glLightfv(lightNo, GL_POSITION, it->pos_);
		glLightfv(lightNo, GL_DIFFUSE, it->diffuse_);
		glLightfv(lightNo, GL_SPECULAR, it->specular_);
		glLightfv(lightNo, GL_AMBIENT, it->ambient_);
		lightNo++;
	}
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light->enAmbient_);
}

void loadTexture(char* textureFile, size_t k)
{
	FIBITMAP* tImage = FreeImage_Load(FreeImage_GetFileType(textureFile, 0), textureFile);
	FIBITMAP* t32BitsImage = FreeImage_ConvertTo32Bits(tImage);
	int iWidth = FreeImage_GetWidth(t32BitsImage);
	int iHeight = FreeImage_GetHeight(t32BitsImage);

	glBindTexture(GL_TEXTURE_2D, texObject[k]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage));
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	FreeImage_Unload(t32BitsImage);
	FreeImage_Unload(tImage);
}

void display()
{
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      // for use in cleaning color buffers
	glClearDepth(1.0f);                        // Depth Buffer (it's the buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// this makes the scene black and clears the z buffer

	// viewport transformation
	glViewport((GLint)view->x_, (GLint)view->y_, (GLsizei)view->width_, (GLsizei)view->height_);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective((GLdouble)view->fovy_, (GLfloat)winWidth/(GLfloat)winHeight, (GLdouble)view->dnear_, (GLdouble)view->dfar_);
	
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	(GLdouble)view->eye_[0], (GLdouble)view->eye_[1], (GLdouble)view->eye_[2],		// eye
				(GLdouble)view->vat_[0], (GLdouble)view->vat_[1], (GLdouble)view->vat_[2],		// center
				(GLdouble)view->vup_[0], (GLdouble)view->vup_[1], (GLdouble)view->vup_[2] );	// up

	// note that light should be set after gluLookAt
	lighting();

	// draw objects listed in the scene file on the screen
	for (vector<Model>::iterator it=scene->mList_.begin(); it!=scene->mList_.end(); it++) {
		glPushMatrix();
			// find the selected mesh in the scene file
			Mesh* obj = nullptr;
			for (vector<Mesh>::iterator jt=objects.begin(); jt!=objects.end(); jt++)
				if (jt->objFile_ == it->objFile_) {
					obj = &(*jt);
					break;
				}
			if (obj == nullptr)
				cout << "Don't have " << it->objFile_ << " in the object files" << endl;

			glTranslated((GLdouble)it->transfer_[0], (GLdouble)it->transfer_[1], (GLdouble)it->transfer_[2]);
			glRotated((GLdouble)it->angle_, (GLdouble)it->rotate_[0], (GLdouble)it->rotate_[1], (GLdouble)it->rotate_[2]);
			glScaled((GLdouble)it->scale_[0], (GLdouble)it->scale_[1], (GLdouble)it->scale_[2]);

			int lastMaterial = -1;
			for(size_t i=0; i<obj->fTotal_; ++i) {
				// set material property if this face used different material
				if(lastMaterial != obj->faceList_[i].m)
				{
					lastMaterial = (int)obj->faceList_[i].m;
					glMaterialfv(GL_FRONT, GL_AMBIENT  , obj->mList_[lastMaterial].Ka);
					glMaterialfv(GL_FRONT, GL_DIFFUSE  , obj->mList_[lastMaterial].Kd);
					glMaterialfv(GL_FRONT, GL_SPECULAR , obj->mList_[lastMaterial].Ks);
					glMaterialfv(GL_FRONT, GL_SHININESS, &obj->mList_[lastMaterial].Ns);

					//you can obtain the texture name by it->mList_[lastMaterial].map_Kd
					//load them once in the main function before mainloop
					//bind them in display function here
				}

				glBegin(GL_TRIANGLES);
					for (size_t j=0; j<3; ++j) {
						//textex corrd. it->tList[it->faceList_[i][j].t].ptr
						glNormal3fv(obj->nList_[ obj->faceList_[i][j].n ].ptr);
						glVertex3fv(obj->vList_[ obj->faceList_[i][j].v ].ptr);
					}
				glEnd();
			}
		glPopMatrix();
	}
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	winWidth = w;
	winHeight = h;
}

void keyboard(unsigned char key, int x, int y)
{
	forth[0] = (view->vat_[0] - view->eye_[0]) / movCamUnit;
	forth[1] = (view->vat_[1] - view->eye_[1]) / movCamUnit;
	forth[2] = (view->vat_[2] - view->eye_[2]) / movCamUnit;
	rSide[0] = (forth[1] * view->vup_[2] - forth[2] * view->vup_[1]) / movCamUnit;
	rSide[1] = (forth[2] * view->vup_[0] - forth[0] * view->vup_[2]) / movCamUnit;
	rSide[2] = (forth[0] * view->vup_[1] - forth[1] * view->vup_[0]) / movCamUnit;

	if (key == 'w') {	// zoom in
		printf("keyboard: %c\n", key);
		for (size_t i = 0; i < 3; i++)
			view->eye_[i] += forth[i];
		glutPostRedisplay();
	}
	else if (key == 'a') {	// move left
		printf("keyboard: %c\n", key);
		for (size_t i = 0; i < 3; i++)
			view->eye_[i] -= rSide[i];
		glutPostRedisplay();
	}
	else if (key == 's') {	// zoom out
		printf("keyboard: %c\n", key);
		for (size_t i = 0; i < 3; i++)
			view->eye_[i] -= forth[i];
		glutPostRedisplay();
	}
	else if (key == 'd') {	//move right
		printf("keyboard: %c\n", key);
		for (size_t i = 0; i < 3; i++)
			view->eye_[i] += rSide[i];
		glutPostRedisplay();
	}
	else if (key >= '1' && key <= '9') {	// select n-th object
		selectObj = key - '1';
		if (selectObj >= scene->mTotal_)
			selectObj = scene->mTotal_ - 1;
		cout << "selected object: " << selectObj + 1 << endl;
	}
}

void drag(int x, int y)
{
	if (x > preMouseX) {		// right
		scene->mList_[selectObj].transfer_[0] += movObjUnit;
		cout << "moving object " << selectObj + 1 << " right" << endl;
	}
	else if (x < preMouseX) {	// left
		scene->mList_[selectObj].transfer_[0] -= movObjUnit;
		cout << "moving object " << selectObj + 1 << " left" << endl;
	}
	if (y < preMouseY) {		// up
		scene->mList_[selectObj].transfer_[1] += movObjUnit;
		cout << "moving object " << selectObj + 1 << " up" << endl;
	}
	else if (y > preMouseY) {	// down
		scene->mList_[selectObj].transfer_[1] -= movObjUnit;
		cout << "moving object " << selectObj + 1 << " down" << endl;
	}
	preMouseX = x;
	preMouseY = y;
	glutPostRedisplay();
}
