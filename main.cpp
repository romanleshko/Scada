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
#define WHITE    1.0, 1.0, 1.0, 1.0
#define BLACK    0.0, 0.0, 0.0, 1.0
#define PI         3.14159


static GLfloat vertices[]={

    1,  0,  1, 
    1,  0, -1,    
   -1,  0, -1,   
   -1,  0,  1,    
   
};

static GLfloat normais[] = {
    0.0,  1.0,  0.0,
    0.0,  1.0,  0.0,
    0.0,  1.0,  0.0,
    0.0,  1.0,  0.0,
};


static GLfloat cor[]={
 
    0.5,  0.5, 0.5,   
    0.5,  0.5, 0.5,    
    0.5,  0.5, 0.5,    
    0.5,  0.5, 0.5,   
};


GLboolean   frenteVisivel=1;

static GLuint   square[] = {0, 1, 2, 3};
static GLuint   STAIR_WIDTH = 2;
static GLuint   STAIR_LENGTH = 1;
static GLuint   STAIR_HEIGHT = 1;
static GLuint   N_STAIRS = 10;

//skybox
GLUquadricObj*  sky  = gluNewQuadric();
GLUquadricObj*  earth_disk  = gluNewQuadric();

GLfloat         sizeSky  = 30;


//------------------------------------------------------------ Sistema Coordenadas + objectos
GLint        wScreen=1400, hScreen=1000;           //.. janela (pixeis)
GLfloat        xC=40.0, yC=100.0, zC=100.0;        //.. Mundo  (unidades mundo)

//------------------------------------------------------------ Observador
GLfloat  rVisao=3.0, aVisao=0.5*PI, incVisao=0.1;
GLfloat  angPersp=109.0;
GLfloat  obsPini[] ={-5, 5, 0};
GLfloat  obsPfin[] ={obsPini[0]-rVisao*cos(aVisao), obsPini[1], obsPini[2]-rVisao*sin(aVisao)};


// LUZ

GLfloat intensidade=0.0;
GLfloat luzGlobalCorAmb[4]={intensidade,intensidade,intensidade,1.0};   // 

GLint   ligaFoco=1;
GLint	luzAmbiente=1;

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



// TEXTURAS
GLuint   texture[10];
RgbImage imag;




//========================================================= ILUMINACAO                 
void initLights(void){

	// FOCO
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzGlobalCorAmb);
	glLightfv(GL_LIGHT1, GL_POSITION,      focoPini );
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION,focoDir );
	glLightf (GL_LIGHT1, GL_SPOT_EXPONENT ,focoExp);
    glLightf (GL_LIGHT1, GL_SPOT_CUTOFF,   focoCut);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,       focoCorDif );   
	glLightfv(GL_LIGHT1, GL_SPECULAR,      focoCorEsp  );

	// Pontual
	GLfloat cor_ilum[4] = {WHITE};
	GLfloat pos_ilum[3] = {0, 20, 0};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, cor_ilum);
	glLightfv(GL_LIGHT0, GL_POSITION, pos_ilum);
	glLightfv(GL_LIGHT2, GL_SPECULAR, cor_ilum);

	
	if (ligaFoco)
		glEnable(GL_LIGHT1);
	else
		glDisable(GL_LIGHT1);

	if (luzAmbiente)
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
	imag.LoadBmpFile("skybox_space.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 
	imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData());

	glGenTextures(1, &texture[1]);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	imag.LoadBmpFile("flat_earth.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);	
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

}
void drawChao(){
	glColor3f(1,1,1);

	glEnable(GL_TEXTURE_2D);
	gluQuadricTexture(earth_disk, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D,texture[1]);

	glPushMatrix();
		glTranslatef(0, -1, 0);
		glRotatef (90, -1, 0, 0);
		gluDisk(earth_disk, 0.0, 5.0 ,100, 100);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}


void inicializa(void)
{   

	glClearColor(0,0,0,0);
	glShadeModel(GL_SMOOTH);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);                                    


	GLfloat  black []={ 0.0 ,0.0 ,0.0, 1};
	GLfloat  white []={ 1 ,1 ,1, 1 };

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);



	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	  

    //glClearColor(BLACK);     

    //glShadeModel(GL_SMOOTH);  
    //glFrontFace(GL_CW);
    //glEnable(GL_CULL_FACE);     
    //glCullFace(GL_BACK);   
   
    
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 0, normais);
    glEnableClientState(GL_NORMAL_ARRAY);
    glColorPointer(3, GL_FLOAT, 0, cor);
    glEnableClientState(GL_COLOR_ARRAY);
	
	initTexturas();
    initLights();

}

void drawSkySphere(){
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D,texture[0]);
	glMaterialfv(GL_FRONT,GL_AMBIENT,  whitePlasticAmb  );
	glMaterialfv(GL_FRONT,GL_DIFFUSE,  whitePlasticDif );
	glMaterialfv(GL_FRONT,GL_SPECULAR, whitePlasticSpec);
	glMaterialf (GL_FRONT,GL_SHININESS,whitePlasticCoef);
	
	glPushMatrix();
		glRotatef (       90, -1, 0, 0);
		gluQuadricDrawStyle ( sky, GLU_FILL   );
		gluQuadricNormals   ( sky, GLU_SMOOTH );
		gluQuadricTexture   ( sky, GL_TRUE    );
		gluSphere ( sky, sizeSky*1, 150, 150);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);			
};



void drawEixos()
{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Eixo X
    glColor4f(RED);
    glBegin(GL_LINES);
    glVertex3i( 0, 0, 0);
    glVertex3i(10, 0, 0);
    glEnd();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Eixo Y
    glColor4f(GREEN);
    glBegin(GL_LINES);
    glVertex3i(0,  0, 0);
    glVertex3i(0, 10, 0);
    glEnd();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Eixo Z
    glColor4f(BLUE);
    glBegin(GL_LINES);
    glVertex3i( 0, 0, 0);
    glVertex3i( 0, 0,10);
    glEnd();
    
}


void drawStairs() {

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture[2]);
    glPushMatrix();
    for (int i = 0; i < N_STAIRS; i++) {
       
        glPushMatrix();
             glScalef(STAIR_LENGTH, STAIR_HEIGHT, STAIR_WIDTH);

            glPushMatrix();
                glTranslatef(STAIR_LENGTH, STAIR_HEIGHT , 0);
                glRotatef(90, 0 , 0, 1);
                glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, square);
            glPopMatrix();
            
            glPushMatrix();
                glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, square);   
            glPopMatrix();

    
            
        glPopMatrix();
        glTranslatef(STAIR_LENGTH * 2, STAIR_HEIGHT * 2, 0);
			
    }
    glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void drawSpring(float inner, float outer, int n_circles) {

    glPushMatrix();
        glTranslatef(2, 2, 0);
        glRotated(90, 1, 0, 0);

        for (int i = 0; i < n_circles; i++) {
            
            switch (i%4)
            {
                case 0:
                    glColor4f(RED);
                    break;
                case 1:
                    glColor4f(GREEN);
                    break;
                case 2:
                    glColor4f(BLUE);
                    break;
                case 3:
                    glColor4f(YELLOW);
                    break;
            }

            glutSolidTorus(inner, outer, 20, 20);
            glTranslatef(0, 0, -0.05);
            
        }
        
    glPopMatrix();
}

void drawScene(){

	initLights();

    drawChao();
    drawStairs();
    drawSpring(0.05, 0.5, 20);
	drawSkySphere();
}

void display(void){
    
    //================================================================= APaga ecran/profundidade
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    
    //================================================================= N„o modificar
    glViewport (0, 0, wScreen, hScreen);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(angPersp, (float)wScreen/hScreen, 0.1, 30*zC);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(obsPini[0], obsPini[1], obsPini[2], obsPfin[0],obsPfin[1],obsPfin[2], 0, 1, 0);
    //================================================================= N„o modificar


    drawEixos();
    drawScene();
    
    printf("Observer at: (%.2f, %.2f, %.2f) looking to (%.2f, %.2f, %.2f)\n", obsPini[0], obsPini[1], obsPini[2], obsPfin[0], obsPfin[1], obsPfin[2]);
    
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
		luzAmbiente = !luzAmbiente;
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
    
    




//======================================================= MAIN
//======================================================= MAIN
int main(int argc, char** argv){
    
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize (wScreen, hScreen);
    glutInitWindowPosition (300, 100);
    glutCreateWindow ("    |FaceVisivel:'f'|      |Observador:'SETAS'|        |Andar-'a/s'|        |Rodar -'e/d'| ");
    
    inicializa();
    
    glutSpecialFunc(teclasNotAscii);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    
    return 0;
}




