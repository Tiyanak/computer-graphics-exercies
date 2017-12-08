//*************************************************************************************************************
//	Crtanje tijela
//	Tijelo se crta u funkciji myObject
//	
//	Zadatak: Treba ucitati tijelo zapisano u *.obj, sjencati i ukloniti staznje poligone
//	S tastature l - pomicanje ocista po x osi +
//		k - pomicanje ocista po x osi +
//              r - pocetni polozaj
//              esc izlaz iz programa
//*************************************************************************************************************

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include "obj.cpp"
#include <glm\vec4.hpp>
#include <glm\mat4x4.hpp>
#include <glm\mat3x4.hpp>
#include <math.h>
#include <glm\ext.hpp>
#include <glm\gtx\vector_angle.hpp>
#include <Windows.h>
#include <IL/ilut.h>
#include <time.h>
#include <stdlib.h>
#include <GL/wglew.h>

//
//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window;
GLuint width = 1024, height = 720;

typedef struct _Ociste {
	GLdouble	x;
	GLdouble	y;
	GLdouble	z;
} Ociste;

Ociste ociste = { 0.0, 50.0, -200.0 };
Ociste glediste = { 0.0, 0.0, 0.0 };
float velocity = 0.0, gravity = 0.5;
int brojCestica = 500, brojCesticaVatre = 500, brojCesticaDima = 500;
float sizeIzvora = 200, sizeCestice = 1.0f;

//*********************************************************************************
//	Models.
//*********************************************************************************

class Vertex {
	public:
		float x, y, z;
		Vertex(float x, float y, float z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}
		Vertex() {
			this->x = 0.0f; this->y = 0.0f; this->z = 0.0f;
		}
};

class Poly {
	public:
		Vertex *v1, *v2, *v3;
		Poly(Vertex *v1, Vertex *v2, Vertex *v3) {
			this->v1 = v1;
			this->v2 = v2;
			this->v3 = v3;
		}
};

class Izvor {
public:
	Vertex *pozicija, *boja;
	float size;
	Izvor(float size) {
		this->boja = new Vertex(20.0f, 20.0f, 20.0f);
		this->pozicija = new Vertex(0.0f, 20.0f, 0.0f);
		this->size = size;
	}
	Izvor(float x, float y, float z, float r, float g, float b, float sizeIn) {
		this->boja = new Vertex(r, g, b);
		this->pozicija = new Vertex(x, y, z);
		this->size = sizeIn;
	}
};

Izvor izvor = Izvor(0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 255.0f, sizeIzvora);
Izvor izvorVatre = Izvor(0.0f, 0.0f, 0.0f, 255.0f, 102.0f, 0.0f, 3.0f);
Izvor izvorDima = Izvor(0.0f, 30.0f, 0.0f, 255.0f, 102.0f, 0.0f, 3.0f);

class Cestica {
	public:
		Vertex *boja, *pozicija;
		float v, g, life;
		bool alive;
		Cestica() {
			this->boja = new Vertex(255, 255, 255);
			float randPositionX = (float)(rand() % ((int)izvor.size*2)) - izvor.size;
			float randPositionZ = (float)(rand() % ((int)izvor.size * 2)) - izvor.size;
			this->pozicija = new Vertex(randPositionX+izvor.pozicija->x, izvor.pozicija->y, randPositionZ+izvor.pozicija->z);
			this->v = velocity; this->g = gravity; this->alive = true, this->life = 1.0f;
		}
		Cestica(float life, float izvorX, float izvorY, float izvorZ) {
			this->boja = new Vertex(255, 102, 0);
			this->pozicija = new Vertex(izvorX, izvorY, izvorZ);
			this->v = velocity; this->g = gravity; this->alive = true, this->life = life;
		}
};

class FloorGrid {
	public: 
		Vertex *boja, *pozicija;
		float size;
		FloorGrid() {
			this->boja = new Vertex(0.0f, 0.0f, 0.0f);
			this->pozicija = new Vertex(0.0f, 0.0f, 0.0f);
			this->size = 50;
		}
		FloorGrid(float x, float y, float z, float r, float g, float b, float sizeIn) {
			this->boja = new Vertex(r, g, b);
			this->pozicija = new Vertex(x, y, z);
			this->size = sizeIn;
		}
};

FloorGrid floorGrid = FloorGrid(0.0f, 0.0f, 0.0f, 50, 255, 0, sizeIzvora);

//
//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay();
void myReshape(int width, int height);
void myMouse(int button, int state, int x, int y);
void myKeyboard(unsigned char theKey, int mouseX, int mouseY);
void redisplay_all(void);
void myIdle();
void init();

void myObject();
void myParticle(Cestica cestica, int index); void myFireParticle(Cestica c, int index); void mySmokeParticle(Cestica c, int index);
void myParticleSystem(); void myFireSystem(); void mySmokeSystem();
void mySource();
void drawSource(Izvor i);
void drawGround(FloorGrid fg);

GLuint loadTexture(const char *filename);

//
//*********************************************************************************
//	Glavni program.
//*********************************************************************************

GLuint tekstura, fb, depth_rb, teksturaVatre, teksturaDima;
vector<Cestica> cestice, cesticeVatre, cesticeDima;
int currentTime = 0, previousTime = 0, t = 0;

int main(int argc, char** argv) {

	srand(time(NULL));

	init();

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutInit(&argc, argv);
	window = glutCreateWindow("Cestice");

	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);
	glutIdleFunc(myIdle);

	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	tekstura = loadTexture("particles/snow.bmp");
	teksturaVatre = loadTexture("particles/explosion.bmp");
	teksturaDima = loadTexture("particles/smoke.bmp");

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Set The Blending Function For Translucency
	glEnable(GL_BLEND);

	GLuint tex, texFire, texSmoke;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glGenTextures(2, &texFire);
	glBindTexture(GL_TEXTURE_2D, texFire);

	glGenTextures(3, &texSmoke);
	glBindTexture(GL_TEXTURE_2D, texSmoke);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	printf("esc: izlaz iz programa\n");

	glutMainLoop();
	return 0;
}
//
//*********************************************************************************
//	Osvjezavanje prikaza. 
//*********************************************************************************

void myDisplay(void)
{
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		         // boja pozadine - bijela
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	drawGround(floorGrid);
	drawSource(izvor);
	drawSource(izvorVatre);

	myParticleSystem();
	myFireSystem();
	mySmokeSystem();

	glutSwapBuffers();

}

void drawGround(FloorGrid fg) {

	float cx = fg.pozicija->x, cy = fg.pozicija->y, cz = fg.pozicija->z, r = fg.size;
	Vertex topR, topL, bottomR, bottomL;
	Vertex topRdown, topLdown, bottomRdown, bottomLdown;

	topR = Vertex(cx + r, cy, cz + r);
	topL = Vertex(cx - r, cy, cz + r);
	bottomR = Vertex(cx + r, cy, cz - r);
	bottomL = Vertex(cx - r, cy, cz - r);

	topRdown = Vertex(cx + r, cy-3, cz + r);
	topLdown = Vertex(cx - r, cy-3, cz + r);
	bottomRdown = Vertex(cx + r, cy-3, cz - r);
	bottomLdown = Vertex(cx - r, cy-3, cz - r);

	// GROUND
	glColor3ub(fg.boja->x, fg.boja->y, fg.boja->z);
	glBegin(GL_QUADS);
		
		glTexCoord2f(topR.x, topR.z);
		glVertex3f(topR.x, topR.y, topR.z); 
		glTexCoord2f(topL.x, topL.z);
		glVertex3f(topL.x, topL.y, topL.z); 
		glTexCoord2f(bottomL.x, bottomL.z);
		glVertex3f(bottomL.x, bottomL.y, bottomL.z); 
		glTexCoord2f(bottomR.x, bottomR.z);
		glVertex3f(bottomR.x, bottomR.y, bottomR.z); // gore

		glVertex3f(topRdown.x, topRdown.y, topRdown.z); glVertex3f(topLdown.x, topLdown.y, topLdown.z); glVertex3f(bottomLdown.x, bottomLdown.y, bottomLdown.z); glVertex3f(bottomRdown.x, bottomRdown.y, bottomRdown.z); // dole

		glVertex3f(bottomR.x, bottomR.y, bottomR.z); glVertex3f(bottomL.x, bottomL.y, bottomL.z); glVertex3f(bottomLdown.x, bottomLdown.y, bottomLdown.z); glVertex3f(bottomRdown.x, bottomRdown.y, bottomRdown.z); // bok prednji
		glVertex3f(topR.x, topR.y, topR.z); glVertex3f(topL.x, topL.y, topL.z); glVertex3f(topLdown.x, topLdown.y, topLdown.z); glVertex3f(topRdown.x, topRdown.y, topRdown.z); // bok zadnji
		glVertex3f(topL.x, topL.y, topL.z); glVertex3f(bottomL.x, bottomL.y, bottomL.z); glVertex3f(bottomLdown.x, bottomLdown.y, bottomLdown.z); glVertex3f(topLdown.x, topLdown.y, topLdown.z); // bok lijevi
		glVertex3f(topR.x, topR.y, topR.z); glVertex3f(bottomR.x, bottomR.y, bottomR.z);  glVertex3f(bottomRdown.x, bottomRdown.y, bottomRdown.z); glVertex3f(topRdown.x, topRdown.y, topRdown.z); // bok desni

	glEnd();

}

void drawSource(Izvor i) {

	float cx = i.pozicija->x, cy = i.pozicija->y, cz = i.pozicija->z, r = i.size;
	Vertex topR, topL, bottomR, bottomL;
	Vertex topRdown, topLdown, bottomRdown, bottomLdown;

	topR = Vertex(cx + r, cy, cz + r);
	topL = Vertex(cx - r, cy, cz + r);
	bottomR = Vertex(cx + r, cy, cz - r);
	bottomL = Vertex(cx - r, cy, cz - r);

	topRdown = Vertex(cx + r, cy + 3, cz + r);
	topLdown = Vertex(cx - r, cy + 3, cz + r);
	bottomRdown = Vertex(cx + r, cy + 3, cz - r);
	bottomLdown = Vertex(cx - r, cy + 3, cz - r);

	// GROUND
	glColor3ub(i.boja->x, i.boja->y, i.boja->z);
	glBegin(GL_QUADS);

		glVertex3f(topR.x, topR.y, topR.z); glVertex3f(topL.x, topL.y, topL.z); glVertex3f(bottomL.x, bottomL.y, bottomL.z); glVertex3f(bottomR.x, bottomR.y, bottomR.z); // gore
		glVertex3f(topRdown.x, topRdown.y, topRdown.z); glVertex3f(topLdown.x, topLdown.y, topLdown.z); glVertex3f(bottomLdown.x, bottomLdown.y, bottomLdown.z); glVertex3f(bottomRdown.x, bottomRdown.y, bottomRdown.z); // dole

		glVertex3f(bottomR.x, bottomR.y, bottomR.z); glVertex3f(bottomL.x, bottomL.y, bottomL.z); glVertex3f(bottomLdown.x, bottomLdown.y, bottomLdown.z); glVertex3f(bottomRdown.x, bottomRdown.y, bottomRdown.z); // bok prednji
		glVertex3f(topR.x, topR.y, topR.z); glVertex3f(topL.x, topL.y, topL.z); glVertex3f(topLdown.x, topLdown.y, topLdown.z); glVertex3f(topRdown.x, topRdown.y, topRdown.z); // bok zadnji
		glVertex3f(topL.x, topL.y, topL.z); glVertex3f(bottomL.x, bottomL.y, bottomL.z); glVertex3f(bottomLdown.x, bottomLdown.y, bottomLdown.z); glVertex3f(topLdown.x, topLdown.y, topLdown.z); // bok lijevi
		glVertex3f(topR.x, topR.y, topR.z); glVertex3f(bottomR.x, bottomR.y, bottomR.z);  glVertex3f(bottomRdown.x, bottomRdown.y, bottomRdown.z); glVertex3f(topRdown.x, topRdown.y, topRdown.z); // bok desni

	glEnd();

}

//*********************************************************************************
//	Promjena velicine prozora.
//	Funkcija gluPerspective i gluLookAt se obavlja 
//		transformacija pogleda i projekcija
//*********************************************************************************

void myReshape(int w, int h)
{
		 printf("MR: width=%d, height=%d\n",w,h);
		width = w; height = h;
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);        // aktivirana matrica projekcije
		glLoadIdentity();
		gluPerspective(45.0, (float)width / height, 5.0f, 100.0f); // kut pogleda, x/y, prednja i straznja ravnina odsjecanja
		
		gluLookAt(ociste.x, ociste.y, ociste.z, glediste.x, glediste.y, glediste.z, 0.0, 1.0, 0.0);	// ociste x,y,z; glediste x,y,z; up vektor x,y,z
		glMatrixMode(GL_MODELVIEW);         // aktivirana matrica modela	
}

void updatePerspective()
{
	glMatrixMode(GL_PROJECTION);        // aktivirana matrica projekcije
	glLoadIdentity();
	gluPerspective(45.0, (float)width / height, 0.0, 1000.0); // kut pogleda, x/y, prednja i straznja ravnina odsjecanja
	gluLookAt(ociste.x, ociste.y, ociste.z, glediste.x, glediste.y, glediste.z, 0.0, 1.0, 0.0);	// ociste x,y,z; glediste x,y,z; up vektor x,y,z
	glMatrixMode(GL_MODELVIEW);         // aktivirana matrica modela
}

//*********************************************************************************
//	Crta moj objekt. Ovdje treba naciniti prikaz ucitanog objekta.
//*********************************************************************************

void myObject() {

	glBegin(GL_TRIANGLES);

	glColor3ub(255, 0, 0);

	glEnd();

}

void myParticleSystem() {
	for (int i=0; i<brojCestica; i++) {
		if (cestice.size() < brojCestica && i >= cestice.size()) {
			cestice.push_back(Cestica());
		} else {
			myParticle(cestice.at(i), i);
		}
		
	}
}

void myFireSystem() {
	for (int i = 0; i < brojCesticaVatre; i++) {
		myFireParticle(cesticeVatre.at(i), i);
	}
}

void mySmokeSystem() {
	for (int i = 0; i < brojCesticaDima; i++) {
		mySmokeParticle(cesticeDima.at(i), i);
	}
}

void drawSquare(Cestica c) {

	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(2.0f, 2.0f, 0.0);
		glTexCoord2i(0, 1); glVertex3f(-2.0f, 2.0f, 0.0);
		glTexCoord2i(1, 1); glVertex3f(-2.0f, -2.0f, 0.0);
		glTexCoord2i(1, 0); glVertex3f(2.0f, -2.0f, 0.0);
	glEnd();

}

void myFireParticle(Cestica c, int index) {

	int x = c.pozicija->x;
	int y = c.pozicija->y;
	int z = c.pozicija->z;

	// Draw particles
	glPushMatrix();
	glColor3ub((int)c.boja->x, (int)c.boja->y, (int)c.boja->z);

	Vertex s(0.0, 0.0, 1.0);
	Vertex os(0.0, 0.0, 0.0);
	Vertex e(0.0, 0.0, 0.0);

	e.x = -ociste.x;
	e.y = -ociste.y;
	e.z = -ociste.z;

	os.x = s.y*e.z - e.y*s.z;
	os.y = e.x*s.z - s.x*e.z;
	os.z = s.x*e.y - s.y*e.x;

	double apsS = pow(pow((double)s.x, 2.0) + pow((double)s.y, 2.0) + pow((double)s.z, 2.0), 0.5);
	double apsE = pow(pow((double)e.x, 2.0) + pow((double)e.y, 2.0) + pow((double)e.z, 2.0), 0.5);
	double se = s.x*e.x + s.y*e.y + s.z*e.z;
	double kut = acos(se / (apsS*apsE));
	kut *= 180.0f / glm::pi<float>();

	glTranslatef(x, y, z);
	glRotatef(kut, os.x, os.y, os.z);

	glBindTexture(GL_TEXTURE_2D, teksturaVatre);
	glEnable(GL_TEXTURE_2D);

	drawSquare(c);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glPopMatrix();

	int pomakni = rand() % 20;

	if (pomakni < 1) {
		int dx = rand() % 3 - 1;
		int dy = rand() % 2;
		int dz = rand() % 3 - 1;

		c.pozicija->x = x + dx;
		c.pozicija->y = y + dy;
		c.pozicija->z = z + dz;

		float lifethreshold = (float)(c.pozicija->y - izvorVatre.pozicija->y) / 30.0f;
		c.life = 1.0 - lifethreshold;
		c.boja->y = 255.0f - 255.0f * c.life;
		if (index == 0) {
			cout << c.life << " : (" << c.boja->x << ", " << c.boja->y << ", " << c.boja->z << ")\n";
		}
		
	}
	
	if (c.life <= 0.0 && index < brojCesticaVatre) {
		cesticeVatre.at(index) = Cestica(1.0f, izvorVatre.pozicija->x, izvorVatre.pozicija->y, izvorVatre.pozicija->z);
	}
	else if (index >= brojCesticaVatre) {
		cesticeVatre.pop_back();
	}
	else {
		cesticeVatre.at(index) = c;
	}

}

void mySmokeParticle(Cestica c, int index) {

	int x = c.pozicija->x;
	int y = c.pozicija->y;
	int z = c.pozicija->z;

	// Draw particles
	glPushMatrix();
	glColor3ub((int)c.boja->x, (int)c.boja->y, (int)c.boja->z);

	Vertex s(0.0, 0.0, 1.0);
	Vertex os(0.0, 0.0, 0.0);
	Vertex e(0.0, 0.0, 0.0);

	e.x = -ociste.x;
	e.y = -ociste.y;
	e.z = -ociste.z;

	os.x = s.y*e.z - e.y*s.z;
	os.y = e.x*s.z - s.x*e.z;
	os.z = s.x*e.y - s.y*e.x;

	double apsS = pow(pow((double)s.x, 2.0) + pow((double)s.y, 2.0) + pow((double)s.z, 2.0), 0.5);
	double apsE = pow(pow((double)e.x, 2.0) + pow((double)e.y, 2.0) + pow((double)e.z, 2.0), 0.5);
	double se = s.x*e.x + s.y*e.y + s.z*e.z;
	double kut = acos(se / (apsS*apsE));
	kut *= 180.0f / glm::pi<float>();

	glTranslatef(x, y, z);
	glRotatef(kut, os.x, os.y, os.z);

	glBindTexture(GL_TEXTURE_2D, teksturaDima);
	glEnable(GL_TEXTURE_2D);

	drawSquare(c);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glPopMatrix();

	int pomakni = rand() % 40;

	if (pomakni < 1) {
		int dx = rand() % 5 - 3;
		int dy = rand() % 2;
		int dz = rand() % 5 - 3;

		c.pozicija->x = x + dx;
		c.pozicija->y = y + dy;
		c.pozicija->z = z + dz;

		float lifethreshold = (float)(c.pozicija->y - izvorDima.pozicija->y) / 30.0f;
		c.life = 1.0 - lifethreshold;
		c.boja->y = 255.0f - 255.0f * c.life;

	}

	if (c.life <= 0.0 && index < brojCesticaDima) {
		cesticeDima.at(index) = Cestica(1.0f, izvorDima.pozicija->x, izvorDima.pozicija->y, izvorDima.pozicija->z);
	}
	else if (index >= brojCesticaDima) {
		cesticeDima.pop_back();
	}
	else {
		cesticeDima.at(index) = c;
	}

}

void myParticle(Cestica c, int index) {

	int x = c.pozicija->x;
	int y = c.pozicija->y;
	int z = c.pozicija->z;

	// Draw particles
	glPushMatrix();
	glColor3ub((int)c.boja->x, (int)c.boja->y, (int)c.boja->z);

	Vertex s(0.0, 0.0, 1.0);
	Vertex os(0.0, 0.0, 0.0);
	Vertex e(0.0, 0.0, 0.0);

	e.x = - ociste.x;
	e.y = - ociste.y;
	e.z = - ociste.z;

	os.x = s.y*e.z - e.y*s.z;
	os.y = e.x*s.z - s.x*e.z;
	os.z = s.x*e.y - s.y*e.x;

	double apsS = pow(pow((double)s.x, 2.0) + pow((double)s.y, 2.0) + pow((double)s.z, 2.0), 0.5);
	double apsE = pow(pow((double)e.x, 2.0) + pow((double)e.y, 2.0) + pow((double)e.z, 2.0), 0.5);
	double se = s.x*e.x + s.y*e.y + s.z*e.z;
	double kut = acos(se / (apsS*apsE));
	kut *= 180.0f / glm::pi<float>();

	glTranslatef(x, y, z);
	glRotatef(kut, os.x, os.y, os.z);
	
	glBindTexture(GL_TEXTURE_2D, tekstura);
	glEnable(GL_TEXTURE_2D);

	/*float tx1 = atan2(x, z) / (2. * glm::pi<float>()) + 0.5;
	float ty1 = asin(z) / glm::pi<float>() + .5;
	glTexCoord2f(tx1, ty1);*/

	/*glutSolidSphere(sizeCestice, 20, 20);*/
	drawSquare(c);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	
	glPopMatrix();

	c.pozicija->y -= c.v / (rand() % 10000);
	c.v += c.g;

	if (c.pozicija->y < 0 && index < brojCestica) {
		cestice.at(index) = Cestica();
	}
	else if (index >= brojCestica){
		cestice.pop_back();
	}
	else {
		cestice.at(index) = c;
	}

}

//
//*********************************************************************************
//	Mis.
//*********************************************************************************

void myMouse(int button, int state, int x, int y)
{
	//Desna tipka - brise canvas.
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		ociste.x = 0;
		updatePerspective();
		glutPostRedisplay();
	}

}

//*********************************************************************************
//	Tastatura tipke - esc - izlazi iz programa.
//*********************************************************************************

void myKeyboard(unsigned char key, int mouseX, int mouseY) {

	if (key == 'd') { ociste.x += 2.0f; printf("ociste x+ : %f\n", ociste.x); }
	if (key == 'a') { ociste.x -= 2.0f; printf("ociste x- : %f\n", ociste.x); }
	if (key == 'e') { ociste.y += 2.0f; printf("ociste y+ : %f\n", ociste.y); }
	if (key == 'q') { ociste.y -= 2.0f; printf("ociste y- : %f\n", ociste.y); }
	if (key == 's') { ociste.z -= 2.0f; printf("ociste z- : %f\n", ociste.z); }
	if (key == 'w') { ociste.z += 2.0f; printf("ociste z+ : %f\n", ociste.z); }

	if (key == '0') { gravity += 0.1f; printf("gravity : %f\n", gravity); }
	if (key == '9') { gravity -= 0.1f; printf("gravity : %f\n", gravity); }

	if (key == '4') { izvor.pozicija->x += 2.0f; printf("izvor x+ : %f\n", izvor.pozicija->x); }
	if (key == '6') { izvor.pozicija->x -= 2.0f; printf("izvor x- : %f\n", izvor.pozicija->x); }
	if (key == '3') { izvor.pozicija->y += 2.0f; printf("izvor y+ : %f\n", izvor.pozicija->y); }
	if (key == '1') { izvor.pozicija->y -= 2.0f; printf("izvor y- : %f\n", izvor.pozicija->y); }
	if (key == '2') { izvor.pozicija->z -= 2.0f; printf("izvor z- : %f\n", izvor.pozicija->z); }
	if (key == '8') { izvor.pozicija->z += 2.0f; printf("izvor z+ : %f\n", izvor.pozicija->z); }

	if (key == 'm') { brojCestica += 10; printf("broj cestica : %d\n", brojCestica); }
	if (key == 'n' && brojCestica > 0) { brojCestica -= 10; printf("broj cestica : %d\n", brojCestica); }

	if (key == '+') { sizeCestice += 0.1f; printf("Velicina cestice : %f\n", sizeCestice); }
	if (key == '-' && sizeCestice > 0.1) { sizeCestice -= 0.1f; printf("Velicina cestice : %f\n", sizeCestice); }

	updatePerspective();
	glutPostRedisplay();

}

void myIdle() {
	glutPostRedisplay();
}

GLuint loadTexture(const char * filename)
{
	ILuint imageID;				// Create an image ID as a ULuint
	GLuint textureID;			// Create a texture ID as a GLuint
	ILboolean success;			// Create a flag to keep track of success/failure
	ILenum error;				// Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); 		// Generate the image ID
	ilBindImage(imageID); 			// Bind the image
	success = ilLoadImage(filename); 	// Load the image file

											// If we managed to load the image, then we can start to do things with it...
	if (success)
	{

		// Convert the image into a suitable format to work with
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if (!success)
		{
			error = ilGetError();
			std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
			exit(-1);
		}

		// Generate a new texture
		glGenTextures(1, &textureID);

		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Set texture interpolation method to use linear interpolation (no MIPMAPS)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Specify the texture specification
		glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
			0,				// Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),	// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
			0,				// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
			GL_UNSIGNED_BYTE,		// Image data type
			ilGetData());			// The actual image data itself
	}
	else // If we failed to open the image file in the first place...
	{
		error = ilGetError();
		std::cout << "Image load failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
		exit(-1);
	}

	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	std::cout << "Texture creation successful." << std::endl;

	return textureID;
}

void init() {

	// Initialize particles
	for (int i = 0; i< brojCestica; i++) {
		cestice.push_back(Cestica());
	}

	for (int i = 0; i < brojCesticaVatre; i++) {
		cesticeVatre.push_back(Cestica(1.0f, izvorVatre.pozicija->x, izvorVatre.pozicija->y, izvorVatre.pozicija->z));
	}

	for (int i = 0; i < brojCesticaDima; i++) {
		cesticeDima.push_back(Cestica(1.0f, izvorDima.pozicija->x, izvorDima.pozicija->y, izvorDima.pozicija->z));
	}

}