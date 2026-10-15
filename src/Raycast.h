#include <algorithm>
#include <cmath>
#include <limits>

#include <QVector3D>

#include "AABB.h"

inline bool rayAABB(QVector3D origin, QVector3D dir, AABB box)
{
    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::infinity();

    auto testAxis = [&](float o, float d, float min, float max)
    {
        if (std::abs(d) < 0.00001f)
            return o >= min && o <= max;

        float t1 = (min - o) / d;
        float t2 = (max - o) / d;

        if (t1 > t2)
            std::swap(t1, t2);

        tMin = std::max(tMin, t1);
        tMax = std::min(tMax, t2);

        return tMin <= tMax;
    };

    return testAxis(origin.x(), dir.x(), box.min.x(), box.max.x())
        && testAxis(origin.y(), dir.y(), box.min.y(), box.max.y())
        && testAxis(origin.z(), dir.z(), box.min.z(), box.max.z());
}