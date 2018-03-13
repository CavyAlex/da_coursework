#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include "Preprocessing.h"
#include "Searching.h"
#include <stdint.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 7) {
        cout << "Too few parameters";
        return 0;
    }
    int landCount = 12;
    vector<TNode> nodes;
    vector<uint32_t> landmarks;
    vector<vector <float> > landDist;
    vector<pair<uint32_t, uint32_t> > edges;
    uint32_t maxComp = 0;
    if (!strcmp(argv[1], "preprocess")) {
        string nodesFile, edgesFile, graphFile;
        for (int i = 2; i < argc; i += 2) {
            if (!strcmp(argv[i], "--nodes")) {
                nodesFile = argv[i + 1];
            }
            else if (!strcmp(argv[i], "--edges")) {
                edgesFile = argv[i + 1];
            }
            else if (!strcmp(argv[i], "--output")) {
                graphFile = argv[i + 1];
            }
            else {
                cout << "Wrong parameters";
                return 0;
            }
        }
        double meanLat = 0, meanLon = 0;
        ReadNodes(nodesFile, nodes, meanLat, meanLon);
        uint32_t edgCount = CalcEdges(edgesFile, nodes);
        edges.resize(edgCount);
        ReadEdges(edgesFile, nodes, edges);
        maxComp = CalcComponents(nodes, edges);
        GetLandmarks(nodes, landCount, landmarks, maxComp, meanLat, meanLon);
        landDist.resize(landmarks.size());
        for (size_t i = 0; i < landmarks.size(); i++) {
            CalcLandmarks(nodes, edges, landDist[i], landmarks[i]);
        }
        WriteGraph(graphFile, nodes, edges, landmarks, landDist, maxComp);
    }
    else if (!strcmp(argv[1], "search")) {
        string graphFile, inputFile, outputFile;
        bool path = 0;
        for (int i = 2; i < argc; i += 2) {
            if (!strcmp(argv[i], "--graph")) {
                graphFile = argv[i + 1];
            }
            else if (!strcmp(argv[i], "--input")) {
                inputFile = argv[i + 1];
            }
            else if (!strcmp(argv[i], "--output")) {
                outputFile = argv[i + 1];
            }
            else if (!strcmp(argv[i], "--full-output")) {
                path = 1;
                i--;
            }
            else {
                cout << "Wrong parameters";
                return 0;
            }
        }
        ReadGraph(graphFile, nodes, edges, landmarks, landDist, maxComp);
        vector<uint32_t> parent(nodes.size());
        ifstream in(inputFile);
        ofstream out(outputFile);
        double distance = 0;
        uint32_t start, finish;
        while (in >> start) {
            in >> finish;
            int landStart = 0, landFin = 0;
            double d1 = -1, d2 = -1;
            uint32_t indStart = lower_bound(nodes.begin(), nodes.end(), start, Comp) - nodes.begin(),
                indFin = lower_bound(nodes.begin(), nodes.end(), finish, Comp) - nodes.begin();
            if (nodes[indStart].components != nodes[indFin].components) {
                out << -1 << endl;
                if (path) {
                    out << 0 << endl;
                }
                continue;
            }
            if (nodes[indStart].components != maxComp) {
                distance = Search(indStart, indFin, nodes, edges, parent, landDist[landStart], landDist[landFin], false);
            }
            else {
                for (size_t i = 0; i < landmarks.size(); i++) {
                    uint32_t indL = landmarks[i];
                    double d = GetDistance(nodes[indFin].lat, nodes[indL].lat, nodes[indFin].lon, nodes[indL].lon);
                    if (d1 == -1 || d < d1) {
                        d1 = d;
                        landFin = i;
                    }
                    d = GetDistance(nodes[indStart].lat, nodes[indL].lat, nodes[indStart].lon, nodes[indL].lon);
                    if (d2 == -1 || d2 < d) {
                        d2 = d;
                        landStart = i;
                    }
                }
                distance = Search(indStart, indFin, nodes, edges, parent, landDist[landStart], landDist[landFin], true);
            }
            if (distance < 0) {
                out << -1 << endl;
                if (path) {
                    out << 0 << endl;
                }
            }
            else {
                out << std::fixed << std::setprecision(7) << distance << endl;
                if (path) {
                    uint32_t ind = indFin;
                    vector<uint32_t> pAns;
                    while (ind != indStart) {
                        pAns.push_back(nodes[ind].currentId);
                        ind = parent[ind];
                    }
                    pAns.push_back(start);
                    out << pAns.size() << ' ';
                    for (int i = pAns.size() - 1; i >= 0; i--) {
                        out << pAns[i] << ' ';
                    }
                    out << endl;
                }
            }
        }
        in.close();
    }
    return 0;
}