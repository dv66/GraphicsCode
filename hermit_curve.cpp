#include <bits/stdc++.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))

using namespace std;

enum modes{
    UPDATE, EDIT
};


struct point2d
{
    double x, y;
    bool operator ==(const point2d& p) const{
        return x==p.x && y==p.y;
    }
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
int updateModeClickCount;
int updatePointPositionIndex;
int updateArrowIndex;

int mode;












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
    glColor3f(red, green, blue);
    vector<point2d> circlePoints;
    for(int i=0;i<=segments;i++){
        point2d newPoint;
        circlePoints.push_back(newPoint);
        circlePoints[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        circlePoints[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(int i=0;i<segments;i++){
        point2d origin; origin.x=0;origin.y=0;
        drawTriangle(circlePoints[i],circlePoints[i+1], origin, red, green, blue);
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



void generateArrowParameters(point2d p, point2d q, int pos=-1){
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
    if (pos==-1) arrows.push_back(newArrow);
    else{
        arrows[pos] = newArrow;
    }
}


void printVector(vector<double>& v){
    for(int i = 0 ; i <v.size(); i++){
        cout << v[i]<<" ";
    }
    cout << "\n-------------------------------------------------------------------"<<endl;
}

void printVector(double* v){
    for(int i = 0 ; i <4; i++){
        cout << v[i]<<" ";
    }
    cout << "\n-------------------------------------------------------------------"<<endl;
}

vector<double> forwardDifference(double a, double b , double c, double d){
    int NEED = 101;
    double inc = 1/((double)NEED-1);
    double T[4];
    T[0]=0.0; T[1]=T[0]+inc; T[2]=T[1]+inc; T[3]=T[2]+inc;
    double* X = new double[NEED];
    double* X_2 = new double[NEED];
    double* X_3 = new double[NEED];
    double* X_4 = new double[NEED];
    for(int i = 0; i < 4; i++){
        double val = a*(T[i]*T[i]*T[i])+b*(T[i]*T[i])+c*(T[i])+d;
        X[i] = val;
    }
    for (int i = 1,k=0 ; i <4; i++,k++){
        X_2[k]=(X[i]-X[i-1]);
    }
    for (int i = 1,k=0 ; i <3; i++,k++){
        X_3[k]=(X_2[i]-X_2[i-1]);
    }
    for (int i = 1,k=0 ; i <2; i++,k++){
        X_4[k]=(X_3[i]-X_3[i-1]);
    }
    /***************************************************************/
    for(int i = 2; i<NEED; i++) X_3[i]=(X_3[i-1]+X_4[0]);
    for(int i = 3; i<NEED; i++) X_2[i]=(X_2[i-1] + X_3[i-1]);
    for(int i = 4; i<NEED; i++) X[i] = (X[i-1] + X_2[i-1]);
    vector<double> newVector;
    for(int i = 0 ; i<NEED ; i++) newVector.push_back(X[i]);
    return newVector;
}



void generateHermiteCurveParameters(){
    point2d p1, r1, p4, r4;
    int arrLen = arrows.size();
    arcs.clear();
    for (int i = 0 ; i <arrLen ; i++){
        p1 = arrows[i].startPoint;
        r1 = arrows[i].endPoint;
        p4 = arrows[(i+1)%arrLen].startPoint;
        r4 = arrows[(i+1)%arrLen].endPoint;
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

        /**
            forward difference method
        */
        vector<double> forwardX = forwardDifference(ax,bx,cx,dx);
        vector<double> forwardY = forwardDifference(ay,by,cy,dy);
        for(int i = 0 ; i<forwardX.size(); i++){
            point2d newPoint; newPoint.x = forwardX[i]; newPoint.y =forwardY[i];
            arcPoints.push_back(newPoint);
        }
        Curve newArc; newArc.connectionPoints = arcPoints;
        arcs.push_back(newArc);
    }
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




void drawMovingPoint(point2d cur, double radius=5, double segments=6){
    glPushMatrix();{
        glTranslatef(cur.x, cur.y, 0);
        drawCircle(radius,segments, 0,0.8,0.8);
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
            if(drawState && mode == EDIT){
                pos = 0;
                movingPointTrack.clear();
                movePointOnTheLoop();
                if(movingPointTrack.size())
                    movingPointOn ^= 1;
            }
			break;

        case 'u':
            /** if point is moving on track then stop it and take it to its initial state*/
            movingPointOn = 0;
            mode = UPDATE;
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
                double clickX = (double)x;
                double clickY = (double)(windowHeight - y);
                /** if point add mode is ON*/
                if (!drawState && mode == EDIT){
                    totalPoints++;
                    point2d newPoint;
                    newPoint.x = clickX;
                    newPoint.y = clickY;
                    cp.push_back(newPoint);
                    if (totalPoints % 2 == 0){
                        generateArrowParameters(cp[totalPoints-2], cp[totalPoints-1]);
                    }
                }

                /** if UPDATE mode is ON*/
                else if(mode == UPDATE){
                    if(updateModeClickCount == 0){
                        double dis = 1000000;
                        for(int i = 0 ; i < cp.size(); i++){
                            double d = sqrt((cp[i].x-clickX)*(cp[i].x-clickX) + (cp[i].y-clickY)*(cp[i].y-clickY));
                            if(d < dis){
                                dis = d;
                                updatePointPositionIndex = i;
                            }
                        }
                        updateModeClickCount+=1;
                    }
                    else{
                        int arrowIndex;
                        point2d clickedPoint; clickedPoint.x = clickX; clickedPoint.y = clickY;
                        for(int i = 0 ; i < arrows.size(); i++){
                            if(arrows[i].startPoint == cp[updatePointPositionIndex]){
                                generateArrowParameters(clickedPoint, arrows[i].endPoint, i);
                                break;
                            }
                            if(arrows[i].endPoint == cp[updatePointPositionIndex]){
                                generateArrowParameters(arrows[i].startPoint, clickedPoint, i);
                                break;
                            }
                        }
                        cp[updatePointPositionIndex].x = clickX;
                        cp[updatePointPositionIndex].y = clickY;
                        updateModeClickCount = 0;
                        generateHermiteCurveParameters();
                        mode = EDIT;
                    }
                }

			}
			break;

		case GLUT_RIGHT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
			    movingPointOn = 0;
                if (totalPoints%2 == 0 && totalPoints>=4 && !drawState){
                    generateHermiteCurveParameters();
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



    /** if EDIT mode is ON an movingpointstate is ON*/
    if(movingPointOn && mode == EDIT){
        drawMovingPoint(movingPointTrack[pos]);
    }


    /** Draw Circle on the Point to be Updated */
	if(mode == UPDATE){
        if(updateModeClickCount == 1){
            drawMovingPoint(cp[updatePointPositionIndex],10,10);
        }
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
    mode = EDIT;
    updateModeClickCount = 0;
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
