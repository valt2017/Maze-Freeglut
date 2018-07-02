// Glut.cpp : Defines the entry point for the console application.
// Created according tutorials on
// http://www.lighthouse3d.com/

#include "stdafx.h"
#include <GL/glut.h>
#include "math.h"

// angle of rotation for the camera direction
float angle = 0.0f;
// actual vector representing the camera's direction
float lx = 0.5f, lz = -1.0f;
// XZ position of the camera
float x = 0.0f, z = 5.0f;
// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMove = 0;

const char CURSOR = 'O';
const char CIEL = 'C';
const char STENA = '#';
const char VOLNA = ' ';
int iX, iY;
char ** Pole;

void changeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;
	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);
	// Reset Matrix
	glLoadIdentity();
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// Set the correct perspective.
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void computePos(float deltaMove) {
	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
}

void computeDir(float deltaAngle) {
	angle += deltaAngle;
	lx = sin(angle);
	lz = -cos(angle);
}

void renderScene(void) {
	if (deltaMove)
		computePos(deltaMove);
	if (deltaAngle)
		computeDir(deltaAngle);
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Reset transformations
	glLoadIdentity();
	// Set the camera
	gluLookAt(x, 1.0f, z, x + lx, 1.0f, z + lz, 0.0f, 1.0f, 0.0f);
	// Draw ground
	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, -5.0f, -100.0f);
	glVertex3f(-100.0f, -5.0f, 100.0f);
	glVertex3f(100.0f, -5.0f, 100.0f);
	glVertex3f(100.0f, -5.0f, -100.0f);
	glEnd();
	// Draw Walls 
	for (int j = 0; j < iY; j++)
		for (int i = 0; i < iX; i++) {
			glPushMatrix();
			glTranslatef(i*10.0, 0, j*10.0);
			if (Pole[j][i] == STENA) {
				glColor3f(0.0f, 0.0f, 1.0f);
				glTranslatef(0.0f, 0.75f, 0.0f);
				glutWireCube(10.0f);
			}
			if (Pole[j][i] == CIEL) {
				glColor3f(1.0f, 0.0f, 0.0f);
				glTranslatef(0.0f, 0.75f, 0.0f);
				glutSolidSphere(0.75f, 20, 20);
			}
			if (Pole[j][i] == CURSOR) {
				glColor3f(0.0f, 1.0f, 0.0f);
				glTranslatef(0.0f, 0.75f, 0.0f);
				glutSolidSphere(0.75f, 20, 20);
			}
			glPopMatrix();
		}
	glutSwapBuffers();
}

void pressKey(int key, int xx, int yy) {
	switch (key) {
	case GLUT_KEY_LEFT: deltaAngle = -0.002f; break;
	case GLUT_KEY_RIGHT: deltaAngle = 0.002f; break;
	case GLUT_KEY_UP: deltaMove = 0.1f; break;
	case GLUT_KEY_DOWN: deltaMove = -0.1f; break;
	}
}

void releaseKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT: deltaAngle = 0.0f; break;
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN: deltaMove = 0; break;
	}
}

char ** NahrajPole(char * Subor, int * pX, int * pY){
	int i = 0, iRozX = 0, iRozY = 0;
	char ** pIn;
	std::ifstream pFin(Subor);

	if (!pFin){
		std::cout << "Chyba pri otvarani suboru: " << Subor << std::endl;
		exit(EXIT_FAILURE);
	}
	pFin >> iRozX >> iRozY;
	while (pFin.get() != '\n')
		continue;
	*pX = iRozX;
	*pY = iRozY;
	pIn = new char*[iRozY * sizeof(char*)];
	if (pIn == NULL){
		std::cout << "Maticu sa nepodarilo alokovat" << std::endl;
		pFin.close();
		return NULL;
	}
	for (i = 0; i < iRozY; i++)	{
		pIn[i] = new char[(iRozX + 2) * sizeof(char)]; // '\n'+ '\0'
		if (pIn[i] == NULL)	{
			std::cout << "Maticu sa nepodarilo alokovat" << std::endl;
			pFin.close();
			return NULL;
		}
		pFin.getline(pIn[i], iRozX + 2); // '\n'+ '\0'
		pIn[i][iRozX] = '\0';
	}
	pFin.close();
	return pIn;
}

void UvolniPole(char ** pIn, int iRozY){
	int i;
	for (i = 0; i < iRozY; i++)
		delete pIn[i];
	delete pIn;
}

int main(int argc, char **argv) {
	if (argc != 2)	{
		std::cout << "Error: level not found.\n";
		exit(EXIT_FAILURE);
	}
	Pole = NahrajPole(argv[1], &iX, &iY);
	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(320, 320);
	glutCreateWindow("Bludisko");
	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	glutSpecialFunc(pressKey);
	// here are the new entries
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);
	// OpenGL init
	glEnable(GL_DEPTH_TEST);
	// enter GLUT event processing cycle
	glutMainLoop();
	UvolniPole(Pole, iY);
	return EXIT_SUCCESS;
}