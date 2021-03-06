#include "Frustum2D31.h"

bool OsmAnd::Frustum2D31::test(const PointI& p_) const
{
    AreaI64 bbox;
    bbox.topLeft = bbox.bottomRight = p0;
    bbox.enlargeToInclude(p1);
    bbox.enlargeToInclude(p2);
    bbox.enlargeToInclude(p3);
    const auto tilesCount = static_cast<int64_t>(1ull << ZoomLevel31);
    const auto xMinK = qFloor(static_cast<double>(bbox.left()) / tilesCount);
    const auto xMaxK = qCeil(static_cast<double>(bbox.right()) / tilesCount);
    const auto yMinK = qFloor(static_cast<double>(bbox.top()) / tilesCount);
    const auto yMaxK = qCeil(static_cast<double>(bbox.bottom()) / tilesCount);

    // 3+ repeats for world include any point in world
    if (qAbs(xMaxK - xMinK) >= 3 && qAbs(yMaxK - yMinK) >= 3)
        return true;

    PointI64 dP;
    const PointI64 p(p_);
    for (auto xK = xMinK; xK <= xMaxK; xK++)
    {
        dP.x = tilesCount * xK;
        for (auto yK = yMinK; yK <= yMaxK; yK++)
        {
            dP.y = tilesCount * yK;
            if (Frustum2DI64::test(p + dP))
                return true;
        }
    }

    return false;
}

bool OsmAnd::Frustum2D31::test(const PointI& lp0_, const PointI& lp1_) const
{
    AreaI64 bbox;
    bbox.topLeft = bbox.bottomRight = p0;
    bbox.enlargeToInclude(p1);
    bbox.enlargeToInclude(p2);
    bbox.enlargeToInclude(p3);
    const auto tilesCount = static_cast<int64_t>(1ull << ZoomLevel31);
    const auto xMinK = qFloor(static_cast<double>(bbox.left()) / tilesCount);
    const auto xMaxK = qCeil(static_cast<double>(bbox.right()) / tilesCount);
    const auto yMinK = qFloor(static_cast<double>(bbox.top()) / tilesCount);
    const auto yMaxK = qCeil(static_cast<double>(bbox.bottom()) / tilesCount);

    // 3+ repeats for world include any point in world
    if (qAbs(xMaxK - xMinK) >= 3 && qAbs(yMaxK - yMinK) >= 3)
        return true;

    PointI64 dP;
    const PointI64 lp0(lp0_);
    const PointI64 lp1(lp1_);
    for (auto xK = xMinK; xK <= xMaxK; xK++)
    {
        dP.x = tilesCount * xK;
        for (auto yK = yMinK; yK <= yMaxK; yK++)
        {
            dP.y = tilesCount * yK;
            if (Frustum2DI64::test(lp0 + dP, lp1 + dP))
                return true;
        }
    }

    return false;
}

bool OsmAnd::Frustum2D31::test(const QVector<PointI>& path) const
{
    if (path.isEmpty())
        return false;

    const auto pathSize = path.size();
    if (pathSize == 1)
        return test(path.first());
    if (pathSize == 2)
        return test(path.first(), path.last());

    auto pPoint = path.constData();
    auto pPrevPoint = pPoint++;
    for (auto idx = 1; idx < pathSize; idx++)
    {
        if (test(*(pPrevPoint++), *(pPoint++)))
            return true;
    }

    return false;
}
