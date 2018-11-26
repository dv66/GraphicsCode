#include <bits/stdc++.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))

using namespace std;



struct point2d
{
    double x, y;
};

void drawTriangle(point2d a, point2d b, point2d c, double red, double green , double blue);



struct Arrow{
    point2d startPoint;
    point2d endPoint;
    point2d arrowRightVertex;
    point2d arrowLeftVertex;

    void draw(){
        glPushMatrix();{
            drawTriangle(endPoint, arrowRightVertex, arrowLeftVertex, 0.8,0,0);
            glColor3f(1.0,1.0,1.0);
            glBegin(GL_LINES);{
                    glVertex3f(startPoint.x, startPoint.y, 0);
                    glVertex3f(endPoint.x, endPoint.y, 0);
            }glEnd();
        }
        glPopMatrix();
    }
};


struct Curve{
    vector<point2d> connectionPoints;
    void draw(){
        for (int i=1; i < connectionPoints.size(); i++){
            glBegin(GL_LINES);{
                glVertex3f(connectionPoints[i-1].x, connectionPoints[i-1].y, 0);
                glVertex3f(connectionPoints[i].x, connectionPoints[i].y, 0);
            }glEnd();
        }
    }
};





/**
Global Variables
*/
const double windowWidth = 1200;
const double windowHeight = 800;
const double arrowAngle = 0.15;

vector<point2d> cp;
vector<Arrow> arrows;
vector<Curve> arcs;
vector<point2d> movingPointTrack;

// 0 for add point, 1 for draw loop
int drawState;
int isEvenNumbersOfPointsAdded;
int totalPoints;
bool movingPointOn;
int movingPointSpeed = 1;
int pos;
















int cpidx;

void drawSquare()
{
    glBegin(GL_QUADS);
    {
        glVertex3d( 3,  3, 0);
        glVertex3d( 3, -3, 0);
        glVertex3d(-3, -3, 0);
        glVertex3d(-3,  3, 0);
    }
    glEnd();
}



void drawCircle(double radius,int segments, double red, double green, double blue){
    vector<point2d> points;
    glColor3f(red, green, blue);
    for(int i=0;i<=segments;i++){
        point2d newPoint;
        points.push_back(newPoint);
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(int i=0;i<segments;i++){
        point2d origin; origin.x=0;origin.y=0;
        drawTriangle(points[i],points[i+1], origin, red, green, blue);
    }
}


void drawTriangle(point2d a, point2d b, point2d c, double red, double green , double blue){
    glBegin(GL_TRIANGLES);
        glColor3f(red,green,blue);
        glVertex2f(a.x, a.y);
        glVertex2f(b.x, b.y);
        glVertex2f(c.x, c.y);
    glEnd();
}

point2d clockWiseRotation(point2d vec){
    double v_perp_X = vec.y;
    double v_perp_Y = -vec.x;
    point2d newVec;
    newVec.x = (vec.x*cos(arrowAngle))+(v_perp_X*sin(arrowAngle));
    newVec.y = (vec.y*cos(arrowAngle))+(v_perp_Y*sin(arrowAngle));
    return newVec;
}


point2d counterClockWiseRotation(point2d vec){
    double v_perp_X = -vec.y;
    double v_perp_Y = vec.x;
    point2d newVec;
    newVec.x = (vec.x*cos(arrowAngle))+(v_perp_X*sin(arrowAngle));
    newVec.y = (vec.y*cos(arrowAngle))+(v_perp_Y*sin(arrowAngle));
    return newVec;
}



void generateArrowParameters(point2d p, point2d q){
    point2d directionVector;
    directionVector.x = p.x - q.x ;
    directionVector.y = p.y - q.y ;
    double magnitude = sqrt((directionVector.x*directionVector.x) + (directionVector.y*directionVector.y));
    directionVector.x /= magnitude;
    directionVector.y /= magnitude;
    point2d arrowRightVertex = clockWiseRotation(directionVector);
    point2d arrowLeftVertex = counterClockWiseRotation(directionVector);
    point2d s;
    point2d t;
    const double scaleFactor = 40;
    s.x = q.x + scaleFactor*arrowRightVertex.x; s.y = q.y + scaleFactor*arrowRightVertex.y;
    t.x = q.x + scaleFactor*arrowLeftVertex.x; t.y = q.y + scaleFactor*arrowLeftVertex.y;
    Arrow newArrow;
    newArrow.startPoint = p;
    newArrow.endPoint = q;
    newArrow.arrowRightVertex = s;
    newArrow.arrowLeftVertex = t;
    arrows.push_back(newArrow);
}




void generateHermiteCurveParameters(point2d p1, point2d r1, point2d p4, point2d r4){
    double ax, ay, bx, by, cx, cy, dx, dy;
    point2d r1Vector;
    point2d r4Vector;
    r1Vector.x = r1.x-p1.x; r1Vector.y = r1.y-p1.y;
    r4Vector.x = r4.x-p4.x; r4Vector.y = r4.y-p4.y;
    ax = 2*p1.x - 2*p4.x + r1Vector.x + r4Vector.x;
    bx = (-3)*p1.x + 3*p4.x -2*r1Vector.x -1*r4Vector.x;
    cx = r1Vector.x;
    dx = p1.x;
    ay = 2*p1.y - 2*p4.y + r1Vector.y + r4Vector.y;
    by = (-3)*p1.y + 3*p4.y -2*r1Vector.y -1*r4Vector.y;
    cy = r1Vector.y;
    dy = p1.y;
    double t = 0;
    vector<point2d> arcPoints;

    /** this part has to be changed
        forward difference method must be used
    */
    while(t <= 1){
        double X = ax*(t*t*t) + bx*(t*t) + cx*t + dx;
        double Y = ay*(t*t*t) + by*(t*t) + cy*t + dy;
        point2d newPoint; newPoint.x = X; newPoint.y =Y;
        arcPoints.push_back(newPoint);
        t += 0.01;
    }
    Curve newArc; newArc.connectionPoints = arcPoints;
    arcs.push_back(newArc);
}








void movePointOnTheLoop(){
    for (int i = 0 ; i < arcs.size(); i++){
        for (int j = 0; j < arcs[i].connectionPoints.size(); j++){
            point2d P;
            P.x = arcs[i].connectionPoints[j].x;
            P.y = arcs[i].connectionPoints[j].y;
            movingPointTrack.push_back(P);
        }
    }
}




void drawMovingPoint(point2d cur){
    glPushMatrix();{
        glTranslatef(cur.x, cur.y, 0);
        drawCircle(5, 6, 0,0.8,0.8);
    }glPopMatrix();
}






/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
















void keyboardListener(unsigned char key, int x,int y){
	switch(key){

        case 'a':
            pos = 0;
            movingPointTrack.clear();
            movePointOnTheLoop();
            if(movingPointTrack.size())
                movingPointOn ^= 1;
			break;

		default:
			break;
	}
}

void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			break;
		case GLUT_KEY_UP:		// up arrow key
			break;

		case GLUT_KEY_RIGHT:
			break;
		case GLUT_KEY_LEFT:
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
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
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP

                /** if point add mode is ON*/
                if (!drawState){
                    totalPoints++;
                    point2d newPoint;
                    newPoint.x = (double)x;
                    newPoint.y = (double)(windowHeight - y);
                    cp.push_back(newPoint);
                    if (totalPoints % 2 == 0){
                        generateArrowParameters(cp[totalPoints-2], cp[totalPoints-1]);
                        if (totalPoints >= 4) generateHermiteCurveParameters(cp[totalPoints-4],cp[totalPoints-3],cp[totalPoints-2],cp[totalPoints-1]);
                    }
                }

			}
			break;

		case GLUT_RIGHT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
                if (totalPoints%2 == 0 && totalPoints>=4 && !drawState){
                    generateHermiteCurveParameters(cp[totalPoints-2],cp[totalPoints-1],cp[0],cp[1]);
                    drawState = 1;
                }
            }
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

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(150*cos(cameraAngle), 150*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0,0,0,	0,0,-1,	0,1,0);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


    /** Draw Points */
    for (int i = 0; i < cp.size(); i++){
        if (i%2)  glColor3f(1, 1, 0);
        else glColor3f(0, 1, 0);
        glPushMatrix();{
            glTranslatef(cp[i].x, cp[i].y, 0);
            drawSquare();
        }glPopMatrix();
    }




    /** Draw arrows*/
    for (int i = 0 ; i < arrows.size(); i++){
        arrows[i].draw();
    }


    /** if drawState is ON */
    if(drawState){
        /** Draw arcs*/
        for (int i = 0 ; i < arcs.size(); i++){
            arcs[i].draw();
        }
    }

    if(movingPointOn){
        drawMovingPoint(movingPointTrack[pos]);
    }

//    drawCircle(double radius,int segments, double red, double green, double blue)

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){

    if (movingPointOn){
        if (pos < movingPointTrack.size()) {
            pos = pos + movingPointSpeed;
        }
        else pos = 0;
    }
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization


    drawState = 0;
    movingPointOn = 0;
    isEvenNumbersOfPointsAdded = 1;
    totalPoints = 0;
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
	gluOrtho2D(0, windowWidth, 0, windowHeight);
	//gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Curves");

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
