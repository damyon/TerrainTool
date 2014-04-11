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


#include "DiamondSquareNoise.h"
#ifdef WIN32
#define _USE_MATH_DEFINES
#include <time.h>
#include <cstdlib>
#endif
#include <math.h>
#include <string.h>
#include <stdlib.h>


double DiamondSquareNoise::rand()
{
#ifdef __APPLE__ || TARGET_OS_MAC || TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || UNIX || LINUX || __MINGW32__ || __GNUC__
	return _min + static_cast <double> (random()) /( static_cast <double> (RAND_MAX/(_max-_min)));
#elif WIN32
	srand(time(0));
	return _min + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (_max - _min)));
#endif

}


DiamondSquareNoise::DiamondSquareNoise() :
_heights(NULL), _size(0), _min(0), _max(0), _margin(100)
{

}


double DiamondSquareNoise::noise(double x, double z)
{
    return _heights[(int)(x + _margin) + ((int)(z + _margin) * _size)];
}


void DiamondSquareNoise::init(double maxx, double maxz, double rangemin, double rangemax, int seed)
{
    double range;
    
    srand(seed);
    _size = (unsigned int) fmax(maxx, maxz);
    _min = rangemin;
    _max = rangemax;
    // Determine the smallest power of 2+1 that fits our requested array size + margins.
    
    unsigned int subdivide = 1, arraysize = 2;
    while (arraysize + 1 < (_size + 2*_margin)) {
        arraysize *= 2;
    }
    arraysize += 1;
    _size = arraysize;
    
    _heights = new double[_size * _size];
    memset(_heights, 0, sizeof(double) * _size * _size);
    
    // Initialise the corners.
    _heights[0] = this->rand();
    _heights[_size-1] = this->rand();
    _heights[_size*(_size-1) ] = this->rand();
    _heights[(_size*_size)-1] = this->rand();
    
    // Now to the iterations until we are down to a square size of 1x1.
    subdivide = 0;
    while (
        this->diamond(subdivide) && 
        this->square(subdivide)) {
        
        subdivide += 1;
    }

    // Now apply some smoothing to make it look a bit nicer.
    this->smooth();

}


void DiamondSquareNoise::smooth()
{
    int k, i, j, size2 = _size * _size;
    // simple smoothing
    for (k = 0; k < 2; k++) {
        for (i = 0; i < _size; i++) {
            for (j = 0; j < _size; j++) {
                _heights[i + (j * _size)] = (_heights[((i-1) + ((j-1) * _size)) % (size2)] +
                                                        _heights[(i + ((j-1) * _size)) % (size2)] +
                                                        _heights[((i+1) + ((j-1) * _size)) % (size2)] +
                                                        _heights[((i-1) + (j * _size)) % (size2)] +
                                                        _heights[(i + (j * _size)) % (size2)] +
                                                        _heights[((i+1) + (j * _size)) % (size2)] +
                                                        _heights[((i-1) + ((j+1) * _size)) % (size2)] +
                                                        _heights[(i + ((j+1) * _size)) % (size2)] +
                                                        _heights[((i+1) + ((j+1) * _size)) % (size2)]) / 9.0f;
            }
        }
    }

}


bool DiamondSquareNoise::diamond(unsigned int subdivide)
{
    unsigned int squaresize = _size - 1, i = 0, j = 0;
    unsigned int indexTL, indexTR, indexBL, indexBR, indexC;
	double halfRange = (_max - _min) / (2 * pow(2, (float)subdivide - 1));
    for (i = 0; i < subdivide; i++) {
        squaresize /= 2;
    }
    if (squaresize == 1) {
        return false;
    }
    
    for (i = 0; i * squaresize < _size-1; i++) {
        for (j = 0; j * squaresize < _size-1; j++) {
            indexTL = j * squaresize * _size + i * squaresize;
            indexTR = j * squaresize * _size + (i + 1) * squaresize;
            indexBL = (j + 1) * squaresize * _size + i * squaresize;
            indexTR = (j + 1) * squaresize * _size + (i + 1) * squaresize;
            indexC = ((j * squaresize) + (squaresize / 2)) * _size + (i * squaresize) + (squaresize / 2);
            
            // Average the corners
            _heights[indexC] = (_heights[indexTL] + _heights[indexTR] + _heights[indexBL] + _heights[indexBR]) / 4.0f;
            // Add randomness
			_heights[indexC] += (this->rand() / pow(2, (float)subdivide - 1)) - halfRange;
        }
    }
    
    return true;
}


bool DiamondSquareNoise::square(unsigned int subdivide)
{
    unsigned int squaresize = _size - 1, i = 0, j = 0;
    unsigned int indexTL, indexTR, indexBL, indexBR, indexL, indexT, indexR, indexB;
	double halfRange = (_max - _min) / (2 * pow(2, (float)subdivide - 1));
    for (i = 0; i < subdivide; i++) {
        squaresize /= 2;
    }
    if (squaresize == 1) {
        return false;
    }
    
    for (i = 0; i * squaresize < _size-1; i++) {
        for (j = 0; j * squaresize < _size-1; j++) {
            indexTL = j * squaresize * _size + i * squaresize;
            indexTR = j * squaresize * _size + (i + 1) * squaresize;
            indexBL = (j + 1) * squaresize * _size + i * squaresize;
            indexTR = (j + 1) * squaresize * _size + (i + 1) * squaresize;
            indexL = ((j * squaresize) + (squaresize / 2)) * _size + (i * squaresize);
            indexR = ((j * squaresize) + (squaresize / 2)) * _size + ((i+1) * squaresize);
            indexT = (j * squaresize) * _size + (i * squaresize) + (squaresize / 2);
            indexB = ((j+1) * squaresize) * _size + (i * squaresize) + (squaresize / 2);
            
            // Average the sides
            _heights[indexT] = (_heights[indexTL] + _heights[indexTR]) / 2.0f;
            _heights[indexL] = (_heights[indexTL] + _heights[indexBL]) / 2.0f;
            _heights[indexB] = (_heights[indexBL] + _heights[indexBR]) / 2.0f;
            _heights[indexR] = (_heights[indexTR] + _heights[indexBR]) / 2.0f;
            // Add randomness
			_heights[indexT] += (this->rand() / pow(2, (float)subdivide - 1)) - halfRange;
			_heights[indexB] += (this->rand() / pow(2, (float)subdivide - 1)) - halfRange;
			_heights[indexL] += (this->rand() / pow(2, (float)subdivide - 1)) - halfRange;
			_heights[indexR] += (this->rand() / pow(2, (float)subdivide - 1)) - halfRange;
        }
    }
    
    return true;
}



DiamondSquareNoise::~DiamondSquareNoise()
{
    if (_heights) {
        delete _heights;
    }
}




