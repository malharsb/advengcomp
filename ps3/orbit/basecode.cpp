#include <stdio.h>
#include <math.h>
#include "fssimplewindow.h"

const double YsPi=3.1415927;

class CameraObject
{
public:
    double x,y,z;
    double h,p,b;

    double fov,nearZ,farZ;

    CameraObject();
    void Initialize(void);
    void SetUpCameraProjection(void);
    void SetUpCameraTransformation(void);

    void GetForwardVector(double &vx,double &vy,double &vz);
};

CameraObject::CameraObject()
{
    Initialize();
}

void CameraObject::Initialize(void)
{
    x=0;
    y=0;
    z=0;
    h=0;
    p=0;
    b=0;

    fov=YsPi/6.0;  // 30 degree
    nearZ=0.1;
    farZ=200.0;
}

void CameraObject::SetUpCameraProjection(void)
{
    int wid,hei;
    double aspect;

    FsGetWindowSize(wid,hei);
    aspect=(double)wid/(double)hei;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov*180.0/YsPi,aspect,nearZ,farZ);
}

void CameraObject::SetUpCameraTransformation(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotated(-b*180.0/YsPi,0.0,0.0,1.0);  // Rotation about Z axis
    glRotated(-p*180.0/YsPi,1.0,0.0,0.0);  // Rotation about X axis
    glRotated(-h*180.0/YsPi,0.0,1.0,0.0);  // Rotation about Y axis
    glTranslated(-x,-y,-z);
}

void CameraObject::GetForwardVector(double &vx,double &vy,double &vz)
{
    vx=-cos(p)*sin(h);
    vy= sin(p);
    vz=-cos(p)*cos(h);
}


class OrbitingViewer
{
public:
    double h,p;
    double dist;
    double focusX,focusY,focusZ;

    OrbitingViewer();
    void Initialize(void);
    void SetUpCamera(CameraObject &camera);
};

OrbitingViewer::OrbitingViewer()
{
    Initialize();
}

void OrbitingViewer::Initialize(void)
{
    h=0;
    p=0;
    dist=50.0;
    focusX=0.0;
    focusY=0.0;
    focusZ=0.0;
}

void OrbitingViewer::SetUpCamera(CameraObject &camera)
{
    camera.h=h;
    camera.p=p;
    camera.b=0.0;

    double vx,vy,vz;
    camera.GetForwardVector(vx,vy,vz);
    camera.x=focusX-vx*dist;
    camera.y=focusY-vy*dist;
    camera.z=focusZ-vz*dist;
}

void DrawCube(double x1,double y1,double z1,double x2,double y2,double z2)
{
    glColor3ub(0,0,255);

    glBegin(GL_QUADS);

    glVertex3d(x1,y1,z1);
    glVertex3d(x2,y1,z1);
    glVertex3d(x2,y2,z1);
    glVertex3d(x1,y2,z1);

    glVertex3d(x1,y1,z2);
    glVertex3d(x2,y1,z2);
    glVertex3d(x2,y2,z2);
    glVertex3d(x1,y2,z2);

    glVertex3d(x1,y1,z1);
    glVertex3d(x2,y1,z1);
    glVertex3d(x2,y1,z2);
    glVertex3d(x1,y1,z2);

    glVertex3d(x1,y2,z1);
    glVertex3d(x2,y2,z1);
    glVertex3d(x2,y2,z2);
    glVertex3d(x1,y2,z2);

    glVertex3d(x1,y1,z1);
    glVertex3d(x1,y2,z1);
    glVertex3d(x1,y2,z2);
    glVertex3d(x1,y1,z2);

    glVertex3d(x2,y1,z1);
    glVertex3d(x2,y2,z1);
    glVertex3d(x2,y2,z2);
    glVertex3d(x2,y1,z2);

    glEnd();


    glColor3ub(0,255,255);

    glBegin(GL_LINES);
    glVertex3d(x1,y1,z1);
    glVertex3d(x1,y1,z2);
    glVertex3d(x1,y1,z2);
    glVertex3d(x2,y1,z2);
    glVertex3d(x2,y1,z2);
    glVertex3d(x2,y1,z1);
    glVertex3d(x2,y1,z1);
    glVertex3d(x1,y1,z1);
    glVertex3d(x1,y2,z1);
    glVertex3d(x1,y2,z2);
    glVertex3d(x1,y2,z2);
    glVertex3d(x2,y2,z2);
    glVertex3d(x2,y2,z2);
    glVertex3d(x2,y2,z1);
    glVertex3d(x2,y2,z1);
    glVertex3d(x1,y2,z1);
    glVertex3d(x1,y1,z1);
    glVertex3d(x1,y2,z1);
    glVertex3d(x1,y1,z2);
    glVertex3d(x1,y2,z2);
    glVertex3d(x2,y1,z2);
    glVertex3d(x2,y2,z2);
    glVertex3d(x2,y1,z1);
    glVertex3d(x2,y2,z1);
    glEnd();
}

int main(void)
{
    int terminate=0;
    CameraObject camera;
    OrbitingViewer orbit;

    camera.z=10.0;

    FsOpenWindow(16,16,800,600,1);

    while(0==terminate)
    {
        FsPollDevice();

        int wid,hei;
        FsGetWindowSize(wid,hei);

        int key=FsInkey();
        switch(key)
        {
        case FSKEY_ESC:
            terminate=1;
            break;
        }


        if(0!=FsGetKeyState(FSKEY_LEFT))
        {
            orbit.h+=YsPi/180.0;
        }
        if(0!=FsGetKeyState(FSKEY_RIGHT))
        {
            orbit.h-=YsPi/180.0;
        }
        if(0!=FsGetKeyState(FSKEY_UP))
        {
            orbit.p+=YsPi/180.0;
        }
        if(0!=FsGetKeyState(FSKEY_DOWN))
        {
            orbit.p-=YsPi/180.0;
        }
        if(0!=FsGetKeyState(FSKEY_F) && orbit.dist>1.0)
        {
            orbit.dist/=1.05;
        }
        if(0!=FsGetKeyState(FSKEY_B) && orbit.dist<200.0)
        {
            orbit.dist*=1.05;
        }
        orbit.SetUpCamera(camera);

        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

        glViewport(0,0,wid,hei);

        // Set up 3D drawing
        camera.SetUpCameraProjection();
        camera.SetUpCameraTransformation();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1,1);

        // 3D drawing from here
        DrawCube(-10.0,-10.0,-10.0,10.0,10.0,10.0);

        // Set up 2D drawing
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);

        FsSwapBuffers();
        FsSleep(25);
    }

    return 0;
}


