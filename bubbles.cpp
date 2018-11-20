#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))
#define ABS(x)  (((x)>0)? (x) : -(x))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;





struct point
{
	double x,y,z;
};



double boundaryX , boundaryY;
double smallRadius;
double largeRadius;

double speed;
double rotationAngle;

double v_perp_X ;
double v_perp_Y ;

double vec1X, vec1Y;
double vec2X, vec2Y;
double pos1X, pos1Y;
double pos2X, pos2Y;

double n_vectorX1, n_vectorX2;
double n_vectorY1, n_vectorY2;
double a_X1, a_X2;
double a_Y1, a_Y2;
double dotProduct_a_n1, dotProduct_a_n2;


double directionVectorMagnitude;
double radiusLineScaleFactor;


int colCount=0;


void drawCircle(double radius,int segments, double red, double green, double blue)
{
    int i;
    struct point points[100];
//    glColor3f(0.7,0.7,0.7);
    glColor3f(red, green, blue);
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

void drawTriangle(double x1, double x2, double y1, double y2, double z1, double z2){
    glBegin(GL_TRIANGLES);
        glColor3f(0.5,0,0);
        glVertex2f(x1,x2);
        glVertex2f(y1,y2);
        glVertex2f(z1,z2);
    glEnd();
}

void clockWiseRotation(){
    v_perp_X = vec1Y;
    v_perp_Y = -vec1X;
    vec1X = (vec1X*cos(rotationAngle))+(v_perp_X*sin(rotationAngle));
    vec1Y = (vec1Y*cos(rotationAngle))+(v_perp_Y*sin(rotationAngle));
}


void counterClockWiseRotation(){
    v_perp_X = -vec1Y;
    v_perp_Y = vec1X;
    vec1X = (vec1X*cos(rotationAngle))+(v_perp_X*sin(rotationAngle));
    vec1Y = (vec1Y*cos(rotationAngle))+(v_perp_Y*sin(rotationAngle));
}



void drawBubbles(){

    glPushMatrix();{
        glTranslatef(pos1X, pos1Y,0);
        drawCircle(smallRadius,20, 1, 100, 0);
        glColor3f(1,1,1);
        glBegin(GL_LINES);
            glVertex2f(0,0);
            double bal = 1;
            glVertex2f(bal*radiusLineScaleFactor*vec1X,bal*radiusLineScaleFactor*vec1Y);
        glEnd();
    }glPopMatrix();


    glPushMatrix();{
        glTranslatef(pos2X, pos2Y,0);
        drawCircle(smallRadius,20, 0, 1 , 0);
        glColor3f(1,1,1);
        glBegin(GL_LINES);
            glVertex2f(0,0);
            glVertex2f(radiusLineScaleFactor*vec2X,radiusLineScaleFactor*vec2Y);
        glEnd();
    }glPopMatrix();

//    drawTriangle(10,10,30,20,40,80);
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			printf("%lf \n", sqrt(vec1X*vec1X + vec1Y*vec1Y));
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
			clockWiseRotation();
			break;
		case GLUT_KEY_LEFT:
			counterClockWiseRotation();
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
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
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
	gluLookAt(0,0,80,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

    drawCircle(largeRadius,50, 1,1,1);
    drawBubbles();


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}
















void animate(){
    double distFromCenter1 = sqrt((pos1X*pos1X)+(pos1Y*pos1Y));
    double distFromCenter2 = sqrt((pos2X*pos2X)+(pos2Y*pos2Y));
    double distanceBetweenBalls = sqrt( (pos1X-pos2X)*(pos1X-pos2X) + (pos1Y-pos2Y)*(pos1Y-pos2Y));

    /**
    * if balls collide with each other
    */
    if(distanceBetweenBalls <= 2*smallRadius){
        double n_mag = sqrt((pos2X-pos1X)*(pos2X-pos1X) + (pos2Y-pos1Y)*(pos2Y-pos1Y));

        // reflection of ball_1
        n_vectorX1=(pos2X-pos1X)/n_mag;
        n_vectorY1=(pos2Y-pos1Y)/n_mag;
        a_X1 = vec1X;
        a_Y1 = vec1Y;
        dotProduct_a_n1 = ((a_X1*n_vectorX1)+(a_Y1*n_vectorY1))*2;
        vec1X = vec1X - dotProduct_a_n1*n_vectorX1;
        vec1Y  = vec1Y - dotProduct_a_n1*n_vectorY1;

        // reflection of ball_2
        n_vectorX2= -((pos2X-pos1X)/n_mag);
        n_vectorY2= -((pos2Y-pos1Y)/n_mag);
        a_X2 = vec2X;
        a_Y2 = vec2Y;
        dotProduct_a_n2 = ((a_X2*n_vectorX2)+(a_Y2*n_vectorY2))*2;
        vec2X = vec2X - dotProduct_a_n2*n_vectorX2;
        vec2Y  = vec2Y - dotProduct_a_n2*n_vectorY2;
//        printf("SPEED = %lf\n" , sqrt((vec1X*vec1X)+(vec1Y*vec1Y)));
    }


    /** if ball 1 touches boundary */
    if(distFromCenter1+smallRadius >= largeRadius) {
        // reflecting the vector (vec_X, vec_y) about the normal unit vector at the touch point
        // reflected vector r = a_vec - 2*(dotProduct(a_vec,n_vec))n_vec
        n_vectorX1 = -pos1X/distFromCenter1;
        n_vectorY1 = -pos1Y/distFromCenter1;
        a_X1 = vec1X;
        a_Y1 = vec1Y;
        dotProduct_a_n1 = 2*((a_X1*n_vectorX1)+(a_Y1*n_vectorY1));
        vec1X = vec1X - dotProduct_a_n1*n_vectorX1;
        vec1Y  = vec1Y - dotProduct_a_n1*n_vectorY1;
//        printf("SPEED = %lf\n" , sqrt((vec1X*vec1X)+(vec1Y*vec1Y)));
    }

    /** if ball 2 touches boundary */
    if(distFromCenter2+smallRadius >= largeRadius) {
        n_vectorX2 = -pos2X/distFromCenter2;
        n_vectorY2 = -pos2Y/distFromCenter2;
        a_X2 = vec2X;
        a_Y2 = vec2Y;
        dotProduct_a_n2 = 2*((a_X2*n_vectorX2)+(a_Y2*n_vectorY2));
        vec2X = vec2X - dotProduct_a_n2*n_vectorX2;
        vec2Y  = vec2Y - dotProduct_a_n2*n_vectorY2;
//        printf("SPEED = %lf\n" , sqrt((vec1X*vec1X)+(vec1Y*vec1Y)));
    }

	pos1X += vec1X;
	pos1Y += vec1Y;
	pos2X += vec2X;
	pos2Y += vec2Y;


	//codes for any changes in Models, Camera
	glutPostRedisplay();
}
















void init(){
    /** tunable parameters */
    smallRadius = 5;
    largeRadius = 50;
    speed = 0.5;
    rotationAngle = 0.08;


    pos1X = 0;
    pos1Y = 0;
    pos2X = smallRadius+5;
    pos2Y = smallRadius+5;


	vec1X = 0.1;
    vec1Y = sqrt((speed*speed) - (vec1X*vec1X));
    vec2X = 0.1;
    vec2Y = sqrt((speed*speed) - (vec2X*vec2X));

    directionVectorMagnitude = sqrt(vec1X*vec1X + vec1Y*vec1Y);
    radiusLineScaleFactor = smallRadius/directionVectorMagnitude;
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
	gluPerspective(100,	1,	1,	1000.0);
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

	glutCreateWindow("Bubbles");

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
