/*
    Terrain Tool - Interactive terrain editor.
    Copyright (C) 2014  Damyon Wiese <damyon.wiese@gmail.com>

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

/**
 * This class generates a random terrain from the given parameters.
 **/
class TerrainGenerator
{
public:
    /**
     * Constructor...
     *
     **/
    TerrainGenerator();
    
    /**
     * Called to generate a new random terrain from the given values.
     *
     * @return void
     **/
    void buildTerrain();
    
    /**
     * Called to update the terrain after the heightmap has been modified.
     *
     * @return void
     **/
    void updateTerrain();
    
    /**
     * Used to get the current terrain object. Callers should not store a reference to this terrain
     * because it will be deleted and a new terrain generated when the heightmap is modified.
     *
     * @return Terrain*
     **/
    Terrain * getTerrain();
    
    /**
     * Destructor
     *
     **/
    ~TerrainGenerator();
    
    /**
     * Set the size of one side of the height field.
     *
     * @param heightFieldSize ...
     * @return void
     **/
    void setHeightFieldSize(unsigned int heightFieldSize);
    
    /**
     * Get the size of one size of the height field.
     *
     * @return unsigned int
     **/
    unsigned int getHeightFieldSize();
    
    /**
     * Set the patch size for the terrain. Will take effect when the terrain is rebuilt.
     *
     * @param patchSize the new patch size.
     * @return void
     **/
    void setPatchSize(unsigned int patchSize);
    
    /**
     * Get the patch size for the terrain.
     *
     * @return unsigned int
     **/
    unsigned int getPatchSize();
    
    /**
     * Set the number of details levels for the terrain. Will take effect when the terrain gets rebuilt.
     *
     * @param detailLevels ...
     * @return void
     **/
    void setDetailLevels(unsigned int detailLevels);
    
    /**
     * Get the number of detail levels for the terrain.
     *
     * @return unsigned int
     **/
    unsigned int getDetailLevels();
    
    /**
     * Set the seed for the random number generator. This is used to regenerate the same random terrain.
     *
     * @return unsigned int
     **/
    void setSeed(unsigned int seed);
    
    /**
     * Get the seed for the random number generator.
     *
     * @return unsigned int
     **/
    unsigned int getSeed();
    
    /**
     * Set the scale vector for the terrain. Will take effect when the terrain is rebuilt.
     *
     * @param terrainScale 
     * @return void
     **/
    void setTerrainScale(Vector3 terrainScale);
    
    /**
     * Get the current scale vector.
     *
     * @return :Vector3
     **/
    Vector3 getTerrainScale();
    
    /**
     * Set the size of the vertical skirts that prevent gaps in the terrain. Will take effect when
     * the terrain is rebuilt.
     *
     * @param terrainScale 
     * @return void
     **/
    void setSkirtScale(float skirtScale);
    
    /**
     * Get the current size of the vertical skirts that prevent gaps in the terrain.
     *
     * @return float
     **/
    float getSkirtScale();
    
    /**
     * Set the minimum bounds for the height value for the generated terrain.
     *
     * @param minHeight
     * @return void
     **/
    void setMinHeight(float minHeight);
    
    /**
     * Get the minimum bounds for the height value for the generated terrain.
     *
     * @return float
     **/
    float getMinHeight();
    
    /**
     * Set the maximum  bounds for the height value for the generated terrain.
     *
     * @param minHeight
     * @return void
     **/
    void setMaxHeight(float maxHeight);
    
    /**
     * Get the maximum bounds for the height value for the generated terrain.
     *
     * @return float
     **/
    float getMaxHeight();
    
    /**
     * Flatten a circle of the terrain. The contents of the circle are all brought to the average - and some smoothing is done to the edges.
     *
     * @param x x coordinate for the center of the circle.
     * @param z z coordinate for the center of the circle.
     * @param scale the radius of the circle
     * @return void
     **/
    void flatten(float x, float z, float scale);
    
    /**
     * Raise a circle of the terrain. The shape created is something like an egg.
     *
     * @param x x coordinate for the center of the circle.
     * @param z z coordinate for the center of the circle.
     * @param scale the radius of the circle
     * @return void
     **/
    void raise(float x, float z, float scale);
    
    /**
     * Lower a circle of the terrain. The shape created is something like an egg (inverted).
     *
     * @param x x coordinate for the center of the circle.
     * @param z z coordinate for the center of the circle.
     * @param scale the radius of the circle
     * @return void
     **/
    void lower(float x, float z, float scale);
    
    /**
     * Smooth the contents of a circle on the terrain.
     *
     * @param x x coordinate for the center of the circle.
     * @param z z coordinate for the center of the circle.
     * @param scale the radius of the circle
     * @return void
     **/
    void smooth(float x, float z, float scale);
    
    /**
     * Helper method to compute the average height for a circle in the terrain.
     *
     * @param x x coordinate for the center of the circle.
     * @param z z coordinate for the center of the circle.
     * @param scale the radius of the circle
     * @return float
     **/
    float average(float x, float z, float scale);
    
private:
    /**
     * The file path of the first texture blend map.
     **/
    char _layer1BlendFile[2048];
    
    /**
     * The file path of the second texture blend map.
     **/
    char _layer2BlendFile[2048];
    
    /**
     * Calculate the real world distance between 2 points. (excluding the y axis)
     **/
    float distanceFromCenter(float x, float z, float centerX, float centerZ);
    
    /**
     * Used to map from heightmap coordinates to real world coordinates.
     **/
    const Matrix& getInverseWorldMatrix() const;
    
    /**
     * Generate new blend images for the texture mapping. The blend maps are based on characteristics of the terrain like height or slope.
     **/
    void createTransparentBlendImages();
     
    /**
     * The current terrain object.
     **/
    Terrain *_terrain;
    
    /**
     * The heightmap that generated the terrain.
     **/
    HeightField *_heightField;
    
    /**
     * The resolution of the texture blend maps (they are square)
     **/
    unsigned int _blendResolution;
    
    /**
     * The resolution of the height map.
     **/
    unsigned int _heightFieldSize;
    
    /**
     * The patch size for the generated terrain.
     **/
    unsigned int _patchSize;
    
    /**
     * The number of detail levels to use in the terrain.
     **/
    unsigned int _detailLevels;
    
    /**
     * Random seed that gets set before generating the random terrain.
     **/
    unsigned int _seed;
    
    /**
     * Scale of the terrain.
     **/
    Vector3 _terrainScale;
    
    /**
     * Vertical skirt height.
     **/
    float _skirtScale;
    
    /**
     * Minimum bounds for height values in the terrain.
     **/
    float _minHeight;
    
    /**
     * Maximum bounds for height values in the terrain.
     **/
    float _maxHeight;
    
    /**
     * Triggering isDirty causes the terrain to be regenerated.
     **/
    bool _isDirty;
    
    /**
     * Used to map coordinates.
     **/
    mutable Matrix _inverseWorldMatrix;
};

#endif // TERRAINGENERATOR_H
