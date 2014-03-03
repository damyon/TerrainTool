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
#include <math.h>
#include <stdlib.h>

TerrainGenerator::TerrainGenerator()
: _terrain(NULL), _heightFieldSize(256), _patchSize(32), _detailLevels(3), _seed(0), _terrainScale(Vector3(1000, 300, 1000)), _skirtScale(1.0f), _minHeight(0.0f), _maxHeight(150.0f), _isDirty(true)
{
    timeval time;
    gettimeofday(&time, NULL);
    _seed = time.tv_usec;
    
}

const Matrix& TerrainGenerator::getInverseWorldMatrix() const
{
    _inverseWorldMatrix.set(_terrain->getNode()->getWorldMatrix());
    
    // Apply local scale and invert
    _inverseWorldMatrix.scale(_terrainScale);
    _inverseWorldMatrix.invert();
    
    return _inverseWorldMatrix;
}

void TerrainGenerator::flatten(float x, float z, float scale)
{
    float cols = _heightField->getColumnCount();
    float rows = _heightField->getRowCount();
    float *usedHeights = _heightField->getArray();
    unsigned int i, j, repeats;
 
    GP_ASSERT(cols > 0);
    GP_ASSERT(rows > 0);

    // Since the specified coordinates are in world space, we need to use the 
    // inverse of our world matrix to transform the world x,z coords back into
    // local heightfield coordinates for indexing into the height array.
    Vector3 v = getInverseWorldMatrix() * Vector3(x, 0.0f, z);
    Vector3 s = getInverseWorldMatrix() * Vector3(scale, 0.0f, 0.0f);
    float average = this->average(x, z, scale);
   
    float localx = v.x + (cols - 1) * 0.5f;
    float localz = v.z + (rows - 1) * 0.5f;
    float localscale = s.x;
  
    for (i = 0; i < _heightFieldSize; i++) {
        for (j = 0; j < _heightFieldSize; j++) {
            float dist = this->distanceFromCenter((float)i, (float)j, localx, localz);
            
            float strength = (localscale*0.9f) - dist;
            
            if (strength > 0) {
                usedHeights[i + (j * _heightFieldSize)] = average;
            }
        }
    }


    this->smooth(x, z, scale);
    
}

float TerrainGenerator::average(float x, float z, float scale)
{
    float cols = _heightField->getColumnCount();
    float rows = _heightField->getRowCount();
    float *usedHeights = _heightField->getArray();
    unsigned int i, j, repeats;
 
    GP_ASSERT(cols > 0);
    GP_ASSERT(rows > 0);

    // Since the specified coordinates are in world space, we need to use the 
    // inverse of our world matrix to transform the world x,z coords back into
    // local heightfield coordinates for indexing into the height array.
    Vector3 v = getInverseWorldMatrix() * Vector3(x, 0.0f, z);
    Vector3 s = getInverseWorldMatrix() * Vector3(scale, 0.0f, 0.0f);
    
    float localx = v.x + (cols - 1) * 0.5f;
    float localz = v.z + (rows - 1) * 0.5f;
    float localscale = s.x;
  
    float average = 0.0f;
    unsigned int count = 0;

    for (i = 0; i < _heightFieldSize; i++) {
        for (j = 0; j < _heightFieldSize; j++) {
            float dist = this->distanceFromCenter((float)i, (float)j, localx, localz);
            
            float strength = localscale - dist;
            
            if (strength > 0) {
                average += usedHeights[i + (j * _heightFieldSize)];
                count++;
            }
        }
    }

    if (count == 0) {
        count = 1;
    }
    return average / count;
}


void TerrainGenerator::updateTerrain()
{
    Node *node = NULL;
    
    if (_terrain) {
        node = _terrain->getNode();
    }
    // SAFE_RELEASE(_terrain);
    _terrain = Terrain::create(_heightField, 
                               _terrainScale, 
                               _patchSize,
                               _detailLevels, 
                               _skirtScale, 
                               NULL,
                               NULL);
    
    
    if (node) {
        node->setTerrain(_terrain);
    }
}


void TerrainGenerator::smooth(float x, float z, float scale)
{
    float cols = _heightField->getColumnCount();
    float rows = _heightField->getRowCount();
    float *usedHeights = _heightField->getArray();
    unsigned int i, j, repeats;
 
    GP_ASSERT(cols > 0);
    GP_ASSERT(rows > 0);

    // Since the specified coordinates are in world space, we need to use the 
    // inverse of our world matrix to transform the world x,z coords back into
    // local heightfield coordinates for indexing into the height array.
    Vector3 v = getInverseWorldMatrix() * Vector3(x, 0.0f, z);
    Vector3 s = getInverseWorldMatrix() * Vector3(scale, 0.0f, 0.0f);
    
    float localx = v.x + (cols - 1) * 0.5f;
    float localz = v.z + (rows - 1) * 0.5f;
    float localscale = s.x;
  
    for (repeats = 0; repeats < 2; repeats++) {
        for (i = 0; i < _heightFieldSize; i++) {
            for (j = 0; j < _heightFieldSize; j++) {
                float dist = this->distanceFromCenter((float)i, (float)j, localx, localz);
                
                float strength = localscale - dist;
                
                if (strength > 0) {
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
    }
    
    this->updateTerrain();
}

float TerrainGenerator::distanceFromCenter(float x, float z, float centerx, float centerz) {
    return sqrtf(powf(x - centerx, 2.0f) + powf(z - centerz, 2.0f));
    
}

void TerrainGenerator::lower(float x, float z, float scale)
{
    float cols = _heightField->getColumnCount();
    float rows = _heightField->getRowCount();
    float *usedHeights = _heightField->getArray();
    unsigned int i, j, repeats;
 
    GP_ASSERT(cols > 0);
    GP_ASSERT(rows > 0);

    // Since the specified coordinates are in world space, we need to use the 
    // inverse of our world matrix to transform the world x,z coords back into
    // local heightfield coordinates for indexing into the height array.
    Vector3 v = getInverseWorldMatrix() * Vector3(x, 0.0f, z);
    Vector3 s = getInverseWorldMatrix() * Vector3(scale, 0.0f, 0.0f);
    
    float localx = v.x + (cols - 1) * 0.5f;
    float localz = v.z + (rows - 1) * 0.5f;
    float localscale = s.x;
  
    for (i = 0; i < _heightFieldSize; i++) {
        for (j = 0; j < _heightFieldSize; j++) {
            float dist = this->distanceFromCenter((float)i, (float)j, localx, localz);
            
            float strength = localscale - dist;
            
            if (strength > 0) {
                usedHeights[i + (j * _heightFieldSize)] -= strength;
            }
        }
    }

    //this->smooth(x, z, scale);
    this->updateTerrain();
}

void TerrainGenerator::raise(float x, float z, float scale)
{
    float cols = _heightField->getColumnCount();
    float rows = _heightField->getRowCount();
    float *usedHeights = _heightField->getArray();
    unsigned int i, j, repeats;
 
    GP_ASSERT(cols > 0);
    GP_ASSERT(rows > 0);

    // Since the specified coordinates are in world space, we need to use the 
    // inverse of our world matrix to transform the world x,z coords back into
    // local heightfield coordinates for indexing into the height array.
    Vector3 v = getInverseWorldMatrix() * Vector3(x, 0.0f, z);
    Vector3 s = getInverseWorldMatrix() * Vector3(scale, 0.0f, 0.0f);
    
    float localx = v.x + (cols - 1) * 0.5f;
    float localz = v.z + (rows - 1) * 0.5f;
    float localscale = s.x;
  
    for (i = 0; i < _heightFieldSize; i++) {
        for (j = 0; j < _heightFieldSize; j++) {
            float dist = this->distanceFromCenter((float)i, (float)j, localx, localz);
            
            float strength = localscale - dist;
            
            if (strength > 0) {
                usedHeights[i + (j * _heightFieldSize)] += strength;
            }
        }
    }

    //this->smooth(x, z, scale);
    this->updateTerrain();
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
    //SAFE_RELEASE(_terrain);
    
    _heightField = HeightField::create(_heightFieldSize, _heightFieldSize);
    
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
        
    float *usedHeights = _heightField->getArray();
    
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
    
    this->updateTerrain();
    
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
    SAFE_RELEASE(_heightField);
}

