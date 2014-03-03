/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2014  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include "gameplay.h"

using namespace gameplay;

class TerrainGenerator
{
public:
    TerrainGenerator();
    
    void rebuildTerrain();
    void updateTerrain();
    Terrain * getTerrain();
    
    ~TerrainGenerator();
    
    void setHeightFieldSize(unsigned int heightFieldSize);
    unsigned int getHeightFieldSize();
    void setPatchSize(unsigned int patchSize);
    unsigned int getPatchSize();
    void setDetailLevels(unsigned int detailLevels);
    unsigned int getDetailLevels();
    void setSeed(unsigned int seed);
    unsigned int getSeed();
    void setTerrainScale(Vector3 terrainScale);
    Vector3 getTerrainScale();
    void setSkirtScale(float skirtScale);
    float getSkirtScale();
    void setMinHeight(float minHeight);
    float getMinHeight();
    void setMaxHeight(float maxHeight);
    float getMaxHeight();
    
    void flatten(float x, float z, float scale);
    void raise(float x, float z, float scale);
    void lower(float x, float z, float scale);
    void smooth(float x, float z, float scale);
    float average(float x, float z, float scale);
private:
    float distanceFromCenter(float x, float z, float centerX, float centerZ);
    const Matrix& getInverseWorldMatrix() const;
    bool diamond(float *heights, float range, unsigned int subdivide, unsigned int size);
    bool square(float *heights, float range, unsigned int subdivide, unsigned int size);
    float rand();
    Terrain *_terrain;
    HeightField *_heightField;
    
    unsigned int _heightFieldSize, _patchSize, _detailLevels, _seed;
    Vector3 _terrainScale;
    float _skirtScale, _minHeight, _maxHeight;
    bool _isDirty;
    mutable Matrix _inverseWorldMatrix;
};

#endif // TERRAINGENERATOR_H
