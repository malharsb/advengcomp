#include <fssimplewindow.h> // For FsSubSecondTimer
#include "meshlattice.h"

#include "ps7lib.h"

#include <vector>
#include <cmath>
#include <queue>
#include <unordered_set>
#include <stdio.h>

#define hakuna(a,b) ((a)<(b) ? (a) : (b))
#define matata(a,b) ((a)>(b) ? (a) : (b))

std::vector <float> MakeSliceVertexArray(const YsShellExt &mesh)
{
	auto t0=FsSubSecondTimer();
	std::vector <float> sliceVtx;

	// You fill this part.

	// Get min and max values of y
	YsVec3 minXyz, maxXyz;
	mesh.GetBoundingBox(minXyz,maxXyz);
	auto yMin = minXyz.yf();
	auto yMax = maxXyz.yf();
	auto dy = (yMax-yMin)/100.0;

	// Define vectors to hold x,y,z coordinates of plane-line intersection points
	std::vector<float> Yvec;
	std::vector<float> Xvec;
	std::vector<float> Zvec;

	// Define iterators
	int counter=0;
	auto plHd = mesh.NullPolygon();

	// Iterate through all counters
	while(true== mesh.MoveToNextPolygon(plHd))
	{
		// Get vertices of current polygon
		auto vtxHdvec = mesh.GetPolygonVertex(plHd);
		Yvec.clear();
		Xvec.clear();
		Zvec.clear();

		// Get minimum and maximum y values of the polygon
		for (auto vtHd : vtxHdvec)
		{
			auto vt = mesh.GetVertexPosition(vtHd);
			Yvec.push_back(vt.yf());
			Xvec.push_back(vt.xf());
			Zvec.push_back(vt.zf());
		}

		auto yMinP = hakuna(Yvec[0], hakuna(Yvec[1], Yvec[2]));
		auto yMaxP = matata(Yvec[0], matata(Yvec[1], Yvec[2]));

		// Calculate the starting plane of the polygon
		int n = int((yMinP-yMin)/dy);
		auto ySt = ((n+1)*dy) + yMin;

		// Iterate through all the planes between the min and max of the poly
		for (auto y=ySt; y<=yMaxP; y+=dy)
		{
			// Create plane for a particular y
			auto pln = YsPlane(YsVec3(0.0, y, 0.0), YsVec3(0.0, 1.0, 0.0));

			for (int v=0; v<3; v++)
			{
				// Initialize
				int vOther[] = {1,2,0};
				YsVec3 crs = YsVec3(0.0, 0.0, 0.0);

				// Create starting and ending point of the line
				auto p1 = YsVec3(Xvec[v], Yvec[v], Zvec[v]);
				auto p2 = YsVec3(Xvec[vOther[v]], Yvec[vOther[v]], Zvec[vOther[v]]);

				// Get point of intersection
				auto YSRESULT = pln.GetPenetration(crs, p1, p2);

				// If YSOK, Add crs to sliceVtx
				if (YSRESULT==YSOK)
				{
					sliceVtx.push_back(crs.xf());
					sliceVtx.push_back(crs.yf());
					sliceVtx.push_back(crs.zf());
				}
				else
				{
					continue;
				}
			}
		}

		counter++;
	}

	printf("%d milli seconds\n",(int)FsSubSecondTimer()-t0);
	return sliceVtx;
}

#define USE_LATTICE

std::vector <float> MakeRayIntersectionVertexArray(const YsShellExt &mesh)
{
	auto t0=FsSubSecondTimer();
	std::vector <float> rayItscVtx;
	
	// You fill this part.

	// Get min and max values of bounding box X,Y,Z
	YsVec3 minXyz, maxXyz;
	mesh.GetBoundingBox(minXyz,maxXyz);
	auto yMin = minXyz.yf();
	auto yMax = maxXyz.yf();
	auto xMin = minXyz.xf();
	auto xMax = maxXyz.xf();
	auto dy = (yMax-yMin)/100.0;
	auto dx = (xMax-xMin)/100.0;

	// Define vectors to hold x,y,z coordinates of plane-line intersection points
	std::vector<float> Yvec;
	std::vector<float> Xvec;
	std::vector<float> Zvec;

	// Define iterators
	int counter=0;
	auto plHd = mesh.NullPolygon();

	// Iterate through all counters
	while(true== mesh.MoveToNextPolygon(plHd))
	{
		
		// Get vertices of current polygon
		auto vtxHdvec = mesh.GetPolygonVertex(plHd);
		Yvec.clear();
		Xvec.clear();
		Zvec.clear();

		// Get minimum and maximum x,y,z values of the polygon
		for (auto vtHd : vtxHdvec)
		{
			auto vt = mesh.GetVertexPosition(vtHd);
			Yvec.push_back(vt.yf());
			Xvec.push_back(vt.xf());
			Zvec.push_back(vt.zf());
		}
		auto yMinP = hakuna(Yvec[0], hakuna(Yvec[1], Yvec[2]));
		auto yMaxP = matata(Yvec[0], matata(Yvec[1], Yvec[2]));
		auto xMinP = hakuna(Xvec[0], hakuna(Xvec[1], Xvec[2]));
		auto xMaxP = matata(Xvec[0], matata(Xvec[1], Xvec[2]));
		auto zMinP = hakuna(Zvec[0], hakuna(Zvec[1], Zvec[2]));
		auto zMaxP = matata(Zvec[0], matata(Zvec[1], Zvec[2]));

		// Calculate the starting plane of the polygon
		int ny = int((yMinP-yMin)/dy);
		auto ySt = ((ny+1)*dy) + yMin;
		int nx = int((xMinP-xMin)/dx);
		auto xSt = ((nx+1)*dx) + xMin;

		// Iterate through all the planes between the min and max of the poly
		for (auto y=ySt; y<=yMaxP; y+=dy)
		{
			for (auto x=xSt; x<=xMaxP; x+=dx)
			{

				// Create line start and end points
				auto p1 = YsVec3(x,y,zMinP);
				auto p2 = YsVec3(x,y,zMaxP);

				// Convert vector<float> to YsVec3;
				auto p3 = YsVec3(Xvec[0], Yvec[0], Zvec[0]);
				auto p4 = YsVec3(Xvec[1], Yvec[1], Zvec[1]);
				auto p5 = YsVec3(Xvec[2], Yvec[2], Zvec[2]);

				// Build a plane from the vertices
				auto pln = YsPlane(YsVec3(0.0,0.0,0.0),YsVec3(0.0,0.0,0.0));
				auto YSRESULT1 = pln.MakePlaneFromTriangle(p3, p4, p5);
				if (YSRESULT1==YSERR)
				{
					printf("EXCEPTION!! YSERR while making plane\n");
				}
				
				// Get point of intersection
				auto crs = YsVec3(0.0,0.0,0.0);
				auto YSRESULT = pln.GetPenetration(crs, p1, p2);

				// If the point of intersection lies within the plane, add to vector
				if (YSRESULT==YSOK)
				{
					rayItscVtx.push_back(crs.xf());
					rayItscVtx.push_back(crs.yf());
					rayItscVtx.push_back(crs.zf());
				}
				else
				{
					continue;
				}
			}
		}
	}

	printf("%d milli seconds\n",(int)FsSubSecondTimer()-t0);
	return rayItscVtx;
}


void Paint(YsShellExt &mesh,YsShell::PolygonHandle from,const double angleTolerance)
{

	// Define openList and closedList for Breadth First Search
	std::queue<YsShellExt::PolygonHandle> openList;
	std::unordered_set<unsigned int> closedList;

	// Add starting handle to openList and get its normal
	openList.push(from);
	auto norm1 = mesh.GetNormal(from);

	// Continue while all valid neighbors are explored
	while(!openList.empty())
	{
		// Explore the front element of queue
		// Set its color, pop it from the openList, add to closedList
		auto plHd = openList.front();
		openList.pop();
		mesh.SetPolygonColor(plHd, YsRed());
		closedList.insert(mesh.GetSearchKey(plHd));
	
		// Get all neighbors
		auto nEdge = mesh.GetPolygonNumVertex(plHd);
		for (decltype(nEdge) i=0; i<nEdge; ++i)
		{
			auto neiPlHd = mesh.GetNeighborPolygon(plHd,i);

			if (neiPlHd==nullptr)
			{
				continue;
			}

			// Get normal of neighbor
			auto norm2 = mesh.GetNormal(neiPlHd);

			// Calculate angle
			auto dot_prod = norm1*norm2;
			auto l2Norm1 = sqrt(norm1*norm1);
			auto l2Norm2 = sqrt(norm2*norm2);
			auto den = l2Norm1*l2Norm2;
			auto ang = acos(dot_prod/den);
			ang = (ang/YsPi)*180.0;

			// If angle < tolerance and we haven't explored the polygon before, add to openList
			// Also add to closedList
			auto isNotInList = (closedList.find(mesh.GetSearchKey(neiPlHd))==closedList.end()) ? 1 : 0;
			if((ang<angleTolerance) && (isNotInList==1))
			{
				closedList.insert(mesh.GetSearchKey(neiPlHd));
				openList.push(neiPlHd);
			}
		}
	}
}

// auto temp1 = ((Yvec[1] < Yvec[2]) ? Yvec[1] : Yvec[2]);
// auto yMinP = ((Yvec[0] < temp1) ? Yvec[0] : temp1);
// auto temp2 = ((Yvec[1] > Yvec[2]) ? Yvec[1] : Yvec[2]);
// auto yMaxP = ((Yvec[0] > temp2) ? Yvec[0] : temp2);


// auto temp1 = ((Yvec[1] < Yvec[2]) ? Yvec[1] : Yvec[2]);
// auto yMinP = ((Yvec[0] < temp1) ? Yvec[0] : temp1);
// auto temp2 = ((Yvec[1] > Yvec[2]) ? Yvec[1] : Yvec[2]);
// auto yMaxP = ((Yvec[0] > temp2) ? Yvec[0] : temp2);

// auto temp3 = ((Xvec[1] < Xvec[2]) ? Xvec[1] : Xvec[2]);
// auto xMinP = ((Xvec[0] < temp3) ? Xvec[0] : temp3);
// auto temp4 = ((Xvec[1] > Xvec[2]) ? Xvec[1] : Xvec[2]);
// auto xMaxP = ((Xvec[0] > temp4) ? Xvec[0] : temp4);

// auto temp5 = ((Zvec[1] < Zvec[2]) ? Zvec[1] : Zvec[2]);
// auto zMinP = ((Zvec[0] < temp5) ? Zvec[0] : temp5);
// auto temp6 = ((Zvec[1] > Zvec[2]) ? Zvec[1] : Zvec[2]);
// auto zMaxP = ((Zvec[0] > temp6) ? Zvec[0] : temp6);