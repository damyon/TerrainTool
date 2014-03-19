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

#ifndef TERRAINTOOLAUTOBINDINGRESOLVER_H
#define TERRAINTOOLAUTOBINDINGRESOLVER_H

#include "gameplay.h"

using namespace gameplay;

/**
 * This class is used to supply parameters to the terrain shader.
 **/
class TerrainToolAutoBindingResolver : RenderState::AutoBindingResolver
{
public:
    /**
     * Constructor.
     */
    TerrainToolAutoBindingResolver();
    
    /**
     * The light determines the direction of shadows etc.
     *
     * @param light The scene light.
     * @return void
     **/
    void setLight(Light *light);
    
    /**
     * Destructor.
     *
     **/
    virtual ~TerrainToolAutoBindingResolver();

    /**
     * The callback that gets the parameters for the shader.
     *
     * @param autoBinding The name of the parameter
     * @param node The node for the material
     * @param parameter The Material parameter
     * @return bool
     **/
    bool resolveAutoBinding(const char* autoBinding, Node* node, MaterialParameter* parameter);

    /**
     * Called by resolveAutoBinding to get the direction of the light.
     *
     * @return :Vector3
     **/
    Vector3 getLightDirection0() const;
    
    /**
     * Called by resolveAutoBinding to get the colour of the light.
     *
     * @return :Vector3
     **/
    Vector3 getLightColor0() const;
private:
    
    /**
     * Reference to the scene light.
     **/
    Light *_light;
};


#endif // TERRAINTOOLAUTOBINDINGRESOLVER_H
