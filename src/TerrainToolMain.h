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
    
    void moveCamera(float elapsedTime);

    enum INPUT_MODE { NAVIGATION, TERRAIN };
       
    const float MOVE_SPEED;
    Scene* _scene;
    Node* _selection;
    SelectionRing *_selectionRing;
    TerrainToolAutoBindingResolver* _binding;
    FirstPersonCamera _camera;
    TerrainGenerator _terrainGenerator;
    Light* _light;
    Form* _mainForm;
    Form* _sizeForm;
    Form* _generateForm;
    Form* _loadForm;
    bool _moveForward, _moveBackward, _moveLeft, _moveRight;
    bool _doAction;
    float _prevX, _prevY;
    float _selectionScale;
    INPUT_MODE _inputMode;
};

struct TerrainHitFilter : public PhysicsController::HitFilter {

    TerrainHitFilter(Terrain* terrain)
    {
        terrainObject = terrain->getNode()->getCollisionObject();
    }

    bool filter(PhysicsCollisionObject* object)
    {
        // Filter out all objects but the terrain
        return (object != terrainObject);
    }

    PhysicsCollisionObject* terrainObject;
};


#endif
