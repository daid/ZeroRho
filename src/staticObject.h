#ifndef STATIC_OBJECT_H
#define STATIC_OBJECT_H

#include <sp2/scene/node.h>

class StaticObject : public sp::Node
{
public:
    StaticObject(sp::string model, float collision_radius, float scale, sp::Vector2d position);
};

#endif//STATIC_OBJECT_H
