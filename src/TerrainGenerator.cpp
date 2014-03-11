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
#include "LodePNG.h"
#include <sys/time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

TerrainGenerator::TerrainGenerator()
: _terrain(NULL), _heightFieldSize(256), _patchSize(32), _detailLevels(3), _seed(0), _terrainScale(Vector3(1000, 300, 1000)), _skirtScale(1.0f), _minHeight(0.0f), _maxHeight(150.0f), _isDirty(true), _blendResolution(1024)
{
    timeval time;
    gettimeofday(&time, NULL);
    _seed = time.tv_usec;
    
    _layer1BlendFile[0] = '\0';
    _layer2BlendFile[0] = '\0';
    
}

void TerrainGenerator::createTransparentBlendImages()
{
    std::vector<unsigned char> blend1, blend2;
    unsigned int x, z, k, k1, k2, k3, k4;
    float worldx, worldz, worldy, intensity;
    float worldminx = _terrain->getBoundingBox().min.x;
    float worldminz = _terrain->getBoundingBox().min.z;
    float worldmaxx = _terrain->getBoundingBox().max.x;
    float worldmaxz = _terrain->getBoundingBox().max.z;
    float terrainminheight = 0.0f, terrainmaxheight = 0.0f;
    blend1.resize(_blendResolution * _blendResolution * 4);
    blend2.resize(_blendResolution * _blendResolution * 4);
    
     for (x = 0; x < _blendResolution; x++) {
        for (z = 0; z < _blendResolution; z++) {
            k = 4 * x + (z * _blendResolution * 4);
           
            worldx = ((float)x / (float)_blendResolution) * (worldmaxx - worldminx) + worldminx;
            worldz = ((float)z / (float)_blendResolution) * (worldmaxz - worldminz) + worldminz;
            worldy = _terrain->getHeight(worldx, worldz);
            
            if (worldy < terrainminheight || (x == 0 && z == 0)) {
                terrainminheight = worldy;
            }

            if (worldy > terrainmaxheight || (x == 0 && z == 0)) {
                terrainmaxheight = worldy;
            }
            
            // purely slope based
            worldy = _terrain->getHeight(worldx, worldz);
            intensity = abs(_terrain->getHeight(worldx-100, worldz) - worldy);
            intensity += abs(_terrain->getHeight(worldx+100, worldz) - worldy);
            intensity += abs(_terrain->getHeight(worldx, worldz-100) - worldy);
            intensity += abs(_terrain->getHeight(worldx, worldz+100) - worldy);
            intensity /= 400.0f;
            if (intensity > 1.0f) {
                intensity = 1.0f;
            }
            intensity *= 254;
            blend2[k] = intensity;
            blend2[k+1] = intensity;
            blend2[k+2] = intensity;
            blend2[k+3] = intensity;
        }
     }
    
    // Layer 1 is determined purely by the height.
    for (x = 0; x < _blendResolution; x++) {
        for (z = 0; z < _blendResolution; z++) {
            k = 4 * x + (z * _blendResolution * 4);
            
            // purely height based
            worldx = ((float)x / (float)_blendResolution) * (worldmaxx - worldminx) + worldminx;
            worldz = ((float)z / (float)_blendResolution) * (worldmaxz - worldminz) + worldminz;
            worldy = _terrain->getHeight(worldx, worldz);
            worldy -= terrainminheight;
            worldy /= (float)(terrainmaxheight - terrainminheight);
            intensity = 254 * worldy;
            
            blend1[k] = intensity;
            blend1[k+1] = intensity;
            blend1[k+2] = intensity;
            blend1[k+3] = intensity;
            
            // average the slope blends
            k1 = 4 * ((x-1) % _blendResolution) + (z * _blendResolution * 4);
            k2 = 4 * ((x+1) % _blendResolution) + (z * _blendResolution * 4);
            k3 = 4 * (x) + (((z-1) % _blendResolution) * _blendResolution * 4);
            k4 = 4 * (x) + (((z+1) % _blendResolution) * _blendResolution * 4);
            blend2[k] = (blend2[k] + blend2[k1] + blend2[k2] + blend2[k3] + blend2[k4]) / 5.0f;
            blend2[k+1] = blend2[k+2] = blend2[k+3] = blend2[k];
            
        }
    }
    char tmpdir[] = "/tmp/fileXXXXXX";
    mkdtemp(tmpdir);
    
    if (_layer1BlendFile[0] != '\0') {
        remove(_layer1BlendFile);
        remove(_layer2BlendFile);
    }
    
    sprintf(_layer1BlendFile, "%s/blend1.png", tmpdir);
    sprintf(_layer2BlendFile, "%s/blend2.png", tmpdir);
    
    lodepng::encode(_layer1BlendFile, blend1, _blendResolution, _blendResolution);
    
    lodepng::encode(_layer2BlendFile, blend2, _blendResolution, _blendResolution);
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
    
    this->createTransparentBlendImages();
    
    _terrain->setLayer(0, "res/common/terrain/grass.dds", Vector2(50, 50));
    _terrain->setLayer(1, "res/common/terrain/dirt.dds", Vector2(50, 50), _layer1BlendFile, 3);
    _terrain->setLayer(2, "res/common/terrain/rock.dds", Vector2(50, 50), _layer2BlendFile, 3);
    
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
    unsigned int iminus, iplus, jminus, jplus;
 
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
                    iminus = i - 1;
                    iplus = i + 1;
                    jminus = j - 1;
                    jplus = j + 1;
                    if (iminus >= _heightFieldSize) {
                        iminus = 0;
                    }
                    if (jminus >= _heightFieldSize) {
                        jminus = 0;
                    }                    
                    if (iplus >= _heightFieldSize) {
                        iplus = _heightFieldSize - 1;
                    }
                    if (jplus > _heightFieldSize) {
                        jplus = _heightFieldSize - 1;
                    }                    
                    
                    usedHeights[i + (j * _heightFieldSize)] = (usedHeights[iminus + (jminus * _heightFieldSize)] +
                                                            usedHeights[i + (jminus * _heightFieldSize)] +
                                                            usedHeights[iplus + (jminus * _heightFieldSize)] +
                                                            usedHeights[iminus + (j * _heightFieldSize)] +
                                                            usedHeights[i + (j * _heightFieldSize)] +
                                                            usedHeights[iplus + (j * _heightFieldSize)] +
                                                            usedHeights[iminus + (jplus * _heightFieldSize)] +
                                                            usedHeights[i + (jplus * _heightFieldSize)] +
                                                            usedHeights[iplus + (jplus * _heightFieldSize)]) / 9.0f;
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
    unsigned int margin = 100;
    
    _heightField = HeightField::create(_heightFieldSize, _heightFieldSize);
    
    unsigned int subdivide = 1, arraysize = 2;
    while (arraysize + 1 < (_heightFieldSize + 2*margin)) {
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
            usedHeights[i + (j * _heightFieldSize)] = heights[(i+margin) + arraysize * (j+margin)];
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

