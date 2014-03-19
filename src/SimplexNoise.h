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


#ifndef SIMPLEXNOISE_H
#define SIMPLEXNOISE_H

#include "INoiseAlgorithm.h"

#include "gameplay.h"

using namespace gameplay;

/**
 * Noise generation algotirm based on simplex noise.
 * This C++ version is based on the java version at: http://webstaff.itn.liu.se/~stegu/simplexnoise/SimplexNoise.java by Stefan Gustavson.
 **/
class SimplexNoise : public INoiseAlgorithm
{
    public:
        /**
         * Constructor
         **/
        SimplexNoise();
        /**
         * Initialiser - store the parameters for the noise generation.
         *
         * @param maxx Maximum X value this generator will be called with.
         * @param maxz Maximum Z value this generator will be called with.
         * @param rangemin The minumum height value to generate.
         * @param rangemax The maximum height value to generate.
         * @return void
         **/
        virtual void init(double maxx, double maxz, double rangemin, double rangemax);
        
        /**
         * This is the business function - get the height value for a given x and z coordinate.
         *
         * @param x X coordinate
         * @param z Z coordinate
         * @return double
         **/
        virtual double noise(double x, double z);
        
        /**
         * Destructor...
         *
         **/
        virtual ~SimplexNoise();
    private:
        /**
         * Get a raw noise value, for a single scale of simplex noise. Simplex noise is designed to generate smooth random values
         * over a given scale. The way to get a fractal looking terrain out of this, is to call this noise function several
         * times with different scales of x and z and sum the results with adjusted weightings. This gives a range of large,
         * medium and small features.
         *
         * @param x X coordinate
         * @param z Z coordinate
         * @return double
         **/
        double noiseSingle(double x, double z);
        
        /**
         * Dot product of just the x and z vectors
         *
         * @param v Vector
         * @param x x scale
         * @param z z scale
         * @return double
         **/
        double dot( const Vector3 & v, double x, double z );
        
        /**
         * Constant used in noise generation.
         **/
        static const double F2;
        
        /**
         * Constant used in noise generation.
         **/
        static const double G2;
        
        /**
         * Unit vectors in all directions of a cube.
         **/
        static const Vector3 v3[12];
        
        /**
         * Precomputed noise functions describing a smooth curve.
         **/
        static const unsigned char p[256];
        
        /**
         * More noise constants
         **/
        static unsigned char perm[512];
        
        /**
         * Even more noise constants
         **/
        static unsigned char permMod12[512];
        
        /**
         * The minumum allowed height value.
         **/
        double _min;
        
        /**
         * The max allowed height value.
         **/
        double _max;
        
        /**
         * The range is used to determine how many iterations to make of the noise function.
         **/
        double _worldScale;
};

#endif // SIMPLEXNOISE_H
