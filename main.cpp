#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "materiais.h"
#include "RgbImage.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define BLUE     0.0, 0.0, 1.0, 1.0
#define RED         1.0, 0.0, 0.0, 1.0
#define YELLOW     1.0, 1.0, 0.0, 1.0
#define GREEN    0.0, 1.0, 0.0, 1.0
#define GRAY     0.3, 0.3, 0.3, 1.0

#define WHITE 1.0, 1.0, 1.0, 1.0

#define SPECULAR_COLOR 0, 0, 0, 0
#define DIFFUSE_COLOR 1.0, 1.0, 1.0, 1.0

#define BLACK    0.0, 0.0, 0.0, 1.0
#define PI         3.14159

#define frand()			((float)rand()/RAND_MAX)
#define MAX_PARTICULAS  200

GLUquadricObj*  quad  = gluNewQuadric();

//---------------------------------------- Particle attributes
typedef struct {
	float   size;		// tamanho
	float	life;		// vida
	float	fade;		// fade
	float	r, g, b;    // color
	GLfloat x, y, z;    // posicao
	GLfloat vx, vy, vz; // velocidade 
    GLfloat ax, ay, az; // aceleracao
} Particle;

Particle  particula[MAX_PARTICULAS];
GLint    milisec = 1000; 

// Escada
GLfloat   STAIR_WIDTH = 5;
GLfloat   STAIR_HEIGHT = 0.3;
GLfloat   STAIR_LENGTH = 1;
GLuint    N_STAIRS = 35;

//skybox
GLfloat         sizeSky  = 15;
GLfloat skyAngle = 0.0;
GLfloat skySpeed = 0.25;

// SUN & MOON
GLfloat celestial_size = 1;
GLfloat celestial_rad = 7.5;
GLfloat sun_ang = 90;
GLfloat moon_ang = 270;
GLfloat celestial_inc = -0.25;
GLfloat celestial_height = 7.5;

//------------------------------------------------------------ Sistema Coordenadas + objectos
GLint          wScreen=1400, hScreen=1000;           //.. janela (pixeis)
GLfloat        xC=40.0, yC=100.0, zC=100.0;        //.. Mundo  (unidades mundo)

//------------------------------------------------------------ Pos Observador ------------------------------------------------------
GLfloat  rVisao=3.0, aVisao=0.5*PI, incVisao=0.1;
GLfloat  angPersp=109.0;
GLfloat  obsPini[] ={-5, 5, 0};
GLfloat  obsPfin[] ={obsPini[0]-rVisao*cos(aVisao), obsPini[1], obsPini[2]-rVisao*sin(aVisao)};


// ----------------------------------------------------------- Pos Sol --------------------------------------------------------------

// LUZ SOL
GLfloat  SunrVisao=3.0, SunaVisao=0.5*PI, SunincVisao=0.1;
GLfloat  SunangPersp=109.0;
GLfloat  angleRad = (sun_ang*PI)/180;

// -----------------------------------------------------------  LUZ OBSERVADOR ---------------------------------------------------------
GLfloat intensidade=0.0;
GLfloat luzGlobalCorAmb[4]={intensidade,intensidade,intensidade,1.0};   
GLint   ligaFoco=0;
GLfloat rFoco=1.1, aFoco=aVisao;
GLfloat incH =0.0, incV=0.0;
GLfloat incMaxH =0.5, incMaxV=0.35;   
GLfloat focoPini[]= { obsPini[0], obsPini[1], obsPini[2], 1.0 };
GLfloat focoPfin[]= { obsPini[0]-rFoco*cos(aFoco), obsPini[1], obsPini[2]-rFoco*sin(aFoco), 1.0};
GLfloat focoDir[] = { focoPfin[0]-focoPini[0], 0, focoPfin[2]-focoPini[2]};
GLfloat focoExp   = 10.0;
GLfloat focoCut   = 60.0;
GLfloat focoCorDif[4] ={ 1, 1, 1, 1.0}; 
GLfloat focoCorEsp[4] ={ 1.0, 1.0, 1.0, 1.0}; 

// -----------------------------------------------------------  FOCO SUN ---------------------------------------------------------
GLfloat sunIntensity=0.000;
GLfloat sunluzGlobalCorAmb[4]={sunIntensity,sunIntensity,sunIntensity,1.0};   
GLint   luzSun=1;
GLfloat rSun=50, aSun=aVisao;
GLfloat sunExp   = 10.0;
GLfloat sunCut   = 90.0;
GLfloat sunCorDif[4] ={ 1, 0.77, 0.56, 1.0}; 
GLfloat sunCorEsp[4] ={ 1, 0.77, 0.56, 1.0}; 


// --------------------------------------------------------------------------------------------------------------------------------------



// TEXTURAS
GLuint   texture[10];
RgbImage imag;

// PORTAL
GLfloat angle = 0.0;
GLfloat portalSize = 5;
GLfloat portalHeight = 12;
GLfloat portalSpeed = -0.25;


//========================================================= ILUMINACAO                 
void initLights(void){

	// FOCO - Observador
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzGlobalCorAmb);
	glLightfv(GL_LIGHT1, GL_POSITION,      focoPini );
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION,focoDir );
	glLightf (GL_LIGHT1, GL_SPOT_EXPONENT ,focoExp);
    glLightf (GL_LIGHT1, GL_SPOT_CUTOFF,   focoCut);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,       focoCorDif );
	glLightfv(GL_LIGHT1, GL_SPECULAR,      focoCorEsp  );

	// FOCO - Sol
	angleRad = (sun_ang*PI)/180;

	GLfloat sunPini[] = {celestial_rad*cos(angleRad), 7.5, celestial_rad*sin(angleRad), 1.0f};

	GLfloat sunDir[] = { 0, -1, 0};

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzGlobalCorAmb);
	glLightfv(GL_LIGHT0, GL_POSITION,      sunPini );
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION,sunDir );
	glLightf (GL_LIGHT0, GL_SPOT_EXPONENT ,sunExp);
    glLightf (GL_LIGHT0, GL_SPOT_CUTOFF,   sunCut);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,       sunCorDif );
	glLightfv(GL_LIGHT0, GL_SPECULAR,      sunCorEsp  );
	
	
	if (ligaFoco)
		glEnable(GL_LIGHT1);
	else
		glDisable(GL_LIGHT1);
	
	if (luzSun)
		glEnable(GL_LIGHT0);
	else
		glDisable(GL_LIGHT0);
	
}

void initTexturas(void) {
    
    //----------------------------------------- SKY
	
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	imag.LoadBmpFile("hubble.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 
	imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData());

	glGenTextures(1, &texture[1]);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	imag.LoadBmpFile("flat.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, 
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData());  	

	glGenTextures(1, &texture[2]);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	imag.LoadBmpFile("scada.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData()); 

	glGenTextures(1, &texture[3]);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	imag.LoadBmpFile("moon.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData()); 

	glGenTextures(1, &texture[4]);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	imag.LoadBmpFile("sun.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData()); 

	glGenTextures(1, &texture[5]);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	imag.LoadBmpFile("portal.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData()); 

}

void initParticulas(Particle *particula)
{
 GLfloat v, theta, phi;
 int i;
 GLfloat px, py, pz;
 GLfloat ps;

	px = 0.0;
	py = 0.0;
	pz = 0.0;
	ps = 4.5;



 for(i=0; i<MAX_PARTICULAS; i++)  {

	//---------------------------------  
	v     = 1*frand()+0.02;
    theta = 2.0*frand()*M_PI;   // [0..2pi]
	phi   = frand()*M_PI;		// [0.. pi]
    
    particula[i].size = ps ;		// tamanh de cada particula
    particula[i].x	  = px + 0.1*frand()*px;    // [-200 200]
    particula[i].y	  = py + 0.1*frand()*py;	// [-200 200]
    particula[i].z	  = pz + 0.1*frand()*pz;	// [-200 200]
        
	particula[i].vx = v * cos(theta) * sin(phi);	// esferico
    particula[i].vy = v * cos(phi);
    particula[i].vz = v * sin(theta) * sin(phi);
	particula[i].ax = 0.01f;
    particula[i].ay = -0.01f;
    particula[i].az = 0.015f;

	particula[i].r = 1.0f;
	particula[i].g = 1.0f;	
	particula[i].b = 1.0f;	
	particula[i].life = 1.0f;		                
	particula[i].fade = 0.001f;	// Em 100=1/0.01 iteracoes desaparece
	}
}

void drawChao(){
	glColor4f(1, 1, 1, 0.5f);
	glNormal3f(0, 1, 0);
	glEnable(GL_TEXTURE_2D);
	gluQuadricTexture(quad, GL_TRUE);
	gluQuadricDrawStyle(quad, GLU_FILL);
	glBindTexture(GL_TEXTURE_2D,texture[1]);

	glPushMatrix();
		glTranslatef(-4, -1, 0);
		glRotatef (90, -1, 0, 0);
		gluDisk(quad, 0.0, 13.5 ,100, 100);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}


void inicializa(void)
{   

	glClearColor(0,0,0,0);
	glShadeModel(GL_SMOOTH);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);                                    

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_AMBIENT_AND_DIFFUSE);


	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	
	initTexturas();
    initLights();

}

void drawSkySphere(){
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D,texture[0]);

	glPushMatrix();
		glRotatef (90, -1, 0, 0);
		glRotatef(skyAngle, 0, 0, 1);
		gluSphere ( quad, sizeSky*1, 80, 80);
		skyAngle += skySpeed;
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);			
};

void drawStairs() {
	glEnable(GL_BLEND);
	glColor4f(1,1,1,0.4);
	glTranslatef(0,1,0);
	glPushMatrix();
	int ang = 10; // stairs rotation angle
	for (int i = 0; i < N_STAIRS; i++) {
		
		glPushMatrix();
			glScalef(STAIR_LENGTH, STAIR_HEIGHT, STAIR_WIDTH);
			glutSolidCube(1);
		glPopMatrix();

		glTranslatef(STAIR_LENGTH + 0.1, STAIR_HEIGHT + 0.05, 0);
		glRotatef(ang ,0, 1, 0);
	}
	glPopMatrix();
	glDisable(GL_BLEND);
}

void drawSun(float size) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[4]);

	GLfloat cor_spec[4]  = {0.5, 0.5, 0.5, 1};
	GLfloat cor_emi[4] =  {0.3, 0.3, 0.3, 1};
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cor_spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, cor_spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, cor_emi);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, cor_spec);
	glColor3f(1,1,1);

	GLfloat angleRad = (sun_ang*PI)/180;

	glPushMatrix();

		glTranslatef(celestial_rad*cos(angleRad), celestial_height, celestial_rad*sin(angleRad));
		gluSphere(quad, size, 100, 100);
		sun_ang += celestial_inc;

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void drawMoon(float size) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glColor4f(GRAY);

	GLfloat angleRad = (moon_ang*PI)/180;

	glPushMatrix();

		glTranslatef(celestial_rad*cos(angleRad), celestial_height, celestial_rad*sin(angleRad));
		gluSphere(quad, size, 100, 100);
		moon_ang += celestial_inc;

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
void drawPortal()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glColor4f(WHITE);

	glPushMatrix();
		glTranslatef(0, portalHeight, 0);
		glRotatef(90, 1, 0, 0);
		glRotatef(angle, 0, 0, 1);
		gluDisk(quad, 0, portalSize, 100, 100);
		angle += portalSpeed;

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void showParticulas(Particle *particula, int sistema) {
 int i;


 for (i=0; i<MAX_PARTICULAS; i++)
	{

	glColor4f(1,1,1, particula[i].life);
 	glBegin(GL_QUADS);				        
		glTexCoord2d(0,0); glVertex3f(particula[i].x -particula[i].size, particula[i].y -particula[i].size, particula[i].z);      
		glTexCoord2d(1,0); glVertex3f(particula[i].x +particula[i].size, particula[i].y -particula[i].size, particula[i].z);        
		glTexCoord2d(1,1); glVertex3f(particula[i].x +particula[i].size, particula[i].y +particula[i].size, particula[i].z);            
		glTexCoord2d(0,1); glVertex3f(particula[i].x -particula[i].size, particula[i].y +particula[i].size, particula[i].z);       
	glEnd();	
	particula[i].x += particula[i].vx;
    particula[i].y += particula[i].vy;
    particula[i].z += particula[i].vz;
    particula[i].vx += particula[i].ax;
    particula[i].vy += particula[i].ay;
    particula[i].vz += particula[i].az;
	particula[i].life -= particula[i].fade;	
	}

}

void Timer(int value) 
{
	initParticulas(particula);
	glutPostRedisplay();
	glutTimerFunc(milisec ,Timer, 1); 
}


void drawScene(){

	initLights();
	initParticulas(particula);
	
	drawStairs();
	drawSun(celestial_size);
	drawMoon(celestial_size);
	drawSkySphere();
	drawPortal();

	// REFLECTIONS
	glDisable(GL_DEPTH_TEST);
	glColorMask(0, 0, 0, 0);
	glEnable(GL_STENCIL_TEST);

	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	drawChao();
	showParticulas(particula, 1);
	// desenhar os objectos limitado a zona do stencil buffer
	glEnable(GL_DEPTH_TEST);
	glColorMask(1,1,1,1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_EQUAL, 1 ,1 );
	glPushMatrix();
		glScalef(1,-1,1);
		drawStairs();
		//drawSpring(0.05, 0.5, 20);
		drawSun(celestial_size);
		//drawPortal();
		drawMoon(celestial_size);		
		//drawSkySphere();	
	glPopMatrix();

	

	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawChao();


	glDisable(GL_BLEND);
}

void display(void){
    
    //================================================================= APaga ecran/profundidade
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    
    //================================================================= N„o modificar
    glViewport (0, 0, wScreen, hScreen);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(angPersp, (float)wScreen/hScreen, 0.1, 30*zC);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(obsPini[0], obsPini[1], obsPini[2], obsPfin[0],obsPfin[1],obsPfin[2], 0, 1, 0);
    //================================================================= N„o modificar


    //drawEixos();
    drawScene();
    
    //printf("Observer at: (%.2f, %.2f, %.2f) looking at (%.2f, %.2f, %.2f)\n", obsPini[0], obsPini[1], obsPini[2], obsPfin[0], obsPfin[1], obsPfin[2]);
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Actualizacao
    glutSwapBuffers();
}

void updateVisao(){
	
	obsPfin[0] =obsPini[0]+rVisao*cos(aVisao);
	obsPfin[2] =obsPini[2]-rVisao*sin(aVisao);
	
	focoPini[0]=obsPini[0];
	focoPini[2]=obsPini[2];
	focoPfin[0]=focoPini[0]-rFoco*cos(aFoco+incH);
	focoPfin[2]=focoPini[2]-rFoco*sin(aFoco+incH);
	focoPini[1]=obsPini[1];
	focoPini[2]=obsPini[2];
	focoDir[0] =focoPfin[0]-focoPini[0];
	focoDir[1] =incV;
	focoDir[2] =focoPfin[2]-focoPini[2];
	
	glutPostRedisplay();		
}


void keyboard(unsigned char key, int x, int y){
	switch (key) {


	//--------------------------- Direccao da Lanterna
	//--------------------------- Projeccao	
	case 's':
	case 'S':
		incH=incH-0.05;
		if (incH<-incMaxH ) incH=-incMaxH ; 
		updateVisao();
	break;
	case 'd':
	case 'D':
		incH=incH+0.05;
		if (incH>incMaxH ) incH=incMaxH ; 
		updateVisao();
	break;
	case 'e':
	case 'E':
		incV=incV+0.05;
		if (incV>incMaxV ) incV=incMaxV ; 
		updateVisao();
	break;
	case 'c':
	case 'C':
		incV=incV-0.05;
		if (incV<-incMaxV ) incV=-incMaxV ; 
		updateVisao();
	break;
	
	//--------------------------- Observador
	case 'f':
	case 'F':
		ligaFoco=!ligaFoco;
		glutPostRedisplay();	
	break;

	case 't':
	case 'T':
		luzSun = !luzSun;
		glutPostRedisplay();
		break;

	
	//=============================================
	//--------------------------- Angulo
	case 'z': case 'Z':
		angPersp=angPersp+3;
		if (angPersp>170) angPersp=170;
		glutPostRedisplay();	
	break;
	case 'x': case 'X':
		angPersp=angPersp-3;
		if (angPersp<1) angPersp=1;
		glutPostRedisplay();	
	break;

	//--------------------------- Altura observador
	case 'q': case 'Q':
		obsPini[1]=obsPini[1]+1;
		glutPostRedisplay();	
	break;
	case 'a': case 'A':
		obsPini[1]=obsPini[1]-1;
		glutPostRedisplay();	
	break;
	
		//--------------------------- Escape
	case 27:
		exit(0);
    break;
	
  }
}


    
void teclasNotAscii(int key, int x, int y)
{
    if(key == GLUT_KEY_UP) {
		obsPini[0]=obsPini[0]+incVisao*cos(aVisao); 
		obsPini[2]=obsPini[2]-incVisao*sin(aVisao); 
	}
	if(key == GLUT_KEY_DOWN) {
		obsPini[0]=obsPini[0]-incVisao*cos(aVisao); 
		obsPini[2]=obsPini[2]+incVisao*sin(aVisao); 
	} 
	if(key == GLUT_KEY_LEFT) {
		aVisao = (aVisao + 0.1) ;
		aFoco=aFoco-0.1;		
	}
	if(key == GLUT_KEY_RIGHT) {
		aVisao = (aVisao - 0.1) ;
		aFoco=aFoco+0.1;
	}
	updateVisao();
}
    
void idle(void)
{
	glutPostRedisplay();
}




//======================================================= MAIN
//======================================================= MAIN
int main(int argc, char** argv){
    
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize (wScreen, hScreen);
    glutInitWindowPosition (300, 100);
    glutCreateWindow ("FLAT EARTH");
    
    inicializa();
    
    glutSpecialFunc(teclasNotAscii);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
 	
    glutTimerFunc(milisec , Timer, 1);
    glutMainLoop();
    
    return 0;
}




