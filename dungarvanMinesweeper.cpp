#include <windows.h>	// for timeGetTime()
#include <mmsystem.h>	// ditto
#include <iostream>		// I/O
#include <time.h>
#include <glut.h>	// GLUT
//#include <gl/glu.h>		// for gluPerspective & gluLookAt
#include "model3DS.h" // 3DS model support
#include "enemy.cpp"
#include "bullet.cpp"

#define PI 3.141592654f
#define NUM_LEVELS 3
#define BULLETS_LIM 50
model3DS *dungarvan;

void setupScene();
void updateScene();
void renderScene();
void exitScene();
void keypress(unsigned char key, int x, int y);
void keyup(unsigned char key, int x, int y);
void setViewport(int width, int height);
void toggleFullscreen();
void light();
void character(void);
void drawArm(GLUquadricObj *quadratic);
GLuint importTexture(std::string filename);	
void importSkybox();
void drawEnemy(enemy e);
void initEnemies();
void drawEnemies();
void camera();
void checkLevelComplete();
void drawText();
void renderBitmapString(float x, float y, int spacing, void *font, std::string string);
void drawIntro();
void restart();
void keyboardMovement();

// Setup some global variables for tracking through iterations of the glut main loop
bool disco = false, fullscreen=false, lighting=true, thirdPerson=true, topDown=false, sound=true, intro=true, win=false, dead=false;
int armAngle=50;
bool armDirection = true;
int windowId;
float multiplier = 2;
float xRotation = 0, yRotation = 0;
float xPos=0, yPos=0, zPos=0;
int globalHeight = 22;
float prevX=NULL, prevY=NULL;
bool fire = false; // Is this the ideal way to do this?
bool wPressed=false, aPressed=false, sPressed=false, dPressed=false;
GLuint      textureId;
DWORD		lastTickCount;
GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0};
GLfloat left_light_position[] = {1500,700, -600, 1.0}; 
GLfloat right_light_position[] = {-1500,700,+600, 1.0};

GLuint north, south, east, west, top, bottom;

// Enemies
enemy *enemies[100];
int numEnemies = 100;
int level = 0, lives=10;
int enemyCount[5];
int enemiesLeft = 6;

// Bullets
bullet *b;
bullet *bullets[BULLETS_LIM];
int bulletCounter=0;
GLUquadricObj *quad[BULLETS_LIM];	


// Level
float lowerX = -800.0;
float upperX = 100.0;
float lowerZ = -400.0;
float upperZ = 400.0;

// Fog stuff
bool fog = false;
#define FOG_COLOR 0.4, 0.4, 0.4
float fog_thickness = 0.3;
float fog_start = 120;
float fog_end = 300;


void character (void) {
	glPushMatrix();
		glDisable(GL_LIGHTING);
		glTranslatef(0, -4.5, -14);
		glPushMatrix();
			// Body
				
			glColor3f(1.0, 0.7, 0.3);
			GLUquadricObj *quadratic;	
			quadratic=gluNewQuadric();
			gluSphere(quadratic,2.5f,32,32);
		glPopMatrix();
		glTranslatef(0, +3.5, 0);
		glColor3f(1.0, 0.6, 0.0);
		gluSphere(quadratic,1.0f,32,32);

		glPushMatrix();
			// Arm 1
			drawArm(quadratic);
		glPopMatrix();
		glPushMatrix();
			// Arm 2
			glRotatef(180, 0.0, 1.0, 0.0);
			drawArm(quadratic);
		glPopMatrix();
		glEnable(GL_LIGHTING);
	glPopMatrix();
}

void drawArm(GLUquadricObj *quadratic){
	glColor3f(0.9, 0.9, 0.9);
	// Distance from body, height
	glTranslatef(1.75, -2.5, 0);
	glRotatef(90,1.0,0.0,0.0);
	glRotatef(armAngle,0.0,1.0,0.0);
	gluCylinder(quadratic, 0.5, 0.5, 3,32,32);
}

void fireBullets(){
	glDisable(GL_LIGHTING);
	for (int i=0; i<BULLETS_LIM; i++){
		if (bullets[i]->isFiring()){
			if (i>2){
				printf("break");
			}
			glPushMatrix();
			glTranslatef(bullets[i]->getX(), bullets[i]->getY(), bullets[i]->getZ());
			//Draw bullets 
			glColor3f(0.5, 0.6, 0.0);
			gluSphere(quad[i],bullets[i]->getRadius(),32,32);
			if (bullets[i]->increment(lowerX, upperX, lowerZ, upperZ)) {
				//NOP
			}else{ 
				bullets[i]->stop(); 
			}
			glPopMatrix();
		}
	}
	glEnable(GL_LIGHTING);
	
	
	
	
	
	for (int i=0; i<enemyCount[level]; i++) {
		for (int j=0; j<BULLETS_LIM; j++){
			if (!enemies[i]->isDead() && bullets[j]->checkCollission(enemies[i]->getX(), enemies[i]->getY(), enemies[i]->getZ(), enemies[i]->getRadius())){
				enemies[i]->kill();
				enemiesLeft--;
				bullets[j]->stop();
				if (sound) PlaySound(L"audio/explode.wav",NULL,SND_FILENAME|SND_ASYNC);
				printf("\nBANG! There are %i enemies left. \n ", enemiesLeft);
				checkLevelComplete();
			}
		}
	}
}
void checkLevelComplete(){
	bool allDead = true;
	for (int i=0; i<enemyCount[level]; i++){
		if (!enemies[i]->isDead()){
			allDead = false;
		}
	}
	if (allDead && level == NUM_LEVELS){
		win = true;
	}
	if (allDead && level<NUM_LEVELS){
		level++;
		printf("\nlevel++ to %i with %i enemies\n", level, enemyCount[level]);
		if (sound) PlaySound(L"audio/level.wav",NULL,SND_FILENAME|SND_ASYNC);
		initEnemies();
	}
	

	if (level==1){
		fog=true;
		light();
	}
}

void camera(){
	
	if (topDown){
		glTranslated(-xPos,0,-zPos); // Translate to current xPos, yPos, zPos
		glRotatef(90,1.0,0.0,0.0); // X
		glRotatef(yRotation,0.0,1.0,0.0); // Y
	} else {  
		glRotatef(xRotation,1.0,0.0,0.0); // X
		glRotatef(yRotation,0.0,1.0,0.0); // Y
		yPos = globalHeight;
		 glTranslated(-xPos,yPos,-zPos); // Translate to current xPos, yPos, zPos
	}
}

void drawIntro(){
	if (yRotation<180) yRotation+=2;
	if (xRotation>0) xRotation-=2;
	if (xPos<=-241) xPos+=2; else intro=false;
	if (yPos<=globalHeight) yPos++;
	if (zPos>=-46) zPos--;
	glRotatef(xRotation,1.0,0.0,0.0); // X
	glRotatef(yRotation,0.0,1.0,0.0); // Y
	
	glTranslated(-xPos,yPos,-zPos); // Translate to current xPos, yPos, zPos
}
void restart(){
	dead = false;
	level = 0;
	lives=10;
	enemiesLeft = 6;
	initEnemies();
}

void skybox(){
	// http://sidvind.com/wiki/Skybox_tutorial was used to aid the construction of this


	glPushMatrix();

	glRotatef(xRotation,1.0,0.0,0.0); // X
    glRotatef(yRotation,0.0,1.0,0.0); // Y

	
	   // Reset and transform the matrix.

	glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
	 
	// Colour our sky blue just incase
//	glColor4f(0.1,0.3,0.8,1);

	   // Render the front quad
  glBindTexture(GL_TEXTURE_2D, north);
   glBegin(GL_QUADS);
       glTexCoord2f(0, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
       glTexCoord2f(1, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
       glTexCoord2f(1, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
       glTexCoord2f(0, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
   glEnd();

   // Render the left quad
  glBindTexture(GL_TEXTURE_2D, west);
   glBegin(GL_QUADS);
       glTexCoord2f(0, 0); glVertex3f(  0.5f, -0.5f,  0.5f );
       glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
       glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
       glTexCoord2f(0, 1); glVertex3f(  0.5f,  0.5f,  0.5f );
   glEnd();

   // Render the back quad
   glBindTexture(GL_TEXTURE_2D, south);
   glBegin(GL_QUADS);
       glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f,  0.5f );
       glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f,  0.5f );
       glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f,  0.5f );
       glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f,  0.5f );
   glEnd();


    // Render the right quad
   glBindTexture(GL_TEXTURE_2D, east);
   glBegin(GL_QUADS);
       glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
       glTexCoord2f(1, 0); glVertex3f( -0.5f, -0.5f,  0.5f );
       glTexCoord2f(1, 1); glVertex3f( -0.5f,  0.5f,  0.5f );
       glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
   glEnd();


    // Render the top quad
  glBindTexture(GL_TEXTURE_2D, top);
   glBegin(GL_QUADS);
       glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
       glTexCoord2f(0, 0); glVertex3f( -0.5f,  0.5f,  0.5f );
       glTexCoord2f(1, 0); glVertex3f(  0.5f,  0.5f,  0.5f );
       glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
   glEnd();
   // Render the bottom quad
  glBindTexture(GL_TEXTURE_2D, bottom);
   glBegin(GL_QUADS);
       glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
       glTexCoord2f(0, 1); glVertex3f( -0.5f, -0.5f,  0.5f );
       glTexCoord2f(1, 1); glVertex3f(  0.5f, -0.5f,  0.5f );
       glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
   glEnd();
   // Restore enable bits and matrix
   glPopAttrib();
   glPopMatrix();


}


void renderScene(){
	
	

	// Set the colour behind our skybox
	if (disco){	
		// Every 10 ticks
		if (lastTickCount%10==0) glClearColor((float)1000/rand(),(float)1000/rand(),(float)1000/rand(),1.0);
	}else{
		glClearColor(0.0,0.0,0.0,1.0); 
	}
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    // Clear framebuffer & depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    
    // Reset Modelview matrix      		
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	
	skybox();
	drawText();
	
	if (thirdPerson) character();
	keyboardMovement();
	if (intro) { 
		drawIntro();
	} else {
		camera();
	}
	drawEnemies();
	if (fire) fireBullets();

    // Draw textured tetrahedron
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0,1.0,1.0);
    glBindTexture(GL_TEXTURE_2D,textureId);
		
	// Draw Dungarvan
	glPushMatrix();
	
	dungarvan->draw();

    glDisable(GL_TEXTURE_2D);
	glPopMatrix();


	
    // Swap double buffer for flicker-free animation
    glutSwapBuffers();
        
}

void drawText(){
	// The tutorial at http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho helped with this
	// Draw some text on the screen
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// 2D orthographic with window's res
	gluOrtho2D(0, 1600, 1200, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	// Needed for itoa - convert each int to char. Seriously, C++!? :-(
	char buffer [33];
	_itoa_s(lives, buffer, 10);

	if (win) { 
		renderBitmapString(700,50,0,GLUT_BITMAP_TIMES_ROMAN_24,"//You win!//");
	}
	if (dead){
		renderBitmapString(700,50,0,GLUT_BITMAP_TIMES_ROMAN_24,"//Game over! Click to restart.//");
	}
	if (intro){
		renderBitmapString(700,50,0,GLUT_BITMAP_TIMES_ROMAN_24,"//Minesweeper//");
		renderBitmapString(600,80,0,GLUT_BITMAP_TIMES_ROMAN_24,"W A S D + mouse to move. Click to shoot. ");

	}else{
	std::string stringLives = "Lives: " + (std::string)buffer;
	_itoa_s(enemiesLeft, buffer, 10);
	std::string stringEnemiesLeft = "Enemies Left: " + (std::string)buffer;
	_itoa_s(level+1, buffer, 10);
	std::string stringLevel = "Level: " + (std::string)buffer;
//	char *lives = "Lives: 3";
//	char *enemiesLeft = "Enemies: 6" + (char)enemiesLeft;
//	char *level = "Level: 1";
	renderBitmapString(5,15,0,GLUT_BITMAP_HELVETICA_18,stringLives);
	renderBitmapString(5,30,0,GLUT_BITMAP_HELVETICA_18,stringEnemiesLeft);
	renderBitmapString(5,45,0,GLUT_BITMAP_HELVETICA_18,stringLevel);
	}
	glPopMatrix();

	
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);



}

void renderBitmapString(float x, float y, int spacing, void *font, std::string string) {
	// NB this function is from http://www.lighthouse3d.com/opengl/glut/index.php?bmpfont
  //char *c;
  int x1=x;
  for (int i=0; i<(float)string.length(); i++){
	char c = string.at(i);
	glColor3f(0.0, 1.0, 0.0);
	glRasterPos2f(x1,y);
	glutBitmapCharacter(font, c);
	x1 = x1 + glutBitmapWidth(font,c) + spacing;
  }

  
}



void drawEnemy(enemy e){
	glPushMatrix();
		glDisable(GL_LIGHTING);
		glTranslatef(e.getX(), e.getY(), e.getZ());
		if (level==0) glColor3f(0.6,0.6,0.6);
		if (level==1) glColor3f(1,0.0,0.0);
		if (level==2) glColor3f(0.1,0.1,0.1);
		GLUquadricObj *quadratic;	
		quadratic=gluNewQuadric();
		float r=0, g=0, b=0;
		gluSphere(quadratic,e.getRadius(),32,32);
		glPushMatrix();
			glTranslatef(0, e.getRadius(), 0);
			glColor3f(r,g,b);
			gluSphere(quadratic,e.getRadius()/4,32,32);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0, -e.getRadius(), 0);
			glColor3f(r,g,b);
			gluSphere(quadratic,e.getRadius()/4,32,32);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(e.getRadius(), 0, 0);
			glColor3f(r,g,b);
			gluSphere(quadratic,e.getRadius()/4,32,32);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-e.getRadius(), 0, 0);
			glColor3f(r,g,b);
			gluSphere(quadratic,e.getRadius()/4,32,32);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0, 0, e.getRadius());
			glColor3f(r,g,b);
			gluSphere(quadratic,e.getRadius()/4,32,32);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0, 0, -e.getRadius());
			glColor3f(r,g,b);
			gluSphere(quadratic,e.getRadius()/4,32,32);
		glPopMatrix();
		glEnable(GL_LIGHTING);
	glPopMatrix();
}

void updateScene(){
	
	// Wait until at least 16ms passed since start of last frame
	// Effectively caps framerate at ~60fps
	while(timeGetTime()-lastTickCount<16);
	lastTickCount=timeGetTime();
    
    // Increment angle for next frame
	if (armAngle<=100&&armDirection){
		 armAngle+=2;
	}
	if (armAngle>=50&&!armDirection){
		armAngle-=2;
	}
	if (armAngle==100){
		armDirection=false;
	}
	if (armAngle==50){
		armDirection=true;
	}

	// Do any other updates here
	
	// Draw the next frame
    glutPostRedisplay();

}

void keypress(unsigned char key, int x, int y){
	int mod = glutGetModifiers();

	int run = 1;
	if (mod == GLUT_ACTIVE_SHIFT){
		run = run * 2;
	}

	// Test if user pressed ESCAPE (ascii 27)
	// If so, exit the program
    if(key==27){  // ESC
		exitScene();
	}
	
	if (key==32){ // Space bar
	}
	if (key=='w')
		wPressed=true;
	
	if (key=='a')
		aPressed=true;
	
	if (key=='s')
		sPressed=true;

	if (key=='d')
		dPressed=true;
	if (key== 'x'){
		multiplier += 0.5;
	}
	if (key== 'c'){
		multiplier -= 0.5;
	}
	// Change the view
	if (key=='v'){
		if (thirdPerson) thirdPerson=false; else thirdPerson=true;
	}


	if (key=='y'){
		if (disco) { disco = false; } else {disco = true;}
	}

	if (key=='f'){
		toggleFullscreen();
	}
	if (key=='l'){
		// Cheat!
	//	for (int i=0; i<enemyCount[level]; i++){
		//	enemies[i]->kill();
		//}
		//checkLevelComplete();
		/*
		if (!lighting){
			glEnable(GL_LIGHTING);
			lighting = true;
		}else{
			glDisable(GL_LIGHTING);
			lighting = false;
		}
		*/
	}
	

	
}
void keyup(unsigned char key, int x, int y){
	if (key=='w')
		wPressed=false;
	
	if (key=='a')
		aPressed=false;
	
	if (key=='s')
		sPressed=false;

	if (key=='d')
		dPressed=false;
}
void keyboardMovement(){
	int run = 1;
	if (wPressed){
		float xRotationrad, yRotationrad;
		yRotationrad = (yRotation / 180 * PI);
		xRotationrad = (xRotation / 180 * PI); 
		xPos += run * multiplier * float(sin(yRotationrad)) ;
		yPos -= run * multiplier * float(sin(xRotationrad)) ;
		zPos -= run * multiplier * float(cos(yRotationrad)) ;

	}
	if (sPressed){
		float xRotationrad, yRotationrad;
		
		yRotationrad = (yRotation / 180 * PI);
		xRotationrad = (xRotation / 180 * PI); 
		xPos -= run * multiplier* float(sin(yRotationrad));
		yPos += run * multiplier* float(sin(xRotationrad));
		zPos += run * multiplier* float(cos(yRotationrad));
	}
	if (aPressed){
		float yRotationrad;
		yRotationrad = (yRotation / 180 * PI);
		xPos -= run * multiplier * float(cos(yRotationrad));
		zPos -= run * multiplier * float(sin(yRotationrad));
	}
	if (dPressed){
		float yRotationrad;
		yRotationrad = (yRotation / 180 * PI);
		xPos += run * multiplier * float(cos(yRotationrad));
		zPos += run * multiplier * float(sin(yRotationrad));
	}

}
void mouseMove(int x, int y){
	float movementX = 0;
	float movementY = 0;
	if (prevX!=NULL) movementX = (float)x - prevX;
	if (prevY!=NULL) movementY = (float)y - prevY;
	
	if (!intro){
		xRotation += movementY;
		yRotation += movementX;
	}

	prevX = (float)x;
	prevY = (float)y;
}
void mouseClick(int button, int state, int x, int y){
	if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){

		if (dead){
			// Click to restart when dead
			restart();
		}else{
			// Setup for firing a bullet
			float rotation = yRotation;
			
			bullets[bulletCounter] = new bullet(xPos, globalHeight-50, +zPos, rotation, 1.0, 10);
			fire = true;

			bulletCounter++;
			// Reset our bullet array counter, it's at the limit
			if (bulletCounter==10) bulletCounter=0;
			printf("Creating bullet at xPos = %f, zPos=%f with r=%f\n", xPos, zPos, rotation);
			if (sound) PlaySound(L"audio/shoot.wav",NULL,SND_FILENAME|SND_ASYNC);
		}
	}
}

void initEnemies(){
	// Setup level based variables before init'ing
	enemyCount[0] = 6;
	enemyCount[1] = 11;
	enemyCount[2] = 23;
	enemyCount[3] = 30;
	enemyCount[4] = 50;
	enemiesLeft = enemyCount[level];
	float enemySize = 30.0f;
	float enemySpeed = 1.0f;
	if (level==1){
		enemySize = 15.0f;
		enemySpeed = 1.5f;
	}
	if (level==2 || level==3){
		enemySize = 13.0f;
		enemySpeed = 2.0f;
	}
	int enemyHeight = -globalHeight + (enemySize/2);
	srand ( lastTickCount );

	// Now init every enemy required for this level

	for (int i=0; i<enemyCount[level]; i++){
		
		int randX = 0, randZ = 0;
		int xBoundSize = abs(lowerX - upperX);
		int zBoundSize = abs(lowerX - upperX);
		randX = rand() % xBoundSize + lowerX;
		randZ = rand() % zBoundSize + lowerZ;
		
		enemies[i] = new enemy(randX, enemyHeight, randZ, enemySize, enemySpeed);
	}

	// Now override a few of our random enemies to be static
	enemies[0] = new enemy(-204, enemyHeight, -248, enemySize, 0); // Town Square
	enemies[10] = new enemy(145, enemyHeight, -128, enemySize, 0); // Supervalue carpark
	enemies[12] = new enemy(153, enemyHeight, 21, enemySize, 0); // Supervalue carpark		

	for (int i=0; i<BULLETS_LIM; i++){
		bullets[i] = new bullet(0, 0, 0, 0, 0, 0);
		bullets[i]->stop();
		quad[i]=gluNewQuadric();
	}
}
void drawEnemies(){
	for (int i=0; i<enemyCount[level]; i++){
		if (!enemies[i]->isDead()){
				drawEnemy(*enemies[i]);
		// Ideally we should be able to call increment and pass a level1 object but C++ arrays are silly. 
				//printf("x, y, z %f,%f,%f - enemeypos %f, %f, %f\n", xPos, yPos, zPos, enemies[i]->getX(), enemies[i]->getY(), enemies[i]->getZ());
			if (enemies[i]->checkCollission(xPos, yPos, zPos)){
				if (sound) PlaySound(L"audio/killEnemy.wav",NULL,SND_FILENAME|SND_ASYNC);
				printf("\nlives--\n");
				lives--;
				//enemies[i]->reverseDirection();
				enemies[i]->kill();
				enemiesLeft--;
				if (lives<0){
					dead = true;
				}
			}
			enemies[i]->increment(lowerX, upperX, lowerZ, upperZ);
		}
	}
}

void light(){
	//glTexEnvi(GL_MODULATE);
	if (lighting) glEnable(GL_LIGHTING);	

	// Create a "sunlight"
	GLfloat sun[] = {1.0, 1.0, 1.0, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sun);
	GLfloat lightPosition[] = {0.5f, 0.75f, 0.75f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);


	// Material Settings
	GLfloat allLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// turn on full specular and color by default in material
	glMateriali(GL_FRONT, GL_SHININESS, 64);
	glMaterialfv(GL_FRONT, GL_SPECULAR, allLight);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, allLight);
	glMaterialfv(GL_FRONT, GL_AMBIENT, allLight);
	glEnable(GL_LIGHT0);

	if(fog){
		// Enable and Set Fog
		GLfloat fogColor[] = { FOG_COLOR, 1.0 };
	
		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, GL_LINEAR);
		
		glFogf(GL_FOG_START, fog_start);		
		glFogf(GL_FOG_END, fog_end);	
		glFogf(GL_FOG_DENSITY, fog_thickness);	
		glFogfv(GL_FOG_COLOR, fogColor);
	}else{
		// No Fog
		glDisable(GL_FOG);
	}




	
	
	
}

void setupScene(){

	std::cout<<"Initializing scene..."<<std::endl;
		
	light();
	
	initEnemies();


	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);

	dungarvan = new model3DS("dungarvan\\dungarvan.3ds", 2);

	if (intro) {
		xPos = -575;
		yPos = -50;
		zPos = 267;
		xRotation = 29;
		yRotation = 29;		
	}
      
}

void exitScene(){

    std::cout<<"Exiting scene..."<<std::endl;

    // Close window
    if (!fullscreen) glutDestroyWindow(windowId);

    // Free any allocated memory

    // Exit program
    exit(0);
}

void setViewport(int width, int height) {

    // Work out window ratio, avoid divide-by-zero
    if(height==0)height=1;
	float ratio = float(width)/float(height);

	// Reset projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Fill screen with viewport
	glViewport(0, 0, width, height);

	if (topDown){
		glOrtho(1000, -1000, globalHeight, -2500, -1000, 1000);
	}else{
		// Set a 45 degree perspective
		gluPerspective(45, ratio, .1, 2000);
	}
	
	
	

}

void toggleFullscreen(){

	if (fullscreen){
		glutLeaveGameMode();
		glutInitWindowPosition(50,50);
		glutInitWindowSize(1600,1200);
		windowId = glutCreateWindow("Lab 4: Loading a textured model");
	}else{
		glutDestroyWindow(windowId);
		glutGameModeString("1600x1200:24");
		glutEnterGameMode(); 
	}
}

GLuint importTexture(std::string filename){
	std::transform(filename.begin(),filename.end(),filename.begin(),tolower);

	GLuint newTextureId;
	glGenTextures(1, &newTextureId);

	textureBMP newTexture(filename, newTextureId);

	return newTextureId;
}

void importSkybox(){
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// Make skybox textures global so only imported once
	north = importTexture("skybox\\north.bmp");
	south = importTexture("skybox\\south.bmp");
	east = importTexture("skybox\\east.bmp");
	west = importTexture("skybox\\west.bmp");
	top = importTexture("skybox\\top.bmp");
	bottom = importTexture("skybox\\bottom.bmp");
}


int main(int argc, char *argv[]){
        
    // Initialise OpenGL
    glutInit(&argc, argv); 

	

    // Set window position, size & create window
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    

	if (fullscreen){
		glutGameModeString("1900x1080:24");
		glutEnterGameMode(); 
	}else{
		glutInitWindowPosition(50,50);
		glutInitWindowSize(1600,1200);
		windowId = glutCreateWindow("Lab 4: Loading a textured model");
	}

    
    // Set GLUT callback functions
    glutReshapeFunc(setViewport);
    glutDisplayFunc(renderScene);
    glutIdleFunc(updateScene);
    glutKeyboardFunc(keypress);
	glutKeyboardUpFunc(keyup);
	glutPassiveMotionFunc(mouseMove);
	glutMouseFunc(mouseClick);


    // Setup OpenGL state & scene resources (models, textures etc)
    setupScene();
	importSkybox();

    // Show window & start update loop
    glutMainLoop();    

    return 0;
    
}