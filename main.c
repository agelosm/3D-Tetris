#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <GL/gl.h>
#include <GL/glut.h>

/* Define the dimension of the grid, the size of a single edge, and the window size */
#define N 15
#define VERTEX_SIZE 20
#define W  (double)(800)
#define H  (double)(650)
#define initWidth  (double)(N*VERTEX_SIZE+800)
#define initHeight  (double)(N*VERTEX_SIZE+350)
#define LINE_SIZE (double)(2*N*VERTEX_SIZE/initHeight)
#define MENU_ITEM_SIZE 106
#define LINE 400./800.
#define WALL 100./800.


/*Variables that represent the current rotation and zooming */
float yRotation = 0.;
int rotationDir=1; //Used to change direction of ratation
int spin=1; //Used to start/stop rotation
float xRotation = 0, zRotation = 0;
float zoom = 3;
//functions to handle zoom, spin and rotation

void toggleRotation()
{
    rotationDir*=-1;
}
void toggleSpin()
{
     spin*=-1;
}
void zoomIn(){
 zoom*=1.2;
}
void zoomOut(){
 zoom*=0.8;
}
void doRotate(){
 yRotation += 0.6*rotationDir;
}

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
void createFence(float p, float q)
{
  glPushMatrix( ); //FENCE


          glBegin(GL_QUADS);
          glColor3f(218./255.f,142./255.f,255./255.f); glVertex3f(-p, -q,0.0);
          glColor3f(255./255.,142./255.,207./255.); glVertex3f(p, -q,0.0);
          glColor3f(215./255.,50./255.,73./255.); glVertex3f(p, q,0.0);
          glColor3f(255./255.,203./255.,0./255.); glVertex3f(-p, q,0.0);

      glEnd();
  glPopMatrix( );
}
void createGround()
{
  glPushMatrix( );
      glBegin(GL_QUADS);
          glColor3f(88./255.f,145./255.f,17./255.f); glVertex3f(-LINE, -LINE,0.0);
          glColor3f(65./255.,99./255.,23./255.); glVertex3f(LINE, -LINE,0.0);
          glColor3f(45./255.,56./255.,22./255.); glVertex3f(LINE, LINE,0.0);
          glColor3f(88./255.,145./255.,17./255.); glVertex3f(-LINE, LINE,0.0);
      glEnd();
  glPopMatrix( );

}
void createCylinder()
{
    glPushMatrix( );
      glTranslatef(LINE/3,0,0);
      glBegin(GL_POLYGON);
          glColor3f(110./255.,68./255.,24./255.);
          gluCylinder(gluNewQuadric(), 0.1, 0.1, 0.2,100 , 100);
        glEnd();
    glPopMatrix( );
}
void createCone()
{
    glPushMatrix();
        glTranslatef(LINE/3,0,0.2);
        glBegin(GL_POLYGON);
            glColor3f(10./255.,118./255.,124./255.);
            gluCylinder(gluNewQuadric(), 0.1, 0.1, 0.01,100 , 100);
        glEnd();

    glTranslatef(0,0,0.01);
      glBegin(GL_POLYGON);
          glColor3f(166./255.,155./255.,33./255.);
          gluCylinder(gluNewQuadric(), 0.1, 0, 0.1,100 , 100); //create hut roof
      glEnd();
    glPopMatrix( );
 }
void createSphere()
{
  glPushMatrix();
      glColor3f(.174/255., 232./255., 230./255.);
      glTranslatef(-0.3,0,0);
      glutSolidSphere(0.1,100,100);
    glPopMatrix();
}
void createCube()
{
    glPushMatrix( );
        glColor3f(0.6, 0.1, 0.9);
        glTranslatef(0.3,0.3,WALL/2.);
        glutSolidCube(WALL);
    glPopMatrix( );
}
void createPrism()
{
    glPushMatrix( );
        glColor3f(.109/255.,15./255., 20./255.);
        glTranslatef(0.3,0.3,0.1);
        glRotatef(45,0,1,0);

        glutSolidCube(WALL-0.001);
    glPopMatrix( );
}
void createPerimeter()
{
    glPushMatrix();
        glRotatef(90,1,0,0);
        glPushMatrix();
            glTranslatef(0.0,WALL,LINE);
            createFence(LINE, WALL);
        glPopMatrix();
        glPushMatrix();
            glRotatef(90,0,1,0);
            glTranslatef(0.0,WALL,LINE);
            createFence(LINE, WALL);
        glPopMatrix();
        glPushMatrix();
            glRotatef(90,0,1,0);
            glTranslatef(0.0,WALL,-LINE);
            createFence(LINE, WALL);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(-1./3,WALL,-LINE);
            createFence(LINE/3, WALL);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(1./3,WALL,-LINE);
            createFence(LINE/3, WALL);
        glPopMatrix();
    glPopMatrix();
}
void display(void)
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glLoadIdentity();

  /*Firstly we zoom in/out */
  glScalef(zoom,zoom,1);

  /*Then we translate our center to the center of the grid */
  glTranslatef(0,0,-3);
  glRotatef(-50,1,0,0);

  /* We rotate accordingly */
  //glRotatef(zRotation,0,1,0);
  //glRotatef(xRotation,1,0,0);
  glRotatef(yRotation,0,0,1);

  createGround();
  createPerimeter();
  
  glPushMatrix( );
    
  glEnable( GL_CLIP_PLANE1 );
  
  double eq[] = {0.,0.,1.,-WALL};
  glClipPlane(GL_CLIP_PLANE1, eq);
createPrism();
  glDisable( GL_CLIP_PLANE1 );
  glPopMatrix();
  
  createCylinder();
  createCone();
  createCube();
  createSphere();

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

int xOld, yOld;

/*
void mousebutton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
	  xOld = x;
	  yOld = y;

	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){

	}

}
*/
void motion(int x, int y)
{

	  zRotation -= (xOld - x)/4.;
	  xRotation -= (yOld -y)/4.;
	  xOld = x;
	  yOld = y;
	  glutPostRedisplay();

}


void update(int value){

    if(spin>0)
  // if(1==0)
        doRotate(); //rotationDir will be either + or - to indicate direction. This will be changed on key press (s)
        glutPostRedisplay();
        glutTimerFunc(30,update,0);


}
void keyboard(unsigned char key, int x, int y){

    if((key ==83 || key==115 )&& spin>0){ //S or s Keys to change direction
       toggleRotation();
    }
    if(key ==82 || key==114){ //R or r Keys to change direction
       toggleSpin();
    }
    if(key==27)
        exit(0);
}



void special(int key, int x, int y){

     switch(key)
     {
     case GLUT_KEY_DOWN: //down arrow
        zoomOut();
        break;
     case GLUT_KEY_UP: //up arrow
       zoomIn();
        break;
     }
  glutPostRedisplay();

}

void demo_menu(int id)
{
    switch(id)
    {
    case 1:
        exit(0);
        break;
    case 2:
        toggleRotation();
        break;
    case 3:
        toggleSpin();
        break;
    case 4:
        zoomIn();
        break;
    case 5:
        zoomOut();
        break;
    }
}


int main(int argc, char** argv)
{

  //srand(time(NULL));

  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize (initWidth, initHeight);
  glutInitWindowPosition (100, 100);
  glutCreateWindow ("Super Cool Cubes");
  init ();

  //Create menu
  glutCreateMenu(demo_menu);
    glutAddMenuEntry("Quit",1);
    glutAddMenuEntry("Toggle Rotation",2);
    glutAddMenuEntry("Toggle Spin",3);
    glutAddMenuEntry("Zoom In", 4);
    glutAddMenuEntry("Zoom Out",5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);


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
