#ifndef PREPROCESSING_H
#define PREPROCESSING_H
#include <vector>
#include <unordered_map>
#include "Struct.h"
#include <stdint.h>
using namespace std;
uint32_t CalcEdges(string & edge_file, vector<TNode> & nodes);
void ReadEdges(string & edgeFile, vector<TNode> & nodes, vector<pair<uint32_t, uint32_t> > & edges);
void ReadNodes(string & nodeFile, vector<TNode> & nodes, double & meanLat, double & meanLon);
void WriteGraph(string & graphFile, vector<TNode> & nodes, vector<pair<uint32_t, uint32_t> > & edges, vector<uint32_t> & landmarks,
    vector<vector <float> > & landDist, uint32_t & maxComp);

void GetLandmarks(vector<TNode> & nodes, int & landCount, vector<uint32_t> & landmarks, uint32_t & maxComp, double & meanLat, double & meanLon);
void CalcLandmarks(vector<TNode> & nodes, vector<pair<uint32_t, uint32_t> > & edges, vector<float> & distance, uint32_t & start);

uint32_t CalcComponents(vector<TNode> & nodes, vector<pair<uint32_t, uint32_t> > & edges);
uint32_t Bfs(vector<TNode> & nodes, vector<pair<uint32_t, uint32_t> > & edges, vector<bool> & used, uint32_t & indStart, uint32_t & ind);
#endif
