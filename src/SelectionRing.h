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


#ifndef SELECTIONRING_H
#define SELECTIONRING_H

#include "gameplay.h"

using namespace gameplay;

/**
 * Selection ring represents a renderable shape that indicates a selected region on the heightmap.
 * The selection is always a circle, but the size and location changes.
 **/
class SelectionRing: public Ref
{
private:
    /**
     * The root scene node
     **/
    Scene *_scene;
    
    /**
     * The root node of the selection (contains child nodes)
     **/
    Node *_node;
    
    /**
     * The scale of the selection in world units.
     **/
    float _scale;
    
    /**
     * The x coordinate of the selection
     **/
    float _x;
    
    /**
     * The z coordinate of the selection
     **/
    float _z;
    
    /**
     * How many renderable nodes to paint around the selection circle.
     **/
    int _ringCount;
    
    /**
     * Position the nodes in the ring so each sits just above the terrain.
     *
     * @return void
     **/
    void setRingNodeHeights(Terrain *terrain);
public:
    /**
     * Getter for the x position
     *
     * @return float
     **/
    float getPositionX();
    
    /**
     * Getter for the y position
     *
     * @return float
     **/
    float getPositionZ();
    
    /**
     * Getter for the scale of the ring (radius)
     *
     * @return float
     **/
    float getScale();
    
    /**
     * Constructor
     *
     * @param scene Root node of the scene graph.
     **/
    SelectionRing(Scene *scene);
    
    /**
     * Set the scale of the selection
     *
     * @param scale The new scale value
     * @param terrain Used to get the heights of the renderable objects around the circle.
     * @return void
     **/
    void setScale(float scale, Terrain *terrain);
    
    /**
     * Set the x and z coordinates of the center of the selection.
     *
     * @param x x coordinate
     * @param z z coordinate
     * @param terrain Used to get the heights of the renderable objects around the circle.
     * @return void
     **/
    void setPosition(float x, float z, Terrain *terrain);
    
    /**
     * Destructor
     **/
    virtual ~SelectionRing();
};

#endif // SELECTIONRING_H
