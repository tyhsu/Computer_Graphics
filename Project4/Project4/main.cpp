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
#include "ShaderLoader.h"
using namespace std;

#define NUM_TEXTURE 20
unsigned int texObject[NUM_TEXTURE];

string objFiles[] = { "Sphere.obj", "Scalp.obj" };
size_t objNum = 2;

vector<Mesh> objects;
View *view;
Light *light;
Scene *scene;

int winWidth, winHeight;
int texTechnique = 0;
double rSide[3], forth[3];
int preMouseX = 250, preMouseY = 250;
double movCamUnit = 5.0f;

GLhandleARB PhongShaderProgram, HairSimuProgram;
int segmentNum = 15;
double segmentLen = 0.5f, gravityY = -0.1f;
float projectMatrix[16];

void loadTexture(const char* textureFile, size_t k);
void loadCubeMap(char textureFiles[6][100], size_t k);
void loadShaders();
void viewing();
void lighting();
void texBeforeRender(Textures tex);
void renderMesh(int index);
void texAfterRender(Textures tex);
void display();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);

int main(int argc, char** argv)
{
	view = new View("Peter.view");
	light = new Light("Peter.light");
	scene = new Scene("Peter.scene");

	for (size_t i = 0; i < objNum; i++) {
		Mesh obj(objFiles[i].c_str());
		objects.push_back(obj);
	}
	cout << endl << "--------------------- finish loading files ---------------------" << endl;

	glutInit(&argc, argv);
	glutInitWindowSize(view->width_, view->height_);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("OpenGL Project 4");

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
	FreeImage_DeInitialise();
	cout << endl << "-------------------- finish loading textures --------------------" << endl;

	GLenum glew_error;
	if ((glew_error = glewInit()) != GLEW_OK)
		return -1;
	loadShaders();
	cout << endl << "-------------------- finish loading shaders --------------------" << endl;

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
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

void loadShaders()
{
	PhongShaderProgram = glCreateProgram();
	if (PhongShaderProgram != 0) {
		ShaderLoad(PhongShaderProgram, "../Project4/PhongShading.vert", GL_VERTEX_SHADER);
		ShaderLoad(PhongShaderProgram, "../Project4/PhongShading.frag", GL_FRAGMENT_SHADER);
	}

	HairSimuProgram = glCreateProgram();
	if (HairSimuProgram != 0) {
		ShaderLoad(HairSimuProgram, "../Project4/HairSimulation.vert", GL_VERTEX_SHADER);
		ShaderLoad(HairSimuProgram, "../Project4/HairSimulation.geom", GL_GEOMETRY_SHADER);
		ShaderLoad(HairSimuProgram, "../Project4/HairSimulation.frag", GL_FRAGMENT_SHADER);
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

void texBeforeRender(Textures tex)
{
	texTechnique = tex.technique_;
	if (texTechnique == 0) {		// no-texture
		//cout << "no-texture" << endl;
	}
	else if (texTechnique == 1) {	// single-texture
		//cout << "single-texture:" << tex.imageList_[0].texID_ << endl;
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texObject[tex.imageList_[0].texID_]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	else if (texTechnique == 2) {	// multi-texture
		//cout << "multi-texture: " << tex.imageList_[0].texID_ << " " << tex.imageList_[1].texID_ << endl;
		for (size_t i = 0; i < 2; i++) {
			GLenum glTexture = GL_TEXTURE0 + i;
			glActiveTexture(glTexture);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texObject[tex.imageList_[i].texID_]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
		}
	}
	else {							// cube-map
		//cout << "cube-map: " << tex.texID_ << endl;
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glEnable(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texObject[tex.texID_]);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
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
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// this makes the scene black and clears the z buffer

	viewing();
	// note that light should be set after gluLookAt
	lighting();

	glGetFloatv(GL_PROJECTION_MATRIX, projectMatrix);

	// render Sphere.obj (head)
	glUseProgram(PhongShaderProgram);
	glUniform1i(glGetUniformLocation(PhongShaderProgram, "colorTexture"), 0);
	texBeforeRender(scene->texList_[0]);
	renderMesh(0);
	texAfterRender(scene->texList_[0]);

	// render Scalp.obj (hair)
	glDepthMask(GL_FALSE);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(HairSimuProgram);
	glUniform1f(glGetUniformLocation(HairSimuProgram, "segmentLen"), segmentLen);
	glUniform1i(glGetUniformLocation(HairSimuProgram, "segmentNum"), segmentNum);
	glUniform3f(glGetUniformLocation(HairSimuProgram, "gravity"), 0.0f, gravityY, 0.0f);
	glUniformMatrix4fv(glGetUniformLocation(HairSimuProgram, "projectMatrix"), 1, false, projectMatrix);
	texBeforeRender(scene->texList_[1]);
	renderMesh(1);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	texAfterRender(scene->texList_[1]);
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
	else if (key == 'd') {	// move right
		printf("keyboard: %c\n", key);
		for (size_t i = 0; i < 3; i++)
			view->eye_[i] += rSide[i];
		glutPostRedisplay();
	}
	else if (key == 'r') {	// increase the length of segment
		printf("keyboard: %c\n", key);
		segmentLen += 0.1f;
		printf("hair length: %.1f\n", segmentLen);
		glutPostRedisplay();
	}
	else if (key == 'f') {	// decrease the length of segment
		printf("keyboard: %c\n", key);
		segmentLen -= 0.1f;
		if (segmentLen < 0.0) segmentLen = 0.0;
		printf("hair length: %.1f\n", segmentLen);
		glutPostRedisplay();
	}
	else if (key == 't') {	// increase the number of segment
		printf("keyboard: %c\n", key);
		segmentNum += 1;
		printf("hair number: %d\n", segmentNum);
		glutPostRedisplay();
	}
	else if (key == 'g') {	// decrease the number of segment
		printf("keyboard: %c\n", key);
		segmentNum -= 1;
		if (segmentNum < 0) segmentNum = 0;
		printf("hair number: %d\n", segmentNum);
		glutPostRedisplay();
	}
	else if (key == 'y') {	// increase the y of gravity vector
		printf("keyboard: %c\n", key);
		gravityY += 0.1f;
		printf("gravity: (0.0, %.1f, 0.0)\n", gravityY);
		glutPostRedisplay();
	}
	else if (key == 'h') {	// decrease the y of gravity vector
		printf("keyboard: %c\n", key);
		gravityY -= 0.1f;
		printf("gravity: (0.0, %.1f, 0.0)\n", gravityY);
		glutPostRedisplay();
	}
}

