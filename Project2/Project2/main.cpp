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

#define NUM_OBJECT 9
#define NUM_TEXTURE 1
unsigned int texObject[NUM_TEXTURE];

vector<Mesh> objects;
View *view;
Light *light;
Scene *scene;

int winWidth, winHeight;
int texTechnique = 0;
double rSide[3], forth[3];
size_t selectObj = 0;
int preMouseX = 250, preMouseY = 250;
double movCamUnit = 5.0, movObjUnit = 25;

void loadTexture(const char* textureFile, size_t k);
void loadCubeMap(char textureFiles[][], size_t k);
void viewing();
void lighting();
void texBeforeRender(Textures tex);
void texAfterRender(Textures tex);
void display();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);
void drag(int x, int y);

int main(int argc, char** argv)
{
	//char objFiles[NUM_OBJECT][100] = { "box.obj", "bunny.obj", "venus.obj" };
	char objFiles[NUM_OBJECT][100] = { "bush.obj", "gem.obj", "groundv2.obj", "hedge.obj", "leaves.obj", "LittleFountain.obj", "skybox.obj", "trunk.obj", "water.obj" };

	for (size_t i=0; i<NUM_OBJECT; i++) {
		Mesh obj(objFiles[i]);
		objects.push_back(obj);
	}
	/*
	view = new View("view.view");
	light = new Light("light.light");
	scene = new Scene("scene.scene");
	*/
	view = new View("park.view");
	light = new Light("park.light");
	scene = new Scene("park.scene");
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
	for (vector<Textures>::iterator it=scene->texList_.begin(); it!=scene->texList_.end(); it++) {
		if (it->technique_ != 3) {	//not use cube-map
			for (vector<TexImage>::iterator jt = it->imageList_.begin(); jt != it->imageList_.end(); jt++) {
				jt->texID_ = texNo;
				loadTexture(jt->imageFile.c_str(), texNo++);
			}
		}
		else {						//use cube-map
			char iList[6][100];
			for (size_t j = 0; j < 6; j++)
				strcpy(iList[j], it->imageList_[j].imageFile.c_str());
			it->texID_ = texNo;
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

void loadTexture(const char* textureFile, size_t k)
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

void loadCubeMap(char textureFiles[][], size_t k)
{
	FIBITMAP *tImage[6], *t32BitsImage[6];
	int iWidth[6], iHeight[6];
	for (size_t i=0; i<6; i++) {
		tImage[i] = FreeImage_Load(FreeImage_GetFileType(textureFiles[i], 0), textureFiles[i]);
		t32BitsImage[i] = FreeImage_ConvertTo32Bits(tImage[i]);
		iWidth[i] = FreeImage_GetWidth(t32BitsImage[i]);
		iHeight[i] = FreeImage_GetHeight(t32BitsImage[i]);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, texObject[k]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, iWidth[0], iHeight[0], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[0]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, iWidth[1], iHeight[1], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[1]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, iWidth[2], iHeight[2], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[2]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, iWidth[3], iHeight[3], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[3]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, iWidth[4], iHeight[4], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[4]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, iWidth[5], iHeight[5], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[5]));
	glTexEnvi(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_2D);

	for (size_t i=0; i<6; i++) {
		FreeImage_Unload(t32BitsImage[i]);
		FreeImage_Unload(tImage[i]);
	}
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

void texBeforeRender(Textures tex)
{
	texTechnique = tex.technique_;
	if (texTechnique == 0) {		// no-texture
	}
	else if (texTechnique == 1) {	// single-texture
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texObject[ tex.imageList_[0].texID_ ]);
	}
	else if (texTechnique == 2) {	// multi-texture
		for (size_t i = 0; i < tex.imageTotal_; i++) {
			size_t id = tex.imageList_[i].texID_;
			GLenum GLtexture = GL_TEXTURE0 + id;
			glActiveTexture(GLtexture);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
		}
	}
	else {							// cube-map
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glEnable(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex.texID_);
	}
}

void texAfterRender(Textures tex)
{
	texTechnique = tex.technique_;
	if (texTechnique == 0) {		// no-texture
	}
	else if (texTechnique == 1) {	// single-texture
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

	}
	else if (texTechnique == 2) {	// multi-texture
		for (size_t i = tex.imageTotal_ - 1; i >= 0; i--) {
			size_t id = tex.imageList_[i].texID_;
			GLenum GLtexture = GL_TEXTURE0 + id;
			glActiveTexture(GLtexture);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	else {							// cube-map

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

	size_t lastTexIndex = -1;
	Textures currTex;
	// draw objects listed in the scene file on the screen
	for (vector<Model>::iterator jt = scene->modelList_.begin(); jt != scene->modelList_.end(); jt++) {
		glPushMatrix();
			if (lastTexIndex != jt->texIndex_) {
				if (lastTexIndex != -1)
					texAfterRender(currTex);
				lastTexIndex = jt->texIndex_;
				currTex = scene->texList_[lastTexIndex];
				texBeforeRender(currTex);
			}

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
					glMaterialfv(GL_FRONT, GL_AMBIENT, obj->matList_[lastMaterial].Ka);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, obj->matList_[lastMaterial].Kd);
					glMaterialfv(GL_FRONT, GL_SPECULAR, obj->matList_[lastMaterial].Ks);
					glMaterialfv(GL_FRONT, GL_SHININESS, &obj->matList_[lastMaterial].Ns);

					//you can obtain the texture name by obj->matList_[lastMaterial].map_Kd
					//load them once in the main function before mainloop
					//bind them in display function here
				}

				glBegin(GL_TRIANGLES);
					for (size_t j = 0; j < 3; ++j) {
						if (texTechnique == 1 || texTechnique == 3)			// single-texture or cube-map
							glTexCoord2fv(obj->tList_[ obj->faceList_[i][j].t ].ptr);
						else if (texTechnique == 2) {	// multi-texture
							for (size_t k = 0; k < currTex.imageTotal_; k++) {
								GLenum GLtexture = GL_TEXTURE0 + currTex.imageList_[k].texID_;
								glMultiTexCoord2fv(GLtexture, obj->tList_[ obj->faceList_[i][j].t ].ptr);
							}
						}
						
						glNormal3fv(obj->nList_[ obj->faceList_[i][j].n ].ptr);
						glVertex3fv(obj->vList_[ obj->faceList_[i][j].v ].ptr);
					}
				glEnd();
			}
		glPopMatrix();
	}
	
	texAfterRender(scene->texList_[lastTexIndex]);
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
