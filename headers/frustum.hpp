#pragma once
#include "const.hpp"
#include "../glm/glm.hpp"
#include "chunk.hpp"

class Frustum
{
public:
    //! TYPEDEF/ENUMS:
    enum Plane
    {
        Right,
        Left,
        Bottom,
        Top,
        Front,
        Back
    };
    enum
    {
        A,
        B,
        C,
        D
    };

    //! CTOR/DTOR:
    Frustum();
    virtual ~Frustum();

    //! SERVICES:
    void Transform(const glm::mat4 &proj, const glm::mat4 &view);
    void Normalize(Plane plane);

    //! CULLING:
    enum Visibility
    {
        Completly,
        Partially,
        Invisible
    };
    // Visibility IsInside(const glm::vec3 &point) const;
    Visibility IsInside(const glm::vec3 v) const;

    //! ACCESSORS:
    glm::dvec4 GetPlane(Plane plane) const;

private:
    //! MEMBERS:
    double m_data[6][4];
};

////////////////////////////////////////////////////////////////////////////////
// Frustum inline implementation:
////////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------------*/
inline Frustum::Frustum()
{
}
/*----------------------------------------------------------------------------*/
inline Frustum::~Frustum()
{
}

/*----------------------------------------------------------------------------*/
inline glm::dvec4 Frustum::GetPlane(Plane plane) const
{
    return glm::dvec4(m_data[plane][A], m_data[plane][B], m_data[plane][C], m_data[plane][D]);
}
