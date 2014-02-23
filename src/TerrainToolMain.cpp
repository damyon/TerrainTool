#include "TerrainToolMain.h"

// Declare our game instance
TerrainToolMain game;

TerrainToolMain::TerrainToolMain()
    : _scene(NULL), _mainForm(NULL), _generateForm(NULL), _loadForm(NULL), _moveForward(false), _moveBackward(false), _moveLeft(false), _moveRight(false), _prevX(0), _prevY(0), MOVE_SPEED(1.0f)
{
}

void TerrainToolMain::initialize()
{
    // Create an empty scene.
    _scene = Scene::create();
    _scene->setAmbientColor(1, 1, 0.86f);
    
    // Setup a fly cam.
    _camera.initialize(0.1f, 20000.0f, 45);
    _camera.rotate(0.0f, -MATH_DEG_TO_RAD(10));
    _scene->addNode(_camera.getRootNode());
    _scene->setActiveCamera(_camera.getCamera());
    
    // Create a light source.
    _light = Light::createDirectional(Vector3::one());
    Node* lightNode = _scene->addNode("light");
    lightNode->setLight(_light);
    
    
    _mainForm = Form::create("res/main.form");
    _sizeForm = Form::create("res/size.form");
 
    Control *control = _mainForm->getControl("FlattenButton");
    control->addListener(this, Control::Listener::CLICK);
    
    control = _mainForm->getControl("RaiseButton");
    control->addListener(this, Control::Listener::CLICK);
    
    control = _mainForm->getControl("LowerButton");
    control->addListener(this, Control::Listener::CLICK);
    
    control = _mainForm->getControl("NavigateButton");
    control->addListener(this, Control::Listener::CLICK);
    
    
    _binding = new TerrainToolAutoBindingResolver();
    _binding->setLight(_light);
  
    // Generate a default terrain.
    Node* node = _scene->addNode("terrain");
    Terrain * terrain = _terrainGenerator.getTerrain();
    node->setTerrain(terrain);
    
    _camera.setPosition(Vector3(0, 900, 0));
    
    
}

void TerrainToolMain::finalize()
{
    SAFE_RELEASE(_mainForm);
    SAFE_RELEASE(_generateForm);
    SAFE_RELEASE(_loadForm);
    SAFE_RELEASE(_light);
    SAFE_RELEASE(_scene);
    delete _binding;
}

void TerrainToolMain::controlEvent(Control* control, Control::Listener::EventType evt)
{
    if (strcmp(control->getId(), "NavigateButton") == 0) {
        _inputMode = NAVIGATION;
        _sizeForm->setVisible(false);
    } else if (strcmp(control->getId(), "RaiseButton") == 0) {
        _inputMode = TERRAIN_RAISE;
        _sizeForm->setVisible(true);
    } else if (strcmp(control->getId(), "LowerButton") == 0) {
        _inputMode = TERRAIN_LOWER;
        _sizeForm->setVisible(true);
    } else if (strcmp(control->getId(), "FlattenButton") == 0) {
        _inputMode = TERRAIN_FLATTEN;
        _sizeForm->setVisible(true);
    }
}


void TerrainToolMain::moveCamera(float elapsedTime)
{
    if (_moveForward) {
        _camera.moveForward(elapsedTime * MOVE_SPEED);
    }
    if (_moveBackward) {
        _camera.moveBackward(elapsedTime * MOVE_SPEED);
    }
    if (_moveLeft) {
        _camera.moveLeft(elapsedTime * MOVE_SPEED);
    }
    if (_moveRight) {
        _camera.moveRight(elapsedTime * MOVE_SPEED);
    }
}


void TerrainToolMain::update(float elapsedTime)
{
    moveCamera(elapsedTime);
    
    if (_mainForm) {
        _mainForm->update(elapsedTime);
    }
    if (_generateForm) {
        _generateForm->update(elapsedTime);
    }
    if (_loadForm) {
        _loadForm->update(elapsedTime);
    }
    
    // Rotate model
   // _scene->findNode("box")->rotateY(MATH_DEG_TO_RAD((float)elapsedTime / 1000.0f * 180.0f));
}

void TerrainToolMain::render(float elapsedTime)
{
    // Clear the color and depth buffers
    clear(CLEAR_COLOR_DEPTH, Vector4::zero(), 1.0f, 0);

    // Visit all the nodes in the scene for drawing
    _scene->visit(this, &TerrainToolMain::drawScene);
    
    if (_mainForm) {
        _mainForm->draw();
    }
    if (_sizeForm) {
        _sizeForm->draw();
    }
    if (_generateForm) {
        _generateForm->draw();
    }
    if (_loadForm) {
        _loadForm->draw();
    }
    
}

bool TerrainToolMain::drawScene(Node* node)
{
    // If the node visited contains a model, draw it
    Model* model = node->getModel(); 
    if (model)
    {
        model->draw();
    } else if (node->getTerrain())
    {
        Terrain* terrain = node->getTerrain();
        terrain->draw();
    }

    return true;
}

void TerrainToolMain::keyEvent(Keyboard::KeyEvent evt, int key)
{
    if (evt == Keyboard::KEY_PRESS)
    {
        switch (key)
        {
        case Keyboard::KEY_ESCAPE:
            exit();
            break;
        case Keyboard::KEY_W:
        case Keyboard::KEY_UP_ARROW:
            if (_inputMode == NAVIGATION) {
                _moveForward = true;
            }
            break;
        case Keyboard::KEY_S:
        case Keyboard::KEY_DOWN_ARROW:
            if (_inputMode == NAVIGATION) {
                _moveBackward = true;
            }
            break;
        case Keyboard::KEY_A:
        case Keyboard::KEY_LEFT_ARROW:
            if (_inputMode == NAVIGATION) {
                _moveLeft = true;
            }
            break;
        case Keyboard::KEY_D:
        case Keyboard::KEY_RIGHT_ARROW:
            if (_inputMode == NAVIGATION) {
                _moveRight = true;
            }
            break;
        }
        
    } else if (evt == Keyboard::KEY_RELEASE) {
        switch (key)
        {
        case Keyboard::KEY_W:
        case Keyboard::KEY_UP_ARROW:
            if (_inputMode == NAVIGATION) {
                _moveForward = false;
            }
            break;
        case Keyboard::KEY_S:
        case Keyboard::KEY_DOWN_ARROW:
            if (_inputMode == NAVIGATION) {
                _moveBackward = false;
            }
            break;
        case Keyboard::KEY_A:
        case Keyboard::KEY_LEFT_ARROW:
            if (_inputMode == NAVIGATION) {
                _moveLeft = false;
            }
            break;
        case Keyboard::KEY_D:
        case Keyboard::KEY_RIGHT_ARROW:
            if (_inputMode == NAVIGATION) {
                _moveRight = false;
            }
            break;
        }
    }
    
}

void TerrainToolMain::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    switch (evt)
    {
    case Touch::TOUCH_PRESS:
        break;
    case Touch::TOUCH_RELEASE:
        break;
    case Touch::TOUCH_MOVE:
        int deltaX = x - _prevX;
        int deltaY = y - _prevY;
        
        if (_inputMode == NAVIGATION) {
            float pitch = -MATH_DEG_TO_RAD(deltaY * 0.5f);
            float yaw = MATH_DEG_TO_RAD(deltaX * 0.5f);
            _camera.rotate(yaw, pitch);
        }
        break;
    };
    _prevX = x;
    _prevY = y;
}

