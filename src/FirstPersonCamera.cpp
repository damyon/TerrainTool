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


#include "FirstPersonCamera.h"

FirstPersonCamera::FirstPersonCamera()
    : _pitchNode(NULL), _rootNode(NULL)
{
    
}

FirstPersonCamera::~FirstPersonCamera()
{
    SAFE_RELEASE(_pitchNode);
    SAFE_RELEASE(_rootNode);
}

void FirstPersonCamera::initialize(float nearPlane, float farPlane, float fov)
{
    SAFE_RELEASE(_pitchNode);
    SAFE_RELEASE(_rootNode);
    _rootNode = Node::create("FirstPersonCamera_root");
    _pitchNode = Node::create("FirstPersonCamera_pitch");
    _rootNode->addChild(_pitchNode);

    float aspectRatio = Game::getInstance()->getAspectRatio();
    assert(aspectRatio > 0.0f);
    Camera* camera = Camera::createPerspective(fov, aspectRatio, nearPlane, farPlane);
    _pitchNode->setCamera(camera);
    SAFE_RELEASE(camera);
}

Node* FirstPersonCamera::getRootNode()
{
    return _rootNode;
}

Camera* FirstPersonCamera::getCamera()
{
    if (_pitchNode)
        return _pitchNode->getCamera();
    return NULL;
}

void FirstPersonCamera::setPosition(const Vector3& position)
{
    _rootNode->setTranslation(position);
}

void FirstPersonCamera::moveForward(float amount)
{
    Vector3 v = _pitchNode->getForwardVectorWorld();
    v.normalize().scale(amount);
    _rootNode->translate(v);
}

void FirstPersonCamera::moveBackward(float amount)
{
    moveForward(-amount);
}

void FirstPersonCamera::moveLeft(float amount)
{
    _rootNode->translateLeft(amount);
}

void FirstPersonCamera::moveRight(float amount)
{
    _rootNode->translateLeft(-amount);
}

void FirstPersonCamera::moveUp(float amount)
{
    _rootNode->translateUp(amount);
}

void FirstPersonCamera::moveDown(float amount)
{
    _rootNode->translateUp(-amount);
}

void FirstPersonCamera::rotate(float yaw, float pitch)
{
    _rootNode->rotateY(-yaw);
    _pitchNode->rotateX(pitch);
}

