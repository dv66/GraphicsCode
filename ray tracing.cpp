#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <bits/stdc++.h>
#include <windows.h>
#include <glut.h>
#include "bitmap_image.hpp"
#define pi (2*acos(0.0))
#define LIMIT 20
using namespace std;

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double targetDistance;
double rotationAngle;
double backTotalLength;
double frontTotalLength;
double movingSpeed;
double incX;
double incY;






/**
ray-tracing assignment variables
*/

double nearDistance;
double farDistance;
double fovY;
double aspectRatio;
double levelOfRecursion;
double numPixels;
double checkerboarchCellWidth;
double ambientCheckerboard, diffuseCheckerboard, reflectionCheckerboard;
int numObjects;
int screen_x, screen_y;
int numNormalLightSources;
int numSpotLightSources;





















struct point
{
	double x,y,z;

	point(double xx, double yy, double zz){
        x=xx; y=yy; z=zz;
	}

	point(){}

	point operator+(const point& p) const{
	    point tp;
	    tp.x = x+p.x;
	    tp.y = y+p.y;
	    tp.z = z+p.z;
        return tp;
	}

	point operator-(const point& p) const{
	    point tp;
	    tp.x = x-p.x;
	    tp.y = y-p.y;
	    tp.z = z-p.z;
        return tp;
	}


	void normalize(){
        double magnitude = sqrt(x*x + y*y+z*z);
        x = x/magnitude;
        y = y/magnitude;
        z = z/magnitude;
	}


};

double dot(point p, point q){
        return q.x*p.x + q.y*p.y + q.z*p.z;
}

point scale(double t, point v){
    point T;
    T.x = t*v.x; T.y = t*v.y;T.z = t*v.z;
    return T;
}

point cross(point p, point q){
    point T;
    T.x = p.y*q.z - p.z*q.y;
    T.y = -(p.x*q.z - q.x*p.z);
    T.z = p.x*q.y - p.y*q.x;
    return T;
}

double magnitude(point a){
    return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

struct Color{
    double r, g, b;
    Color(){}
    Color(double rr, double gg, double bb){
        r = rr;
        g = gg;
        b = bb;
    }
};


point bU;
point bR;
point bL;
point fU;
point fR;
point fL;


class SpotLight{
public:
    point pos;
    point lookingAt;
    double cutoffAngle; /// in degrees
    double falloffParam;
    SpotLight(){}
    SpotLight(point p, double fall,point look , double cut){ pos=p; falloffParam=fall;lookingAt=look; cutoffAngle=cut; }

};

class NormalLight{
    public:

    point pos;
    double falloffParam;
    NormalLight(){}
    NormalLight(point p, double fall){ pos=p; falloffParam=fall; }
};








point currentPosition;
point uVector;
point rVector;
point lVector;


class Sphere{
public:
    point center;
    double radius;
    Color color;
    double amb, dif, spec, reflect;
    double shininess;
    Sphere(point cent, double r, Color c, double a, double d, double sp, double refl, double shiny){
        center = cent;
        radius = r;
        color = c;
        amb = a;
        dif = d;
        spec = sp;
        reflect = refl;
        shininess = shiny;
    }

    void print_vals(){
        cout << center.x << " "<<center.y << " "<<center.z << " " << radius << " " <<color.r <<" " << color.g<<" " <<color.b
        <<" " <<amb <<" " << dif<<" " << spec<<" " << reflect<<" " << shininess<<endl;
    }

    void drawSphere(int slices=50,int stacks=20){
        struct point points[100][100];
        int i,j;
        double h,r;
        glPushMatrix();
        glTranslatef(center.x, center.y, center.z);
        glColor3f(color.r, color.g, color.b);
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
//            glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
            for(j=0;j<slices;j++)
            {
//                    glColor3f((double)i/(double)slices,(double)i/(double)slices,(double)i/(double)slices);
                glBegin(GL_QUADS);{
                    //upper hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
    //                //lower hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
    				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
    				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
    				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
                }glEnd();
            }
        }
        glPopMatrix();
    }
};




void drawTriangle(point p, point m, point n, double r, double g, double b){
    glColor3f(r,g,b);
	glBegin(GL_TRIANGLES);{
		glVertex3f( p.x, p.y,p.z);
		glVertex3f( m.x, m.y,m.z);
		glVertex3f( n.x, n.y,n.z);
	}glEnd();
}
void drawSquare(double a, double r, double g, double b)
{
    glColor3f(r,g,b);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}

struct TrianglularPlane{
    point p;
    point q;
    point r;
    Color c;
    void drawPlane(double rr, double g, double b){
        drawTriangle(p,q,r,rr,g,b);
    }
};


class Pyramid{
public:
    point lowestPoint;
    double width, height;
    Color color;
    double amb, dif, spec, reflect;
    double shininess;
    point peakPoint;
    TrianglularPlane planes[4];


    Pyramid(point low, double w, double h,Color c, double a, double d, double sp, double refl, double shiny){
        lowestPoint = low;
        width = w;
        height = h;
        color = c;
        amb = a;
        dif = d;
        spec = sp;
        reflect = refl;
        shininess = shiny;
        peakPoint = lowestPoint;
        peakPoint.z += h;
        point cornerPoint1, cornerPoint2,cornerPoint3, cornerPoint4;
        cornerPoint1.x=lowestPoint.x+(width/2), cornerPoint1.y = lowestPoint.y+(width/2), cornerPoint1.z = lowestPoint.z;
        cornerPoint2.x=lowestPoint.x+(width/2), cornerPoint2.y = lowestPoint.y-(width/2), cornerPoint2.z = lowestPoint.z;
        cornerPoint3.x=lowestPoint.x-(width/2), cornerPoint3.y = lowestPoint.y-(width/2), cornerPoint3.z = lowestPoint.z;
        cornerPoint4.x=lowestPoint.x-(width/2), cornerPoint4.y = lowestPoint.y+(width/2), cornerPoint4.z = lowestPoint.z;
        planes[0].p = peakPoint;planes[0].q =cornerPoint1 ;planes[0].r = cornerPoint2;
        planes[1].p = peakPoint;planes[1].q =cornerPoint2 ;planes[1].r = cornerPoint3;
        planes[2].p = peakPoint;planes[2].q =cornerPoint3 ;planes[2].r = cornerPoint4;
        planes[3].p = peakPoint;planes[3].q =cornerPoint4 ;planes[3].r = cornerPoint1;
    }

    void print_vals(){
        cout << lowestPoint.x << " "<<lowestPoint.y << " "<<lowestPoint.z << " " << width << " " << height
        <<" " <<color.r <<" " << color.g<<" " <<color.b
        <<" " <<amb <<" " << dif<<" " << spec<<" " << reflect<<" " << shininess<<endl;
    }

    void drawPyramid(){
        glPushMatrix();
            glTranslatef(lowestPoint.x,lowestPoint.y,lowestPoint.z);
            drawSquare(width/2,color.r, color.g,color.b);
            glPopMatrix();
            for(int i=0; i <4; i++){
                planes[i].drawPlane(color.r, color.g,color.b);
            }

    }
};



vector<Sphere> spheres;
vector<Pyramid> pyramids;
vector<NormalLight> normalLights;
vector<SpotLight> spotLights;


void drawAxes()
{
		glColor3f(1.0, 0.0, 0.0);
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






void drawCheckerboard(){
    int val = 1;

    /**
    can change this value of boardSize to an EVEN number (always EVEN)
    center cell is always white
    */
    int boardSize = 30;




    for(int k = -boardSize; k <= boardSize ; k++){
        glPushMatrix();
        glTranslatef(-boardSize*2*checkerboarchCellWidth, checkerboarchCellWidth*2*k, 0);
        for(int i = 0; i <2*boardSize; i++){
            drawSquare(checkerboarchCellWidth, (double)val, (double)val ,(double) val) ;
            glTranslatef(checkerboarchCellWidth*2, 0, 0);
            val ^=1;
        }
        glPopMatrix();
        val ^= 1;
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



double determinant(double m[3][3]){
    return m[0][0]*(m[1][1]*m[2][2] - m[1][2]*m[2][1]) - m[0][1]*(m[1][0]*m[2][2] - m[1][2]*m[2][0])
           + m[0][2]*(m[1][0]*m[2][1] - m[2][0]*m[1][1]);
}



double getTriangleArea(point a, point b, point c){
    point B = b-a;
    point C = c-a;
    point T = cross(B,C);
    return 0.5*magnitude(T);
}

bool checkInsideTriangle(TrianglularPlane plane, point x){
    double tolerance = 0.1;
    double area1 = getTriangleArea(plane.p,plane.q,plane.r);
    double area2 = getTriangleArea(plane.p,plane.q,x);
    double area3 = getTriangleArea(plane.p,plane.r,x);
    double area4 = getTriangleArea(plane.q,plane.r,x);
    return abs(area2+area3+area4 - area1) <tolerance;
}



Color getCheckerboardCellColor(point intersectionPoint){
   int dx = ((int)ceil(abs(intersectionPoint.x)/checkerboarchCellWidth)) / 2;
   int dy = ((int)ceil(abs(intersectionPoint.y)/checkerboarchCellWidth)) / 2;
   Color c(1,1,1);
   if((dx%2 && dy%2) || (dx%2==0 && dy%2==0)) return c;
   else {
        c.r=0,c.g=0,c.b=0; return c;
   }
}


Color getColor(point start){
    point direction = start - currentPosition;
    direction.normalize();

    /**************** check spheres ****************/
    double t = 100000;
    Color col(0,0,0);
    for(int i=0; i<spheres.size(); i++){
        Sphere s = spheres[i];
        double a = 1;
        double b = 2 * dot(start-s.center, direction);
        double c = dot(start-s.center, start-s.center) - s.radius * s.radius;
        if (b*b - 4*a*c >=0 ){
            double d = sqrt(b*b - 4*a*c);
            double t1 = -(b+d)/(2*a);
            double t2 = -(b-d)/(2*a);
            if(t1>=0 && t1 < t){
                t = t1; col=s.color;
            }
            if(t2>=0 && t2 < t){
                t = t2; col=s.color;
            }
        }
    }




    /**************** check pyramid **********************/

    for(int i=0; i<pyramids.size(); i++){
        for(int j=0; j<4; j++){
            TrianglularPlane plane = pyramids[i].planes[j];
            /*** check intersection ***/
            point C = plane.p;
            point A = plane.q - plane.p;
            point B = plane.r - plane.p;
            point P = start;
            point V = direction;
            double top[3][3]={{C.x-P.x,-A.x,-B.x},{C.y-P.y,-A.y,-B.y},{C.z-P.z,-A.z,-B.z}};
            double bottom[3][3] = {{V.x, -A.x, -B.x},{V.y, -A.y, -B.y},{V.z, -A.z, -B.z}};
            double den = determinant(bottom);
            if (den == 0) continue;
            double tt = determinant(top)/den;
            if(tt < 0) continue;
            point intersectionPoint = P + scale(tt, V);
            bool check = checkInsideTriangle(plane, intersectionPoint);
            if(check && tt < t){
                t = tt;
                col = pyramids[i].color;
            }
        }
    }






    /**************** check checkerboard **********************/
    /*** check intersection ***/
    point C(0,0,0);
    point A(1,0,0);
    point B(0,1,0);
    point P = start;
    point V = direction;
    double top[3][3]={{C.x-P.x,-A.x,-B.x},{C.y-P.y,-A.y,-B.y},{C.z-P.z,-A.z,-B.z}};
    double bottom[3][3] = {{V.x, -A.x, -B.x},{V.y, -A.y, -B.y},{V.z, -A.z, -B.z}};
    double den = determinant(bottom);
    double tt = determinant(top)/den;
    if (den && tt >=0){
        point intersectionPoint = P + scale(tt, V);
        if(tt < t ){
            t = tt;
            col = getCheckerboardCellColor(intersectionPoint);
        }
    }






//    if(magnitude(start +scale(t, direction)) > farDistance){
//        col.r=0;col.g=0;col.b=0;
//    }
    return col;
}



void createBitmapImage(){
    double frameHeight = 2 * nearDistance * tan((fovY/2)*(pi/180));
    double fovX = fovY * aspectRatio;
    double frameWidth = 2 * nearDistance * tan((fovX/2)*(pi/180));
    point midPoint;
    midPoint.x = currentPosition.x + nearDistance*lVector.x;
    midPoint.y = currentPosition.y + nearDistance*lVector.y;
    midPoint.z = currentPosition.z + nearDistance*lVector.z;
    incX = frameWidth/screen_x;
    incY = frameHeight/screen_y;

    point leftTop;
    leftTop.x = midPoint.x - rVector.x*(frameWidth/2) +  uVector.x*(frameHeight/2);
    leftTop.y = midPoint.y - rVector.y*(frameWidth/2) +  uVector.y*(frameHeight/2);
    leftTop.z = midPoint.z - rVector.z*(frameWidth/2) +  uVector.z*(frameHeight/2);

/************************ point buffer array construct *******************************************************************/
    point** pointBuffer;
    pointBuffer = new point* [screen_x];
    for(int i=0; i<screen_x; i++){
        pointBuffer[i] = new point[screen_y];
    }
    pointBuffer[0][0] = leftTop;
    for(int i=1; i<screen_x; i++){
        pointBuffer[i][0].x = pointBuffer[i-1][0].x - uVector.x*incY;
        pointBuffer[i][0].y = pointBuffer[i-1][0].y - uVector.y*incY;
        pointBuffer[i][0].z = pointBuffer[i-1][0].z - uVector.z*incY;
    }

    for(int i=0; i<screen_x ; i++){
        for(int j=1; j < screen_y; j++){
            pointBuffer[i][j].x = pointBuffer[i][j-1].x + rVector.x*incX;
            pointBuffer[i][j].y = pointBuffer[i][j-1].y + rVector.y*incX;
            pointBuffer[i][j].z = pointBuffer[i][j-1].z + rVector.z*incX;
        }
    }
/*******************************************************************************************/


    Color** pixels = new Color*[screen_x];
    Color backgroud(0,0,0);
    for (int i = 0; i < screen_x; i++) {
        pixels[i] = new Color [screen_y];
        for (int j = 0; j < screen_y; j++) {
            pixels[i][j] = getColor(pointBuffer[i][j]);
        }
    }

     // the following code generates a bmp image. do not change this.
    bitmap_image image(screen_x, screen_y);
    for (int x = 0; x < screen_x; x++) {
        for (int y = 0; y < screen_y; y++) {
            image.set_pixel(x, y, pixels[y][x].r*255, pixels[y][x].g*255, pixels[y][x].b*255);
        }
    }
    image.save_image("out.bmp");
    cout << "DONE" << endl;
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
        case '0':
            createBitmapImage();
			break;


		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
            currentPosition.x-= lVector.x*movingSpeed;
            currentPosition.y-= lVector.y*movingSpeed;
            currentPosition.z-= lVector.z*movingSpeed;
			break;
		case GLUT_KEY_UP:		// up arrow key
            currentPosition.x+= lVector.x*movingSpeed;
            currentPosition.y+= lVector.y*movingSpeed;
            currentPosition.z+= lVector.z*movingSpeed;
			break;

		case GLUT_KEY_RIGHT:
            currentPosition.x+= rVector.x*movingSpeed;
            currentPosition.y+= rVector.y*movingSpeed;
            currentPosition.z+= rVector.z*movingSpeed;
			break;
		case GLUT_KEY_LEFT:
            currentPosition.x-= rVector.x*movingSpeed;
            currentPosition.y-= rVector.y*movingSpeed;
            currentPosition.z-= rVector.z*movingSpeed;
			break;

		case GLUT_KEY_PAGE_UP:
		    currentPosition.x+= uVector.x*movingSpeed;
            currentPosition.y+= uVector.y*movingSpeed;
            currentPosition.z+= uVector.z*movingSpeed;
			break;
		case GLUT_KEY_PAGE_DOWN:
		    currentPosition.x-= uVector.x*movingSpeed;
            currentPosition.y-= uVector.y*movingSpeed;
            currentPosition.z-= uVector.z*movingSpeed;
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
//			shoot();
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

	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();


	gluLookAt(currentPosition.x, currentPosition.y, currentPosition.z,
           currentPosition.x+lVector.x,currentPosition.y+lVector.y,currentPosition.z+lVector.z,	uVector.x,uVector.y,uVector.z);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects
//    drawAxes();
    drawCheckerboard();
    for(int i=0; i<spheres.size(); i++) spheres[i].drawSphere();
    for(int i=0; i<pyramids.size(); i++) pyramids[i].drawPyramid();


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
    movingSpeed = 2.0;

    rotationAngle = 0.03;
    targetDistance = 305;

    screen_x = 500;
    screen_y = 500;

    /**
    current position vector
    and u,r,l vectors
    */
    // back view
    currentPosition.x = 300;
    currentPosition.y = 50;
    currentPosition.z = 50;
    uVector.x = 0, uVector.y = 0, uVector.z = 1;
    rVector.x = 0, rVector.y = 1, rVector.z = 0;
    lVector.x = -1, lVector.y = 0, lVector.z = 0;








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
	gluPerspective(fovY, aspectRatio, nearDistance,	farDistance);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}


void parseInput(){
    ifstream fin("description.txt");
    fin >> nearDistance >> farDistance >> fovY >> aspectRatio;
    fin >> levelOfRecursion;
    fin >> numPixels;
    fin >> checkerboarchCellWidth;
    fin >> ambientCheckerboard >> diffuseCheckerboard >> reflectionCheckerboard;
    fin >> numObjects;
    checkerboarchCellWidth/=2;
    for(int i=0; i<numObjects ; i++){
        string object;
        fin >> object ;

        if (object == "sphere"){
            point center; double x, y, z, radius, r, g, b, am, dif, spec, refl, shin;
            Color c;
            fin >> x >>y >> z;
            fin >> radius ;
            fin >> r >> g >> b;
            fin >> am >> dif >> spec >> refl;
            fin >> shin;
            c.r = r ; c.g = g; c.b = b;
            center.x = x; center.y = y ; center.z = z;
            Sphere sphere(center, radius, c, am, dif, spec, refl, shin);
            spheres.push_back(sphere);
        }
        else if(object == "pyramid"){
            point lowestPoint;
            double width, x, y, z,height,r, g, b, am, dif, spec, refl, shin;
            Color c;
            fin >> x >>y >> z;
            fin >> width >> height;
            fin >> r >> g >> b;
            fin >> am >> dif >> spec >> refl;
            fin >> shin;
            c.r = r ; c.g = g; c.b = b;
            lowestPoint.x = x; lowestPoint.y = y ; lowestPoint.z = z;
            Pyramid pyramid(lowestPoint, width, height, c, am, dif, spec, refl, shin);
            pyramids.push_back(pyramid);
        }
        else{

        }
    }


    fin >> numNormalLightSources;
    for(int i=0; i<numNormalLightSources; i++){
        double x,y,z, falloff;
        fin >> x >> y >> z >> falloff;
        point c(x,y,z);
        NormalLight normalLight(c, falloff);
        normalLights.push_back(normalLight);
    }
    fin >> numSpotLightSources;
    for(int i=0; i<numSpotLightSources; i++){
        double x,y,z, falloff, lx,ly,lz, angle;
        fin >> x >> y >> z >> falloff;
        fin >> lx>>ly>>lz;
        fin >> angle;
        point c(x,y,z); point l(lx,ly,lz);
        SpotLight spotLight(c,falloff, l, angle);
        spotLights.push_back(spotLight);
    }
}




int main(int argc, char **argv){
	glutInit(&argc,argv);
	screen_x = screen_y = 500;
	glutInitWindowSize(screen_x, screen_y);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Ray Tracer");





    parseInput();
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

