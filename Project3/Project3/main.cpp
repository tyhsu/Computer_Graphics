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

#define NUM_TEXTURE 20
unsigned int texObject[NUM_TEXTURE];

string objFiles[] = { "Cornell_box.obj", "Mirror.obj", "ToySit.obj", "ToyStand.obj" };
size_t objNum = 4;
vector<Mesh> objects;
View *view;
Light *light;
Scene *scene;

int winWidth, winHeight;
int texTechnique = 0;
GLfloat transmittance = 0.5f, reflectance = 0.5f;
double cameraRight[3], viewForward[3];
size_t selectObjIndex = 0;
int preMouseX = 250, preMouseY = 250;
double movCamUnit = 5.0, movObjUnit = 2;

void loadTexture(const char* textureFile, size_t k);
void loadCubeMap(char textureFiles[6][100], size_t k);
void viewing();
void moveCamera();
void lighting();
void renderMesh(int index);
void texBeforeRender(Textures* tex);
void texAfterRender(Textures* tex);
void display();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);
void drag(int x, int y);

int main(int argc, char** argv)
{
	view = new View("CornellBox.view");
	light = new Light("CornellBox.light");
	scene = new Scene("CornellBox.scene");

	for (size_t i = 0; i<objNum; i++) {
		Mesh obj(objFiles[i].c_str());
		objects.push_back(obj);
	}
	/* =========== set the ambient value of Mirror =========== */
	{
		//objects[1].matList_[0].Ka[3] = 0.2f;
		objects[1].matList_[0].Kd[3] = 0.2f;
		//objects[1].matList_[0].Ks[3] = 0.2f;
	}
	cout << endl << "--------------------- finish loading files ---------------------" << endl;

	glutInit(&argc, argv);
	glutInitWindowSize(view->width_, view->height_);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ACCUM);
	glutCreateWindow("OpenGL Project 2");

	glewInit();
	FreeImage_Initialise();
	glGenTextures(NUM_TEXTURE, texObject);
	size_t texNo = 0;
	for (vector<Textures>::iterator it = scene->texList_.begin(); it != scene->texList_.end(); it++) {
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
	cout << textureFile << ": " << k << endl;
	FIBITMAP* tImage = FreeImage_Load(FreeImage_GetFileType(textureFile, 0), textureFile);
	FIBITMAP* t32BitsImage = FreeImage_ConvertTo32Bits(tImage);
	int iWidth = FreeImage_GetWidth(t32BitsImage);
	int iHeight = FreeImage_GetHeight(t32BitsImage);

	glBindTexture(GL_TEXTURE_2D, texObject[k]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage));
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1000);

	FreeImage_Unload(t32BitsImage);
	FreeImage_Unload(tImage);
}

void loadCubeMap(char textureFiles[6][100], size_t k)
{
	FIBITMAP *tImage[6], *t32BitsImage[6];
	int iWidth[6], iHeight[6];
	for (size_t i = 0; i<6; i++) {
		tImage[i] = FreeImage_Load(FreeImage_GetFileType(textureFiles[i], 0), textureFiles[i]);
		t32BitsImage[i] = FreeImage_ConvertTo32Bits(tImage[i]);
		iWidth[i] = FreeImage_GetWidth(t32BitsImage[i]);
		iHeight[i] = FreeImage_GetHeight(t32BitsImage[i]);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, texObject[k]);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, iWidth[0], iHeight[0], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[0]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, iWidth[1], iHeight[1], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[1]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, iWidth[2], iHeight[2], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[2]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, iWidth[3], iHeight[3], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[3]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, iWidth[4], iHeight[4], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[4]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, iWidth[5], iHeight[5], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(t32BitsImage[5]));
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexEnvi(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 1000);

	for (size_t i = 0; i<6; i++) {
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

void moveCamera()
{
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(-40 - (GLdouble)view->eye_[0], (GLdouble)view->eye_[1], (GLdouble)view->eye_[2],	// eye
		-40 - (GLdouble)view->vat_[0], (GLdouble)view->vat_[1], (GLdouble)view->vat_[2],		// center
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
	for (vector<Source>::iterator it = light->lList_.begin(); it != light->lList_.end(); it++) {
		glEnable(lightNo);
		glLightfv(lightNo, GL_POSITION, it->pos_);
		glLightfv(lightNo, GL_DIFFUSE, it->diffuse_);
		glLightfv(lightNo, GL_SPECULAR, it->specular_);
		glLightfv(lightNo, GL_AMBIENT, it->ambient_);
		lightNo++;
	}
	glEnable(GL_LIGHT_MODEL_AMBIENT);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light->enAmbient_);
}

void renderMesh(int index)
{
	glPushMatrix();

	Model* model = &scene->searchModel(objFiles[index]);
	glTranslated((GLdouble)model->translate_[0], (GLdouble)model->translate_[1], (GLdouble)model->translate_[2]);
	glRotated((GLdouble)model->angle_, (GLdouble)model->rotate_[0], (GLdouble)model->rotate_[1], (GLdouble)model->rotate_[2]);
	glScaled((GLdouble)model->scale_[0], (GLdouble)model->scale_[1], (GLdouble)model->scale_[2]);

	Mesh* obj = &objects[index];
	// for each face in the mesh object
	int lastMaterial = -1;
	for (size_t i = 0; i < obj->fTotal_; ++i) {
		// set material property if this face used different material
		if (lastMaterial != obj->faceList_[i].m) {
			lastMaterial = (int)obj->faceList_[i].m;
			glMaterialfv(GL_FRONT, GL_AMBIENT, obj->matList_[lastMaterial].Ka);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, obj->matList_[lastMaterial].Kd);
			glMaterialfv(GL_FRONT, GL_SPECULAR, obj->matList_[lastMaterial].Ks);
			glMaterialfv(GL_FRONT, GL_SHININESS, &obj->matList_[lastMaterial].Ns);
			glMaterialfv(GL_FRONT, GL_EMISSION, obj->matList_[lastMaterial].Ke);

			//you can obtain the texture name by obj->matList_[lastMaterial].map_Kd
			//load them once in the main function before mainloop
			//bind them in display function here
		}

		glBegin(GL_TRIANGLES);
		// for each vertex in the face (triangle)
		for (size_t j = 0; j < 3; ++j) {
			if (texTechnique == 1 || texTechnique == 3)			// single-texture or cube-map
				glTexCoord2f(obj->tList_[obj->faceList_[i][j].t].ptr[0], obj->tList_[obj->faceList_[i][j].t].ptr[1]);
			else if (texTechnique == 2) {	// multi-texture
				for (size_t k = 0; k < 2; k++) {
					GLenum glTexture = GL_TEXTURE0 + k;
					glMultiTexCoord2fv(glTexture, obj->tList_[obj->faceList_[i][j].t].ptr);
				}
			}
			glNormal3fv(obj->nList_[obj->faceList_[i][j].n].ptr);
			glVertex3fv(obj->vList_[obj->faceList_[i][j].v].ptr);
		}
		glEnd();
	}
	glPopMatrix();
}

void texBeforeRender(Textures* tex)
{
	texTechnique = tex->technique_;
	if (texTechnique == 0) {		// no-texture
	}
	else if (texTechnique == 1) {	// single-texture
		cout << "single-texture:" << tex->imageList_[0].texID_ << endl;
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texObject[tex->imageList_[0].texID_]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5f);
	}
	else if (texTechnique == 2) {	// multi-texture
		cout << "multi-texture: " << tex->imageList_[0].texID_ << " " << tex->imageList_[1].texID_ << endl;
		for (size_t i = 0; i < 2; i++) {
			GLenum glTexture = GL_TEXTURE0 + i;
			glActiveTexture(glTexture);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texObject[tex->imageList_[i].texID_]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
		}
	}
	else {							// cube-map
		cout << "cube-map: " << tex->texID_ << endl;
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glEnable(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texObject[tex->texID_]);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
}

void texAfterRender(Textures* tex)
{
	texTechnique = tex->technique_;
	if (texTechnique == 0) {		// no-texture
	}
	else if (texTechnique == 1) {	// single-texture
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

	}
	else if (texTechnique == 2) {	// multi-texture
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {							// cube-map

		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}

void display()
{
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      // for use in cleaning color buffers
	glClearDepth(1.0f);                        // Depth Buffer (it's the buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LESS);                    // The Type Of Depth Test To Do
	glEnable(GL_STENCIL_TEST);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	viewing();
	// note that light should be set after gluLookAt
	lighting();


	/* ========== Set the stencil buffer (the window) ========== */
	{
		glStencilFunc(GL_ALWAYS, 1, 0xff);	// in case of an 8 bit stencil buffer
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xff);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		// Mirror
		renderMesh(1);
	}

	/* =========== Render polygons on the stencil mask, the window ========== */
	{
		glClearAccum(0.0f, 0.0f, 0.0f, 0.0f);
		glStencilFunc(GL_EQUAL, 1, 0xff);
		glStencilMask(0x00);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

		// Refraction (the standing teddy bear behind the window)
		glClear(GL_COLOR_BUFFER_BIT);
		glFrontFace(GL_CCW);
		// ToyStand
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glDepthMask(GL_TRUE);
		renderMesh(3);
		// Mirror
		renderMesh(1);
		glAccum(GL_ACCUM, transmittance);
		glDisable(GL_BLEND);

		// Reflection (the sitting teddy bear and the walls reflected from the window)
		glClear(GL_COLOR_BUFFER_BIT);
		//moveCamera();
		//lighting();
		//glScalef(1, 1, -1);
		glFrontFace(GL_CW);
		glPushMatrix();
			glScalef(-1.0f, 1.0f, 1.0f);
			glTranslatef(40.0f, 0.0f, 0.0f);
			// Cornell_box
			renderMesh(0);
			// ToySit
			renderMesh(2);
		glPopMatrix();
		glAccum(GL_ACCUM, reflectance);
		glDisable(GL_STENCIL_TEST);
	}

	/* =========== Combination =========== */
	{
		// return the accumulation buffer
		glClear(GL_COLOR_BUFFER_BIT);	//???
		//viewing();
		//lighting();
		glFrontFace(GL_CCW);
		glAccum(GL_RETURN, 1.0f);

		// draw other scene expect the window's area
		// Cornell_box
		renderMesh(0);
		// ToySit
		renderMesh(2);
	}

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	winWidth = w;
	winHeight = h;
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	viewForward[0] = (view->vat_[0] - view->eye_[0]) / movCamUnit;
	viewForward[1] = (view->vat_[1] - view->eye_[1]) / movCamUnit;
	viewForward[2] = (view->vat_[2] - view->eye_[2]) / movCamUnit;
	cameraRight[0] = (viewForward[1] * view->vup_[2] - viewForward[2] * view->vup_[1]) / movCamUnit;
	cameraRight[1] = (viewForward[2] * view->vup_[0] - viewForward[0] * view->vup_[2]) / movCamUnit;
	cameraRight[2] = (viewForward[0] * view->vup_[1] - viewForward[1] * view->vup_[0]) / movCamUnit;

	if (key == 'w') {	// zoom in
		printf("keyboard: %c - zoom in\n", key);
		for (size_t i = 0; i < 3; i++)
			view->eye_[i] += viewForward[i];
		glutPostRedisplay();
	}
	else if (key == 'a') {	// move left
		printf("keyboard: %c - move left\n", key);
		for (size_t i = 0; i < 3; i++)
			view->eye_[i] -= cameraRight[i];
		glutPostRedisplay();
	}
	else if (key == 's') {	// zoom out
		printf("keyboard: %c - zoom out\n", key);
		for (size_t i = 0; i < 3; i++)
			view->eye_[i] -= viewForward[i];
		glutPostRedisplay();
	}
	else if (key == 'd') {	//move right
		printf("keyboard: %c - move right\n", key);
		for (size_t i = 0; i < 3; i++)
			view->eye_[i] += cameraRight[i];
		glutPostRedisplay();
	}
	else if (key == 'r') {	//increase reflectance
		reflectance += 0.1f;
		if (reflectance > 1.0f) reflectance = 1.0f;
		printf("keyboard: %c + reflectance: %.1f\n", key, reflectance);
		glutPostRedisplay();
	}
	else if (key == 'f') {	//decrease reflectance
		reflectance -= 0.1f;
		if (reflectance < 0.0f) reflectance = 0.0f;
		printf("keyboard: %c - reflectance: %.1f\n", key, reflectance);
		glutPostRedisplay();
	}
	else if (key == 't') {	//increase transmittance
		transmittance += 0.1f;
		if (transmittance > 1.0f) transmittance = 1.0f;
		printf("keyboard: %c + transmittance: %.1f\n", key, transmittance);
		glutPostRedisplay();
	}
	else if (key == 'g') {	//decrease transmittance
		transmittance -= 0.1f;
		if (transmittance < 0.0f) transmittance = 0.0f;
		printf("keyboard: %c - transmittance: %.1f\n", key, transmittance);
		glutPostRedisplay();
	}
	else if (key >= '1' && key <= '9') {	// select n-th object
		selectObjIndex = key - '1';
		if (selectObjIndex >= scene->modelTotal_)
			selectObjIndex = scene->modelTotal_ - 1;
		cout << "selected object: " << selectObjIndex + 1 << endl;
	}
}

void drag(int x, int y)
{
	if (x > preMouseX) {		// right
		scene->modelList_[selectObjIndex].translate_[0] += movObjUnit;
		cout << "moving object " << selectObjIndex + 1 << " right" << endl;
	}
	else if (x < preMouseX) {	// left
		scene->modelList_[selectObjIndex].translate_[0] -= movObjUnit;
		cout << "moving object " << selectObjIndex + 1 << " left" << endl;
	}
	if (y < preMouseY) {		// up
		scene->modelList_[selectObjIndex].translate_[1] += movObjUnit;
		cout << "moving object " << selectObjIndex + 1 << " up" << endl;
	}
	else if (y > preMouseY) {	// down
		scene->modelList_[selectObjIndex].translate_[1] -= movObjUnit;
		cout << "moving object " << selectObjIndex + 1 << " down" << endl;
	}
	preMouseX = x;
	preMouseY = y;
	glutPostRedisplay();
}
