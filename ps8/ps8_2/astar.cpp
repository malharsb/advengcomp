#include "astar.h"
#include <iostream>
#include <map>
#include <cstdlib> 

// Calculate Heuristic
double calculate_h(const YsShellExt& shl, YsShellExt::VertexHandle vh1, YsShellExt::VertexHandle vh2)
{
	auto vh1x = shl.GetVertexPosition(vh1).x();
	auto vh1y = shl.GetVertexPosition(vh1).y();
	auto vh1z = shl.GetVertexPosition(vh1).z();

	auto vh2x = shl.GetVertexPosition(vh2).x();
	auto vh2y = shl.GetVertexPosition(vh2).y();
	auto vh2z = shl.GetVertexPosition(vh2).z();

	int p = 1;
	return std::fabs(std::pow(vh1x - vh2x, p)) + std::fabs(std::pow(vh1y - vh2y, p)) + std::fabs(std::pow(vh1z - vh2z, p));
}

// Update the F score if a more efficient path is found
void replaceFScore(const YsShellExt &shl, std::multimap<double, YsShellExt::VertexHandle>& fScore, YsShellExt::VertexHandle handle, const double newF)
{
	std::multimap<double, YsShellExt::VertexHandle> :: iterator it, foundIt;
	bool isfound = false;

	// Iterate through the multimap to find the vertex
	for (it=fScore.begin() ; it!=fScore.end() ; it++)
	{
		if ((*it).second == handle)
		{
			foundIt = it;
			isfound = true;
		}
	}

	// If the vertex is found, update the value
	if (isfound)
	{
		fScore.erase(foundIt);
		isfound = false;
		fScore.insert(std::make_pair(newF, handle));
	}
	else
	{
		std::cout << "Couldn't find key" << std::endl;
	}
}

std::vector <YsShellExt::VertexHandle> A_Star(const YsShellExt &shl,YsShellExt::VertexHandle startVtHd,YsShellExt::VertexHandle goalVtHd)
{

	// Create openSet, closedSet, cameFrom, gScore, fScore
	std::unordered_set<YSHASHKEY> closedSet;
	std::unordered_set<YSHASHKEY> openSet;
	auto startHK = shl.GetSearchKey(startVtHd);
	openSet.insert(startHK);

	std::unordered_map <YSHASHKEY,YsShellExt::VertexHandle> cameFrom;
	std::unordered_map<YSHASHKEY, double> gScore;
	std::multimap<double, YsShellExt::VertexHandle> fScore;

	// Initialize values of gScore, add elements to fScore
	for (auto VtHd : shl.AllVertex())
	{	
		auto vtHK = shl.GetSearchKey(VtHd);
		fScore.insert(std::make_pair(INFINITY, VtHd));
		gScore[vtHK] = INFINITY;
	}

	// Initialize F value of start node
	gScore[startHK] = 0.0;
	auto startF = gScore[startHK] + calculate_h(shl, startVtHd, goalVtHd);
	replaceFScore(shl, fScore, startVtHd, startF);

	while (!openSet.empty())
	{

		// Obtain vertex handle with the smallest f value
		std::multimap<double, YsShellExt::VertexHandle> :: iterator currIt = fScore.begin();
		auto currVtHd = (*currIt).second;

		// Check for null pointer
		if (currVtHd == nullptr)
		{
			std::cout << "Null currVtHd encountered!! " << std::endl;
		}

		// Check if search has reached goal
		if (currVtHd == goalVtHd)
		{
			std::cout << "Goal Found" << std::endl;
			return A_Star_ReconstructPath(shl, cameFrom, currVtHd);
		}

		// Remove from openSet, add to closedSet
		openSet.erase(shl.GetSearchKey(currVtHd));
		fScore.erase(currIt);
		closedSet.insert(shl.GetSearchKey(currVtHd));

		// Get neighbors of current vertex
		auto VtHdVec = shl.GetConnectedVertex(currVtHd);
		for (auto neiVtHd : VtHdVec)
		{

			auto neiKey = shl.GetSearchKey(neiVtHd);

			// If already explored (present in closedSet) exit
			if (closedSet.find(neiKey)!=closedSet.end())
			{
				continue;
			}

			// Add to openSet if not already present
			if (openSet.find(neiKey)==openSet.end())
			{
				openSet.insert(neiKey);
			}

			// Check if this is a more efficient path by recalculating the vertiex's g value
			auto tempG = gScore[shl.GetSearchKey(currVtHd)] + calculate_h(shl, neiVtHd, currVtHd);
			if (tempG >= gScore[neiKey])
			{
				continue;
			}

			// Update path 
			cameFrom[neiKey] = currVtHd;
			
			// Compute new f for the vertex
			gScore[neiKey] = tempG;
			auto newF = gScore[neiKey] + calculate_h(shl, neiVtHd, goalVtHd);
			replaceFScore(shl, fScore, neiVtHd, newF);

		}
	}

	return std::vector <YsShellExt::VertexHandle>(); // Path not found.
}


std::vector <YsShellExt::VertexHandle> A_Star_ReconstructPath(
    const YsShellExt &shl,const std::unordered_map <YSHASHKEY,YsShellExt::VertexHandle> &cameFrom,YsShellExt::VertexHandle current)
{
	std::vector <YsShellExt::VertexHandle> path;

	path.push_back(current);

	while (cameFrom.find(shl.GetSearchKey(current)) != cameFrom.end())
	{
		current = cameFrom.at(shl.GetSearchKey(current));
		path.push_back(current);
	}

	return path;
}