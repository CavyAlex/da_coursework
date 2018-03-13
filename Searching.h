#ifndef SEARCHING_H
#define SEARCHING_H
#include <vector>
#include <unordered_map>
#include "Struct.h"
#include <stdint.h>
void ReadGraph(string & graphFile, vector<TNode> & Nodes, vector<pair<uint32_t, uint32_t> > & edges, vector<uint32_t> & landmarks,
    vector<vector <float> > & landDist, uint32_t & maxComp);
double Search(uint32_t & start, uint32_t & finish, vector<TNode> & Nodes, vector<pair<uint32_t, uint32_t> > & edges, vector<uint32_t> & parent,
    vector <float> & landDist1, vector <float> & landDist2, bool hFlag);

#endif