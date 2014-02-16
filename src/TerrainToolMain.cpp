#include "TerrainToolMain.h"

// Declare our game instance
TerrainToolMain game;

TerrainToolMain::TerrainToolMain()
    : _scene(NULL), _terrain(NULL), _mainForm(NULL), _generateForm(NULL), _loadForm(NULL)
{
}

void TerrainToolMain::initialize()
{
    // Load game scene from file
    _scene = Scene::load("res/empty.scene");

    // Set the aspect ratio for the scene's camera to match the current resolution
    _scene->getActiveCamera()->setAspectRatio(getAspectRatio());
    
    _mainForm = Form::create("res/main.form");
 
    // Manually load the default terrain.
}

void TerrainToolMain::finalize()
{
    SAFE_RELEASE(_mainForm);
    SAFE_RELEASE(_generateForm);
    SAFE_RELEASE(_loadForm);
    SAFE_RELEASE(_terrain);
    SAFE_RELEASE(_scene);
}

void TerrainToolMain::update(float elapsedTime)
{
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
        break;
    };
}
