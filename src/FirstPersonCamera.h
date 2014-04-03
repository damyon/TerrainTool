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


#ifndef FIRSTPERSONCAMERA_H_
#define FIRSTPERSONCAMERA_H_

#include "gameplay.h"

using namespace gameplay;

/**
 * FirstPersonCamera controls a camera like a first person shooter game.
 */
class FirstPersonCamera
{
public:

    /**
     * Constructor.
     */
    FirstPersonCamera();

    /**
     * Destructor.
     */
    ~FirstPersonCamera();

    /**
     * Initializes the first person camera. Should be called after the Game has been initialized.
     */
    void initialize(float nearPlane = 1.0f, float farPlane = 1000.0f, float fov = 45.0f);

    /**
     * Gets root node. May be NULL if not initialized.
     * 
     * @return Root node or NULL.
     */
    Node* getRootNode();

    /**
     * Gets the camera. May be NULL.
     * 
     * @return Camera or NULL.
     */
    Camera* getCamera();

    /**
     * Sets the position of the camera.
     * 
     * @param position The position to move to.
     */
    void setPosition(const Vector3& position);

    /**
     * Gets the position of the camera.
     * 
     * @return the current camera position
     */
    const Vector3& getPosition();

    /**
     * Moves the camera forward in the direction that it is pointing. (Fly mode)
     */
    void moveForward(float amount);

    /**
     * Moves the camera in the opposite direction that it is pointing.
     */
    void moveBackward(float amount);

    /**
     * Strafes that camera left, which is perpendicular to the direction it is facing.
     */
    void moveLeft(float amount);

    /**
     * Strafes that camera right, which is perpendicular to the direction it is facing.
     */
    void moveRight(float amount);

    /**
     * Move the camera up.
     */
    void moveUp(float amount);

    /**
     * Move the camera down.
     */
    void moveDown(float amount);

    /**
     * Rotates the camera in place in order to change the direction it is looking.
     * 
     * @param yaw Rotates the camera around the yaw axis in radians. Positive looks right, negative looks left.
     * @param pitch Rotates the camera around the ptich axis in radians. Positive looks up, negative looks down.
     */
    void rotate(float yaw, float pitch);

private:

    
    /**
     * Node descriping the orientation of the camera.
     **/
    Node* _pitchNode;
    
    /**
     * Root node for the camera, can be used to move/rotate etc.
     **/
    
    Node* _rootNode;
};

#endif