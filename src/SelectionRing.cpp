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


#include "SelectionRing.h"
#include <math.h>

SelectionRing::SelectionRing(Scene* scene)
:_ringCount(16)
{
    // Store a reference to the scene node.
    _scene = scene;
    _scene->addRef();
    
    // Load a 3d model for a single point around the ring.
    Bundle* bundle;
    bundle = Bundle::create("res/selection.gpb");
    Node *one = bundle->loadNode("SelectionModel");
    
    // Attempt to set a material for the model - this is not working and I need to work out why.
    one->getModel()->setMaterial("res/demo.material#colored", 0);

    // We dont need this anymore.
    SAFE_RELEASE(bundle);
    
    // Save a reference to the model node.
    _node = _scene->addNode("SelectionRing");
    
    // Duplicate the node at even points around the selection ring.
    int i = 0;
    for (i = 0; i < _ringCount; i++) {
        float x = sin((float)i/_ringCount * MATH_PIX2);
        float y = cos((float)i/_ringCount * MATH_PIX2);
        Node *clone = one->clone();
        clone->setTranslationX(x);
        clone->setTranslationX(y);
        _node->addChild(clone);
        SAFE_RELEASE(clone);
    }
    
    _scene->addNode(_node);
}
float SelectionRing::getPositionX()
{
    return _x;
}

float SelectionRing::getPositionZ()
{
    return _z;
}

float SelectionRing::getScale()
{
    return _scale;
}

SelectionRing::~SelectionRing()
{
    SAFE_RELEASE(_node);
    SAFE_RELEASE(_scene);
}

void SelectionRing::setPosition(float x, float z, Terrain *terrain)
{
    _x = x;
    _z = z;
    
    this->setRingNodeHeights(terrain);
}

void SelectionRing::setRingNodeHeights(Terrain *terrain)
{
    Node *ring = _node->getFirstChild();
    int i = 0;

    // Adjust the height of each node in the ring so it sits just above the terrain.
    while(ring) {
        float offsetx = sin((float)i/_ringCount * MATH_PIX2) * _scale;
        float offsetz = cos((float)i/_ringCount * MATH_PIX2) * _scale;
        // adjust the height seperately for each node in the ring.
        float height = terrain->getHeight(offsetx + _x, offsetz + _z) + 100;
        ring->setScale(_scale / 8.0f);
        ring->setTranslation(offsetx + _x, height, offsetz + _z);
        ring = ring->getNextSibling();
        i++;
    }

}


void SelectionRing::setScale(float scale, Terrain *terrain)
{
    _scale = scale;
    
    this->setRingNodeHeights(terrain);
}


