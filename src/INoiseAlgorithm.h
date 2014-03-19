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


#ifndef INOISEALGORITHM_H
#define INOISEALGORITHM_H

/**
 * Interface for a noise algorithm used for terrain generation.
 **/
class INoiseAlgorithm
{
    public:
        /**
         * Just to make subclasses desctructors work.
         *
         **/
        virtual ~INoiseAlgorithm() {}
        /**
         * Initialise the algorithm
         *
         * @param maxx The maximum range of the X coord
         * @param maxz The maximum range of the Z coord
         * @param rangemin The minimum height
         * @param rangemax The maximum height
         * @return void
         **/
        virtual void init(double maxx, double maxz, double rangemin, double rangemax) = 0;
        
        
        /**
         * Generate a height for an X/Z coordinate.
         *
         * @param x The x coordinate
         * @param z The z coordinate
         * @return double
         **/
        virtual double noise(double x, double z) = 0;
};

#endif // INOISEALGORITHM_H
