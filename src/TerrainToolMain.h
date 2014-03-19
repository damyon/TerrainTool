#ifndef TEMPLATEGAME_H_
#define TEMPLATEGAME_H_

#include "gameplay.h"

#include "FirstPersonCamera.h"
#include "TerrainGenerator.h"
#include "SelectionRing.h"
#include "TerrainToolAutoBindingResolver.h"

using namespace gameplay;

/**
 * Main game class.
 */
class TerrainToolMain: public Game, Control::Listener
{
public:

    /**
     * Constructor.
     */
    TerrainToolMain();
    
    /**
     * @see Game::keyEvent
     */
	void keyEvent(Keyboard::KeyEvent evt, int key);
	
    /**
     * @see Game::touchEvent
     */
    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

protected:

    /**
     * @see Game::initialize
     */
    void initialize();

    /**
     * @see Game::finalize
     */
    void finalize();

    /**
     * @see Game::update
     */
    void update(float elapsedTime);

    /**
     * @see Game::render
     */
    void render(float elapsedTime);

    void controlEvent(Control* control, EventType evt);

private:

    /**
     * Draws the scene each frame.
     */
    bool drawScene(Node* node);
    
    /**
     * If the camera is moving, move it relative to the amount of time that has passed.
     *
     * @param elapsedTime ...
     * @return void
     **/
    void moveCamera(float elapsedTime);

    /**
     * Used by the ui to switch input states.
     **/
    enum INPUT_MODE { NAVIGATION, TERRAIN, PAINT };
       
    /**
     * How fast we fly.
     **/
    const float MOVE_SPEED;
    
    /**
     * The root scene node.
     **/
    Scene* _scene;
    
    /**
     * The root of the selection node.
     **/
    Node* _selection;
    
    /**
     * The selection ring instance.
     **/
    SelectionRing *_selectionRing;
    
    /**
     * Used to pass params to the terrain shader.
     **/
    TerrainToolAutoBindingResolver* _binding;
    
    /**
     * This is the scene camera.
     **/
    FirstPersonCamera _camera;
    
    /**
     * Used to generate and modify the terrain.
     **/
    TerrainGenerator _terrainGenerator;
    
    /**
     * The scene light.
     **/
    Light* _light;
    
    /**
     * A ui form.
     **/
    Form* _mainForm;
    
    /**
     * A ui form.
     **/
    Form* _sizeForm;
    
    /**
     * A ui form.
     **/
    Form* _generateForm;
    
    /**
     * A ui form.
     **/
    Form* _loadForm;
    
    
    /**
     * boolean flags to say if we are moving.
     **/
    bool _moveForward, _moveBackward, _moveLeft, _moveRight;
    
    /**
     * Used to cancel an action.
     **/
    bool _doAction;
    
    /**
     * Store the last cursor position.
     **/
    float _prevX, _prevY;
    
    /**
     * The scale of the selection.
     **/
    float _selectionScale;
    
    /**
     * The current input mode.
     **/
    INPUT_MODE _inputMode;
};

/**
 * Used to quickly filter ray collisions to collisions with the terrain (for picking).
 **/

struct TerrainHitFilter : public PhysicsController::HitFilter {

    /**
     * Constructor -- stores a reference to the terrain.
     *
     * @param terrain ...
     **/
    TerrainHitFilter(Terrain* terrain)
    {
        terrainObject = terrain->getNode()->getCollisionObject();
    }

    /**
     * Only return true if the hit is on the terrain.
     *
     * @param object The result of the ray collision test.
     * @return bool
     **/
    bool filter(PhysicsCollisionObject* object)
    {
        // Filter out all objects but the terrain
        return (object != terrainObject);
    }

    /**
     * Stored so it can be used in the hit detection.
     **/
    PhysicsCollisionObject* terrainObject;
};


#endif
