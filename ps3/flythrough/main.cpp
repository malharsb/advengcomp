/* ////////////////////////////////////////////////////////////

File Name: main.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */
#include <stdio.h>
#include <math.h>
#include <fslazywindow.h>  // DOUBT - WHY THE WARNING??

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
	void GetUpVector(double &vx,double &vy,double &vz);
	void GetRightVector(double &vx,double &vy,double &vz);
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

void CameraObject::GetUpVector(double &vx,double &vy,double &vz)
{
	const double ux0=0.0;
	const double uy0=1.0;
	const double uz0=0.0;

	const double ux1=cos(b)*ux0-sin(b)*uy0;
	const double uy1=sin(b)*ux0+cos(b)*uy0;
	const double uz1=uz0;

	const double ux2=ux1;
	const double uy2=cos(p)*uy1-sin(p)*uz1;
	const double uz2=sin(p)*uy1+cos(p)*uz1;

	vx=cos(-h)*ux2-sin(-h)*uz2;
	vy=uy2;
	vz=sin(-h)*ux2+cos(-h)*uz2;
}

void CameraObject::GetRightVector(double &vx,double &vy,double &vz)
{
	const double ux0=1.0;
	const double uy0=0.0;
	const double uz0=0.0;

	const double ux1=cos(b)*ux0-sin(b)*uy0;
	const double uy1=sin(b)*ux0+cos(b)*uy0;
	const double uz1=uz0;

	const double ux2=ux1;
	const double uy2=cos(p)*uy1-sin(p)*uz1;
	const double uz2=sin(p)*uy1+cos(p)*uz1;

	vx=cos(-h)*ux2-sin(-h)*uz2;
	vy=uy2;
	vz=sin(-h)*ux2+cos(-h)*uz2;
}

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;

	int terminate;
	int wid,hei;
	CameraObject camera;  

public:
	FsLazyWindowApplication();
	virtual void BeforeEverything(int argc,char *argv[]);
	virtual void GetOpenWindowOption(FsOpenWindowOption &OPT) const;
	virtual void Initialize(int argc,char *argv[]);
	virtual void Interval(void);
	virtual void BeforeTerminate(void);
	virtual void Draw(void);
	virtual bool UserWantToCloseProgram(void);
	virtual bool MustTerminate(void) const;
	virtual long long int GetMinimumSleepPerInterval(void) const;
	virtual bool NeedRedraw(void) const;
};

FsLazyWindowApplication::FsLazyWindowApplication()
{
	needRedraw=false;
}

/* virtual */ void FsLazyWindowApplication::BeforeEverything(int argc,char *argv[])
{
}
/* virtual */ void FsLazyWindowApplication::GetOpenWindowOption(FsOpenWindowOption &opt) const
{
	opt.x0=16;
	opt.y0=16;
	opt.wid=800;
	opt.hei=600;
}
/* virtual */ void FsLazyWindowApplication::Initialize(int argc,char *argv[])
{
	terminate = 0;
    camera.z=10.0;  
    camera.y=5.0;  
	
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	FsGetWindowSize(wid,hei); // DOUBT - WHAT ABOUT THIS CAN I USE IT AS IT IS????

	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}

	if(0!=FsGetKeyState(FSKEY_LEFT))
	{
		camera.h+=YsPi/180.0;
	}
	if(0!=FsGetKeyState(FSKEY_RIGHT))
	{
		camera.h-=YsPi/180.0;
	}
	if(0!=FsGetKeyState(FSKEY_UP))
	{
		camera.p+=YsPi/180.0;
	}
	if(0!=FsGetKeyState(FSKEY_DOWN))
	{
		camera.p-=YsPi/180.0;
	}
	if(0!=FsGetKeyState(FSKEY_F))
	{
		double vx,vy,vz;
		camera.GetForwardVector(vx,vy,vz);
		camera.x+=vx*0.5;
		camera.y+=vy*0.5;
		camera.z+=vz*0.5;
	}
	if(0!=FsGetKeyState(FSKEY_B))
	{
		double vx,vy,vz;
		camera.GetForwardVector(vx,vy,vz);
		camera.x-=vx*0.5;
		camera.y-=vy*0.5;
		camera.z-=vz*0.5;
	}


	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{

	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	glViewport(0,0,wid,hei);

	// Set up 3D drawing
	camera.SetUpCameraProjection();
	camera.SetUpCameraTransformation();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1,1);

	// 3D drawing from here
	glColor3ub(0,0,255);

	glBegin(GL_LINES);
	int x;
	for(x=-500; x<=500; x+=20)
	{
		glVertex3i(x,0,-500);
		glVertex3i(x,0, 500);
		glVertex3i(-500,0,x);
		glVertex3i( 500,0,x);
	}
	glEnd();



	// Set up 2D drawing
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);

	FsSwapBuffers();

	needRedraw=false;
}
/* virtual */ bool FsLazyWindowApplication::UserWantToCloseProgram(void)
{
	return true; // Returning true will just close the program.
}
/* virtual */ bool FsLazyWindowApplication::MustTerminate(void) const
{
	return FsLazyWindowApplicationBase::MustTerminate();
}
/* virtual */ long long int FsLazyWindowApplication::GetMinimumSleepPerInterval(void) const
{
	return 25;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
}
/* virtual */ bool FsLazyWindowApplication::NeedRedraw(void) const
{
	return needRedraw;
}


static FsLazyWindowApplication *appPtr=nullptr;

/* static */ FsLazyWindowApplicationBase *FsLazyWindowApplicationBase::GetApplication(void)
{
	if(nullptr==appPtr)
	{
		appPtr=new FsLazyWindowApplication;
	}
	return appPtr;
}
