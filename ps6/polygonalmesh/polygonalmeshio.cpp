#include <vector>
#include "polygonalmesh.h"

bool PolygonalMesh::LoadBinStl(const char fn[])
{
    FILE *fp=fopen(fn,"rb");
    if(nullptr!=fp)
    {
        unsigned char title[80];
        fread(title,1,80,fp); // Skip title

        unsigned char dw[4];
        fread(dw,4,1,fp);  // Read 4 bytes
        auto nTri=BinaryToInt(dw);
        printf("%d triangles\n",nTri);

        int nTriActual=0;
        for(int i=0; i<nTri; ++i)
        {
            unsigned char buf[50];  // 50 bytes per triangle
            if(50==fread(buf,1,50,fp))
            {
                AddBinaryStlTriangle(buf);
                ++nTriActual;
            }
            else
            {
                break;
            }
        }
        printf("Actually read %d\n",nTriActual);

        fclose(fp);
		return true;
    }
	return false;
}

bool PolygonalMesh::CPUisLittleEndian(void)
{
    unsigned int one=1;
    auto *dat=(const unsigned char *)&one;
    if(1==dat[0])
    {
        return true;
    }
    return false;
}
int PolygonalMesh::BinaryToInt(const unsigned char dw[4])
{
    int b0=(int)dw[0];
    int b1=(int)dw[1];
    int b2=(int)dw[2];
    int b3=(int)dw[3];
    return b0+b1*0x100+b2*0x10000+b3*0x1000000;
}
float PolygonalMesh::BinaryToFloat(const unsigned char dw[4])
{
    if(true==CPUisLittleEndian())
    {
        const float *fPtr=(const float *)dw;
        return *fPtr;
    }
    else
    {
        float value;
        auto *valuePtr=(unsigned char *)&value;
        valuePtr[0]=dw[3];
        valuePtr[1]=dw[2];
        valuePtr[2]=dw[1];
        valuePtr[3]=dw[0];
        return value;
    }
}

void PolygonalMesh::AddBinaryStlTriangle(const unsigned char buf[50])
{
    float nx=BinaryToFloat(buf),ny=BinaryToFloat(buf+4),nz=BinaryToFloat(buf+8);
    const YsVec3 nom(nx,ny,nz);

	const YsVec3 tri[3]=
	{
	    YsVec3(BinaryToFloat(buf+12),BinaryToFloat(buf+16),BinaryToFloat(buf+20)),
	    YsVec3(BinaryToFloat(buf+24),BinaryToFloat(buf+28),BinaryToFloat(buf+32)),
	    YsVec3(BinaryToFloat(buf+36),BinaryToFloat(buf+40),BinaryToFloat(buf+44)),
	};

	const VertexHandle triVtHd[3]=
	{
		AddVertex(tri[0]),
		AddVertex(tri[1]),
		AddVertex(tri[2]),
	};

	auto plHd=AddPolygon(3,triVtHd);
	SetPolygonColor(plHd,YsBlue());
	SetPolygonNormal(plHd,nom);
}


bool PolygonalMesh::SaveBinStl(const char fn[]) const
{
    // Open a file, create buffer
    FILE *fp=fopen(fn,"wb"); // Open in Write Binary mode
    unsigned char buf[80];
    fwrite(buf, 1, 80, fp); // Dummy write 80 bytes to skip comments


    // Write the number of polygons
    int nTri = GetNumPolygon();
    fwrite((unsigned char*)&nTri, 1, 4, fp);


    // Initialize first pointer and counter
    auto plHd = NullPolygon();
    unsigned char buf2[2] = "0";
    int counter=0;


    if(nullptr!=fp)
    {
        // Iterate through all the triangles
        for (decltype(nTri) i=0; i<nTri; ++i)
        {

            if(true== MoveToNextPolygon(plHd))
            {
                counter++; 

                auto nom = GetNormal(plHd);
                auto nx = nom.xf();
                auto ny = nom.yf();
                auto nz = nom.zf();
                
                fwrite((unsigned char*)&nx,1,4,fp);
                fwrite((unsigned char*)&ny,1,4,fp);
                fwrite((unsigned char*)&nz,1,4,fp);

                auto vtxHdvec = GetPolygonVertex(plHd);
                for (auto vtHd : vtxHdvec)
                {
                    auto vt = GetVertexPosition(vtHd);
                    auto vx = vt.xf();
                    auto vy = vt.yf();
                    auto vz = vt.zf();

                    fwrite((unsigned char*)&vx,1,4,fp);
                    fwrite((unsigned char*)&vy,1,4,fp);
                    fwrite((unsigned char*)&vz,1,4,fp);
                }
                fwrite(buf2, 1, 2, fp);
            }

            else
            {
                break;
            }

        }

        fclose(fp);
        printf("Saved STL , counter: %d",counter);
        return true;
    }

	return false;
}


