#ifndef VECTOR3_H
#define VECTOR3_H

class MyHashFunction
{
public:
    // Use sum of lengths of first and last names
    // as hash function.
    size_t operator()(const glm::vec3 &v) const
    {
        return v.x + v.y + v.z;
    }
};

#endif