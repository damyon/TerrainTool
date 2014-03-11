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


#ifndef DIAMONDSQUARENOISE_H
#define DIAMONDSQUARENOISE_H
#include "INoiseAlgorithm.h"

class DiamondSquareNoise : public INoiseAlgorithm
{
    private:
        double* _heights;
        unsigned int _size;
        double _min, _max;
        double rand();
        bool diamond(unsigned int subdivide);
        bool square(unsigned int subdivide);
    public:
        DiamondSquareNoise();
        virtual void init(double maxx, double maxz, double rangemin, double rangemax);
        virtual double noise(double x, double z);
        virtual ~DiamondSquareNoise();
};

#endif // DIAMONDSQUARENOISE_H
