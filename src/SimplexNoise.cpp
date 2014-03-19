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



#include "SimplexNoise.h"
#include <math.h>
#include "gameplay.h"

const double SimplexNoise::F2 = 0.5 * (sqrt( 3.0 ) - 1.0);
const double SimplexNoise::G2 = (3.0 - sqrt( 3.0 )) / 6.0;
const unsigned char SimplexNoise::p[256] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
    142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,
    203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
    74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,
    220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,
    132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,
    186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,
    59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,
    70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,
    178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,
    241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,
    176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,
    128,195,78,66,215,61,156,180
};
const Vector3  SimplexNoise::v3[12] = {
    Vector3 (1,1,0),Vector3 (-1,1,0),Vector3 (1,-1,0),Vector3 (-1,-1,0),Vector3 (1,0,1),
    Vector3 (-1,0,1),Vector3 (1,0,-1),Vector3 (-1,0,-1),Vector3 (0,1,1),Vector3 (0,-1,1),
    Vector3 (0,1,-1),Vector3 (0,-1,-1)
};
unsigned char SimplexNoise::perm[512] = {0};
unsigned char SimplexNoise::permMod12[512] = {0};

double SimplexNoise::dot(const Vector3 & g, double x, double z)
{
    return g.x * x + g.z * z;
}

void SimplexNoise::init(double maxx, double maxz, double rangemin, double rangemax)
{
    _min = rangemin;
    _max = rangemax;
    _worldScale = maxx;
    if (maxz > maxx) {
        _worldScale = maxz;
    }
    for ( unsigned int i = 0; i < 512; ++i ) {
        perm[i] = p[i & 255];
        permMod12[i] = static_cast<unsigned char>(perm[i] % 12);
    }
    
}

SimplexNoise::SimplexNoise() : _min(0), _max(0)
{

}

double SimplexNoise::noise(double x, double z)
{
    const double lacunarity = 0.1;
    const double gain = 4;
    double sum = 0.0;
    double amplitude = 1.0 / _worldScale;

    int i;
    x *= amplitude;
    z *= amplitude;
    
    // We start at a scale of 1 feature for the entire world (a big hill or valley)
    // Then reduce the scale by a factor of 10, until we are at a 1:1 resolution.
    while (amplitude < (_worldScale)) {
        sum += noiseSingle(x, z) * (1.0 / amplitude) / _worldScale;
        x *= gain;
        z *= gain;
        amplitude *= gain;
    }

    // clamp to min and max.
    sum += 1.0;
    sum *= _max - _min;
    sum += _min;
    return sum;
}


double SimplexNoise::noiseSingle(double xin, double zin)
{
    // Easily understandable noise function (you lost me at hypercubes).
    double s = (xin + zin) * F2;
    int i = floor( xin + s );
    int j = floor( zin + s );
    double t = (i + j) * G2;
    double x0 = xin - (i - t);
    double z0 = zin - (j - t);
    unsigned char i1 = 0, j1 = 1;
    if ( x0 > z0 ) {
        i1 = 1;
        j1 = 0;
    }
    double x1 = x0 - i1 + G2;
    double z1 = z0 - j1 + G2;
    double x2 = x0 - 1.0 + 2.0 * G2;
    double z2 = z0 - 1.0 + 2.0 * G2;
    unsigned char ii = i & 255;
    unsigned char jj = j & 255;
    unsigned char gi0 = permMod12[ii + perm[jj]];
    unsigned char gi1 = permMod12[ii + i1 + perm[jj + j1]];
    unsigned char gi2 = permMod12[ii + 1 + perm[jj + 1]];
    double n0 = 0.0;
    double t0 = 0.5 - x0 * x0 - z0 * z0;
    if ( t0 >= 0.0 ) {
        t0 *= t0;
        n0 = t0 * t0 * dot( v3[gi0], x0, z0);
    }
    double n1 = 0.0;
    double t1 = 0.5 - x1 * x1 - z1 * z1;
    if ( t1 >= 0.0 ) {
        t1 *= t1;
        n1 = t1 * t1 * dot( v3[gi1], x1, z1 );
    }
    double n2 = 0.0;
    double t2 = 0.5 - x2 * x2 - z2 * z2;
    if ( t2 >= 0.0 ) {
        t2 *= t2;
        n2 = t2 * t2 * dot( v3[gi2], x2, z2 );
    }
    return 70.0 * (n0 + n1 + n2);
}

SimplexNoise::~SimplexNoise()
{
    // Nothing to clean up because our heights are computed on demand.
}


