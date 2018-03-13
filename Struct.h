#ifndef STRUCT_H
#define STRUCT_H
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <stdint.h>
using namespace std;

struct TNode {
    uint32_t currentId, index, components;
    double lon, lat;
    TNode() {}
    TNode(uint32_t & id, double & lat1, double & lon1);
};

bool Comp(TNode n, uint32_t id);


double GetDistance(double & lat1, double & lat2, double & lon1, double &lon2);

#endif