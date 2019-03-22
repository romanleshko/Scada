#include <stdlib.h>
#include <stdio.h>
#include <math.h>
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
    -1,  0,  1,    
    -1,  0, -1,    
    1,  0, -1,    
    1,  0,  1,    
};

static GLfloat normais[] = {
    0.0,  1.0,  0.0,
    0.0,  1.0,  0.0,
    0.0,  1.0,  0.0,
    0.0,  1.0,  0.0,
};


static GLfloat cor[]={
 
    0.0,  0.0, 1.0,   
    0.0,  0.0, 1.0,    
    1.0,  0.0, 1.0,    
    1.0,  0.0, 1.0,   
};

//=========================================================== FACES DA MESA
GLboolean   frenteVisivel=1;

static GLuint   square[] = {0, 1, 2, 3};
static GLuint   STAIR_WIDTH = 2.5;
static GLuint   STAIR_LENGTH = 1.7;
static GLuint   STAIR_HEIGHT = 1;
static GLuint   N_STAIRS = 10;




//------------------------------------------------------------ Sistema Coordenadas + objectos
GLint        wScreen=800, hScreen=500;        //.. janela (pixeis)
GLfloat        xC=10.0, yC=10.0, zC=10.0;        //.. Mundo  (unidades mundo)

//------------------------------------------------------------ Observador
GLfloat  rVisao=5, aVisao=PI, incVisao=0.05;
GLfloat  obsP[] ={-1, 3.0, -1};
GLfloat  obsPfin[] ={obsP[0]-rVisao*cos(aVisao), obsP[1], obsP[2]-rVisao*sin(aVisao)};
GLfloat  angZoom=90;
GLfloat  incZoom=3;





//================================================================================
//=========================================================================== INIT
void inicializa(void)
{
    glClearColor(BLACK);        //ÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖApagar
    glEnable(GL_DEPTH_TEST);    //ÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖProfundidade
    glShadeModel(GL_SMOOTH);    //ÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖInterpolacao de cores
    /*
    glEnable(GL_CULL_FACE);        //ÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖFaces visiveis
    glCullFace(GL_BACK);        //ÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖMostrar so as da frente
    */
    glVertexPointer(3, GL_FLOAT, 0, vertices); //ÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖVertex arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 0, normais);
    glEnableClientState(GL_NORMAL_ARRAY);
    glColorPointer(3, GL_FLOAT, 0, cor);
    glEnableClientState(GL_COLOR_ARRAY);
}



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
    
    glPushMatrix();
    for (int i = 0; i < N_STAIRS; i++) {
       
        glTranslatef(STAIR_LENGTH * 2, STAIR_HEIGHT *2, 0);
        glPushMatrix();
             glScalef(STAIR_LENGTH, STAIR_HEIGHT, STAIR_WIDTH);

            glPushMatrix();
                
                glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, square);   
            glPopMatrix();
    
            glPushMatrix();
                glTranslatef(STAIR_LENGTH, STAIR_HEIGHT , 0);
                glRotatef(90, 0 , 0, 1);
                glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, square); 
            glPopMatrix();
            
        glPopMatrix();

    }
    glPopMatrix();
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
    
    drawStairs();
    drawSpring(0.05, 0.5, 20);
}

void display(void){
    
    //================================================================= APaga ecran/profundidade
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    
    //================================================================= N„o modificar
    glViewport (0, 0, wScreen, hScreen);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(angZoom, (float)wScreen/hScreen, 0.1, 3*zC);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(obsP[0], obsP[1], obsP[2], obsPfin[0],obsPfin[1],obsPfin[2], 0, 1, 0);
    //================================================================= N„o modificar
    
    
    
    
    //ÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖObjectos
    drawEixos();
    drawScene();
    
    printf("Observer at: (%.2f, %.2f, %.2f) looking to (%.2f, %.2f, %.2f)\n", obsP[0], obsP[1], obsP[2], obsPfin[0], obsPfin[1], obsPfin[2]);
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Actualizacao
    glutSwapBuffers();
}

//======================================================= EVENTOS
void keyboard(unsigned char key, int x, int y){
    
    
    switch (key) {
        case 'f':
        case 'F':
            frenteVisivel=!frenteVisivel;
            glutPostRedisplay();
            break;
            
        case 'J':
        case 'j':
            obsPfin[1] -= 0.3;
            glutPostRedisplay();
            break;
            
        case 'K':
        case 'k':
            obsPfin[1] += 0.3;
            glutPostRedisplay();
            break;
            
        case 'e':
        case 'E':
            //??
            glutPostRedisplay();
            break;
            
        case 'd':
        case 'D':
            //??
            glutPostRedisplay();
            break;
            
            
            
            
            
            //--------------------------- Escape
        case 27:
            exit(0);
            break;
    }
    
}

void teclasNotAscii(int key, int x, int y){
    
    if(key == GLUT_KEY_UP) {
		obsP[0]=obsP[0]+incVisao*cos(aVisao); 
		obsP[2]=obsP[2]-incVisao*sin(aVisao); 
	}
	if(key == GLUT_KEY_DOWN) {
		obsP[0]=obsP[0]-incVisao*cos(aVisao); 
		obsP[2]=obsP[2]+incVisao*sin(aVisao); 
	} 
	if(key == GLUT_KEY_LEFT) {
		aVisao = (aVisao + 0.1) ;
	}
	if(key == GLUT_KEY_RIGHT) {
		aVisao = (aVisao - 0.1) ;
	}
    
    obsPfin[0] =obsP[0]+rVisao*cos(aVisao);
	obsPfin[2] =obsP[2]-rVisao*sin(aVisao);
    
    glutPostRedisplay();
    
}


//======================================================= MAIN
//======================================================= MAIN
int main(int argc, char** argv){
    
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize (wScreen, hScreen);
    glutInitWindowPosition (300, 100);
    glutCreateWindow ("{jh,pjmm}@dei.uc.pt|       |FaceVisivel:'f'|      |Observador:'SETAS'|        |Andar-'a/s'|        |Rodar -'e/d'| ");
    
    inicializa();
    
    glutSpecialFunc(teclasNotAscii);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    
    return 0;
}




