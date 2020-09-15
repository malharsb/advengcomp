#include "dha.h"

#include <unordered_set>
#include <queue>
#include <iostream>
#include <cmath>

void BFS(const YsShellExt& mesh,YsShell::PolygonHandle from,const double angleTolerance, std::unordered_map <YSHASHKEY,int>& faceGrp, int groupNum)
{

	// Define openList and closedList for Breadth First Search
	std::queue<YsShellExt::PolygonHandle> openList;
	std::unordered_set<unsigned int> closedList;

	// Add starting handle to openList and get its normal
	openList.push(from);
	

	// Continue while all valid neighbors are explored
	while(!openList.empty())
	{
		// Explore the front element of queue
		// Set its color, pop it from the openList, add to closedList
		auto plHd = openList.front();
		auto norm1 = mesh.GetNormal(plHd);
		faceGrp[mesh.GetSearchKey(plHd)] = groupNum;
		openList.pop();
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

// Input parameters are a mesh and dihedral-angle threshold.
// The return value is a map from a polygon search key to a segment identifier.
std::unordered_map <YSHASHKEY,int> MakeDihedralAngleBasedSegmentation(const YsShellExt &mesh,const double dhaThr)
{
	std::unordered_map <YSHASHKEY,int> faceGrp;
	int groupNum = 1;
	auto PlHd = mesh.NullPolygon();

	// Iterate through each polygon
	while(true== mesh.MoveToNextPolygon(PlHd))
	{
		// If the polygon is not present in the map, use BFS to find its group
		if (faceGrp.count(mesh.GetSearchKey(PlHd))==0)
		{
			BFS(mesh, PlHd, dhaThr, faceGrp, groupNum);
			groupNum++;
		}
	}

	std::cout << "Map Size: " << faceGrp.size() << " Total Polygons: " << mesh.GetNumPolygon() << " Angle tolerance: " << dhaThr << std::endl;
	std::cout << "Total Groups: " << groupNum-1 << std::endl;
	return faceGrp;
}



// Input parameters are a mesh and the segmentation (face grouping) obtained from MakeDihedralAngleBasedSegmentaion.
// Output is a vertex array that can be drawn as GL_LINES.
std::vector <float> MakeGroupBoundaryVertexArray(const YsShellExt &mesh,const std::unordered_map <YSHASHKEY,int> &faceGroupInfo)
{
	std::vector <float> vtxArray;
	auto PlHd = mesh.NullPolygon();

	while(true== mesh.MoveToNextPolygon(PlHd))
	{
		auto nEdge = mesh.GetPolygonNumVertex(PlHd);
		auto cFaceGrp = faceGroupInfo.at(mesh.GetSearchKey(PlHd));
		auto poly1 = mesh.GetPolygonVertex(PlHd);
		// std::cout << "nEdge: " << nEdge << " cFaceGrp: " << cFaceGrp << std::endl;

		for (decltype(nEdge) i=0; i<nEdge; ++i)
		{
			auto neiPlHd = mesh.GetNeighborPolygon(PlHd,i);
			auto nFaceGrp = faceGroupInfo.at(mesh.GetSearchKey(neiPlHd));

			if (cFaceGrp<nFaceGrp)
			{
				auto poly2 = mesh.GetPolygonVertex(neiPlHd);

				// Find common vertices by iterating through all vertices
				for (int k=0; k<poly1.size(); k++)
				{
					for (int j=0; j<poly2.size(); j++)
					{
						if (poly1[k]==poly2[j])
						{
							auto vtx = mesh.GetVertexPosition(poly1[k]);
							vtxArray.push_back(vtx.xf());
							vtxArray.push_back(vtx.yf());
							vtxArray.push_back(vtx.zf());
						}
					}
				}
			}
		}
	}

	return vtxArray;
}




// For bonus questions:
// Input parameters are a mesh and the segmentation (face grouping) obtained from MakeDihedralAngleBasedSegmentaion.
// Paint polygons so that no two neighboring face groups have a same color.
void MakeFaceGroupColorMap(YsShellExt &mesh,const std::unordered_map <YSHASHKEY,int> &faceGroupInfo)
{
	auto PlHd = mesh.NullPolygon();

	while(true== mesh.MoveToNextPolygon(PlHd))
	{
		auto cFaceGrp = faceGroupInfo.at(mesh.GetSearchKey(PlHd));
		double r = std::fmod( std::exp(cFaceGrp+2.257), 1.0);
		double g = std::fmod( std::exp(cFaceGrp+7.911), 1.0);
		double b = std::fmod( std::exp(cFaceGrp+13.171), 1.0);
		YsColor col(r, g, b);
		mesh.SetPolygonColor(PlHd, col);
	}
}

