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

using namespace std;


/* Store the images */
GLubyte tiger[88][88*3];
GLubyte go[240][240];
GLubyte menu1[240][240], menu2[240][240], menu3[240][240], menu4[240][240], menu5[240][240];

/* Current position of the falling cube */
double zMovement = 0, xMovement =0, yMovement = 0, downSpeed = 0.015;

/* Globals to describe the current game condition */
int type;
int isPaused = 0, isSelecting = 0, isMoving = 0, isCounting = 0, inVoid = 0;
int isRotating = 0;
int choosingRotAxis = 0;
int map[N][N][N] = {0};
int accessed[N][N][N] = {0};
int oldType = -1;
int score = 0, sharedEdges = 0, sharedVertices = 0;
int holes, pits;


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
  type = rand()%3+1;

}

/*Variables that represent the current rotation and zooming */
float xRotation = 0, zRotation = 0;
float zoom = 1.5;

void drawCube(int myType ){
  switch(myType){
    case 1:
      glColor3f(161./255.,3./255.,6./255.);
      break;
    case 2:
      glColor3f(21./255.,77./255.,133./255.);
      break;
    case 3:
      glColor3f(23./255.,99./255.,20./255.);
      break;
    case 4:
      if( !isMoving && !choosingRotAxis)
	glColor3f(232./255.,129./255.,26./255.);
      else if( isMoving )
	glColor3f(255./255.,148./255.,218./255.);
      else if( choosingRotAxis )
	glColor3f(145./255.,217./255.,255./255.);
  }
  glutSolidCube  (VERTEX_SIZE*2/initHeight); 
  
  
  
}

void drawFallingCube( ){
  glPushMatrix();  
  glTranslatef( xMovement,zMovement,yMovement);
  drawCube(type);
  glColor4f(0.2,0.2,0.2,0.8);
  glutWireCube(VERTEX_SIZE*2/initHeight);
  glPopMatrix();
}

void accessReset();
void process(int i, int j, int k);


void restoreMapAfterSelection( );

/* Gravity effect for any floating objects */
void putTheObjectDown( ){
  
  int minz = N, count = 0;
  int temp;
  
  for(int i=0; i<N; i++){
    for(int j=0; j<N; j++){
      for( int k=0; k<N; k++){
	if( map[i][j][k] == 4 ){
	  
	  while( k - count > 0 && map[i][j][k-count-1] == 0)
	    count++;
	  if( count < minz )
	    minz = count;
	  break;
	}
	count = 0;
      }
    }
  }
  if( minz > 0){
    for(int i=0; i<N; i++){
      for(int j=0; j<N; j++){
	for( int k=0; k<N; k++){
	  if( map[i][j][k] == 4 ){
	    map[i][j][k-minz] = oldType;
	    map[i][j][k] = 0;
	  }
	}
      }
    }
  }
}


void fillTheBlanks(){

  accessReset();
  for(int i=0; i<N; i++){
    for(int j=0; j<N; j++){
      for( int k=0; k<N; k++){
	if( map[i][j][k] > 0 && !accessed[i][j][k] ){
	  process(i,j,k);
	  putTheObjectDown();
	  restoreMapAfterSelection();
	}
      }
    }
  }
  accessReset();
  
}
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



float xLine = 0., yLine = 0.;

/* In case the user is rotating an object: The line position is handled here */
void showLine( ){
  
    
  glPushMatrix();
  glLineWidth(3);
  glBegin(GL_LINES);
  glColor3f(255./255.,255./255.,0./255.);
  
  
  /* Decide on which of the 3 sides of the grid it currently is */
  if( xLine > -(N+2)*VERTEX_SIZE/initHeight && yLine > -(N+2)*VERTEX_SIZE/initHeight){
    
    glVertex3f((N+1)*VERTEX_SIZE/initHeight + xLine,0,(N+1)*VERTEX_SIZE/initHeight + yLine);
    glColor3f(255./255.,0./255.,217./255.);
    glVertex3f((N+1)*VERTEX_SIZE/initHeight + xLine,2*N*VERTEX_SIZE/initHeight,(N+1)*VERTEX_SIZE/initHeight + yLine);
  }
  else if(  xLine < (N+2)*VERTEX_SIZE/initHeight && yLine > -(N+2)*VERTEX_SIZE/initHeight ){
   
    glVertex3f(0 ,-xLine - (N+3)*VERTEX_SIZE/initHeight, (N+1)*VERTEX_SIZE/initHeight + yLine);
    glColor3f(255./255.,0./255.,217./255.);
    glVertex3f(2*N*VERTEX_SIZE/initHeight ,-xLine - (N+3)*VERTEX_SIZE/initHeight, (N+1)*VERTEX_SIZE/initHeight + yLine);
    
  }
  else if(  yLine < (N+2)*VERTEX_SIZE/initHeight && xLine > -(N+2)*VERTEX_SIZE/initHeight ){
   
    glVertex3f((N+1)*VERTEX_SIZE/initHeight + xLine,-yLine - (N+3)*VERTEX_SIZE/initHeight ,0);
    glColor3f(255./255.,0./255.,217./255.);
    glVertex3f((N+1)*VERTEX_SIZE/initHeight +xLine,-yLine - (N+3)*VERTEX_SIZE/initHeight, 2*N*VERTEX_SIZE/initHeight );
    
  }
  
  glEnd();
  glLineWidth(2);
  glPopMatrix();
  
}



/* Paint landed cubes, and handling the unified cubes as larger objects */
void paintFallenOnes( ){
  
  int noNeighbours[6];

  for(int i=0;i<N;i++){
    for(int j=0; j<N; j++){
      for(int k=0; k<N; k++){
	if( map[i][j][k] > 0 ){
	  glPushMatrix();
	  float currX = (i+0.5)*2*VERTEX_SIZE/initHeight;
	  float currY = (k+0.5)*2*VERTEX_SIZE/initHeight;
	  float currZ = (j+0.5)*2*VERTEX_SIZE/initHeight;
	  glTranslatef(currX ,currY ,currZ);
	  drawCube(map[i][j][k]);
	  
	  glColor4f(0.2,0.2,0.2,0.6);
	  glBegin(GL_LINES);
	  
	  float size = VERTEX_SIZE/initHeight;
	  
	  /*Check which neighbours are there, so we can decide which edges to show*/
	  noNeighbours[0] = ( i == N-1 || map[i+1][j][k] != map[i][j][k] );
	  noNeighbours[1] = ( k == N - 1 || map[i][j][k+1] != map[i][j][k]);
	  noNeighbours[2] = ( k == 0 || map[i][j][k-1] != map[i][j][k] );
	  noNeighbours[3] = ( j == 0 || map[i][j-1][k] != map[i][j][k] );
	  noNeighbours[4] = ( j == N-1 || map[i][j+1][k] != map[i][j][k] );
	  noNeighbours[5] = ( i == 0 || map[i-1][j][k] != map[i][j][k] );
	  
	  if( noNeighbours[0] && noNeighbours[1] ){
	    glVertex3f(size,size,-size);glVertex3f(size,size,size);
	  }
	  if( noNeighbours[0] && noNeighbours[2] ){
	    glVertex3f(size,-size,-size);glVertex3f(size,-size,size);
	  }
	  if( noNeighbours[0] && noNeighbours[3] ){
	    glVertex3f(size,size,-size);glVertex3f(size,-size,-size);
	  }
	  if( noNeighbours[0] && noNeighbours[4] ){
	    glVertex3f(size,-size,size);glVertex3f(size,size,size);
	  }
	  if( noNeighbours[3] && noNeighbours[2] ){
	    glVertex3f(size,-size,-size);glVertex3f(-size,-size,-size);
	  }
	  if( noNeighbours[3] && noNeighbours[1] ){
	    glVertex3f(-size,size,-size);glVertex3f(size,size,-size);
	  }
	  if( noNeighbours[3] && noNeighbours[5] ){
	    glVertex3f(-size,size,-size);glVertex3f(-size,-size,-size);
	  }
	  if( noNeighbours[5] && noNeighbours[2] ){
	    glVertex3f(-size,-size,size);glVertex3f(-size,-size,-size);
	  }
	  if( noNeighbours[5] && noNeighbours[1] ){
	    glVertex3f(-size,size,-size);glVertex3f(-size,size,size);
	  }
	  if( noNeighbours[5] && noNeighbours[4] ){
	    glVertex3f(-size,-size,size);glVertex3f(-size,size,size);
	  }
	  if( noNeighbours[2] && noNeighbours[4] ){
	    glVertex3f(-size,-size,size);glVertex3f(size,-size,size);
	  }
	  if( noNeighbours[1] && noNeighbours[4] ){
	    glVertex3f(size,size,size);glVertex3f(-size,size,size);
	  }
	  
	  glEnd();  
	  glPopMatrix();
	}
	
      }
    }
  }
}

/* In case a cube landed on a different coloured one */
void destroyThem(int x, int y, int z){
 
  switch( type){
    
    case 1:	//red
      
      if( z > 0 && map[x][y][z-1] == 2){
	map[x][y][z-1] = 0;
	map[x][y][z] = 0;
      }

      break;
    case 2:	//blue

      if( z > 0 && map[x][y][z-1] == 1){
	map[x][y][z-1] = 0;
	map[x][y][z] = 0;
      }

      break;
    case 3:	//green

      if( z > 0 && (map[x][y][z-1] == 1 || map[x][y][z-1] == 2) ){
	int count = 2;
	int myType = map[x][y][z-1];
	map[x][y][z-1] = 0;
	while( z-count >=0 && map[x][y][z-count] == myType ){
	  map[x][y][z-count] = 0;
	  count++;
	}
	map[x][y][z] = 0;
      }

      break;

    
    default:
      return;
  }
  
  
  
}

void printMenu( ){
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(2.7,-0.5,-6);
  glRasterPos2f(0,0);
  if( !isPaused )
    glDrawPixels(240,240,GL_LUMINANCE,GL_UNSIGNED_BYTE,menu1);
  else if( isPaused && choosingRotAxis )
    glDrawPixels(240,240,GL_LUMINANCE,GL_UNSIGNED_BYTE,menu3);
  else if( isPaused && isMoving )
    glDrawPixels(240,240,GL_LUMINANCE,GL_UNSIGNED_BYTE,menu4);
  else if( isPaused && isSelecting )
    glDrawPixels(240,240,GL_LUMINANCE,GL_UNSIGNED_BYTE,menu2);
  else
    glDrawPixels(240,240,GL_LUMINANCE,GL_UNSIGNED_BYTE,menu5);

  glPopMatrix();
}

void printLogo( ){
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(3.7,2.2,-6);
  glRasterPos2d(0,0);
  glDrawPixels(88,88,GL_RGB,GL_UNSIGNED_BYTE,tiger);
  glPopMatrix();
}
  
int isFinished = 0;

/* For a new game */
void restoreMap( ){
  
  for(int i=0;i<N;i++)
    for(int k=0; k<N;k++)
      for(int j=0; j<N; j++)
	map[i][j][k] = 0;
}

void printGameOver( ){
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_LIGHTING);
  glTranslatef(0.1,1.2,-6);
  glRasterPos2d(-1,-1);
  glDrawPixels(240,240,GL_LUMINANCE,GL_UNSIGNED_BYTE,go);
  char st[200];
  sprintf(st,"Score: %d", score);
  glLoadIdentity();
  glTranslatef(0.8,0.8,-6);
  glRasterPos2d(-1,-1);
  printText(st,6/255.,11/255.,64/255.,24);
  sprintf(st,"Holes: %d  -  Pits: %d", holes,pits);
  glLoadIdentity();
  glTranslatef(0.5,0.4,-6);
  glRasterPos2d(-1,-1);
  printText(st,6/255.,11/255.,64/255.,24);
  sprintf(st,"Shared edges: %d  -  Shared vertices: %d", sharedEdges, sharedVertices);
  glLoadIdentity();
  glTranslatef(-0.2,0.1,-6);
  glRasterPos2d(-1,-1);
  printText(st,168/255.,15/255.,15/255.,24);
  
  
 
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void countHoles();

void gameEnded(){
  isFinished = 1;
  zMovement = 0;
  countHoles();
  score += 100*pits + 1000*holes -1000*sharedEdges - 1000*sharedVertices;;
  restoreMap();
  accessReset();
  glutPostRedisplay();
}
  
  

void display(void)
{

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  if( isFinished ){
    
    printGameOver();
    glutSwapBuffers();
    return;
  }
  
  glLoadIdentity();
  
  /*Firstly we zoom in/out */
  glScalef(zoom,zoom,1);
  
  /*Then we translate our center to the center of the grid */
  glTranslatef(-0.75 + N*VERTEX_SIZE/initHeight,-0.35 + N*VERTEX_SIZE/initHeight,-3.+ N*VERTEX_SIZE/initHeight);
  
  /* We rotate accordingly */
  glRotatef(zRotation,0,1,0);
  glRotatef(xRotation,1,0,0);
  
  /*Noew we move the center to the point of axis intersection */
  glTranslatef(- N*VERTEX_SIZE/initHeight,- N*VERTEX_SIZE/initHeight,- N*VERTEX_SIZE/initHeight);
  
  /*Figure out on which block of the matrix the cube is */
  int currX = (int)(xMovement*initHeight/(2*VERTEX_SIZE) );
  int currY = (int)(yMovement*initHeight/(2*VERTEX_SIZE));
  int currZ = (int)(zMovement*initHeight/(2*VERTEX_SIZE));
  
  if( choosingRotAxis )
    showLine();
  
  /* The cube has landed */
  if( zMovement < VERTEX_SIZE/initHeight || ( currZ > 0 && map[currX][currY][currZ-1] > 0) ){
    
    if( currZ == N-1)
      gameEnded();
    
    if( zMovement > 0){
      map[ currX ][ currY ][ currZ ]= type;
      destroyThem(currX,currY,currZ);
      
    }
    
    /* Get a random position for the new one to fall */
    zMovement = (2*N - 1)*VERTEX_SIZE/initHeight;
    xMovement = (rand()%(N)*2 + 1)*VERTEX_SIZE/initHeight;
    yMovement = (rand()%(N)*2 + 1)*VERTEX_SIZE/initHeight;
    type = rand()%3 + 1;

  }
  drawFallingCube();
  paintFallenOnes();
  glLineWidth(0.5);
  
  /*Paint the grid */
  for(int i=0; i<N+1; i++){
    
    glPushMatrix();
    glTranslatef( 0, 2*i*VERTEX_SIZE/initHeight,0.);
    glBegin(GL_LINES);
    glColor3f(1.0f,1.0f,1.0f); 
    glVertex3f( 0.0f, 0.0f , 0.0f);
    glVertex3f( LINE_SIZE, 0.0f, 0.0f);
    glVertex3f( 0.0f, 0.0f , 0.0f);
    glVertex3f( 0.0f, 0.0f, LINE_SIZE);
    glEnd();
    glPopMatrix();    
    
    glPushMatrix();
    glTranslatef( 2*i*VERTEX_SIZE/initHeight,0,0);
    glBegin(GL_LINES);
    glVertex3f( 0.0f, 0.0f , 0.0f);
    glVertex3f( 0.0f, LINE_SIZE, 0.0f);
    glVertex3f( 0.0f, 0.0f , 0.0f);
    glVertex3f( 0.0f, 0.0f, LINE_SIZE);
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef( 0 ,0 , 2*i*VERTEX_SIZE/initHeight);
    glBegin(GL_LINES);
    glVertex3f( 0.0f, 0.0f , 0.0f);
    glVertex3f( LINE_SIZE, 0.0f, 0.0f);
    glVertex3f( 0.0f, 0.0f , 0.0f);
    glVertex3f( 0.0f, LINE_SIZE, 0.0f);
    glEnd();
    glPopMatrix();
    
    
  }
  glLineWidth(2);
  printMenu();
  printLogo();
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

/*Globals to be used for the object selection */
int globalI = 0, globalJ = 0, globalK = 0;
int found = 1, firstSearch = 0;

/*Reset the assisting table that decides which of the cells we have allready processed */
void accessReset( ){
  for(int i=0; i<N; i++)
    for(int j=0; j<N; j++)
      for(int k=0; k<N; k++)
	accessed[i][j][k]=0;
}


void globalReset( ){
  globalI = 0;
  globalJ = 0;
  globalK = 0;
}

void globalIncrease( ){
  
  globalK = (globalK+1)%N;
  globalJ = (globalJ + (globalK == 0))%N;
  globalI = (globalI + (globalK==0 && globalJ == 0))%N;
  
  if( !globalK && !globalJ && !globalI){
     accessReset();
    if( firstSearch ){
      found = 0;
    }
  }
  
}


void restoreMapAfterSelection( ){
  if( oldType > - 1){
	  for(int i=0; i<N; i++)
	    for(int j=0; j<N; j++)
	      for(int k=0; k<N; k++){
		if(map[i][j][k] == 4 )
		  map[i][j][k] = oldType;
		else if( map[i][j][k] == 5)
		  map[i][j][k] = 0;
	      }	
	}
}


/*This variables form a cube around the selected object, in order to process it later */
int count = 0;
int minx, maxx, miny, maxy, minz, maxz, isAHole = 1;

void restoreCountingBoundarys( ){
  minx = N-1;
  miny = N-1;
  minz = N-1;
  maxx = 0;
  maxy = 0;
  maxz = 0;
}

int voidZ ;
int smallCubes = 0;

/*This function takes a coloured cell as input, and return the larger object it belongs to */
void process(int i, int j, int k){

  accessed[i][j][k] = 1;
  count++;
  
    oldType = map[i][j][k];
   
  if( isCounting )
    smallCubes++;
    
  
  if( isCounting && !inVoid){
    if( i > maxx )
      maxx = i;
    if( i < minx )
      minx = i;
    if( j > maxy )
      maxy = j;
    if( j < miny )
      miny = j;
    if( k > maxz)
      maxz = k;
    if( k < minz )
      minz = k;
  }
  
  
  
  if( i>0 && map[i-1][j][k] == map[i][j][k] && !accessed[i-1][j][k])
    process(i-1,j,k);  
  if( i<N-1 && map[i+1][j][k] == map[i][j][k] && !accessed[i+1][j][k])
    process(i+1,j,k);
  if( j>0 && map[i][j-1][k] == map[i][j][k] && !accessed[i][j-1][k])
    process(i,j-1,k);
  if( j<N-1 && map[i][j+1][k] == map[i][j][k] && !accessed[i][j+1][k])
    process(i,j+1,k);
  if( k>0 && map[i][j][k-1] == map[i][j][k] && !accessed[i][j][k-1])
    process(i,j,k-1);
  if( k<N-1 && map[i][j][k+1] == map[i][j][k] && !accessed[i][j][k+1])
    process(i,j,k+1);
  
  map[i][j][k] = 4;
   
}

int wrapAround = 0;

/* Select the next object */
void selection( ){
  
  count = 0;
  if( !globalI && !globalJ && !globalK){
    firstSearch = 1;
    found = 1;
  }
  
  while(found && map[globalI][globalJ][globalK] == 0 || accessed[globalI][globalJ][globalK] != 0 )
    globalIncrease(); 
  
  if( !found){
    accessReset();
    globalReset();
      return;
  }
  
  process(globalI,globalJ,globalK);  
  
  if( count > 0 )
    firstSearch = 0;
  
  if( !globalI && !globalJ && !globalK )
    globalIncrease();
  
  glutPostRedisplay();
  
}

/* A list that is used to spot larger holes or pits an object forms */
int flooded[N][N][N];

typedef struct _holeNode{
  int i;
  int j;
  int k;
  struct _holeNode *next;
} holeNode;

holeNode *holeListStart = NULL;

void freeList( ){
  
  holeNode *tmp ;
  
  if( holeListStart == NULL)
    return;
  else if( holeListStart->next == NULL){
    free(holeListStart);
    holeListStart = NULL;
  }
  else{
    tmp = holeListStart->next;
    while(tmp != NULL){
      free(holeListStart);
      holeListStart = tmp;
      tmp = tmp->next;
    }
    holeListStart = NULL;
  }
  
  
  
  
}

void addToList(int i, int j, int k){
  
  holeNode * tmp = holeListStart, tmp2;
  holeNode *newNode =  (holeNode*) malloc( sizeof(holeNode));
  newNode->i = i;
  newNode->j = j;
  newNode->k = k;
  newNode->next = NULL;
  
  if( tmp != NULL){
    while( tmp->next != NULL)
	tmp = tmp->next;

    tmp->next = newNode;
  }
  else
    holeListStart = newNode;
  
}


/*Here we count the pits and the holes */
void countList( ){
  
  int isUnique = 1;
  holeNode * tmp = holeListStart;
  holeNode * tmp2;
  
  while( tmp != NULL){
    tmp2 = tmp->next;
    while( tmp2 != NULL ){
      if( tmp2->k == tmp->k &&  ( (fabs(tmp2->i-tmp->i)==1 && tmp->j == tmp2->j) || ( fabs(tmp2->j-tmp->j)==1 && tmp->i == tmp2->i)  ) ){
	isUnique = 0;
	break;
      }
      tmp2 = tmp2->next;
    }
    if( isUnique && tmp->k==0)
      holes++;
    else if( isUnique && tmp->k > 0)
      pits++;
    
    isUnique = 1;
    tmp = tmp->next;
  }
}

int floodNeighbours(int i, int j, int k){
  
  int result = 1;
  
  if( i==minx || i==maxx || j==miny || j==maxy)
     return 0;
  
   flooded[i][j][k] = 1;
     
   if( i>0 && map[i-1][j][k] == 0 && !flooded[i-1][j][k])
     result *= floodNeighbours(i-1,j,k);
  if( i<N-1 && map[i+1][j][k] == 0 && !flooded[i+1][j][k])
     result *= floodNeighbours(i+1,j,k);
  if( j>0 && map[i][j-1][k] == 0 && !flooded[i][j-1][k])
     result *= floodNeighbours(i,j-1,k);
  if( j<N-1 && map[i][j+1][k] == 0 && !flooded[i][j+1][k])
     result *= floodNeighbours(i,j+1,k);
  if( j<N-1 && i<N-1 && map[i+1][j+1][k] == 0 && !flooded[i+1][j+1][k])
     result *= floodNeighbours(i+1,j+1,k);
  if( j<N-1 && i>0 && map[i-1][j+1][k] == 0 && !flooded[i-1][j+1][k])
     result *= floodNeighbours(i+1,j+1,k);
  if( j >0 && i<N-1 && map[i+1][j-1][k] == 0 && !flooded[i+1][j-1][k])
     result *= floodNeighbours(i+1,j+1,k);
  if( j >0 && i >0 && map[i-1][j-1][k] == 0 && !flooded[i-1][j-1][k])
     result *= floodNeighbours(i+1,j+1,k);
  

  return result;
}

void digTheHole(int i, int j, int k){
  
  int depth = 1, isSurrounded;
 
  while( k-depth>=0 && map[i][j][k-depth] == 0 ){
	
      accessed[i][j][k-depth] = 1;
      depth++;
  }
  depth--;
 
  isSurrounded = floodNeighbours(i,j,k-depth);
  
  if( k-depth > 0 || ( k-depth == 0 && isSurrounded))
    addToList(i,j,k-depth);
}

void floodReset( ){
  for( int i=0; i< N-1; i++){
    for( int j=0; j<N-1; j++){
      for( int k=0; k<N-1; k++){
	flooded[i][j][k] = 0;
      }
    }
  }
}
	
/* Spot vertices of edges that are shared more than they are allowed to */
void checkShared(int i, int j, int k){
  
  
    int isNeighbour[7] = {0};
    isNeighbour[0] = ( k > 0 && map[i][j][k-1] == 4 );
    isNeighbour[1] = ( i > 0 &&  map[i-1][j][k] == 4);
    isNeighbour[2] = ( j > 0 && map[i][j-1][k] == 4 );
    isNeighbour[3] = ( j > 0 && k > 0 && map[i][j-1][k-1] == 4 );
    isNeighbour[4] = ( i > 0 && k > 0 && map[i-1][j][k-1] == 4);
    isNeighbour[5] = ( j > 0 && i > 0 && map[i-1][j-1][k] == 4);
    isNeighbour[6] = ( j > 0 && i > 0 && k > 0 && map[i-1][j-1][k-1] == 4);
    
    if( isNeighbour[0] + isNeighbour[2] + isNeighbour [3] > 1 + (map[i][j][k] != 4) )
      sharedEdges++;
    if( isNeighbour[0] + isNeighbour[4] + isNeighbour[1] > 1 + (map[i][j][k] != 4)  )
      sharedEdges++;
    if( isNeighbour[1] + isNeighbour[2] + isNeighbour[5] > 1 + (map[i][j][k] != 4) )
      sharedEdges++;
    
    if( isNeighbour[0] + isNeighbour[1] + isNeighbour[2] + isNeighbour[3] + isNeighbour[4] + isNeighbour[5] + isNeighbour[6] + (map[i][j][k] == 4) > 3 )
      sharedVertices++;
    
    
}
  
  

void countHoles( ){
  
  accessReset();
  isCounting = 1;
  holes = 0;
  pits = 0;
  sharedEdges = 0;
  sharedVertices = 0;
  inVoid = 0;
  isAHole = 1;
  
  for( int i=0; i< N; i++){
    for( int j=0; j<N; j++){
      for( int k=0; k<N; k++){
	if( map[i][j][k] != 0 && map[i][j][k] < 4 && accessed[i][j][k] == 0){
	  restoreCountingBoundarys();
	  smallCubes = 0;
	  process(i,j,k);
	  score += smallCubes*smallCubes;
	  inVoid = 1;
	  for( int n=maxz; n>=minz;n--){
	    for( int l=minx; l<=maxx; l++){
	      for(int m=miny; m<=maxy ; m++){
		if( map[l][m][n] == 0 && !accessed[l][m][n])
		  digTheHole(l,m,n);
		checkShared(l,m,n);
	      }
	    }
	  }
	  inVoid = 0;
	  restoreMapAfterSelection();
	  floodReset();
	 }
      }
    }
  }
  
  
  countList();
  freeList();
  accessReset();
  isCounting = 0;
  inVoid = 0;
  
}

int backupMap[N][N][N];

void copyRealMap( ){
  for(int i=0; i<N; i++)
    for(int j=0; j<N; j++)
      for(int k=0; k<N; k++)
	backupMap[i][j][k] = map[i][j][k];
}

void copyBackupMap( ){
  for(int i=0; i<N; i++)
    for(int j=0; j<N; j++)
      for(int k=0; k<N; k++)
	 map[i][j][k] = backupMap[i][j][k];
}

int abortRotation = 0;
int newI;
int newJ;
int newK;

void handleRotation(int i, int j, int k, int isReverse){
  
  float lineX = ((N+1)*VERTEX_SIZE/initHeight + xLine);
  float lineY = ( (N+1)*VERTEX_SIZE/initHeight + yLine );
  float lineZ;
  
  float blockCenterX = (i+0.5)*2*VERTEX_SIZE/initHeight;
  float blockCenterY = (j+0.5)*2*VERTEX_SIZE/initHeight;
  float blockCenterZ = (k+0.5)*2*VERTEX_SIZE/initHeight;
 
  float xDist =  lineX - blockCenterX; 
  float yDist = lineY - blockCenterY;
  
  /* X rotation */
  if(xLine > -(N+2)*VERTEX_SIZE/initHeight && yLine > -(N+2)*VERTEX_SIZE/initHeight){
    
    newI = (int)( (lineX - isReverse*yDist)*initHeight/(2*VERTEX_SIZE)  );
    newJ = (int)( (lineY + isReverse*xDist) *initHeight/(2*VERTEX_SIZE)  );
    newK = k;
    
    if( lineX - isReverse*yDist < 0 && newI == 0)
      newI--;
    if( lineY + isReverse*xDist < 0 && newJ == 0)
      newJ--;
    }/* Y rotation */
    else if(  xLine < (N+2)*VERTEX_SIZE/initHeight && yLine > -(N+2)*VERTEX_SIZE/initHeight ){

      lineZ = -xLine - (N+3)*VERTEX_SIZE/initHeight;
      float zDist = lineZ - blockCenterZ;   

      
      newI = i;
      newJ = (int)( (lineY - isReverse*zDist ) *initHeight/(2*VERTEX_SIZE)  );
      newK = (int)( (lineZ + isReverse*yDist)*initHeight/(2*VERTEX_SIZE)  );
      if( lineZ + isReverse*yDist < 0 && newK == 0)
	newK--;
  } /* Z rotation */
  else if(  yLine < (N+2)*VERTEX_SIZE/initHeight && xLine > -(N+2)*VERTEX_SIZE/initHeight ){
    
    lineZ = -yLine - (N+3)*VERTEX_SIZE/initHeight;
      float zDist = lineZ - blockCenterZ;   

    
    newI = (int)( (lineX + isReverse*zDist ) *initHeight/(2*VERTEX_SIZE)  );
    newJ = j;
    newK = (int)( (lineZ - isReverse*xDist)*initHeight/(2*VERTEX_SIZE)  );
    if( lineZ - isReverse*xDist < 0 && newK ==  0)
      newK--;
    
  }
  
  /* Check if something went out of the map */
  if( newI >= 0 && newI <= N-1 && newJ >= 0 && newJ <= N- 1 && newK <= N-1 && newK >=0 ){
      
    if( backupMap[newI][newJ][newK] > 0 && backupMap[newI][newJ][newK] < 4){
	abortRotation = 1;
	 return;
      }
      else{
	backupMap[newI][newJ][newK] = oldType;
	
      }
  }
  else{
      newI = i;
      newJ = j;
      newK = k;
  }  
  
  if( backupMap[i][j][k] == 4)  
    backupMap[i][j][k] = 0;
  
  
}

int abortMovement = 0;

void moveBlock( int i, int j, int k, int whereTo ){
  
  newI = i;
  newJ = j;
  newK = k ;
  
   switch (whereTo ) {
    case 1: 	//down
	    if( j < N - 1){
	      if( backupMap[i][j+1][k] > 0 && backupMap[i][j+1][k] < 4){
		abortMovement = 1;
		return;
	      }
	      else
		backupMap[i][j+1][k] = oldType;

	      newJ++;
	    }
	    break;
      
    case 2:	//right
	    if( i < N - 1){
	      if( backupMap[i+1][j][k] > 0 && backupMap[i+1][j][k] < 4){
		abortMovement = 1;
		return;
	      }
	      else
		backupMap[i+1][j][k] = oldType;
	      
	      newI++;
	    }
	    break;
    case 3:	//up
	    if( j > 0){
	      if( backupMap[i][j-1][k] > 0 && backupMap[i][j-1][k] < 4){
		abortMovement = 1;
		return;
	      }
	      else
		backupMap[i][j-1][k] = oldType;
	      
	      newJ--;
	    }
	    break;
    case 4:	//left
	    if( i > 0){
	      if( backupMap[i-1][j][k] > 0 && backupMap[i-1][j][k] < 4){
		abortMovement = 1;
		return;
	      }
	      else
		backupMap[i-1][j][k] = oldType;
	      
	      newI--;
	    }
	    break;
   }
    if( backupMap[i][j][k] == 4)
      backupMap[i][j][k] = 0;

}


void succesfulChange( ){
      copyBackupMap();
      restoreMapAfterSelection();
      accessReset();
      globalI = newI;
      globalJ = newJ;
      globalK = newK;
      abortMovement = 0;
      abortRotation = 0;
      glutPostRedisplay();
      firstSearch = 1;
      selection();
}

void handleMovement(int whereTo ){

  copyRealMap();
  

  for(int i=0; i<N; i++)
    for(int j=0; j<N; j++)
      for(int k=0; k<N; k++){
	if( map[i][j][k] == 4 ){
	  moveBlock(i,j,k,whereTo);
	  if( abortMovement ){
	    abortMovement = 0;
	    glutPostRedisplay();
	    return;
	  }
	  
	}
      }
      succesfulChange();
}


void makeRotation(int isReverse ){

  copyRealMap();
  
  for(int i=0; i<N; i++){
    for(int j=0; j<N; j++){
      for(int k=0; k<N; k++){
	if( map[i][j][k] == 4 ){
	  handleRotation(i,j,k,isReverse);
	  if( abortRotation ){
	    abortRotation = 0;
	    glutPostRedisplay();
	    return;
	  }
	}
      }
    }
  }
      
     succesfulChange();
}


void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    case 102:
       score = 0;
       gameEnded();
       break;
    case 27:
      exit(0);
    case 122:
      if( zoom <= 2. )
	zoom*=1.1;
      glutPostRedisplay();
      break;
    case 120:
      if( zoom >= 2./4. )
	zoom*=9./10.;
      glutPostRedisplay();
      break;
    case 112:
      isPaused = !isPaused;
      if( isSelecting || isMoving || choosingRotAxis){
	isSelecting--;
	choosingRotAxis = 0;
	isMoving = 0;
	restoreMapAfterSelection();
	globalReset();
	accessReset();
	fillTheBlanks();
      }
      glutPostRedisplay();
      break;
    case 115:
      if( isPaused && !isSelecting){
	accessReset();
	isSelecting = 1;
	choosingRotAxis = 0;
	isMoving = 0;
	selection();
      }
      else if( isPaused && isSelecting ){
	isSelecting = 0;
	choosingRotAxis = 0;
	globalReset();
	accessReset();
	restoreMapAfterSelection();
	glutPostRedisplay();
      }
      break;
    case 9:
      if( isPaused && isSelecting ){
	restoreMapAfterSelection();
	globalIncrease();
	selection();
      }
      break;
    case 13:
      if( isPaused && isSelecting && choosingRotAxis ){
	makeRotation(1);
      }
      break;
    case 8:
      if( isPaused && isSelecting && choosingRotAxis ){
	makeRotation(-1);
      }
      break;
      
    case 114:
      if( isPaused && isSelecting && !choosingRotAxis && !isMoving){
	choosingRotAxis = 1;
	glutPostRedisplay();
      }
      else if( isPaused && isSelecting && choosingRotAxis && !isMoving){
	choosingRotAxis = 0;
	glutPostRedisplay();
      }
      break;
    
    case 109:
      if( isPaused && isSelecting && !choosingRotAxis && !isMoving){
	isMoving = 1;
	glutPostRedisplay();
      }
      else if( isPaused && isSelecting && !choosingRotAxis && isMoving){
	isMoving = 0;
	glutPostRedisplay();
      }
      break;
    case 98:
      if( isFinished )
	isFinished = 0;
      break;
      
      
  }
}

int xOld, yOld;


void mousebutton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
	  xOld = x;
	  yOld = y;
	  isRotating = 1;
	}
  
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){
	  isRotating = 0;
	}
	
}

void motion(int x, int y) 
{
	if( isRotating ){
	  
	  zRotation -= (xOld - x)/4.;
	  xRotation -= (yOld -y)/4.;
	  xOld = x;
	  yOld = y;
	  glutPostRedisplay();
	  
	}
}


void update(int value){
  
      if( !isPaused && !isFinished){
	zMovement -= downSpeed;
	glutPostRedisplay();
      }
      glutTimerFunc(30,update,0);
}

void special(int key, int x, int y){
  
    int currX = (int)(xMovement*initHeight/(2*VERTEX_SIZE) );
    int currY = (int)(yMovement*initHeight/(2*VERTEX_SIZE));
    int currZ = (int)(zMovement*initHeight/(2*VERTEX_SIZE));
     
 switch(key){
   
   case GLUT_KEY_DOWN:
     if( choosingRotAxis && yLine < (N-2)*VERTEX_SIZE/initHeight)
       yLine += 2*VERTEX_SIZE/initHeight;
     else if( !isPaused && !choosingRotAxis && yMovement < 2*(N-1)*VERTEX_SIZE/initHeight && !map[currX][currY+1][currZ])
      yMovement += 2*VERTEX_SIZE/initHeight;
     else if( isMoving ){
       handleMovement(1);
     }
     break;
   case GLUT_KEY_RIGHT:
     if( choosingRotAxis && xLine < (N-2)*VERTEX_SIZE/initHeight)
       xLine += 2*VERTEX_SIZE/initHeight;
     else if(!isPaused &&  !choosingRotAxis &&  xMovement < 2*(N-1)*VERTEX_SIZE/initHeight && !map[currX+1][currY][currZ])
      xMovement += 2*VERTEX_SIZE/initHeight;
     else if( isMoving )
       handleMovement(2);
     break;
   case GLUT_KEY_UP:
     if( choosingRotAxis && yLine > -(N+(2*N + 2)*(xLine > -(N+2)*VERTEX_SIZE/initHeight))*VERTEX_SIZE/initHeight)
       yLine -= 2*VERTEX_SIZE/initHeight;
     else if(!isPaused &&  !choosingRotAxis &&  yMovement  > 2*VERTEX_SIZE/initHeight && !map[currX][currY-1][currZ])
      yMovement -= 2*VERTEX_SIZE/initHeight;
     else if( isMoving )
       handleMovement(3);
     break;
   case GLUT_KEY_LEFT:
     if( choosingRotAxis && xLine > -(N+(2*N + 2)*(yLine > -(N+2)*VERTEX_SIZE/initHeight))*VERTEX_SIZE/initHeight)
       xLine -= 2*VERTEX_SIZE/initHeight;
     else if(!isPaused &&  !choosingRotAxis &&  xMovement >= 2*VERTEX_SIZE/initHeight && !map[currX-1][currY][currZ])
      xMovement -= 2*VERTEX_SIZE/initHeight;
     else if( isMoving )
       handleMovement(4);
     break;
 }
 glutPostRedisplay();
}

void parsePPM( const char *fileName , GLubyte A[88][88*3]){
  
	FILE *fp;
	int B[88][88*3];
	char c;
	fp = fopen (fileName,"r");
	if (fp!=NULL)
	{
	  /*Drope the title*/
	  fscanf(fp, "%s", (char *)(&A[0]) );
	  
	  /*Drop the comments*/
	  
	  fscanf(fp, "%c", &c);
	  c = 0;
	  
	  while( c != '\n')
	    fscanf(fp, "%c", &c);

	  /*Drop the size information*/
	  for(int i=0; i<3; i++)
	    fscanf(fp, "%d", (int *)(&A[0]) );
	  
	  /*Parse the greymaps */
	  for( int i=0; i<88; i++){
	    for( int j=0; j<88*3; j++)
	      fscanf(fp, "%d\n", (int *)(&B[i][j]) );
	  }
	  
	  /*Reverse the matrix, in case it's generated by gimp*/
	  for(int i=0; i<88; i++){
	    for(int j=0; j<88*3; j++)
	      A[i][j] = B[87-i][j];
	  }
	  fclose (fp);
	}
	else{
	  cout << "Error while opening .ppm files\n";
	  exit(0);
	}
}
void parsePGM( const char *fileName , GLubyte A[240][240]){
  
	FILE *fp;
	int B[240][240];
	char c;
	fp = fopen (fileName,"r");
	if (fp!=NULL)
	{
	  /*Drope the title*/
	  fscanf(fp, "%s", (char *)(&A[0]) );
	  
	  /*Drop the comments*/
	  
	  fscanf(fp, "%c", &c);
	  c = 0;
	  
	  while( c != '\n')
	    fscanf(fp, "%c", &c);

	  /*Drop the size information*/
	  for(int i=0; i<3; i++)
	    fscanf(fp, "%d", (int *)(&A[0][0]) );
	  
	  /*Parse the greymaps */
	  for( int i=0; i<240; i++){
	    for(int j=0; j<240; j++)
	      fscanf(fp, "%d\n", (int *)(&B[i][j]) );
	  }
	  
	  /*Reverse the matrix, in case it's generated by gimp*/
	  for(int i=0; i<240; i++){
	    for(int j=0; j<240; j++)
	      A[i][j] = B[239-i][j];
	  }
	  fclose (fp);
	}
	else{
	  cout << "Error while opening .pgm files\n";
	  exit(0);
	}
}

void parseThem( ){
  parsePGM("images/menu1.pgm",menu1);
  parsePGM("images/menu2.pgm",menu2);
  parsePGM("images/menu3.pgm",menu3);
  parsePGM("images/menu4.pgm",menu4);
  parsePGM("images/menu5.pgm",menu5);
  parsePPM("images/cubes.ppm",tiger);
  parsePGM("images/go.pgm",go);
}



int main(int argc, char** argv)
{
  
  srand(time(NULL));
 
  glutInit(&argc, argv);
  parseThem();
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
