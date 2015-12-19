/*Agelos Mourelis, AM:2007, Computer Graphics, Set 2, Winter Semester 2012 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <string.h>

#include <GL/gl.h>
#include <GL/glut.h>

/* Define the dimension of the grid, the size of a single edge, and the window size */
#define N 15
#define VERTEX_SIZE 20
#define initWidth  (double)(N*VERTEX_SIZE+800)
#define initHeight  (double)(N*VERTEX_SIZE+350)
#define LINE_SIZE (double)(2*N*VERTEX_SIZE/initHeight)
#define MENU_ITEM_SIZE 106

#define LINE 400./800.
#define WALL 100./800.
#define W  (double)(800)
#define H  (double)(650)


float yRotation = 0.;
using namespace std;


/* Initialize lighting, blending, and line smoothing */
void init(void) 
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLineWidth(2);
  glClearColor (0.0, 0.0, 0.0, 0.0); 
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_LIGHT0);
  glShadeModel (GL_SMOOTH);
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
  glEnable( GL_LINE_SMOOTH );

}

/*Variables that represent the current rotation and zooming */
float xRotation = 0, zRotation = 0;
float zoom = 1.5;

void printText( char *txt, float r, float g, float b, int fontSize){

    glColor3f( r, g, b );
    
    if( fontSize == 24 ){
      for (int i = 0; i < strlen(txt); i++) 
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, txt[i]);
    }
    else if(fontSize == 10){
      for (int i = 0; i < strlen(txt); i++) 
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, txt[i]);
    }
}

int isFinished = 0;


void display(void)
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  

  
  glLoadIdentity();
  
  /*Firstly we zoom in/out */
  glScalef(zoom,zoom,1);
  
  /*Then we translate our center to the center of the grid */
  glTranslatef(0,0,-3);
  glRotatef(-120,1,0,0);
  
  /* We rotate accordingly */
  glRotatef(zRotation,0,1,0);
  glRotatef(xRotation,1,0,0);
  
  glRotatef(yRotation,0,0,1);
  
  glPushMatrix( );
  glBegin(GL_QUADS);
  glColor3f(88./255.f,145./255.f,17./255.f); glVertex3f(-LINE, -LINE,0.0);
  glColor3f(65./255.,99./255.,23./255.); glVertex3f(LINE, -LINE,0.0);
  glColor3f(45./255.,56./255.,22./255.); glVertex3f(LINE, LINE,0.0);
  glColor3f(88./255.,145./255.,17./255.); glVertex3f(-LINE, LINE,0.0);
  glEnd();
  glPopMatrix( );
  
  glPushMatrix( );
  glTranslatef(LINE/3,0,0);
  glBegin(GL_POLYGON);
  glColor3f(110./255.,68./255.,24./255.);
  gluCylinder(gluNewQuadric(), 0.1, 0.1, 0.2,100 , 100);
  glEnd();
  
  glTranslatef(0,0,0.2);
  glBegin(GL_POLYGON);
  glColor3f(10./255.,118./255.,124./255.);
  gluCylinder(gluNewQuadric(), 0.1, 0.1, 0.01,100 , 100);
  glEnd();
  
  glTranslatef(0,0,0.01);
  glBegin(GL_POLYGON);
  glColor3f(166./255.,155./255.,33./255.);
  gluCylinder(gluNewQuadric(), 0.1, 0, 0.1,100 , 100);
  glEnd();
  glPopMatrix( );
  
  glPushMatrix( );
    double eq[] = {0.,0.,WALL,0.};
  
  
  glEnable( GL_CLIP_PLANE1 );
  glClipPlane(GL_CLIP_PLANE1, eq);
  //glTranslatef( 0.,0.,+0.201 );
  glColor3f(.174/255., 232./255., 230./255.);
  glTranslatef(-0.3,0,0);
  glutSolidSphere(0.1,100,100);
  glPopMatrix( );
  
  
  
  glPushMatrix( );
  glDisable( GL_CLIP_PLANE1);
  glColor3f(0.6, 0.1, 0.9);
  glTranslatef(0.3,0.3,WALL/2.);
  glutSolidCube(WALL);
  
  glPopMatrix( );
  

  
  glutSwapBuffers();
}

void reshape (int w, int h)
{
  glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
  glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
}

int xOld, yOld;


void mousebutton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
	  xOld = x;
	  yOld = y;
	 
	}
   
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){

	}
	
}

void motion(int x, int y) 
{

	  zRotation -= (xOld - x)/4.;
	  xRotation -= (yOld -y)/4.;
	  xOld = x;
	  yOld = y;
	  glutPostRedisplay();

}


void update(int value){
  
	yRotation += 0.6;
	glutPostRedisplay();
	glutTimerFunc(30,update,0);
}

void special(int key, int x, int y){

 glutPostRedisplay();
}



int main(int argc, char** argv)
{
  
  srand(time(NULL));
 
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize (initWidth, initHeight); 
  glutInitWindowPosition (100, 100);
  glutCreateWindow ("Super Cool Cubes");
  init ();
  glutMouseFunc( mousebutton);
  glutMotionFunc( motion);
  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutTimerFunc(35,update,0);
  glutMainLoop();
  return 0;
}
