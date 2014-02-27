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

class SelectionRing: public Ref
{
private:
    Scene *_scene;
    Node *_node;
    float _scale;
    int _ringCount;
    
public:
    SelectionRing(Scene *scene);
    void setScale(float scale, Terrain *terrain);
    void setPosition(float x, float z, Terrain *terrain);
    virtual ~SelectionRing();
};

#endif // SELECTIONRING_H
