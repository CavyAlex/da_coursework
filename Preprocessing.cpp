#include <functional>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <stdint.h>
#include <algorithm>
#include "Preprocessing.h"
#include "Struct.h"

using namespace std;

uint32_t CalcEdges(string & edgeFile, vector<TNode> & nodes) {
    ifstream in(edgeFile);
    uint32_t idTmp[2], indTmp[2];
    int numberN, ind = 0;
    vector<uint32_t> count(nodes.size());
    while (in >> numberN) {
        ind = 0;
        in >> idTmp[ind];
        indTmp[ind] = lower_bound(nodes.begin(), nodes.end(), idTmp[ind], Comp) - nodes.begin();
        for (int i = 1; i < numberN; i++) {
            int ind2 = (ind + 1) % 2;
            in >> idTmp[ind2];
            indTmp[ind2] = lower_bound(nodes.begin(), nodes.end(), idTmp[ind2], Comp) - nodes.begin();
            count[indTmp[ind]]++;
            count[indTmp[ind2]]++;
            ind = ind2;
        }
    }
    in.close();
    for (size_t i = 1; i < nodes.size(); i++) {
        nodes[i].index = nodes[i - 1].index + count[i - 1];
    }
    return nodes[nodes.size() - 1].index + count[nodes.size() - 1];
}

void ReadEdges(string & edgeFile, vector<TNode> & nodes, vector<pair<uint32_t, uint32_t> > & edges) {
    ifstream in(edgeFile);
    uint32_t idTmp[2], indTmp[2];
    int numberN, ind = 0;
    uint32_t *indEdg = new uint32_t[edges.size()];
    for (size_t i = 0; i < nodes.size(); i++) {
        indEdg[i] = nodes[i].index;
    }
    while (in >> numberN) {
        in >> idTmp[ind];
        indTmp[ind] = lower_bound(nodes.begin(), nodes.end(), idTmp[ind], Comp) - nodes.begin();
        for (int i = 1; i < numberN; i++) {
            int ind2 = (ind + 1) % 2;
            in >> idTmp[ind2];
            indTmp[ind2] = lower_bound(nodes.begin(), nodes.end(), idTmp[ind2], Comp) - nodes.begin();
            edges[indEdg[indTmp[ind]]].first = idTmp[ind2];
            edges[indEdg[indTmp[ind]]].second = indTmp[ind2]; 
            indEdg[indTmp[ind]]++;
            edges[indEdg[indTmp[ind2]]].first = idTmp[ind];
            edges[indEdg[indTmp[ind2]]].second = indTmp[ind];
            indEdg[indTmp[ind2]]++;
            ind = ind2;
        }
    }
    delete[] indEdg;
    in.close();
}

void ReadNodes(string & nodeFile, vector<TNode> & nodes, double & meanLat, double & meanLon) {
    uint32_t tmpId;
    double lon, lat;
    ifstream in(nodeFile);
    uint32_t ind = 0;
    while (in >> tmpId) {
        in >> lat >> lon;
        nodes.push_back(TNode(tmpId, lat, lon));
        meanLat += lat;
        meanLon += lon;
        ind++;
    }
    meanLat /= nodes.size();
    meanLon /= nodes.size();
    in.close();
}

void GetLandmarks(vector<TNode> & nodes, int & landCount, vector<uint32_t> & landmarks, uint32_t & maxComp, double & meanLat, double & meanLon) {
    vector<uint32_t> sectors(landCount, -1);
    vector<double> dist(landCount, -1);
    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i].components == maxComp) {
            double angle = atan2(nodes[i].lon - meanLon, nodes[i].lat - meanLat);
            if (angle < 0) {
                angle += 2 * M_PI;
            }
            int curSector = (int)(angle * landCount / (2 * M_PI));
            double dist2 = GetDistance(nodes[i].lat, meanLat, nodes[i].lon, meanLon);
            if (dist2 > dist[curSector]) {
                dist[curSector] = dist2;
                sectors[curSector] = i;
            }
        }
    }
    for (size_t i = 0; i < landCount; i++) {
        if (sectors[i] != -1)
            landmarks.push_back(sectors[i]);
    }
}

void CalcLandmarks(vector<TNode> & nodes, vector<pair<uint32_t, uint32_t> > & edges, vector<float> & distance, uint32_t & indStart) {
    distance.resize(nodes.size(), -1);
    priority_queue<pair<float, uint32_t>, vector<pair<float, size_t>>, greater<pair<float, uint32_t>> > q; 
    q.push(make_pair((float)0, indStart));
    distance[indStart] = 0;
    while (!q.empty()) {
        float dist = q.top().first;
        uint32_t cur = q.top().second;
        q.pop();
        if (dist > distance[cur] && distance[cur] > 0)
            continue;
        uint32_t count = (cur == (nodes.size() - 1) ? edges.size() : nodes[cur + 1].index);
        for (uint32_t i = nodes[cur].index; i < count; i++) {
            uint32_t indV = edges[i].second;
            float dist2 = GetDistance(nodes[cur].lat, nodes[indV].lat, nodes[cur].lon, nodes[indV].lon);
            if (distance[indV] == -1 || ((dist2 + dist) < distance[indV])) {
                distance[indV] = dist2 + dist;
                q.push(make_pair(dist2 + dist, indV));
            }
        }
    }
}

void WriteGraph(string & graphFile, vector<TNode> & nodes, vector<pair<uint32_t, uint32_t> > & edges, vector<uint32_t> & landmarks, vector<vector <float> > & landDist, uint32_t & maxComp) {
    ofstream out(graphFile, ofstream::binary);
    size_t nodeS = nodes.size(), edgeS = edges.size(), landS = landmarks.size();
    out.write((char*) & nodeS, sizeof(size_t));
    out.write((char*)(nodes.data()), nodes.size() * sizeof(nodes[0]));
    out.write((char*) & edgeS, sizeof(size_t));
    out.write((char*)edges.data(), edges.size() * sizeof(edges[0]));
    out.write((char*) & landS, sizeof(size_t));
    out.write((char*)landmarks.data(), landmarks.size() * sizeof(landmarks[0]));
    for (size_t i = 0; i < landDist.size(); i++) {
        out.write((char*)landDist[i].data(), landDist[i].size() * sizeof(landDist[i][0]));
    }
    out.write((char*)& maxComp, sizeof(uint32_t));
    out.close();
}

uint32_t Bfs(vector<TNode> & nodes, vector<pair<uint32_t, uint32_t> > & edges, vector<bool> & used, uint32_t & indStart, uint32_t & ind){
    queue<uint32_t> q;
    q.push(indStart);
    used[indStart] = true;
    nodes[indStart].components = ind;
    uint32_t tr = 1;
    while (!q.empty()) {
        uint32_t v = q.front();
        q.pop();
        uint32_t count = (v == (nodes.size() - 1) ? edges.size() : nodes[v + 1].index);
        for (uint32_t i = nodes[v].index; i < count; i++) {
            uint32_t indV = edges[i].second;
            if (!used[indV]) {
                used[indV] = true;
                nodes[indV].components = ind;
                tr++;
                q.push(indV);
            }
        }
    }
    return tr;
}

uint32_t CalcComponents(vector<TNode> & nodes, vector<pair<uint32_t, uint32_t> > & edges) {
    vector<bool> used(nodes.size(), false);
    bool f = false;
    uint32_t count = 0, count1 = 0, ind = 1, indStart = 0, maxComp = 0;
    while (true) {
        if (used[indStart]) {
            while (indStart < used.size() && used[indStart]) {
                indStart++;
            }
            if (indStart == used.size()) {
                break;
            }
        }
        count1 = Bfs(nodes, edges, used, indStart, ind);
        if (count1 > count) {
            maxComp = ind;
            count = count1;
        }
        ind++;
    }
    return maxComp;
}