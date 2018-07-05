#ifndef TRENCH_H
#define TRENCH_H

#include <sp2/scene/node.h>

class TrenchPoint
{
public:
    TrenchPoint(double width, double center) : width(width), center(center) {}

    double width;
    double center;
};

class Trench : public sp::Node
{
public:
    Trench(const TrenchPoint& entry, const TrenchPoint& exit, int segments);
    
    static constexpr float segment_lenght = 5;
};

#endif//TRENCH_H
