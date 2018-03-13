#include <functional>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include "Searching.h"
#include "Struct.h"
#include <algorithm>
#include <stdint.h>

void ReadGraph(string & graphFile, vector<TNode> & nodes, vector<pair<uint32_t, uint32_t> > & edges, vector<uint32_t> & landmarks, 
    vector<vector <float> > & landDist , uint32_t & maxComp) {
    ifstream in(graphFile, fstream::binary);
    size_t countN = 0, countE = 0, countL = 0;
    in.read((char*) & countN, sizeof(size_t));
    nodes.resize(countN);
    in.read((char*)nodes.data(), nodes.size() * sizeof(nodes[0]));
    in.read((char*) & countE, sizeof(size_t));
    edges.resize(countE);
    in.read((char*)edges.data(), edges.size() * sizeof(edges[0]));
    in.read((char*) & countL, sizeof(size_t));
    landmarks.resize(countL);
    in.read((char*)landmarks.data(), landmarks.size() * sizeof(landmarks[0]));
    landDist.resize(countL);
    for (size_t i = 0; i < landDist.size(); i++) {
        landDist[i].resize(countN);
        in.read((char*)landDist[i].data(), landDist[i].size() * sizeof(landDist[i][0]));
    }
    in.read((char*)& maxComp, sizeof(uint32_t));
    in.close();
}

double Search(uint32_t & indStart, uint32_t & indFin, vector<TNode> & nodes, vector<pair<uint32_t, uint32_t> > & edges, vector<uint32_t> & parent,
    vector <float> & landDist1, vector <float> & landDist2, bool hFlag) {
    vector<double> distance(nodes.size(), -1);
    priority_queue <pair<double, uint32_t>, vector<pair<double, uint32_t>>, greater<pair<double, uint32_t> > > q;
    q.push(make_pair((double)0, indStart));
    distance[indStart] = 0;
    uint32_t cur;
    while (!q.empty()) {
        cur = q.top().second;
        if (cur == indFin) {
            return distance[cur];
        }
        q.pop();
        uint32_t count = (cur == (nodes.size() - 1) ? edges.size() : nodes[cur + 1].index);

        for (uint32_t i = nodes[cur].index; i < count; i++) {
            uint32_t indV = edges[i].second;
            double tentativeScore = distance[cur] + GetDistance(nodes[cur].lat, nodes[indV].lat, nodes[cur].lon, nodes[indV].lon);
            if (tentativeScore >= distance[indV] && distance[indV] > 0) {
                continue;
            }
            else if (tentativeScore < distance[indV] || distance[indV] < 0) {
                distance[indV] = tentativeScore;
                parent[indV] = cur;
                double heuristic;
                if (hFlag)
                    heuristic = max(fabs(landDist1[indV] - landDist1[indFin]), fabs(landDist2[indV] - landDist2[indFin])) * 0.99;
                else
                    heuristic = GetDistance(nodes[indV].lat, nodes[indFin].lat, nodes[indV].lon, nodes[indFin].lon);
                q.push(make_pair(tentativeScore + heuristic, indV));
            }
        }
    }
    return -1;
}


