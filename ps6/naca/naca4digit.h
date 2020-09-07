#ifndef NACA4DIGIT_IS_INCLUDED
#define NACA4DIGIT_IS_INCLUDED

#include <vector>
#include <cstddef>
#include <ysclass.h>
std::vector <YsVec3> MakeNACA4DigitAirfoil(int naca,const double dz);
void MakeNACA4DigitAirfoilWing(class PolygonalMesh &mesh,int naca,float span,float sweepBack,float dihedral,float taper,float dz);
#endif
