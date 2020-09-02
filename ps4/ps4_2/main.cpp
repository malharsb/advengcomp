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
#include <fslazywindow.h>
#include "simplebitmap.h"
#include "hashtable.h"

using namespace std;

template <>
unsigned long long int HashTable<SimpleBitmap,int>::HashCode(const SimpleBitmap &key) const
{
	unsigned long long int sum=0;
	int counter=0;
	int p_array[] ={2,3,5,7,9,11};

	for (int r=0; r<key.GetHeight(); r++)
	{
		for (int c=0; c<key.GetWidth(); c++)
		{
			auto *keyptr = key.GetPixelPointer(r, c);
			
			for (int i=0; i<4; i++)
			{
				int mod = counter%5;
				sum+= p_array[mod]*keyptr[i]; // HTF to you count the bytes??? For now,counting the pixel components
				counter++;
			}

		}

	}

	return sum;

}

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;

	//malhar
	SimpleBitmap inputBm;
	HashTable<SimpleBitmap,int> hashtb;
	int counter;
	int width, height;
	int cuth, cutw;
	int r,c;
	int wid, hei;
	int bon_flag;

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
	opt.x0=0;
	opt.y0=0;
	opt.wid=1200;
	opt.hei=800;
}
/* virtual */ void FsLazyWindowApplication::Initialize(int argc,char *argv[])
{
	// Case when there's no argument 
    if (argv[1]==NULL)
    {
        cout << "Usage: ps4_1 <pngFileName.png>\n";
		SetMustTerminate(true);
    }

	// Define input empty bitmap
    bool isLoaded = inputBm.LoadPng(argv[1]);

    if (isLoaded==false)
    {
        cout << "Error: Failed to read a .PNG file.\n";
		SetMustTerminate(true);
    }

	// initialize counter, get height, get width
	counter=0;
    width = inputBm.GetWidth();
    height = inputBm.GetHeight();
    cuth = 40;
	cutw = 40;
	bon_flag=0;

	int id=0;

	for (int j=0; j<height; j+=cuth)
    {
        for (int i=0; i<width; i+=cutw)
        {
			SimpleBitmap cutBm = inputBm.CutOut(i,j,cutw,cuth);
			cutBm.Invert();

			if (!hashtb.IsIncluded(cutBm))
			{
				
				hashtb.Update(cutBm, id);
				printf("%c",' '+id); 
				id++;
			}
			else
			{
				const int rkey = *hashtb[cutBm];
				printf("%c",' '+rkey); 
			}
			
		}
		cout << "\n";
	}

	cout << "table size: " << hashtb.GetN() << "\n\n";

}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	FsGetWindowSize(wid,hei);

	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}

	if(FSKEY_SPACE==key)
	{
		bon_flag=1;
	}
	

	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	if (!bon_flag)
	{

		glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

		int glr=0;
		int glc=0;

		int counter=0;

		for (auto hd=hashtb.First(); hd.IsNotNull(); hd=hashtb.Next(hd))
		{
			if (counter%30==0)
			{
				glc = glc+cuth;
				glr = 0;
			}

			auto curr = hashtb[hd];
			glRasterPos2i(glr,glc);
			glDrawPixels(cuth, cutw, GL_RGBA, GL_UNSIGNED_BYTE, curr->GetBitmapPointer());
			counter++;
			glr=glr+cutw;

		}

	}
	

	if (bon_flag)
	{
		glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
		int bnr=0;
		int bnc=40;
		glRasterPos2i(bnr,bnc);
		glDrawPixels(cuth+1000, cutw+1000, GL_RGBA, GL_UNSIGNED_BYTE, inputBm.GetBitmapPointer());
	}


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
	return 10;
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
