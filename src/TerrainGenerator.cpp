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


#include "TerrainGenerator.h"
#include <sys/time.h>
#include <stdlib.h>

TerrainGenerator::TerrainGenerator()
: _terrain(NULL), _heightFieldSize(256), _patchSize(32), _detailLevels(3), _seed(0), _terrainScale(Vector3::one()), _skirtScale(1.0f), _minHeight(0.0f), _maxHeight(80.0f), _isDirty(true)
{
    timeval time;
    gettimeofday(&time, NULL);
    _seed = time.tv_usec;
    
}

bool TerrainGenerator::square(float *heights, float range, unsigned int subdivide, unsigned int arraysize) {
    unsigned int squaresize = arraysize - 1, i = 0, j = 0;
    unsigned int indexTL, indexTR, indexBL, indexBR, indexL, indexT, indexR, indexB;
    for (i = 0; i < subdivide; i++) {
        squaresize /= 2;
    }
    if (squaresize == 1) {
        return false;
    }
    
    for (i = 0; i * squaresize < arraysize-1; i++) {
        for (j = 0; j * squaresize < arraysize-1; j++) {
            indexTL = j * squaresize * arraysize + i * squaresize;
            indexTR = j * squaresize * arraysize + (i + 1) * squaresize;
            indexBL = (j + 1) * squaresize * arraysize + i * squaresize;
            indexTR = (j + 1) * squaresize * arraysize + (i + 1) * squaresize;
            indexL = ((j * squaresize) + (squaresize / 2)) * arraysize + (i * squaresize);
            indexR = ((j * squaresize) + (squaresize / 2)) * arraysize + ((i+1) * squaresize);
            indexT = (j * squaresize) * arraysize + (i * squaresize) + (squaresize / 2);
            indexB = ((j+1) * squaresize) * arraysize + (i * squaresize) + (squaresize / 2);
            
            // Average the sides
            heights[indexT] = (heights[indexTL] + heights[indexTR]) / 2.0f;
            heights[indexL] = (heights[indexTL] + heights[indexBL]) / 2.0f;
            heights[indexB] = (heights[indexBL] + heights[indexBR]) / 2.0f;
            heights[indexR] = (heights[indexTR] + heights[indexBR]) / 2.0f;
            // Add randomness
            heights[indexT] += (this->rand() - 0.5f) * range;
            heights[indexB] += (this->rand() - 0.5f) * range;
            heights[indexL] += (this->rand() - 0.5f) * range;
            heights[indexR] += (this->rand() - 0.5f) * range;
        }
    }
    
    return true;
}

bool TerrainGenerator::diamond(float* heights, float range, unsigned int subdivide, unsigned int arraysize)
{
    unsigned int squaresize = arraysize - 1, i = 0, j = 0;
    unsigned int indexTL, indexTR, indexBL, indexBR, indexC;
    for (i = 0; i < subdivide; i++) {
        squaresize /= 2;
    }
    if (squaresize == 1) {
        return false;
    }
    
    for (i = 0; i * squaresize < arraysize-1; i++) {
        for (j = 0; j * squaresize < arraysize-1; j++) {
            indexTL = j * squaresize * arraysize + i * squaresize;
            indexTR = j * squaresize * arraysize + (i + 1) * squaresize;
            indexBL = (j + 1) * squaresize * arraysize + i * squaresize;
            indexTR = (j + 1) * squaresize * arraysize + (i + 1) * squaresize;
            indexC = ((j * squaresize) + (squaresize / 2)) * arraysize + (i * squaresize) + (squaresize / 2);
            
            // Average the corners
            heights[indexC] = (heights[indexTL] + heights[indexTR] + heights[indexBL] + heights[indexBR]) / 4.0f;
            // Add randomness
            heights[indexC] += (this->rand() - 0.5f) * range;
        }
    }
    
    return true;
}

float TerrainGenerator::rand() {
    return (float)(::rand() % 10000) / 10000.0f;
}

void TerrainGenerator::rebuildTerrain()
{
    SAFE_RELEASE(_terrain);
    
    HeightField *heightField = HeightField::create(_heightFieldSize, _heightFieldSize);
    
    unsigned int subdivide = 1, arraysize = 2;
    while (arraysize + 1 < _heightFieldSize) {
        arraysize *= 2;
    }
    arraysize += 1;
    
    // now an array of size subdivide * subdivide will be just larger than the height field array
    // but will be suitable dimensions for diamond square
    
    float* heights = new float[arraysize * arraysize];
    memset(heights, 0, sizeof(float) * arraysize * arraysize);
    // corners first
    srand(_seed);
    float range = _maxHeight - _minHeight;
    heights[0] = (this->rand() * range) + _minHeight;
    heights[arraysize-1] = (this->rand() * range) + _minHeight;
    heights[arraysize*(arraysize-1) ] = (this->rand() * range) + _minHeight;
    heights[(arraysize*arraysize)-1] = (this->rand() * range) + _minHeight;
    
    
    subdivide = 0;
    while (
        diamond(heights, range, subdivide, arraysize) && 
        square(heights, range, subdivide, arraysize)) {
        
        subdivide += 1;
        range /= 2.0f;
    }
        
    float *usedHeights = heightField->getArray();
    
    unsigned int i, j, k;
    for (i = 0; i < _heightFieldSize; i++) {
        for (j = 0; j < _heightFieldSize; j++) {
            usedHeights[i + (j * _heightFieldSize)] = heights[i + arraysize * j];
        }
    }
    
    // simple smoothing
    for (k = 0; k < 3; k++) {
        for (i = 0; i < _heightFieldSize; i++) {
            for (j = 0; j < _heightFieldSize; j++) {
                usedHeights[i + (j * _heightFieldSize)] = (usedHeights[((i-1) + ((j-1) * _heightFieldSize)) % (_heightFieldSize*_heightFieldSize)] +
                                                        usedHeights[(i + ((j-1) * _heightFieldSize)) % (_heightFieldSize*_heightFieldSize)] +
                                                        usedHeights[((i+1) + ((j-1) * _heightFieldSize)) % (_heightFieldSize*_heightFieldSize)] +
                                                        usedHeights[((i-1) + (j * _heightFieldSize)) % (_heightFieldSize*_heightFieldSize)] +
                                                        usedHeights[(i + (j * _heightFieldSize)) % (_heightFieldSize*_heightFieldSize)] +
                                                        usedHeights[((i+1) + (j * _heightFieldSize)) % (_heightFieldSize*_heightFieldSize)] +
                                                        usedHeights[((i-1) + ((j+1) * _heightFieldSize)) % (_heightFieldSize*_heightFieldSize)] +
                                                        usedHeights[(i + ((j+1) * _heightFieldSize)) % (_heightFieldSize*_heightFieldSize)] +
                                                        usedHeights[((i+1) + ((j+1) * _heightFieldSize)) % (_heightFieldSize*_heightFieldSize)]) / 9.0f;
            }
        }
    }
    
    
    // Copy the heights to the heightfield.
    delete heights;
    
    _terrainScale = Vector3(100, 30, 100);
    
    _terrain = Terrain::create(heightField, 
                               _terrainScale, 
                               _patchSize,
                               _detailLevels, 
                               _skirtScale, 
                               NULL,
                               NULL);
    
    
    SAFE_RELEASE(heightField);
    _isDirty = false;
}

unsigned int TerrainGenerator::getDetailLevels()
{
    return _detailLevels;
}

unsigned int TerrainGenerator::getHeightFieldSize()
{
    return _heightFieldSize;
}

float TerrainGenerator::getMaxHeight()
{
    return _maxHeight;
}

float TerrainGenerator::getMinHeight()
{
    return _minHeight;
}

unsigned int TerrainGenerator::getPatchSize()
{
    return _patchSize;
}

unsigned int TerrainGenerator::getSeed()
{
    return _seed;
}

float TerrainGenerator::getSkirtScale()
{
    return _skirtScale;
}

Vector3 TerrainGenerator::getTerrainScale()
{
    return _terrainScale;
}

void TerrainGenerator::setDetailLevels(unsigned int detailLevels)
{
    _detailLevels = detailLevels;
    _isDirty = true;
}

void TerrainGenerator::setHeightFieldSize(unsigned int heightFieldSize)
{
    _heightFieldSize = heightFieldSize;
    _isDirty = true;
}

void TerrainGenerator::setMaxHeight(float maxHeight)
{
    _maxHeight = maxHeight;
    _isDirty = true;
}

void TerrainGenerator::setMinHeight(float minHeight)
{
    _minHeight = minHeight;
    _isDirty = true;
}

void TerrainGenerator::setPatchSize(unsigned int patchSize)
{
    _patchSize = patchSize;
    _isDirty = true;
}

void TerrainGenerator::setSeed(unsigned int seed)
{
    _seed = seed;
    _isDirty = true;
}

void TerrainGenerator::setSkirtScale(float skirtScale)
{
    _skirtScale = skirtScale;
    _isDirty = true;
}

void TerrainGenerator::setTerrainScale(Vector3 terrainScale)
{
    _terrainScale = terrainScale;
    _isDirty = true;
}



















Terrain* TerrainGenerator::getTerrain()
{
    if (!_terrain || _isDirty) {
        rebuildTerrain();
    }
    return _terrain;
}


TerrainGenerator::~TerrainGenerator()
{
    SAFE_RELEASE(_terrain);
}

