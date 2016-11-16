#include <iostream>
#include <vector>
#include <string.h>
#include <string>
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

void viewing();
void lighting();
void loadTexture(char* textureFile, size_t k);
void loadCubeMap(char** textureFiles, size_t k);
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
	size_t texNo = 0;
	// if using cube-map, need to use another function to load textures
	for (vector<Textures>::iterator it=scene->texList_.begin(); it!=scene->texList_.end(); it++) {
		if (it->technique_ != 3) {
			for (vector<string>::iterator jt=it->imageList_.begin(); jt!=it->imageList_.end(); jt++)
				loadTexture(*jt.c_str(), texNo++);
		}
		else {
			char** iList = it->imageList_.data();
			loadCubeMap(iList, texNo++);
		}
	}
	cout << endl << "-------------------- finish loading textures --------------------" << endl;
	FreeImage_DeInitialise();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(drag);
	glutMainLoop();

	return 0;
}

void viewing()
{
	// viewport transformation
	glViewport((GLint)view->x_, (GLint)view->y_, (GLsizei)view->width_, (GLsizei)view->height_);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective((GLdouble)view->fovy_, (GLfloat)winWidth / (GLfloat)winHeight, (GLdouble)view->dnear_, (GLdouble)view->dfar_);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt((GLdouble)view->eye_[0], (GLdouble)view->eye_[1], (GLdouble)view->eye_[2],	// eye
		(GLdouble)view->vat_[0], (GLdouble)view->vat_[1], (GLdouble)view->vat_[2],			// center
		(GLdouble)view->vup_[0], (GLdouble)view->vup_[1], (GLdouble)view->vup_[2]);			// up
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
	glGenerateMipmap(GL_TEXTURE_2D);

	FreeImage_Unload(t32BitsImage);
	FreeImage_Unload(tImage);
}

void loadCubeMap(char** textureFiles, size_t k)
{
	FIBITMAP *tImage[6], *t32BitsImage[6];
	int iWidth[6], iHeight[6];
	for (size_t i=0; i<6; i++) {
		tImage[i] = FreeImage_Load(FreeImage_GetFileType(textureFile, 0), textureFile);
		t32BitsImage[i] = FreeImage_ConvertTo32Bits(tImage);
		iWidth[i] = FreeImage_GetWidth(t32BitsImage);
		iHeight[i] = FreeImage_GetHeight(t32BitsImage);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, texObject[k]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, iWidth[0], iHeight[0], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[0]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, iWidth[1], iHeight[1], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[1]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, iWidth[2], iHeight[2], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[2]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, iWidth[3], iHeight[3], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[3]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, iWidth[4], iHeight[4], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[4]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, iWidth[5], iHeight[5], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[5]));
	glTexEnv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_2D);

	for (size_t i=0; i<6; i++) {
		FreeImage_Unload(t32BitsImage);
		FreeImage_Unload(tImage);
	}
}

void display()
{
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      // for use in cleaning color buffers
	glClearDepth(1.0f);                        // Depth Buffer (it's the buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// this makes the scene black and clears the z buffer

	viewing();
	// note that light should be set after gluLookAt
	lighting();

	// draw objects listed in the scene file on the screen
	for (vector<Model>::iterator jt = scene->modelList_.begin(); jt != scene->modelList_.end(); jt++) {
		glPushMatrix();
			// find the selected mesh in the scene file
			Mesh* obj = nullptr;
			for (vector<Mesh>::iterator kt = objects.begin(); kt != objects.end(); kt++)
				if (jt->objFile_ == jt->objFile_) {
					obj = &(*kt);
					break;
				}
			if (obj == nullptr)
				cout << "Don't have " << jt->objFile_ << " in the object files" << endl;

			glTranslated((GLdouble)jt->transfer_[0], (GLdouble)jt->transfer_[1], (GLdouble)jt->transfer_[2]);
			glRotated((GLdouble)jt->angle_, (GLdouble)jt->rotate_[0], (GLdouble)jt->rotate_[1], (GLdouble)jt->rotate_[2]);
			glScaled((GLdouble)jt->scale_[0], (GLdouble)jt->scale_[1], (GLdouble)jt->scale_[2]);

			int lastMaterial = -1;
			for (size_t i = 0; i < obj->fTotal_; ++i) {
				// set material property if this face used different material
				if (lastMaterial != obj->faceList_[i].m) {
					lastMaterial = (int)obj->faceList_[i].m;
					glMaterialfv(GL_FRONT, GL_AMBIENT, obj->mList_[lastMaterial].Ka);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, obj->mList_[lastMaterial].Kd);
					glMaterialfv(GL_FRONT, GL_SPECULAR, obj->mList_[lastMaterial].Ks);
					glMaterialfv(GL_FRONT, GL_SHININESS, &obj->mList_[lastMaterial].Ns);

					//you can obtain the texture name by jt->modelList_[lastMaterial].map_Kd
					//load them once in the main function before mainloop
					//bind them in display function here
				}

				glBegin(GL_TRIANGLES);
					for (size_t j = 0; j < 3; ++j) {
						//textex corrd. jt->tList[jt->faceList_[i][j].t].ptr
						glNormal3fv(obj->nList_[obj->faceList_[i][j].n].ptr);
						glVertex3fv(obj->vList_[obj->faceList_[i][j].v].ptr);
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
		if (selectObj >= scene->modelTotal_)
			selectObj = scene->modelTotal_ - 1;
		cout << "selected object: " << selectObj + 1 << endl;
	}
}

void drag(int x, int y)
{
	if (x > preMouseX) {		// right
		scene->modelList_[selectObj].transfer_[0] += movObjUnit;
		cout << "moving object " << selectObj + 1 << " right" << endl;
	}
	else if (x < preMouseX) {	// left
		scene->modelList_[selectObj].transfer_[0] -= movObjUnit;
		cout << "moving object " << selectObj + 1 << " left" << endl;
	}
	if (y < preMouseY) {		// up
		scene->modelList_[selectObj].transfer_[1] += movObjUnit;
		cout << "moving object " << selectObj + 1 << " up" << endl;
	}
	else if (y > preMouseY) {	// down
		scene->modelList_[selectObj].transfer_[1] -= movObjUnit;
		cout << "moving object " << selectObj + 1 << " down" << endl;
	}
	preMouseX = x;
	preMouseY = y;
	glutPostRedisplay();
}
