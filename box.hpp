#ifndef __BOX__
#define __BOX__

#include "raytracer.h"
#include "hittable.h"
#include "hittablelist.hpp"

class Box : public Hittable
{
private:
    Point3 box_min;
    Point3 box_max;
    HittableList sides;

    public:
    Box(){}
    Box(const Point3& p0, const Point3& p1, shared_ptr<Material> ptr);

    
};

#endif