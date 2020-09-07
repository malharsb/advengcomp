#include <polygonalmesh.h>
#include "naca4digit.h"
#include "math.h"
#include <vector>



std::vector <YsVec3> MakeNACA4DigitAirfoil(int naca,const double dz)
{
	std::vector <YsVec3> vtx;
	int naca2 = naca;
	printf("naca2: %d \n",naca2);
	float m = (naca2/1000)*0.01;
	naca2 = naca%1000;
	printf("naca2: %d \n",naca2);
	float p = (naca2/100)*0.1;
	float t = (naca2%100)*0.01;
	printf("m: p: t: %f, %f, %f \n",m,p,t);
	float c = 1.0;
	float z2=0;


	// Upper contour
	for (float z=0.0; z<=1.0001 ; z+=dz)
	{
		float yt = 5*t*(0.2969*sqrt(z) - 0.1260*z - 0.3516*pow(z,2) + 0.2843*pow(z,3) - 0.1015*pow(z,4));
		float yc=0;

		if (p==0.0 || p==1.0)
		{
			yc = 0.0;
		}
		else 
		{
			if (z>=0.0 && z<=(p*c))
			{
				yc = ( m/pow(p,2) )*( (2*p*z)/c - pow((z/c),2) );
			}
			else
			{
				yc = (m/(pow((1-p),2)))*( (1-2*p) + (2*p*z)/c - pow((z/c),2) );
			}

		}
		
		float yu = yc + yt;
		// printf("x: 0, yu: %f, z: %f yc: %f yt: %f\n",yu,z, yc, yt);
		vtx.push_back(YsVec3(0, yu, z));

		z2 = z-dz;
	}

	printf("vtx size: %d\n",vtx.size());


	// Lower contour
	for (float z=z2; z>-0.01 ; z-=dz)
	{
		float yt = 5*t*(0.2969*sqrt(z) - 0.1260*z - 0.3516*pow(z,2) + 0.2843*pow(z,3) - 0.1015*pow(z,4));
		float yc=0;

		if (p==0.0 || p==1.0)
		{
			yc = 0.0;
		}
		else
		{
			if (z>=0.0 && z<=(p*c))
			{
				yc = ( m/pow(p,2) )*( (2*p*z)/c - pow((z/c),2) );
			}
			else
			{
				yc = (m/(pow((1-p),2)))*( (1-2*p) + (2*p*z)/c - pow((z/c),2) );
			}
		} 
			
		float yl = yc - yt;
		// printf("x: 0, yl: %f, z: %f yc: %f yt: %f\n",yl,z, yc, yt);
		vtx.push_back(YsVec3(0, yl, z));
	}
	return vtx;
}

void MakeNACA4DigitAirfoilWing(class PolygonalMesh &mesh,int naca,float span,float sweepBack,float dihedral,float taper,float dz)
{
	//inputs: mesh, naca, s, b, d, p, dz

	// Calculate the cross section at x=0
	auto csection_0 = MakeNACA4DigitAirfoil(naca, dz);
	auto csection_ps = csection_0;
	auto csection_ns = csection_0;
	int numPoints = csection_0.size();

	// Calculate the cross section at x=+s
	for (int i=0; i<numPoints; i++)
	{
		csection_ps[i][0] = csection_ps[i][0] + span;
		csection_ps[i][1] = csection_ps[i][1] + dihedral;
		csection_ps[i][2] = taper*csection_ps[i][2] + sweepBack; 
	}

	// Calculate the cross section at x=-s
	for (int j=0; j<numPoints; j++)
	{
		csection_ns[j][0] = csection_ns[j][0] - span;
		csection_ns[j][1] = csection_ns[j][1] + dihedral;
		csection_ns[j][2] = taper*csection_ns[j][2] + sweepBack;
	}

	// Add triangle

	for (int k=0; k<numPoints-1; k++)
	{

		const PolygonalMesh::VertexHandle triVtHd1[3]=
		{
			mesh.AddVertex(csection_0[k]),
			mesh.AddVertex(csection_ps[k]),
			mesh.AddVertex(csection_0[k+1]),
		};
		auto polyHd = mesh.AddPolygon(3,triVtHd1);
		mesh.SetPolygonColor(polyHd,YsBlue());
		

		auto v1 = csection_0[k] - csection_ps[k];
		auto v2 = csection_ps[k] - csection_0[k+1];
		YsVec3 crs=YsUnitVector(v1^v2);
		mesh.SetPolygonNormal(polyHd,crs);




		const PolygonalMesh::VertexHandle triVtHd2[3]=
		{
			mesh.AddVertex(csection_0[k+1]),
			mesh.AddVertex(csection_ps[k+1]),
			mesh.AddVertex(csection_ps[k]),
		};
		polyHd = mesh.AddPolygon(3,triVtHd2);
		mesh.SetPolygonColor(polyHd,YsBlue());

		v1 = csection_0[k+1] - csection_ps[k+1];
		v2 = csection_ps[k+1] - csection_ps[k];
		crs=YsUnitVector(v1^v2);
		mesh.SetPolygonNormal(polyHd,crs);




		const PolygonalMesh::VertexHandle triVtHd3[3]=
		{
			mesh.AddVertex(csection_0[k]),
			mesh.AddVertex(csection_ns[k]),
			mesh.AddVertex(csection_0[k+1]),
		};
		polyHd = mesh.AddPolygon(3,triVtHd3);
		mesh.SetPolygonColor(polyHd,YsBlue());

		v1 = csection_0[k] - csection_ns[k];
		v2 = csection_ns[k] - csection_0[k+1];
		crs=YsUnitVector(v1^v2);
		mesh.SetPolygonNormal(polyHd,crs);


		const PolygonalMesh::VertexHandle triVtHd4[3]=
		{
			mesh.AddVertex(csection_0[k+1]),
			mesh.AddVertex(csection_ns[k+1]),
			mesh.AddVertex(csection_ns[k]),
		};
		polyHd = mesh.AddPolygon(3,triVtHd4);
		mesh.SetPolygonColor(polyHd,YsBlue());

		v1 = csection_0[k+1] - csection_ns[k+1];
		v2 = csection_ns[k+1] - csection_ns[k];
		crs=YsUnitVector(v1^v2);
		mesh.SetPolygonNormal(polyHd,crs);

	}

	printf("%d polygons\n",mesh.GetNumPolygon());
}


