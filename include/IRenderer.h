/**
 * @file
 *
 * @section LICENSE
 *
 * OsmAnd - Android navigation software based on OSM maps.
 * Copyright (C) 2010-2013  OsmAnd Authors listed in AUTHORS file
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __I_RENDERER_H_
#define __I_RENDERER_H_

#include <stdint.h>
#include <memory>
#include <functional>
#include <array>

#include <QQueue>
#include <QSet>
#include <QMap>
#include <QMutex>

#include <SkBitmap.h>

#include <OsmAndCore.h>
#include <CommonTypes.h>
#include <TileZoomCache.h>

namespace OsmAnd {

    class IMapTileProvider;

    class OSMAND_CORE_API IRenderer
    {
    public:
        enum TextureDepth {
            _16bits,
            _32bits,
        };
        typedef std::function<void ()> RedrawRequestCallback;
    private:
        void tileReadyCallback(const TileId& tileId, uint32_t zoom, const std::shared_ptr<SkBitmap>& tile);
    protected:
        IRenderer();

        std::shared_ptr<IMapTileProvider> _tileProvider;

        PointI _windowSize;
        AreaI _viewport;
        float _fieldOfView;
        float _fogDistance;
        float _distanceFromTarget;
        float _azimuth;
        float _elevationAngle;
        PointI _target31;
        uint32_t _zoom;
        bool _viewIsDirty;
        QSet<TileId> _visibleTiles;
        PointD _targetInTile;
        bool _tilesCacheInvalidated;
        TextureDepth _preferredTextureDepth;

        QMutex _renderFrameMutex;

        enum {
            TileSide3D = 100,
        };

        virtual void computeMatrices() = 0;
        virtual void refreshVisibleTileset() = 0;
        
        QMutex _tilesCacheMutex;
        struct OSMAND_CORE_API CachedTile : TileZoomCache::Tile
        {
            CachedTile(const uint32_t& zoom, const TileId& id, const size_t& usedMemory);
            virtual ~CachedTile();
        };
        TileZoomCache _tilesCache;
        virtual void purgeTilesCache();
        void cacheMissingTiles();
        virtual void cacheTile(const TileId& tileId, uint32_t zoom, const std::shared_ptr<SkBitmap>& tileBitmap) = 0;

        QMutex _tilesPendingToCacheMutex;
        struct OSMAND_CORE_API TilePendingToCache
        {
            TileId tileId;
            uint32_t zoom;
            std::shared_ptr<SkBitmap> tileBitmap;
        };
        QQueue< TilePendingToCache > _tilesPendingToCacheQueue;
        std::array< QSet< TileId >, 32 > _tilesPendingToCache;
    public:
        virtual ~IRenderer();

        RedrawRequestCallback redrawRequestCallback;

        virtual void setTileProvider(const std::shared_ptr<IMapTileProvider>& tileProvider);
        const std::shared_ptr<IMapTileProvider>& tileProvider;

        const PointI& windowSize;
        const AreaI& viewport;
        const float& fieldOfView;
        const float& fogDistance;
        const float& distanceFromTarget;
        const float& azimuth;
        const float& elevationAngle;
        const PointI& target31;
        const uint32_t& zoom;
        const QSet<TileId>& visibleTiles;

        virtual void setPreferredTextureDepth(TextureDepth depth);
        const TextureDepth& preferredTextureDepth;

        virtual int getCachedTilesCount() const;

        virtual bool updateViewport(const PointI& windowSize, const AreaI& viewport, float fieldOfView, float viewDepth);
        virtual bool updateCamera(float distanceFromTarget, float azimuth, float elevationAngle);
        virtual bool updateMap(const PointI& target31, uint32_t zoom);

        const bool& viewIsDirty;
        virtual void refreshView() = 0;

        virtual void performRendering() = 0;
    };

#if defined(OSMAND_OPENGL_RENDERER_SUPPORTED)
    OSMAND_CORE_API std::shared_ptr<OsmAnd::IRenderer> OSMAND_CORE_CALL createRenderer_OpenGL();
#endif // OSMAND_OPENGL_RENDERER_SUPPORTED
}

#endif // __I_RENDERER_H_