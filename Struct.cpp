#include "Struct.h"
#include <stdint.h>
TNode::TNode(uint32_t & id, double & lat1, double & lon1) {
    currentId = id;
    lat = lat1;
    lon = lon1;
    index = 0;
    components = 0;
}

bool Comp(TNode n, uint32_t id) {
    return (id > n.currentId) ? true : false;
}


double GetDistance(double & lat1, double & lat2, double & lon1, double &lon2) {
    double d = 2 * asin(sqrt(sin(fabs(lat1 - lat2) * M_PI / 360) * sin(fabs(lat1 - lat2) * M_PI / 360) +
        cos(lat1 * M_PI / 180) * cos(lat2 * M_PI / 180) * sin(fabs(lon1 - lon2) * M_PI / 360) * sin(fabs(lon1 - lon2) * M_PI / 360)));
    return 6371000 * d;
}