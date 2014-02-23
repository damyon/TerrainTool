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


#include "TerrainToolAutoBindingResolver.h"

namespace gameplay
{
    
TerrainToolAutoBindingResolver::TerrainToolAutoBindingResolver()
    : _light(NULL)
{
    GP_WARN("Init Terrain Auto Binding.");

    
}

void TerrainToolAutoBindingResolver::setLight(Light* light)
{
    if (_light) {
        SAFE_RELEASE(_light);
    }
    light->addRef();
    _light = light;
}

TerrainToolAutoBindingResolver::~TerrainToolAutoBindingResolver()
{
    SAFE_RELEASE(_light);
}

bool TerrainToolAutoBindingResolver::resolveAutoBinding(const char* autoBinding, Node* node, MaterialParameter* parameter)
{
    if (strcmp(autoBinding, "LIGHT_DIRECTION_0") == 0)
    {
        parameter->bindValue(this, &TerrainToolAutoBindingResolver::getLightDirection0);
        return true;
    }
    else if (strcmp(autoBinding, "LIGHT_COLOR_0") == 0)
    {
        parameter->bindValue(this, &TerrainToolAutoBindingResolver::getLightColor0);
        return true;
    }

    return false;
}


Vector3 TerrainToolAutoBindingResolver::getLightColor0() const
{
    if (_light) {
        return _light->getNode()->getForwardVectorView();
    }
    return Vector3::one();
}

Vector3 TerrainToolAutoBindingResolver::getLightDirection0() const
{
    if (_light) {
        return _light->getNode()->getForwardVectorView();
    }
    return Vector3::one();
}
}