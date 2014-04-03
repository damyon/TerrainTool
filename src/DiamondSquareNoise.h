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


#ifndef DIAMONDSQUARENOISE_H
#define DIAMONDSQUARENOISE_H
#include "INoiseAlgorithm.h"

/**
 * @brief Diamond square noise algorithm for terrain generation.
 **/
class DiamondSquareNoise : public INoiseAlgorithm
{
    private:
        /**
         * @brief Array of precalculated heights.
         **/
        double* _heights;
        /**
         * @brief The size of one side of the heights array (it's a square)
         **/
        unsigned int _size;
        /**
         * @brief The minimum allowed height
         **/
        double _min;
        
        /**
         * @brief The maximum allowed height
         **/
        
        double _max;
        
        /**
         * @brief A margin is used because the edges of the heights array can look warped (because they get averaged with the other side).
         **/
        double _margin;
        
        /**
         * @brief Return a random number between _max and _min
         *
         * @return double
         **/
        double rand();
        
        /**
         * @brief The diamond part of diamond square. Make the center of each square = the average of the corners + some randomness.
         *
         * @param subdivide The iteration count we are up to
         * @return bool
         **/
        bool diamond(unsigned int subdivide);
        
        /**
         * @brief The square part of diamond square. Make the center of each side of the square the average of the 2 corners that connect to it.
         *
         * @param subdivide The iteration count
         * @return bool
         **/
        bool square(unsigned int subdivide);
        
        /**
         * @brief Apply a 9x9 average across all the generated values.
         *
         * @return void
         **/
        void smooth();
    public:
        /**
         * @brief Constructor
         *
         **/
        DiamondSquareNoise();
        
        /**
         * @brief Initialise this noise generator.
         *
         * @param maxx The maximum X value that this generator will ever be queried for
         * @param maxz The maximum Z value that this generator will ever be queried for
         * @param rangemin The minimum height value to return
         * @param rangemax The maximum height value to return
         * @param seed A random seed
         * @return void
         **/
        virtual void init(double maxx, double maxz, double rangemin, double rangemax, int seed);
        
        /**
         * @brief Return one of the precalculated noise values matching the x and z position.
         *
         * @param x The X coordinate to get the value for
         * @param z The Z coordinate to get the value for
         * @return double The noise value matching these coordinates
         **/
        virtual double noise(double x, double z);
        
        /**
         * @brief Delete the precomputed height array
         *
         **/
        virtual ~DiamondSquareNoise();
};

#endif // DIAMONDSQUARENOISE_H
