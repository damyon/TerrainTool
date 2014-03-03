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


#include "SelectionRing.h"
#include <math.h>

SelectionRing::SelectionRing(Scene* scene)
:_ringCount(16)
{
    _scene = scene;
    _scene->addRef();
    
    Bundle* bundle;
    bundle = Bundle::create("res/selection.gpb");
    Node *one = bundle->loadNode("SelectionModel");
    one->getModel()->setMaterial("res/demo.material#colored", 0);

    SAFE_RELEASE(bundle);
    
    _node = _scene->addNode("SelectionRing");
    //_node->setScale(1000);
    int i = 0;
    for (i = 0; i < _ringCount; i++) {
        float x = sin((float)i/_ringCount * MATH_PIX2);
        float y = cos((float)i/_ringCount * MATH_PIX2);
        Node *clone = one->clone();
        clone->setTranslationX(x);
        clone->setTranslationX(y);
     //   clone->setScale(100.0f);
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
    Node *ring = _node->getFirstChild();
    int i = 0;
    _x = x;
    _z = z;
    while(ring) {
        float offsetx = sin((float)i/_ringCount * MATH_PIX2) * _scale;
        float offsetz = cos((float)i/_ringCount * MATH_PIX2) * _scale;
        // adjust the height seperately for each node in the ring.
        float height = terrain->getHeight(offsetx + x, offsetz + z) + 100;
        ring->setScale(_scale / 8.0f);
        ring->setTranslation(offsetx + x, height, offsetz + z);
        ring = ring->getNextSibling();
        i++;
    }
  
}

void SelectionRing::setScale(float scale, Terrain *terrain)
{
    _scale = scale;
    setPosition(_node->getTranslationX(), _node->getTranslationZ(), terrain);
}


