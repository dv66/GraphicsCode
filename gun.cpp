#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))
#define LIMIT 20
double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

double targetDistance;






double rotationAngle;
double backTotalLength;

double frontTotalLength;

struct point
{
	double x,y,z;
};




point bU;
point bR;
point bL;
point fU;
point fR;
point fL;


point bulletArray[1000];
int numBullets;












point currentPosition;
point uVector;
point rVector;
point lVector;




/** to get the bullet position*/
point point1;
point point2;




point gunBackL;
point gunPosition;

double gunBackRadius;
double gunBackHeight;
double gunBackSlices;
double gunFrontRadius;
double gunFrontHeight;
double gunFrontSlices;


double gunBackAngle =0;
double gunFrontAngle =0;
double frontRollAngle = 0;


void drawAxes()
{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();

}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}




void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
//        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
                glColor3f((double)i/(double)slices,(double)i/(double)slices,(double)i/(double)slices);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
//                //lower hemisphere
//                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
//				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
//				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
//				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}



void lookLeft(){
    double tempX = lVector.x;
    double tempY = lVector.y;
    double tempZ = lVector.z;
    lVector.x = lVector.x*cos(rotationAngle)+(-rVector.x)*sin(rotationAngle);
    lVector.y = lVector.y*cos(rotationAngle)+(-rVector.y)*sin(rotationAngle);
    lVector.z = lVector.z*cos(rotationAngle)+(-rVector.z)*sin(rotationAngle);

    rVector.x = rVector.x*cos(rotationAngle)+(tempX)*sin(rotationAngle);
    rVector.y = rVector.y*cos(rotationAngle)+(tempY)*sin(rotationAngle);
    rVector.z = rVector.z*cos(rotationAngle)+(tempZ)*sin(rotationAngle);
//    printf("U = %lf %lf %lf  -- R = %lf %lf %lf  -- L = %lf %lf %lf\n\n", uVector.x,uVector.y,uVector.z ,rVector.x,rVector.y,rVector.z, lVector.x,lVector.y,lVector.z);
}

void lookRight(){
    double tempX = rVector.x;
    double tempY = rVector.y;
    double tempZ = rVector.z;

    rVector.x = rVector.x*cos(rotationAngle)+(-lVector.x)*sin(rotationAngle);
    rVector.y = rVector.y*cos(rotationAngle)+(-lVector.y)*sin(rotationAngle);
    rVector.z = rVector.z*cos(rotationAngle)+(-lVector.z)*sin(rotationAngle);

    lVector.x = lVector.x*cos(rotationAngle)+(tempX)*sin(rotationAngle);
    lVector.y = lVector.y*cos(rotationAngle)+(tempY)*sin(rotationAngle);
    lVector.z = lVector.z*cos(rotationAngle)+(tempZ)*sin(rotationAngle);
//    printf("U = %lf %lf %lf  -- R = %lf %lf %lf  -- L = %lf %lf %lf\n\n", uVector.x,uVector.y,uVector.z ,rVector.x,rVector.y,rVector.z, lVector.x,lVector.y,lVector.z);
}


void lookUp(){
    double tempX = uVector.x;
    double tempY = uVector.y;
    double tempZ = uVector.z;
    uVector.x = uVector.x*cos(rotationAngle)+(-lVector.x)*sin(rotationAngle);
    uVector.y = uVector.y*cos(rotationAngle)+(-lVector.y)*sin(rotationAngle);
    uVector.z = uVector.z*cos(rotationAngle)+(-lVector.z)*sin(rotationAngle);

    lVector.x = lVector.x*cos(rotationAngle)+(tempX)*sin(rotationAngle);
    lVector.y = lVector.y*cos(rotationAngle)+(tempY)*sin(rotationAngle);
    lVector.z = lVector.z*cos(rotationAngle)+(tempZ)*sin(rotationAngle);
//    printf("U = %lf %lf %lf  -- R = %lf %lf %lf  -- L = %lf %lf %lf\n\n", uVector.x,uVector.y,uVector.z ,rVector.x,rVector.y,rVector.z, lVector.x,lVector.y,lVector.z);
}


void lookDown(){
    double tempX = lVector.x;
    double tempY = lVector.y;
    double tempZ = lVector.z;
    lVector.x = lVector.x*cos(rotationAngle)+(-uVector.x)*sin(rotationAngle);
    lVector.y = lVector.y*cos(rotationAngle)+(-uVector.y)*sin(rotationAngle);
    lVector.z = lVector.z*cos(rotationAngle)+(-uVector.z)*sin(rotationAngle);

    uVector.x = uVector.x*cos(rotationAngle)+(tempX)*sin(rotationAngle);
    uVector.y = uVector.y*cos(rotationAngle)+(tempY)*sin(rotationAngle);
    uVector.z = uVector.z*cos(rotationAngle)+(tempZ)*sin(rotationAngle);
//    printf("U = %lf %lf %lf  -- R = %lf %lf %lf  -- L = %lf %lf %lf\n\n", uVector.x,uVector.y,uVector.z ,rVector.x,rVector.y,rVector.z, lVector.x,lVector.y,lVector.z);
}


void tiltCounterClockwise(){
    double tempX = rVector.x;
    double tempY = rVector.y;
    double tempZ = rVector.z;
    rVector.x = rVector.x*cos(rotationAngle)+(-uVector.x)*sin(rotationAngle);
    rVector.y = rVector.y*cos(rotationAngle)+(-uVector.y)*sin(rotationAngle);
    rVector.z = rVector.z*cos(rotationAngle)+(-uVector.z)*sin(rotationAngle);

    uVector.x = uVector.x*cos(rotationAngle)+(tempX)*sin(rotationAngle);
    uVector.y = uVector.y*cos(rotationAngle)+(tempY)*sin(rotationAngle);
    uVector.z = uVector.z*cos(rotationAngle)+(tempZ)*sin(rotationAngle);
//    printf("U = %lf %lf %lf  -- R = %lf %lf %lf  -- L = %lf %lf %lf\n\n", uVector.x,uVector.y,uVector.z ,rVector.x,rVector.y,rVector.z, lVector.x,lVector.y,lVector.z);
}


void tiltClockwise(){
    double tempX = uVector.x;
    double tempY = uVector.y;
    double tempZ = uVector.z;
    uVector.x = uVector.x*cos(rotationAngle)+(-rVector.x)*sin(rotationAngle);
    uVector.y = uVector.y*cos(rotationAngle)+(-rVector.y)*sin(rotationAngle);
    uVector.z = uVector.z*cos(rotationAngle)+(-rVector.z)*sin(rotationAngle);

    rVector.x = rVector.x*cos(rotationAngle)+(tempX)*sin(rotationAngle);
    rVector.y = rVector.y*cos(rotationAngle)+(tempY)*sin(rotationAngle);
    rVector.z = rVector.z*cos(rotationAngle)+(tempZ)*sin(rotationAngle);
//    printf("U = %lf %lf %lf  -- R = %lf %lf %lf  -- L = %lf %lf %lf\n\n", uVector.x,uVector.y,uVector.z ,rVector.x,rVector.y,rVector.z, lVector.x,lVector.y,lVector.z);
}






/**
gun-back
*/

void turnLeft(){
    double tempX = bL.x;
    double tempY = bL.y;
    double tempZ = bL.z;
    double rotSpeed = (1*pi)/180;
    bL.x = bL.x*cos(rotSpeed)+(-bR.x)*sin(rotSpeed);
    bL.y = bL.y*cos(rotSpeed)+(-bR.y)*sin(rotSpeed);
    bL.z = bL.z*cos(rotSpeed)+(-bR.z)*sin(rotSpeed);

    bR.x = bR.x*cos(rotSpeed)+(tempX)*sin(rotSpeed);
    bR.y = bR.y*cos(rotSpeed)+(tempY)*sin(rotSpeed);
    bR.z = bR.z*cos(rotSpeed)+(tempZ)*sin(rotSpeed);
//    printf("U = %lf %lf %lf  -- R = %lf %lf %lf  -- L = %lf %lf %lf\n\n", uVector.x,uVector.y,uVector.z ,rVector.x,rVector.y,rVector.z, lVector.x,lVector.y,lVector.z);
}

void turnRight(){
    double tempX = bR.x;
    double tempY = bR.y;
    double tempZ = bR.z;
    double rotSpeed = (1*pi)/180;
    bR.x = bR.x*cos(rotSpeed)+(-bL.x)*sin(rotSpeed);
    bR.y = bR.y*cos(rotSpeed)+(-bL.y)*sin(rotSpeed);
    bR.z = bR.z*cos(rotSpeed)+(-bL.z)*sin(rotSpeed);

    bL.x = bL.x*cos(rotSpeed)+(tempX)*sin(rotSpeed);
    bL.y = bL.y*cos(rotSpeed)+(tempY)*sin(rotSpeed);
    bL.z = bL.z*cos(rotSpeed)+(tempZ)*sin(rotSpeed);
}

void turnUp(){
    double tempX = bU.x;
    double tempY = bU.y;
    double tempZ = bU.z;
    double rotSpeed = (1*pi)/180;
    bU.x = bU.x*cos(rotSpeed)+(-bL.x)*sin(rotSpeed);
    bU.y = bU.y*cos(rotSpeed)+(-bL.y)*sin(rotSpeed);
    bU.z = bU.z*cos(rotSpeed)+(-bL.z)*sin(rotSpeed);

    bL.x = bL.x*cos(rotSpeed)+(tempX)*sin(rotSpeed);
    bL.y = bL.y*cos(rotSpeed)+(tempY)*sin(rotSpeed);
    bL.z = bL.z*cos(rotSpeed)+(tempZ)*sin(rotSpeed);
}

void turnDown(){
    double tempX = bL.x;
    double tempY = bL.y;
    double tempZ = bL.z;
    double rotSpeed = (1*pi)/180;
    bL.x = bL.x*cos(rotSpeed)+(-bU.x)*sin(rotSpeed);
    bL.y = bL.y*cos(rotSpeed)+(-bU.y)*sin(rotSpeed);
    bL.z = bL.z*cos(rotSpeed)+(-bU.z)*sin(rotSpeed);

    bU.x = bU.x*cos(rotSpeed)+(tempX)*sin(rotSpeed);
    bU.y = bU.y*cos(rotSpeed)+(tempY)*sin(rotSpeed);
    bU.z = bU.z*cos(rotSpeed)+(tempZ)*sin(rotSpeed);
}
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/


/**
gun-front
*/

void turnFrontLeft(){
    double tempX = fL.x;
    double tempY = fL.y;
    double tempZ = fL.z;
    double rotSpeed = (1*pi)/180;
    fL.x = fL.x*cos(rotSpeed)+(-fR.x)*sin(rotSpeed);
    fL.y = fL.y*cos(rotSpeed)+(-fR.y)*sin(rotSpeed);
    fL.z = fL.z*cos(rotSpeed)+(-fR.z)*sin(rotSpeed);

    fR.x = fR.x*cos(rotSpeed)+(tempX)*sin(rotSpeed);
    fR.y = fR.y*cos(rotSpeed)+(tempY)*sin(rotSpeed);
    fR.z = fR.z*cos(rotSpeed)+(tempZ)*sin(rotSpeed);
//    printf("U = %lf %lf %lf  -- R = %lf %lf %lf  -- L = %lf %lf %lf\n\n", uVector.x,uVector.y,uVector.z ,rVector.x,rVector.y,rVector.z, lVector.x,lVector.y,lVector.z);
}

void turnFrontRight(){
    double tempX = fR.x;
    double tempY = fR.y;
    double tempZ = fR.z;
    double rotSpeed = (1*pi)/180;
    fR.x = fR.x*cos(rotSpeed)+(-fL.x)*sin(rotSpeed);
    fR.y = fR.y*cos(rotSpeed)+(-fL.y)*sin(rotSpeed);
    fR.z = fR.z*cos(rotSpeed)+(-fL.z)*sin(rotSpeed);

    fL.x = fL.x*cos(rotSpeed)+(tempX)*sin(rotSpeed);
    fL.y = fL.y*cos(rotSpeed)+(tempY)*sin(rotSpeed);
    fL.z = fL.z*cos(rotSpeed)+(tempZ)*sin(rotSpeed);
}

void turnFrontUp(){
    double tempX = fU.x;
    double tempY = fU.y;
    double tempZ = fU.z;
    double rotSpeed = (1*pi)/180;
    fU.x = fU.x*cos(rotSpeed)+(-fL.x)*sin(rotSpeed);
    fU.y = fU.y*cos(rotSpeed)+(-fL.y)*sin(rotSpeed);
    fU.z = fU.z*cos(rotSpeed)+(-fL.z)*sin(rotSpeed);

    fL.x = fL.x*cos(rotSpeed)+(tempX)*sin(rotSpeed);
    fL.y = fL.y*cos(rotSpeed)+(tempY)*sin(rotSpeed);
    fL.z = fL.z*cos(rotSpeed)+(tempZ)*sin(rotSpeed);
}

void turnFrontDown(){
    double tempX = fL.x;
    double tempY = fL.y;
    double tempZ = fL.z;
    double rotSpeed = (1*pi)/180;
    fL.x = fL.x*cos(rotSpeed)+(-fU.x)*sin(rotSpeed);
    fL.y = fL.y*cos(rotSpeed)+(-fU.y)*sin(rotSpeed);
    fL.z = fL.z*cos(rotSpeed)+(-fU.z)*sin(rotSpeed);

    fU.x = fU.x*cos(rotSpeed)+(tempX)*sin(rotSpeed);
    fU.y = fU.y*cos(rotSpeed)+(tempY)*sin(rotSpeed);
    fU.z = fU.z*cos(rotSpeed)+(tempZ)*sin(rotSpeed);
}

/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/
/**************************************************************************************************************************************************************/


point backPartPosition;
point frontPartPosition;










void drawSS()

{
    glColor3f(1,0,0);
    drawSquare(20);

    glRotatef(angle,0,0,1);
    glTranslatef(110,0,0);
    glRotatef(2*angle,0,0,1);
    glColor3f(0,1,0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle,0,0,1);
        glTranslatef(60,0,0);
        glRotatef(2*angle,0,0,1);
        glColor3f(0,0,1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3*angle,0,0,1);
    glTranslatef(40,0,0);
    glRotatef(4*angle,0,0,1);
    glColor3f(1,1,0);
    drawSquare(5);
}



void drawHalfSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
    int color = 0;
	for(i=0;i<stacks;i++)
	{
//        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
            glColor3f(color,color,color);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);

			}glEnd();
			color^=1;
		}

	}
}


void drawOpenHalfSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=(2*radius) - radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
    int color = 0;
	for(i=0;i<stacks;i++)
	{
//        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
            glColor3f(color,color,color);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);

			}glEnd();
			color^=1;
		}

	}
}




void drawCylinder(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++){
        h = i ;
		r=radius;
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
    int color = 0;
	for(i=0;i<stacks;i++){
        for(j=0;j<slices;j++){
            glColor3f(color,color,color);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}glEnd();
			color^=1;
		}

	}
}




point dirR;
point dirL;
point dirU;


void gunBackGoRight(){
    if(gunBackAngle <= LIMIT){
        gunBackAngle+=1;
        turnRight();
        turnFrontRight();
        backPartPosition.x = bL.x*backTotalLength;
        backPartPosition.y = bL.y*backTotalLength;
        backPartPosition.z = bL.z*backTotalLength;
    }
}

void gunBackGoLeft(){
    if(gunBackAngle >= -LIMIT){
        gunBackAngle-=1;
        turnLeft();
        turnFrontLeft();
        backPartPosition.x = bL.x*backTotalLength;
        backPartPosition.y = bL.y*backTotalLength;
        backPartPosition.z = bL.z*backTotalLength;
    }
}

double gunBackUpAngle = 0;
void gunBackGoUp(){
    if(gunBackUpAngle <= LIMIT){
        gunBackUpAngle+=1;
        turnUp();
        turnFrontUp();
        backPartPosition.x = bL.x*backTotalLength;
        backPartPosition.y = bL.y*backTotalLength;
        backPartPosition.z = bL.z*backTotalLength;
    }
}

void gunBackGoDown(){
    if(gunBackUpAngle >= -LIMIT){
        gunBackUpAngle-=1;
        turnDown();
        turnFrontDown();
        backPartPosition.x = bL.x*backTotalLength;
        backPartPosition.y = bL.y*backTotalLength;
        backPartPosition.z = bL.z*backTotalLength;
    }
}



void gunFrontGoUp(){
    if(gunFrontAngle <= LIMIT){
        gunFrontAngle+=1;
        turnFrontDown();
    }
}

void gunFrontGoDown(){
    if(gunFrontAngle >= -LIMIT){
        gunFrontAngle-=1;
        turnFrontUp();
    }
}

void frontRollClockwise(){
    if(frontRollAngle >= -LIMIT){
        frontRollAngle-=1;
    }
}

void frontRollAntiClockwise(){
    if(frontRollAngle <= LIMIT){
        frontRollAngle+=1;
    }
}



/**
model of the gun
*/
void drawGun(){

    glPushMatrix();{
        glRotatef(90,0,1,0);
        glRotatef(gunBackAngle,1,0,0);
        glRotatef(gunBackUpAngle,0,1,0);
        glTranslatef(0,0,-gunBackRadius);
        drawHalfSphere(gunBackRadius,gunBackSlices,20);
        glTranslatef(0,0,-gunBackHeight);
        drawCylinder(gunBackRadius, gunBackSlices, gunBackHeight);
        glRotatef(180,0,1,0);
        drawHalfSphere(gunBackRadius,gunBackSlices,20);

        glTranslatef(0,0,gunBackRadius+gunFrontRadius);
        glRotatef(180,1,0,0);
        glRotatef(gunFrontAngle,0,1,0);
        glRotatef(frontRollAngle,0,0,1);
        drawHalfSphere(gunFrontRadius,gunFrontSlices,20);
        glTranslatef(0,0,-gunFrontHeight);
        drawCylinder(gunFrontRadius, gunFrontSlices, gunFrontHeight);
        glRotatef(180,0,1,0);
        drawOpenHalfSphere(gunFrontRadius,gunFrontSlices,20);
    }glPopMatrix();



    // target
    glPushMatrix();{
        glRotatef(90,0,1,0);
        glTranslatef(0,0,-targetDistance);
        double col = 0.5;
        glColor3f(col,col,col);
        drawSquare(60);
    }glPopMatrix();


    int i ;
    for(i = 0; i < numBullets; i++){
        //*bullets on the target
        glPushMatrix();{
            glRotatef(90,0,1,0);
            glTranslatef(-bulletArray[i].z,bulletArray[i].y,bulletArray[i].x+1);
            double col = 1;
            glColor3f(col,0,0);
            drawSquare(2);
        }glPopMatrix();
    }
}


void shoot(){

    /** B Point*/
    backPartPosition.x = bL.x*backTotalLength;
    backPartPosition.y = bL.y*backTotalLength;
    backPartPosition.z = bL.z*backTotalLength;
    double t = (-targetDistance-backPartPosition.x)/fL.x;
    point shootPoint;
    shootPoint.x = backPartPosition.x+(t*fL.x);
    shootPoint.y = backPartPosition.y+(t*fL.y);
    shootPoint.z = backPartPosition.z+(t*fL.z);

    double boundary = 58;
    if (abs(shootPoint.y) <= boundary && abs(shootPoint.z) <= boundary){
        bulletArray[numBullets].x = shootPoint.x;
        bulletArray[numBullets].y = shootPoint.y;
        bulletArray[numBullets].z = shootPoint.z;
        numBullets+=1;
    }
    printf("Shoot coordinate -->   %lf %lf %lf  ------- %d\n", shootPoint.x,shootPoint.y,shootPoint.z, numBullets);
}






void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			lookLeft();
			break;

        case '2':
			lookRight();
			break;

        case '3':
            lookUp();
			break;

        case '4':
            lookDown();
			break;

        case '5':
            tiltClockwise();
			break;

        case '6':
            tiltCounterClockwise();
			break;

        case 'q':
            gunBackGoLeft();

            break;
        case 'w':
            gunBackGoRight();
            break;
        case 'r':
            gunBackGoDown();
            break;
        case 'e':
            gunBackGoUp();
            break;
        case 'a':
            gunFrontGoUp();
            break;
        case 's':
            gunFrontGoDown();
            break;
        case 'd':
            frontRollClockwise();
            break;
        case 'f':
            frontRollAntiClockwise();
            break;
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
            currentPosition.x-= lVector.x;
            currentPosition.y-= lVector.y;
            currentPosition.z-= lVector.z;
			break;
		case GLUT_KEY_UP:		// up arrow key
            currentPosition.x+= lVector.x;
            currentPosition.y+= lVector.y;
            currentPosition.z+= lVector.z;
			break;

		case GLUT_KEY_RIGHT:
            currentPosition.x+= rVector.x;
            currentPosition.y+= rVector.y;
            currentPosition.z+= rVector.z;
			break;
		case GLUT_KEY_LEFT:
            currentPosition.x-= rVector.x;
            currentPosition.y-= rVector.y;
            currentPosition.z-= rVector.z;
			break;

		case GLUT_KEY_PAGE_UP:
		    currentPosition.x+= uVector.x;
            currentPosition.y+= uVector.y;
            currentPosition.z+= uVector.z;
			break;
		case GLUT_KEY_PAGE_DOWN:
		    currentPosition.x-= uVector.x;
            currentPosition.y-= uVector.y;
            currentPosition.z-= uVector.z;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			shoot();
			break;

		case GLUT_RIGHT_BUTTON:

			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	gluLookAt(currentPosition.x, currentPosition.y, currentPosition.z,
           currentPosition.x+lVector.x,currentPosition.y+lVector.y,currentPosition.z+lVector.z,	uVector.x,uVector.y,uVector.z);
//	gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
//	gluLookAt(50, 200,cameraHeight,	0,0,0,	0,0,1);

//    gluLookAt(viewX,viewY,viewZ,	0,0,0,	0,1,0);
	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
    drawGun();
//    glRotatef(20, 0, 0, 1);
//    glTranslatef(0,30,0);
//
//    drawSphere(10, 10, 10);



	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=200.0;
	cameraAngle=1.0;
	angle=0;


    rotationAngle = 0.03;
    targetDistance = 305;

    /**
    current position vector
    and u,r,l vectors
    */
//    // side view
//    currentPosition.x = -40;
//    currentPosition.y = -200;
//    currentPosition.z = 10;
//    uVector.x = 0, uVector.y = 0, uVector.z = 1;
//    rVector.x =1, rVector.y = 0, rVector.z = 0;
//    lVector.x = 0, lVector.y = 1, lVector.z = 0;


    // back view
    currentPosition.x = 100;
    currentPosition.y = 10;
    currentPosition.z = 10;
    uVector.x = 0, uVector.y = 0, uVector.z = 1;
    rVector.x = 0, rVector.y = 1, rVector.z = 0;
    lVector.x = -1, lVector.y = 0, lVector.z = 0;


    // gun direction vectors
    dirR.x = 0;
    dirR.y = 1;
    dirR.z = 0;
    dirL.x = -1;
    dirL.y = 0;
    dirL.z = 0;
    dirU.x = 0;
    dirU.y = 0;
    dirU.z = 1;



    bU.x=0,bU.y=0,bU.z=1;
    bR.x=0,bR.y=1,bR.z=0;
    bL.x=-1,bL.y=0,bL.z=0;
    fU.x=0,fU.y=0,fU.z=1;
    fR.x=0,fR.y=1,fR.z=0;
    fL.x=-1,fL.y=0,fL.z=0;







    gunBackRadius = 10;
    gunBackHeight = 40;
    gunBackSlices = 30;
    gunFrontRadius = 6;
    gunFrontHeight = 80;
    gunFrontSlices = 30;
    gunPosition.x = -gunBackRadius, gunPosition.y = 0, gunPosition.z = 0;
    gunBackL.x = -1, gunBackL.y = 0, gunBackL.z = 0;



    point1.x = -(2* gunBackRadius  + gunBackHeight);
    backTotalLength = abs(point1.x);
    frontTotalLength= abs(2* gunFrontRadius+ gunFrontHeight) ;
    point1.y = 0;
    point1.z = 0;
    point2.x = point1.x - (2* gunFrontRadius+ gunFrontHeight);
    point2.y = 0;
    point2.z = 0;



	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Gun");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
