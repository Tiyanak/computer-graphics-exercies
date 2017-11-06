////*************************************************************************************************************
////	Crtanje tijela
////	Tijelo se crta u funkciji myObject
////	
////	Zadatak: Treba ucitati tijelo zapisano u *.obj, sjencati i ukloniti staznje poligone
////	S tastature l - pomicanje ocista po x osi +
////		k - pomicanje ocista po x osi +
////              r - pocetni polozaj
////              esc izlaz iz programa
////*************************************************************************************************************
//
//#include <stdio.h>
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <vector>
//#include <GL\glew.h>
//#include <GL\freeglut.h>
//#include "obj.cpp"
//#include <glm\vec4.hpp>
//#include <glm\mat4x4.hpp>
//#include <glm\mat3x4.hpp>
//#include <math.h>
//#include <glm\ext.hpp>
//#include <glm\gtx\vector_angle.hpp>
//#include <Windows.h>
////
////*********************************************************************************
////	Pokazivac na glavni prozor i pocetna velicina.
////*********************************************************************************
//
//GLuint window;
//GLuint width = 1024, height = 720;
//
//typedef struct _Ociste {
//	GLdouble	x;
//	GLdouble	y;
//	GLdouble	z;
//} Ociste;
//
//Ociste ociste = { 0.0, 0.0, -15.0};
//Ociste glediste = { 0.0, 0.0, 0.0 };
//
//int iteration = 0;
//glm::vec3 pocOr = glm::vec3(0.0f, 0.0f, 1.0f);
//
////*********************************************************************************
////	Models.
////*********************************************************************************
//
//class Vertex {
//
//public:
//	float x, y, z;
//	Vertex(float x, float y, float z) {
//		this->x = x;
//		this->y = y;
//		this->z = z;
//	}
//};
//
//Vertex tockaKrivuljePrije = Vertex::Vertex(0.0f, 0.0f, 0.0f);
//
//class Poly {
//
//public:
//	Vertex *v1, *v2, *v3;
//	Poly(Vertex *v1, Vertex *v2, Vertex *v3) {
//		this->v1 = v1;
//		this->v2 = v2;
//		this->v3 = v3;
//	}
//
//};
////
////*********************************************************************************
////	Function Prototypes.
////*********************************************************************************
//
//void myDisplay();
//void myReshape(int width, int height);
//void myMouse(int button, int state, int x, int y);
//void myKeyboard(unsigned char theKey, int mouseX, int mouseY);
//void myObject();
//void redisplay_all(void);
//
////
////*********************************************************************************
////	Glavni program.
////*********************************************************************************
//
//class Obj {
//
//	public:
//		string filePath;
//		Vertex *vertexes;
//		Vertex teziste = Vertex::Vertex(0, 0, 0);;
//		Poly *polyes;
//		unsigned int pSize;
//		unsigned int vSize;
//		Obj(string filePath) {
//			this->filePath = filePath;
//			this->pSize = 0;
//			this->vSize = 0;
//		};
//
//		void readObject() {
//
//			ifstream objectStream(filePath);
//			vector<string> lines;
//			string line;
//
//			while (getline(objectStream, line)) {
//				lines.push_back(line);
//			}
//
//			vSize = 0, pSize = 0;
//			for (unsigned int i = 0; i < lines.size(); i++) {
//				if (lines.at(i).empty()) continue;
//				if (lines.at(i).at(0) == 'v') vSize++;
//				if (lines.at(i).at(0) == 'f') pSize++;
//			}
//
//			vertexes = (Vertex *)malloc(vSize * sizeof(Vertex));
//			polyes = (Poly*)malloc(pSize * sizeof(Poly));
//
//			float xt = 0, yt = 0, zt = 0;
//			int vSizeTemp = 0;
//			for (unsigned int i = 0; i < lines.size(); i++) {
//				if (lines.at(i).empty()) continue;
//				if (lines.at(i).at(0) == 'v') {
//
//					float x, y, z;
//				
//					char *cstr = &lines.at(i)[0u];
//
//					sscanf_s(cstr, "v %f %f %f", &x, &y, &z);
//
//					x *= 5; y *= 5; z *= 5;
//					xt += x; yt += y; zt += z;
//
//					Vertex vertex(x, y, z);
//					vertexes[vSizeTemp] = vertex;
//					vSizeTemp++;
//				}
//			}
//
//			xt /= vSizeTemp; yt /= vSizeTemp; zt /= vSizeTemp;
//			teziste.x = xt; teziste.y = yt; teziste.z = zt;
//
//			int pSizeTemp = 0;
//			for (unsigned int i = 0; i < lines.size(); i++) {
//				if (lines.at(i).empty()) continue;
//				if (lines.at(i).at(0) == 'f') {
//					int v1, v2, v3;
//	
//					char *cstr = &lines.at(i)[0u];
//
//					sscanf_s(cstr, "f %d %d %d", &v1, &v2, &v3);
//
//					Poly poly(&vertexes[v1 - 1], &vertexes[v2 - 1], &vertexes[v3 - 1]);
//					polyes[pSizeTemp] = poly;
//					pSizeTemp++;
//				}
//			}
//
//			objectStream.close();
//
//		};
//};
//
//glm::mat4x4 B = glm::mat4x4(-1.0f, 3.0f, -3.0f, 1.0f, 3.0f, -6.0f, 0.0f, 4.0f, -3.0f, 3.0f, 3.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
//glm::mat4x3 Bcrtica = glm::mat4x3(-1.0f, 2.0f, -1.0f, 3.0f, -4.0f, 0.0f, -3.0f, 2.0f, 1.0f, 1.0f, 0.0f, 0.0f);
//
//class Bspline {
//
//	public:
//		string filepath;
//		Vertex *vBSplajn, *tangente, *osiRotacije, *tockeKrivulje;
//		int bSplajnSize = 0;
//		float time = 1 / 500;
//		float *kutovi;
//		int total = 0;
//
//		Bspline(string filepath) {
//			this->filepath = filepath;
//		}
//
//		void readBspline() {
//
//			ifstream reader(this->filepath);
//			string line;
//			vector<string> lines;
//
//			while (getline(reader, line)) {
//				lines.push_back(line);
//			}
//
//			bSplajnSize = lines.size();
//			vBSplajn = (Vertex*)malloc(bSplajnSize * sizeof(Vertex));
//
//			for (int i = 0; i < bSplajnSize; i++) {
//				float x, y, z;
//			
//				char *cstr = &lines.at(i)[0u];
//				sscanf_s(cstr, "%f %f %f", &x, &y, &z);
//				
//				Vertex v(x, y, z);
//				this->vBSplajn[i] = v;
//			}
//
//		};
//
//		void calculateSplajn(int newTime = 500) {
//
//			this->time = (float )1 / newTime;
//
//			tangente = (Vertex*)malloc((bSplajnSize - 3) * (newTime + 1) * sizeof(Vertex));
//			osiRotacije = (Vertex*)malloc((bSplajnSize - 3) * (newTime + 1) * sizeof(Vertex));
//			tockeKrivulje = (Vertex*)malloc((bSplajnSize - 3) * (newTime + 1) * sizeof(Vertex));
//			kutovi = (float*)malloc((bSplajnSize - 3) * (newTime + 1) * sizeof(float));
//		
//			int tangenteCounter = 0;
//
//			for (int i = 0; i < bSplajnSize - 3; i++) {
//				Vertex v1 = vBSplajn[i];
//				Vertex v2 = vBSplajn[i+1];
//				Vertex v3 = vBSplajn[i+2];
//				Vertex v4 = vBSplajn[i+3];
//
//				glm::vec3 startTan = glm::vec3(0, 0, 1);
//				for (float t = 0.0f; t <= 1.0f; t += time) {
//
//					 //jednadba 1.3 - tangenta
//					glm::vec3 timeVector = glm::vec3((float)(pow(t, 2)), (float)(t), 1.0f);
//					glm::mat3x4 R = glm::mat3x4(v1.x, v2.x, v3.x, v4.x, v1.y, v2.y, v3.y, v4.y, v1.z, v2.z, v3.z, v4.z);
//					glm::vec3 timeVectorScalar = timeVector * (1.0f / 2);
//					glm::vec4 timeVectorScalarB = glm::transpose(Bcrtica) * timeVectorScalar;
//					glm::vec3 tangenta = glm::transpose(R) * timeVectorScalarB;
//					tangente[tangenteCounter] = Vertex::Vertex(tangenta[0], tangenta[1], tangenta[2]);
//
//					 //jednadba 1.2 - tocka krivulje
//					glm::vec4 tranTime = glm::vec4((float)pow(t, 3), (float)pow(t, 2), (float)t, 1.0f);
//					glm::vec4 tranTimeScalar = tranTime * ((float)(1) / 6);
//					glm::vec4 tranTimeScalarB = glm::transpose(B) * tranTimeScalar;
//					glm::vec3 tockaKrivulje = glm::transpose(R) * tranTimeScalarB;
//
//					tockeKrivulje[tangenteCounter] = Vertex::Vertex(
//						tockaKrivulje[0],
//						tockaKrivulje[1],
//						tockaKrivulje[2]);
//
//					//jednadba 1.5 - os rotacije
//					Vertex tanPrije = Vertex::Vertex(0.0f, 0.0f, 1.0f);
//					Vertex tanPoslije = tangente[tangenteCounter];
//					Vertex smjerTanVer = Vertex::Vertex((tanPoslije.x - tanPrije.x), (tanPoslije.y - tanPrije.y), (tanPoslije.z - tanPrije.z));
//					glm::vec3 smjerTan = glm::vec3(smjerTanVer.x, smjerTanVer.y, smjerTanVer.z);
//					glm::vec3 osRotacije = glm::cross(pocOr, smjerTan);
//					osiRotacije[tangenteCounter] = Vertex::Vertex(osRotacije[0], osRotacije[1], osRotacije[2]);
//
//					//jednadba 1.6 - kut rotacije	
//					float brojnik = glm::dot(pocOr, smjerTan);
//					float nazivnik = glm::length(pocOr) * glm::length(smjerTan);
//					float kut = glm::acos(brojnik / nazivnik);
//					kut *= (180.0f / glm::pi<float>());
//					kutovi[tangenteCounter] = kut;
//
//					tangenteCounter++;
//
//				}
//
//			}
//
//			total = tangenteCounter;
//
//		}
//
//
//};
////"C:\\Users\\Igor Farszky\\visual studio 2015\\Projects\\objekti\\porsche.obj"
//Obj object = Obj::Obj(string("objekti/Ncc.obj"));
//Bspline spline = Bspline::Bspline("objekti/spline.obj");
//
//int main(int argc, char** argv)
//{
//
//	object.readObject();
//	spline.readBspline();
//	spline.calculateSplajn(100);
//
//	 //postavljanje dvostrukog spremnika za prikaz (zbog titranja)
//	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
//	glutInitWindowSize(width, height);
//	glutInitWindowPosition(100, 100);
//	glutInit(&argc, argv);
//
//	window = glutCreateWindow("Tijelo");
//	glutReshapeFunc(myReshape);
//	glutDisplayFunc(myDisplay);
//	glutMouseFunc(myMouse);
//	glutKeyboardFunc(myKeyboard);
//	printf("esc: izlaz iz programa\n");
//
//	glutMainLoop();
//	return 0;
//}
////
////*********************************************************************************
////	Osvjezavanje prikaza. 
////*********************************************************************************
//
//void myDisplay(void)
//{
//	glLoadIdentity();
//	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		         // boja pozadine - bijela
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	 //nacrtaj tocke krivulje
//	glBegin(GL_LINE_STRIP);
//	for (int i = 0; i < spline.total; i++) {
//		Vertex b1 = spline.tockeKrivulje[i];
//		glColor3ub(0, 0, 255);  glVertex3f(b1.x, b1.y, b1.z);
//	}
//	glEnd();
//
//	glBegin(GL_LINES);
//	for (int i = 0; i < spline.total - 1; i += 20) {
//		Vertex tanPrije = Vertex::Vertex(0.0f, 0.0f, 1.0f);
//		Vertex tanPoslije = spline.tangente[i];
//		Vertex smjerTanVer = Vertex::Vertex((tanPoslije.x - tanPrije.x), (tanPoslije.y - tanPrije.y), (tanPoslije.z - tanPrije.z));
//		glColor3ub(0, 255, 0);  glVertex3f(spline.tockeKrivulje[i].x, spline.tockeKrivulje[i].y, spline.tockeKrivulje[i].z);
//		glColor3ub(0, 255, 0);  glVertex3f(spline.tockeKrivulje[i].x + smjerTanVer.x, spline.tockeKrivulje[i].y + smjerTanVer.y, spline.tockeKrivulje[i].z + smjerTanVer.z);
//	}
//	glEnd();
//
//	// translatiraj objekt
//	Vertex tockaKrivulje = spline.tockeKrivulje[iteration];
//	glTranslatef(tockaKrivulje.x, tockaKrivulje.y, tockaKrivulje.z);
//
//	//rotiraj objekt
//	glRotatef(spline.kutovi[iteration], spline.osiRotacije[iteration].x, spline.osiRotacije[iteration].y, spline.osiRotacije[iteration].z);
//
//	 //pomakni se nazad u srediste
//	Vertex pomak = Vertex(tockaKrivulje.x - tockaKrivuljePrije.x, tockaKrivulje.y - tockaKrivuljePrije.y, tockaKrivulje.z - tockaKrivuljePrije.z);
//	tockaKrivuljePrije.x = tockaKrivulje.x; tockaKrivuljePrije.y = tockaKrivulje.y; tockaKrivuljePrije.z = tockaKrivulje.z;
//
//	ociste.x += pomak.x; ociste.y += pomak.y; ociste.z += pomak.z;
//	glediste.x = tockaKrivulje.x; glediste.y = tockaKrivulje.y; glediste.z = tockaKrivulje.z;
//
//	 //nacrtaj objekt
//	myObject();
//
//	iteration += 2;
//	if (iteration >= spline.total) {
//		iteration = 0;
//	}
//
//	glutSwapBuffers();
//
//}
//
////*********************************************************************************
////	Promjena velicine prozora.
////	Funkcija gluPerspective i gluLookAt se obavlja 
////		transformacija pogleda i projekcija
////*********************************************************************************
//
//void myReshape(int w, int h)
//{
//	 printf("MR: width=%d, height=%d\n",w,h);
//	width = w; height = h;
//	glViewport(0, 0, width, height);
//	glMatrixMode(GL_PROJECTION);        // aktivirana matrica projekcije
//	glLoadIdentity();
//	gluPerspective(45.0, (float)width / height, 5.0f, 100.0f); // kut pogleda, x/y, prednja i straznja ravnina odsjecanja
//	
//	gluLookAt(ociste.x, ociste.y, ociste.z, glediste.x, glediste.y, glediste.z, 0.0, 1.0, 0.0);	// ociste x,y,z; glediste x,y,z; up vektor x,y,z
//	glMatrixMode(GL_MODELVIEW);         // aktivirana matrica modela
//}
//
//void updatePerspective()
//{
//	glMatrixMode(GL_PROJECTION);        // aktivirana matrica projekcije
//	glLoadIdentity();
//	gluPerspective(45.0, (float)width / height, 5.0, 100.0); // kut pogleda, x/y, prednja i straznja ravnina odsjecanja
//	gluLookAt(ociste.x, ociste.y, ociste.z, glediste.x, glediste.y, glediste.z, 0.0, 1.0, 0.0);	// ociste x,y,z; glediste x,y,z; up vektor x,y,z
//	glMatrixMode(GL_MODELVIEW);         // aktivirana matrica modela
//}
//
////*********************************************************************************
////	Crta moj objekt. Ovdje treba naciniti prikaz ucitanog objekta.
////*********************************************************************************
//
//void myObject() {
//
//	glBegin(GL_TRIANGLES);
//
//		glColor3ub(255, 0, 0);
//
//		for (unsigned int j = 0; j < object.pSize; j++) {
//			Poly poly = object.polyes[j];
//			Vertex v1 = *poly.v1;
//			Vertex v2 = *poly.v2;
//			Vertex v3 = *poly.v3;
//
//			glColor3ub(255, 0, 0);  glVertex3f(v1.x, v1.y, v1.z);
//			glColor3ub(255, 0, 0);  glVertex3f(v2.x, v2.y, v2.z);
//			glColor3ub(255, 0, 0);  glVertex3f(v3.x, v3.y, v3.z);
//
//		}
//
//	glEnd();
//
//}
////
////*********************************************************************************
////	Mis.
////*********************************************************************************
//
//void myMouse(int button, int state, int x, int y)
//{
//		//Desna tipka - brise canvas.
//	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
//	{
//		ociste.x = 0;
//		updatePerspective();
//		glutPostRedisplay();
//	}
//	
//}
//
////*********************************************************************************
////	Tastatura tipke - esc - izlazi iz programa.
////*********************************************************************************
//
//void myKeyboard(unsigned char theKey, int mouseX, int mouseY)
//{
//	switch (theKey)
//	{
//	case 'w': ociste.y = ociste.y - 0.2;
//		break;
//
//	case 's': ociste.y = ociste.y + 0.2;
//		break;
//
//	case 'a': ociste.x = ociste.x - 0.2;
//		break;
//
//	case 'd': ociste.x = ociste.x + 0.2;
//		break;
//
//	case 'e': ociste.z = ociste.z + 0.2;
//		break;
//
//	case 'q': ociste.z = ociste.z - 0.2;
//		break;
//
//	case 'r': ociste.x = 0.0;
//		break;
//
//	case 27:  exit(0);
//		break;
//
//	case 'n': myDisplay();
//		break;
//
//	}
//
//	updatePerspective();
//	glutPostRedisplay();
//}
//
