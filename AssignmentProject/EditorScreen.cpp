#include "EditorScreen.h"

#include <GameEngine/ResourceManager.h>
#include <GameEngine/IOManager.h>
#include "LevelReaderWriter.h"

const int MOUSE_LEFT = 0;
const int MOUSE_RIGHT = 1;
const float LIGHT_SELECT_RADIUS = 0.5f;

const b2Vec2 GRAVITY(0.0f, -25.0f);

void WidgetLabel::Draw(GameEngine::SpriteBatch& _sb, GameEngine::SpriteFont& _sf, GameEngine::Window* _w)
{
  if (!widget->isVisible())
  {
    return;
  }

  glm::vec2 pos;
  pos.x = widget->getXPosition().d_scale * _w->GetScreenWidth() - _w->GetScreenWidth() / 2.0f + widget->getWidth().d_offset / 2.0f;
  pos.y = _w->GetScreenHeight() / 2.0f - widget->getYPosition().d_scale * _w->GetScreenHeight();
  _sf.draw(_sb, text.c_str(), pos, glm::vec2(scale), 0.0f, color, GameEngine::Justification::MIDDLE);
}

EditorScreen::EditorScreen(GameEngine::Window* _window) : m_window(_window)
{
  m_screenIndex = SCREEN_INDEX_EDITOR;
}

EditorScreen::~EditorScreen() {/*empty*/ }

int EditorScreen::GetNextScreenIndex() const
{
  return SCREEN_INDEX_NO_SCREEN;
}

int EditorScreen::GetPreviousScreenIndex() const
{
  return SCREEN_INDEX_MAINMENU;
}

void EditorScreen::Build()
{

}

void EditorScreen::Destroy()
{

}

void EditorScreen::OnEntry()
{
  //Initialize the audio engine and play the music
  m_audio.Init();
  m_music = m_audio.LoadMusic("Sound/Post-Punk-1.ogg");
  m_music.Play(-1);
  //Initalialize the spritefont
  m_spriteFont.init("Fonts/chintzy.ttf", 32);

  //set the mousebutton press states to false
  m_mouseButtons[MOUSE_LEFT] = false;
  m_mouseButtons[MOUSE_RIGHT] = false;

  //init camera
  m_camera.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  m_camera.SetScale(32.0f);
  //ui camera has a scale of 1.0f
  m_uiCamera.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  m_uiCamera.SetScale(1.0f);

  //Initialize the debugRenderer
  m_debugRenderer.Init();

  //Initialize the UI
  InitUI();

  //Initialize the Spritebatch
  m_spriteBatch.Init();

  //set up box2d
  m_world = std::make_unique<b2World>(GRAVITY);

  // shader initialization
  //compile the texture shader
  m_textureProgram.CompileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
  m_textureProgram.AddAttribute("vertexPosition");
  m_textureProgram.AddAttribute("vertexColor");
  m_textureProgram.AddAttribute("vertexUV");
  m_textureProgram.LinkShaders();
  // Compile our light shader
  m_lightProgram.CompileShaders("Shaders/lightShading.vert", "Shaders/lightShading.frag");
  m_lightProgram.AddAttribute("vertexPosition");
  m_lightProgram.AddAttribute("vertexColor");
  m_lightProgram.AddAttribute("vertexUV");
  m_lightProgram.LinkShaders();

  //set the blank texture (just used to show the player where he will put some box by outlining it
  m_blankTexture = GameEngine::ResourceManager::GetTexture("Assets/Bricks/blank.png");
}

void EditorScreen::OnExit()
{
  //clear the list boxes
  for (auto& item : m_saveListBoxItems)
  {
    //delete doesn't need to be called as removeItem does this automatically
    m_saveWindowCombobox->removeItem(item);
  }
  m_saveListBoxItems.clear();
  for (auto& item : m_loadListBoxItems)
  {
    //delete doesn't need to be called as removeItem does this automatically
    m_loadWindowCombobox->removeItem(item);
  }
  m_loadListBoxItems.clear();

  //dispose and destroy EVERYTHIGN!
  m_textureProgram.Dispose();
  m_lightProgram.Dispose();
  m_spriteFont.dispose();
  m_spriteBatch.Dispose();
  m_widgetLabels.clear();
  m_audio.Destroy();

  ClearLevel();
  m_world.reset();
  m_gui.DestroyGUI();
}

void EditorScreen::Update()
{
  //update the 2 cameras
  m_camera.Update();
  m_uiCamera.Update();

  //Check for player Inputs
  CheckInput();

  //Box scaling and rotation from keypress
  if ( ((m_objectMode == ObjectMode::PLATFORM || m_objectMode == ObjectMode::FINISH || m_objectMode == ObjectMode::OBSTACLE || 
        m_objectMode == ObjectMode::TRIGGER || m_objectMode == ObjectMode::COIN) && m_selectionMode == SelectionMode::PLACE) ||
        (m_selectedBox != NO_BOX || m_selectedObstacle != NO_OBSTACLE || m_selectedCoin != NO_COIN) )
  {
    const double SCALE_SPEED = 0.01;
    const double ROTATION_SPEED = 0.01;
    // The callbacks will set the member variables for m_boxDims, don't need to set here
    // scale
    if (m_inputManager.IsKeyDown(SDLK_LEFT))
    {
      //decrease the width
      m_widthSpinner->setCurrentValue(m_widthSpinner->getCurrentValue() - SCALE_SPEED);
    }
    else if (m_inputManager.IsKeyDown(SDLK_RIGHT))
    {
      //increase the width
      m_widthSpinner->setCurrentValue(m_widthSpinner->getCurrentValue() + SCALE_SPEED);
    }
    if (m_inputManager.IsKeyDown(SDLK_DOWN))
    {
      //decrease the height
      m_heightSpinner->setCurrentValue(m_heightSpinner->getCurrentValue() - SCALE_SPEED);
    }
    else if (m_inputManager.IsKeyDown(SDLK_UP))
    {
      //increase the height
      m_heightSpinner->setCurrentValue(m_heightSpinner->getCurrentValue() + SCALE_SPEED);
    }

    //rotation
    if (m_inputManager.IsKeyDown(SDLK_e))
    {
      //check wrap-around
      double newValue = m_rotationSpinner->getCurrentValue() - ROTATION_SPEED;
      //if the new value is below 0 degrees that means id did a full spin and is back to 360 degrees (2PI)
      if (newValue < 0.0)
      {
        newValue += M_PI * 2.0;
      }
      m_rotationSpinner->setCurrentValue(newValue);
    }
    else if (m_inputManager.IsKeyDown(SDLK_q))
    {
      double newValue = m_rotationSpinner->getCurrentValue() + ROTATION_SPEED;
      //if the new value is over 360 degrees that means id did a full spin and is back to 0 degrees (-2PI)
      if (newValue > M_PI * 2.0)
      {
        newValue -= M_PI * 2.0;
      }
      m_rotationSpinner->setCurrentValue(newValue);
    }
  }
  // Light scaling from keypress
  if ((m_objectMode == ObjectMode::LIGHT && m_selectionMode == SelectionMode::PLACE) || m_selectedLight != NO_LIGHT)
  {
    const double SCALE_SPEED = 0.1;
    const float ALPHA_SPEED = 1.0f;

    //scaling
    if (m_inputManager.IsKeyDown(SDLK_LEFT))
    {
      //decrease the light radius
      m_sizeSpinner->setCurrentValue(m_sizeSpinner->getCurrentValue() - SCALE_SPEED);
    }
    else if (m_inputManager.IsKeyDown(SDLK_RIGHT))
    {
      //increase the light radius
      m_sizeSpinner->setCurrentValue(m_sizeSpinner->getCurrentValue() + SCALE_SPEED);
    }
    //light intensity (alpha)
    if (m_inputManager.IsKeyDown(SDLK_DOWN))
    {
      m_aSpinner->setCurrentValue(m_aSpinner->getCurrentValue() - ALPHA_SPEED);
    }
    else if (m_inputManager.IsKeyDown(SDLK_UP))
    {
      m_aSpinner->setCurrentValue(m_aSpinner->getCurrentValue() + ALPHA_SPEED);
    }
  }
  //check for deleting an object
  if (m_inputManager.IsKeyPressed(SDLK_DELETE))
  {
    if (m_selectedLight != NO_LIGHT)
    {
      m_lights.erase(m_lights.begin() + m_selectedLight);
      m_selectedLight = NO_LIGHT;
    }
    else if (m_selectedBox != NO_BOX)
    {
      m_boxes[m_selectedBox].Destroy(m_world.get());
      m_boxes.erase(m_boxes.begin() + m_selectedBox);
      m_selectedBox = NO_BOX;
    }
    else if (m_selectedObstacle != NO_OBSTACLE)
    {
      m_obstacles[m_selectedObstacle].Destroy(m_world.get());
      m_obstacles.erase(m_obstacles.begin() + m_selectedObstacle);
      m_selectedObstacle = NO_OBSTACLE;
    }
    else if (m_selectedEnemy != NO_ENEMY)
    {
      m_enemies[m_selectedEnemy].Destroy(m_world.get());
      m_enemies.erase(m_enemies.begin() + m_selectedEnemy);
      m_selectedEnemy = NO_ENEMY;
    }
    else if (m_selectedCoin != NO_COIN)
    {
      m_coins[m_selectedCoin].Destroy(m_world.get());
      m_coins.erase(m_coins.begin() + m_selectedCoin);
      m_selectedCoin = NO_COIN;
    }
  }
  //update the gui
  m_gui.Update();
}

void EditorScreen::Draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0f, 0.01f, 0.2f, 1.0f);

  //draw the world
  DrawWorld();
  //draw the ui
  DrawUI();
}

void EditorScreen::DrawUI()
{
  /********************************
  *         PLACING MODE          *
  *********************************/

  //outlines
  if (m_selectionMode == SelectionMode::PLACE && !isMouseInUI())
  {
    int x, y;
    SDL_GetMouseState(&x, &y);
    glm::vec2 pos = m_camera.ConvertScreenToWorld(glm::vec2(x, y));
    //draw the object placement outlines ( white borders )
    if ( m_objectMode == ObjectMode::PLATFORM || m_objectMode == ObjectMode::TRIGGER || m_objectMode == ObjectMode::FINISH ||
         m_objectMode == ObjectMode::OBSTACLE || m_objectMode == ObjectMode::COIN )
    {
      m_debugRenderer.DrawBox(glm::vec4(pos - m_boxDims * 0.5f, m_boxDims), GameEngine::ColorRGBA8(255, 255, 255, 255), m_rotation);
      m_debugRenderer.End();
      m_debugRenderer.Render(m_camera.GetCameraMatrix(), 2.0f);
    }
    else if (m_objectMode == ObjectMode::LIGHT)
    {
      //make temporary light to render
      Light tempLight;
      tempLight.position = pos;
      tempLight.color = GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, (GLubyte)m_colorPickerAlpha);
      tempLight.size = m_lightSize;

      //draw light
      m_lightProgram.Use();
      GLint pUniform = m_textureProgram.GetUniformLocation("P");
      glUniformMatrix4fv(pUniform, 1, GL_FALSE, &m_camera.GetCameraMatrix()[0][0]);
      //Additive blending
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      m_spriteBatch.Begin();
      tempLight.Draw(m_spriteBatch);
      m_spriteBatch.End();
      m_spriteBatch.RenderBatch();
      m_lightProgram.UnUse();
      //restore alpha blending
      glBlendFunc(GL_SRC0_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      //Selection Radius
      m_debugRenderer.DrawCircle(pos, GameEngine::ColorRGBA8(255, 255, 255, 255), LIGHT_SELECT_RADIUS);
      //Outer Radius
      m_debugRenderer.DrawCircle(pos, tempLight.color, tempLight.size);
      m_debugRenderer.End();
      m_debugRenderer.Render(m_camera.GetCameraMatrix(), 2.0f);
    }
    //
  }
  /********************************
  *         SELECT MODE           *
  *********************************/
  else
  {
    //draw selected light outlines
    if (m_selectedLight != NO_LIGHT)
    {
      //Selection Radius
      m_debugRenderer.DrawCircle(m_lights[m_selectedLight].position, GameEngine::ColorRGBA8(255, 255, 0, 255), LIGHT_SELECT_RADIUS);
      //Outer Radius
      m_debugRenderer.DrawCircle(m_lights[m_selectedLight].position, m_lights[m_selectedLight].color, m_lights[m_selectedLight].size);
      //end the debug renderer
      m_debugRenderer.End();
      //render the circle
      m_debugRenderer.Render(m_camera.GetCameraMatrix(), 2.0f);
    }
    //draw selected Box outlines
    if (m_selectedBox != NO_BOX)
    {
      //make an alias to the current selected box
      const Box& b = m_boxes[m_selectedBox];
      glm::vec4 destRect;
      //revert back to bottom left
      destRect.x = b.GetBody()->GetPosition().x - b.GetDimensions().x / 2.0f;
      destRect.y = b.GetBody()->GetPosition().y - b.GetDimensions().y / 2.0f;
      destRect.z = b.GetDimensions().x;
      destRect.w = b.GetDimensions().y;

      m_debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(255, 255, 0, 255), b.GetBody()->GetAngle());
      //end the debug renderer
      m_debugRenderer.End();
      //render the circle
      m_debugRenderer.Render(m_camera.GetCameraMatrix(), 2.0f);
    }
    //draw selected Obstacle outlines
    if (m_selectedObstacle != NO_OBSTACLE)
    {
      //make an alias to the current selected obstacle
      const Box& obs = m_obstacles[m_selectedObstacle];

      glm::vec4 destRect;
      //revert back to bottom left
      destRect.x = obs.GetBody()->GetPosition().x - obs.GetDimensions().x / 2.0f;
      destRect.y = obs.GetBody()->GetPosition().y - obs.GetDimensions().y / 2.0f;
      destRect.z = obs.GetDimensions().x;
      destRect.w = obs.GetDimensions().y;

      m_debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(255, 255, 0, 255), obs.GetBody()->GetAngle());
      //end the debug renderer
      m_debugRenderer.End();
      //render the circle
      m_debugRenderer.Render(m_camera.GetCameraMatrix(), 2.0f);
    }
    //Draw selected coin outlines
    if (m_selectedCoin != NO_COIN)
    {
      //Here I am manually drawing the debug outlines when selected, so that I can give it a custom color to inform the user that it's clicked
      //Or else it will be the usual debug color specified in the coins cpp file if I had used m_coins[m_selectedCoin].DrawDebug(m_debugRenderer);

      //make an alias to the current selected Coin
      const Coins& coin = m_coins[m_selectedCoin];

      glm::vec4 destRect;
      //revert back to bottom left
      destRect.x = coin.GetBox().GetBody()->GetPosition().x - coin.GetDimensions().x / 2.0f;
      destRect.y = coin.GetBox().GetBody()->GetPosition().y - coin.GetDimensions().y / 2.0f;
      destRect.z = coin.GetDimensions().x;
      destRect.w = coin.GetDimensions().y;

      m_debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(255, 255, 0, 255), coin.GetBox().GetBody()->GetAngle());
      //end the debug renderer
      m_debugRenderer.End();
      //render the outlines
      m_debugRenderer.Render(m_camera.GetCameraMatrix(), 2.0f);
    }
    //draw the selected enemy collision outlines
    if (m_selectedEnemy != NO_ENEMY)
    {
      //not using the .DrawDebug() function which I have in the entity base class,
      //because I want to have a different debug color for the "selected" enemy

      //make an alias to the current selected Coin
      const EnemyRobot& enemy = m_enemies[m_selectedEnemy];
      //set the color
      GameEngine::ColorRGBA8 color(255, 255, 0, 255);
      //Draw the box
      glm::vec4 destRect;
      destRect.x = enemy.GetCapsule().GetBody()->GetPosition().x - enemy.GetColDims().x / 2.0f;
      destRect.y = enemy.GetCapsule().GetBody()->GetPosition().y - (enemy.GetColDims().y - enemy.GetColDims().x) / 2.0f;
      destRect.z = enemy.GetColDims().x;
      destRect.w = enemy.GetColDims().y - enemy.GetColDims().x;
      m_debugRenderer.DrawBox(destRect, color, enemy.GetCapsule().GetBody()->GetAngle());

      //draw the 2 circles
      m_debugRenderer.DrawCircle(glm::vec2(destRect.x + enemy.GetColDims().x / 2.0f, destRect.y), color, enemy.GetColDims().x / 2.0f);
      m_debugRenderer.DrawCircle(glm::vec2(destRect.x + enemy.GetColDims().x / 2.0f, destRect.y + destRect.w), color, enemy.GetColDims().x / 2.0f);
      //end the debug renderer
      m_debugRenderer.End();
      //render the outlines
      m_debugRenderer.Render(m_camera.GetCameraMatrix(), 2.0f);
    }
  }

  //start using the texture program
  m_textureProgram.Use();

  //camera matrix
  glm::mat4 projectionMatrix = m_uiCamera.GetCameraMatrix();
  GLint pUniform = m_textureProgram.GetUniformLocation("P");
  glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

  //start up the spritebatch
  m_spriteBatch.Begin();

  {
    //draw the color picker quad
    const float QUAD_SIZE = 75.0f;

    glm::vec4 destRect;
    //The destination of the quad (world coords) 
    destRect.x = -300.0f;

    destRect.y = 260.0f;

    destRect.z = QUAD_SIZE;
    destRect.w = QUAD_SIZE;
    m_spriteBatch.Draw(destRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_blankTexture.id, 0.0f,
      GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255));

    //draw the color text
    std::string colorText;
    if (m_objectMode == ObjectMode::LIGHT)
    {
      colorText = "RGBA[" + std::to_string((int)m_colorPickerRed) + "," + std::to_string((int)m_colorPickerGreen) + "," +
        std::to_string((int)m_colorPickerBlue) + "," + std::to_string((int)m_colorPickerAlpha) + "]";
    }
    else
    {
      colorText = "RGB[" + std::to_string((int)m_colorPickerRed) + "," + std::to_string((int)m_colorPickerGreen) + "," + 
        std::to_string((int)m_colorPickerBlue) + "]";
    }
    m_spriteFont.draw(m_spriteBatch, colorText.c_str(), glm::vec2(destRect.x + destRect.z * 0.5f, destRect.y + destRect.w), glm::vec2(0.55f), 0.0f,
      GameEngine::ColorRGBA8(255, 255, 255, 255), GameEngine::Justification::MIDDLE);
  }
  //Draw custom labels for widgets
  for (auto& label : m_widgetLabels)
  {
    label.Draw(m_spriteBatch, m_spriteFont, m_window);
  }
  //stop using the spritebatch
  m_spriteBatch.End();
  //render the batches
  m_spriteBatch.RenderBatch();
  //stop using the texture program
  m_textureProgram.UnUse();
  //draw the gui
  m_gui.Draw();
}
void EditorScreen::DrawWorld()
{
  //begin using the texture program
  m_textureProgram.Use();

  //upload texture uniform
  GLint textureUniform = m_textureProgram.GetUniformLocation("mySampler");
  glUniform1i(textureUniform, 0);
  glActiveTexture(GL_TEXTURE0);

  //camera matrix
  glm::mat4 projectionMatrix = m_camera.GetCameraMatrix();
  GLint pUniform = m_textureProgram.GetUniformLocation("P");
  glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

  {
    //start up the spritebatch
    m_spriteBatch.Begin();
    //draw all the boxes, the player, the exit, trigger, obstacles and enemies
    for (auto& b : m_boxes)
    {
      b.Draw(m_spriteBatch);
    }
    for (auto& obs : m_obstacles)
    {
      obs.Draw(m_spriteBatch);
    }
    for (auto& enemy : m_enemies)
    {
      enemy.Draw(m_spriteBatch);
    }
    for (auto& coin : m_coins)
    {
      coin.Draw(m_spriteBatch);
    }
    if (m_hasPlayer)
    {
      m_player.Draw(m_spriteBatch);
    }
    if (m_hasExit)
    {
      m_exit.Draw(m_spriteBatch);
    }
    if (m_hasTrigger)
    {
      m_trigger.Draw(m_spriteBatch);
    }
    //stop the spritebatch
    m_spriteBatch.End();
    //render the batches
    m_spriteBatch.RenderBatch();
    //unuse the texture program
    m_textureProgram.UnUse();
  }

  {
    //draw lights
    m_lightProgram.Use();
    pUniform = m_textureProgram.GetUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    //additive blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    m_spriteBatch.Begin();

    for (auto& light : m_lights)
    {
      light.Draw(m_spriteBatch);
    }

    m_spriteBatch.End();
    m_spriteBatch.RenderBatch();

    m_lightProgram.UnUse();

    //restore alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  //Check if debug rendering is enabled
  if (m_debugRender)
  {
    // player debug
    if (m_hasPlayer)
    {
      m_player.DrawDebug(m_debugRenderer);
    }
    //create the destination rectangle
    glm::vec4 destRect;
    if (m_hasExit)
    {
      //set the main position to bottom left
      destRect.x = m_exit.GetBody()->GetPosition().x - m_exit.GetDimensions().x / 2.0f;
      destRect.y = m_exit.GetBody()->GetPosition().y - m_exit.GetDimensions().y / 2.0f;
      destRect.z = m_exit.GetDimensions().x;
      destRect.w = m_exit.GetDimensions().y;
      m_debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(255, 0, 0, 255), m_exit.GetBody()->GetAngle());
    }
    if (m_hasTrigger)
    {
      //set the main position to bottom left
      destRect.x = m_trigger.GetBody()->GetPosition().x - m_trigger.GetDimensions().x / 2.0f;
      destRect.y = m_trigger.GetBody()->GetPosition().y - m_trigger.GetDimensions().y / 2.0f;
      destRect.z = m_trigger.GetDimensions().x;
      destRect.w = m_trigger.GetDimensions().y;
      m_debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(255, 0, 0, 255), m_trigger.GetBody()->GetAngle());
    }
    //enemy debug
    for (auto& enemy : m_enemies)
    {
      enemy.DrawDebug(m_debugRenderer);
    }
    //render dynamic box borders
    for (auto& b : m_boxes)
    {
      //set the main position to bottom left of the sprite
      destRect.x = b.GetBody()->GetPosition().x - b.GetDimensions().x / 2.0f;
      destRect.y = b.GetBody()->GetPosition().y - b.GetDimensions().y / 2.0f;
      destRect.z = b.GetDimensions().x;
      destRect.w = b.GetDimensions().y;
      //dynamic = green / static = red
      if (b.IsDynamic())
      {
        m_debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(0, 255, 0, 255), b.GetBody()->GetAngle());
      }
      else
      {
        m_debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(255, 0, 0, 255), b.GetBody()->GetAngle());
      }
    }
    //Dynamic obstacle borders
    for (auto& obs : m_obstacles)
    {
      //set the main position to bottom left of the sprite
      destRect.x = obs.GetBody()->GetPosition().x - obs.GetDimensions().x / 2.0f;
      destRect.y = obs.GetBody()->GetPosition().y - obs.GetDimensions().y / 2.0f;
      destRect.z = obs.GetDimensions().x;
      destRect.w = obs.GetDimensions().y;
      //dynamic = green / static = red
      if (obs.IsDynamic())
      {
        m_debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(0, 255, 0, 255), obs.GetBody()->GetAngle());
      }
      else
      {
        m_debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(255, 0, 0, 255), obs.GetBody()->GetAngle());
      }
    }
    //Dynamic coin borders
    for (auto& coin : m_coins)
    {
      coin.DrawDebug(m_debugRenderer);
    }
    //render magenta light selection radius
    for (auto& light : m_lights)
    {
      m_debugRenderer.DrawCircle(light.position, GameEngine::ColorRGBA8(255, 0, 255, 255), LIGHT_SELECT_RADIUS);
    }
    //draw axis
    // +X axis
    m_debugRenderer.DrawLine(glm::vec2(0.0f), glm::vec2(100000.0f, 0.0f), GameEngine::ColorRGBA8(255, 0, 0, 200));
    // -X axis
    m_debugRenderer.DrawLine(glm::vec2(0.0f), glm::vec2(-100000.0f, 0.0f), GameEngine::ColorRGBA8(255, 0, 0, 100));
    // +Y axis
    m_debugRenderer.DrawLine(glm::vec2(0.0f), glm::vec2(0.0f, 100000.0f), GameEngine::ColorRGBA8(0, 255, 0, 200));
    // -Y axis
    m_debugRenderer.DrawLine(glm::vec2(0.0f), glm::vec2(0.0f, -100000.0f), GameEngine::ColorRGBA8(0, 255, 0, 100));

    m_debugRenderer.End();
    m_debugRenderer.Render(m_camera.GetCameraMatrix(), 2.0f);
  }

}
void EditorScreen::ClearLevel()
{
  m_boxes.clear();
  m_coins.clear();
  m_lights.clear();
  m_obstacles.clear();
  m_enemies.clear();
  m_hasPlayer = false;
  m_hasExit = false;
  m_hasTrigger = false;
  m_world.reset();
  m_world = std::make_unique<b2World>(GRAVITY);
}

void EditorScreen::InitUI()
{
  //Init the UI
  m_gui.Init("GUI");
  m_gui.LoadScheme("TaharezLook.scheme");
  m_gui.SetFont("DejaVuSans-10");

  //add group box back panel
  m_groupBox = static_cast<CEGUI::GroupBox*>(m_gui.CreateWidget("TaharezLook/GroupBox", glm::vec4(0.001f, 0.01f, 0.55, 0.85), glm::vec4(0.0f), "GroupBox"));
  //GroupBox Should be behind everything
  m_groupBox->setAlwaysOnTop(false);
  m_groupBox->moveToBack();
  //also disable it, so that clicking on it doesn't move it to the foreground, where it can steal events from other widgets
  m_groupBox->disable();

  {
    //add the color picker
    const float X_POS = 0.01f;
    const float X_DIM = 0.1f, Y_DIM = 0.04f;
    const float Y_POS = 0.04f;
    const float PADDING = 0.005f;

    m_rSpinner = static_cast<CEGUI::Spinner*>(m_gui.CreateWidget("TaharezLook/Spinner", glm::vec4(X_POS, Y_POS, X_DIM, Y_DIM), glm::vec4(0.0f), "RedSpinner"));
    m_rSpinner->setMinimumValue(0.0f);
    m_rSpinner->setMaximumValue(255.0f);
    m_rSpinner->setCurrentValue(m_colorPickerRed);
    m_rSpinner->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&EditorScreen::OnColorPickerRedChanged, this));
    m_rSpinner->setStepSize(1.0f);

    m_gSpinner = static_cast<CEGUI::Spinner*>(m_gui.CreateWidget("TaharezLook/Spinner", glm::vec4(X_POS, Y_POS + Y_DIM + PADDING, X_DIM, Y_DIM), glm::vec4(0.0f), "GreenSlider"));
    m_gSpinner->setMinimumValue(0.0f);
    m_gSpinner->setMaximumValue(255.0f);
    m_gSpinner->setCurrentValue(m_colorPickerGreen);
    m_gSpinner->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&EditorScreen::OnColorPickerGreenChanged, this));
    m_gSpinner->setStepSize(1.0f);

    m_bSpinner = static_cast<CEGUI::Spinner*>(m_gui.CreateWidget("TaharezLook/Spinner", glm::vec4(X_POS, Y_POS + (Y_DIM + PADDING) * 2, X_DIM, Y_DIM), glm::vec4(0.0f), "BlueSlider"));
    m_bSpinner->setMinimumValue(0.0f);
    m_bSpinner->setMaximumValue(255.0f);
    m_bSpinner->setCurrentValue(m_colorPickerBlue);
    m_bSpinner->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&EditorScreen::OnColorPickerBlueChanged, this));
    m_bSpinner->setStepSize(1.0f);

    m_aSpinner = static_cast<CEGUI::Spinner*>(m_gui.CreateWidget("TaharezLook/Spinner", glm::vec4(X_POS + X_DIM + PADDING, Y_POS + Y_DIM + PADDING, X_DIM, Y_DIM), glm::vec4(0.0f), "AlphaSlider"));
    m_aSpinner->setMinimumValue(0.0f);
    m_aSpinner->setMaximumValue(255.0f);
    m_aSpinner->setCurrentValue(m_colorPickerAlpha);
    m_aSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditorScreen::OnColorPickerAlphaChanged, this));
    m_aSpinner->setStepSize(1.0f);
  }

  {
    //add object type radio buttons
    const float X_POS = 0.04f;
    const float Y_POS = 0.20f;
    const float DIMS_PIXELS = 20.0f;
    const float PADDING = 0.088f;
    const float TEXT_SCALE = 0.6f;
    const int GROUP_ID = 1;

    m_playerRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.CreateWidget("TaharezLook/RadioButton", glm::vec4(X_POS, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "PlayerButton"));
    m_playerRadioButton->setSelected(true);
    m_playerRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&EditorScreen::OnPlayerMouseClick, this));
    m_playerRadioButton->setGroupID(GROUP_ID);
    m_widgetLabels.emplace_back(m_playerRadioButton, "Player", TEXT_SCALE);

    m_platformRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.CreateWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "PlatformButton"));
    m_platformRadioButton->setSelected(false);
    m_platformRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&EditorScreen::OnPlatformMouseClick, this));
    m_platformRadioButton->setGroupID(GROUP_ID);
    m_widgetLabels.emplace_back(m_platformRadioButton, "Platform", TEXT_SCALE);

    m_finishRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.CreateWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING * 2.0, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "FinishedButton"));
    m_finishRadioButton->setSelected(false);
    m_finishRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&EditorScreen::OnFinishMouseClick, this));
    m_finishRadioButton->setGroupID(GROUP_ID);
    m_widgetLabels.emplace_back(m_finishRadioButton, "Finish", TEXT_SCALE);

    m_lightRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.CreateWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING * 3.0, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "LightButton"));
    m_lightRadioButton->setSelected(false);
    m_lightRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&EditorScreen::OnLightMouseClick, this));
    m_lightRadioButton->setGroupID(GROUP_ID);
    m_widgetLabels.emplace_back(m_lightRadioButton, "Light", TEXT_SCALE);

    m_triggerRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.CreateWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING * 4.0, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "TriggerButton"));
    m_triggerRadioButton->setSelected(false);
    m_triggerRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&EditorScreen::OnTriggerMouseClick, this));
    m_triggerRadioButton->setGroupID(GROUP_ID);
    m_widgetLabels.emplace_back(m_triggerRadioButton, "Trigger", TEXT_SCALE);

    m_obstacleRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.CreateWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING * 5.0f, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "ObstacleButton"));
    m_obstacleRadioButton->setSelected(false);
    m_obstacleRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&EditorScreen::OnObstaleMouseClick, this));
    m_obstacleRadioButton->setGroupID(GROUP_ID);
    m_widgetLabels.emplace_back(m_obstacleRadioButton, "Obstacle", TEXT_SCALE);

    m_enemyRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.CreateWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING * 5.0f, Y_POS + PADDING, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "EnemyButton"));
    m_enemyRadioButton->setSelected(false);
    m_enemyRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&EditorScreen::OnEnemyMouseClick, this));
    m_enemyRadioButton->setGroupID(GROUP_ID);
    m_widgetLabels.emplace_back(m_enemyRadioButton, "Enemy", TEXT_SCALE);

    m_coinRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.CreateWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING * 4.0, Y_POS + PADDING, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "CoinButton"));
    m_coinRadioButton->setSelected(false);
    m_coinRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&EditorScreen::OnCoinMouseClick, this));
    m_coinRadioButton->setGroupID(GROUP_ID);
    m_widgetLabels.emplace_back(m_coinRadioButton, "Coins", TEXT_SCALE);

    m_objectMode = ObjectMode::PLAYER;
  }

  {
    //add the physics mode radio buttons as well as rotation and size spinner
    const float X_POS = 0.02f;
    const float Y_POS = 0.28f;
    const float DIMS_PIXELS = 20.0f;
    const float PADDING = 0.04f;
    const float TEXT_SCALE = 0.7f;
    const int GROUP_ID = 2;

    m_rigidRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.CreateWidget("TaharezLook/RadioButton", glm::vec4(X_POS, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "RigidButton"));
    m_rigidRadioButton->setSelected(true);
    m_rigidRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&EditorScreen::OnRigidMouseClick, this));
    m_rigidRadioButton->setGroupID(GROUP_ID);
    m_widgetLabels.emplace_back(m_rigidRadioButton, "Rigid", TEXT_SCALE);

    m_dynamicRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.CreateWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING * 2, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "DynamicButton"));
    m_dynamicRadioButton->setSelected(false);
    m_dynamicRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&EditorScreen::OnDynamicMouseClick, this));
    m_dynamicRadioButton->setGroupID(GROUP_ID);
    m_widgetLabels.emplace_back(m_dynamicRadioButton, "Dynamic", TEXT_SCALE);

    //set rotation spinner
    m_rotationSpinner = static_cast<CEGUI::Spinner*>(m_gui.CreateWidget("TaharezLook/Spinner", glm::vec4(X_POS + PADDING * 3.5, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS * 5, DIMS_PIXELS * 2), "RotationSpinner"));
    m_rotationSpinner->setMinimumValue(0.0);
    m_rotationSpinner->setMaximumValue(M_PI * 2.0);
    m_rotationSpinner->setCurrentValue(m_rotation);
    m_rotationSpinner->setStepSize(0.01);
    m_rotationSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);
    m_rotationSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditorScreen::OnRotationValueChange, this));
    m_widgetLabels.emplace_back(m_rotationSpinner, "Rotation", TEXT_SCALE);

    // Light size
    m_sizeSpinner = static_cast<CEGUI::Spinner*>(m_gui.CreateWidget("TaharezLook/Spinner", glm::vec4(X_POS + PADDING * 2.0, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS * 5, DIMS_PIXELS * 2), "SizeSpinner"));
    m_sizeSpinner->setMinimumValue(0.0);
    m_sizeSpinner->setMaximumValue(100.0);
    m_sizeSpinner->setCurrentValue(m_lightSize);
    m_sizeSpinner->setStepSize(0.1);
    m_sizeSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);
    m_sizeSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditorScreen::OnSizeValueChange, this));
    m_widgetLabels.emplace_back(m_sizeSpinner, "Size", TEXT_SCALE);

    m_physicsMode = PhysicsMode::RIGID;

  }

  {
    //add platform dimension spinner ( and the music volume as well )
    const float X_POS = 0.02f;
    const float Y_POS = 0.40f;
    const float DIMS_PIXELS = 20.0f;
    const float PADDING = 0.08f;
    const float TEXT_SCALE = 0.7f;
    m_widthSpinner = static_cast<CEGUI::Spinner*>(m_gui.CreateWidget("TaharezLook/Spinner", glm::vec4(X_POS, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS * 5, DIMS_PIXELS * 2), "WidthSpinner"));
    m_widthSpinner->setMinimumValue(0.0);
    m_widthSpinner->setMaximumValue(10000.0);
    m_widthSpinner->setCurrentValue(m_boxDims.x);
    m_widthSpinner->setStepSize(0.1);
    m_widthSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);
    m_widthSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditorScreen::OnWidthValueChange, this));
    m_widgetLabels.emplace_back(m_widthSpinner, "Width", TEXT_SCALE);

    m_heightSpinner = static_cast<CEGUI::Spinner*>(m_gui.CreateWidget("TaharezLook/Spinner", glm::vec4(X_POS + PADDING * 2.0, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS * 5, DIMS_PIXELS * 2), "HeightSpinner"));
    m_heightSpinner->setMinimumValue(0.0);
    m_heightSpinner->setMaximumValue(10000.0);
    m_heightSpinner->setCurrentValue(m_boxDims.y);
    m_heightSpinner->setStepSize(0.1);
    m_heightSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);
    m_heightSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditorScreen::OnHeightValueChange, this));
    m_widgetLabels.emplace_back(m_heightSpinner, "Height", TEXT_SCALE);

    //music volume spinner
    m_volumeSpinner = static_cast<CEGUI::Spinner*>(m_gui.CreateWidget("TaharezLook/Spinner", glm::vec4(X_POS + PADDING * 2.0, Y_POS * 2.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS * 5, DIMS_PIXELS * 2), "VolumeSpinner"));
    m_volumeSpinner->setMinimumValue(0.0f);
    m_volumeSpinner->setMaximumValue(128.0f);
    m_volumeSpinner->setCurrentValue(128.0f);
    m_volumeSpinner->setStepSize(1.0f);
    m_volumeSpinner->setTextInputMode(CEGUI::Spinner::Integer);
    m_volumeSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditorScreen::OnMusicVolumeChanged, this));
    m_widgetLabels.emplace_back(m_volumeSpinner, "Music Vol", TEXT_SCALE);
  }

  {
    //add selection mode radio buttons and debug render toggle
    const float X_POS = 0.032f;
    const float Y_POS = 0.55f;
    const float DIMS_PIXELS = 20.0f;
    const float PADDING = 0.08f;
    const float TEXT_SCALE = 0.7f;
    const int GROUP_ID = 3;

    m_selectRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.CreateWidget("TaharezLook/RadioButton", glm::vec4(X_POS, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "SelectButton"));
    m_selectRadioButton->setSelected(true);
    m_selectRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&EditorScreen::OnSelectMouseClick, this));
    m_selectRadioButton->setGroupID(GROUP_ID);
    m_widgetLabels.emplace_back(m_selectRadioButton, "Select", TEXT_SCALE);

    m_placeRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.CreateWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "PlaceButton"));
    m_placeRadioButton->setSelected(false);
    m_placeRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&EditorScreen::OnPlaceMouseClick, this));
    m_placeRadioButton->setGroupID(GROUP_ID);
    m_widgetLabels.emplace_back(m_placeRadioButton, "Place", TEXT_SCALE);

    m_debugToggle = static_cast<CEGUI::ToggleButton*>(m_gui.CreateWidget("TaharezLook/Checkbox", glm::vec4(X_POS + PADDING * 2.5, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "DebugToggle"));
    m_debugToggle->setSelected(false);
    m_debugToggle->subscribeEvent(CEGUI::ToggleButton::EventMouseClick, CEGUI::Event::Subscriber(&EditorScreen::OnDebugToggleClick, this));
    m_widgetLabels.emplace_back(m_debugToggle, "Debug", TEXT_SCALE);
    m_debugRender = false;

    m_selectionMode = SelectionMode::SELECT;
  }

  {
    //add save and back buttons
    m_saveButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.03f, 0.64f, 0.1f, 0.05f), glm::vec4(0.0f), "SaveButton"));
    m_saveButton->setText("Save");
    m_saveButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScreen::OnSaveMouseClick, this));

    m_saveButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.03f, 0.71f, 0.1f, 0.05f), glm::vec4(0.0f), "LoadButton"));
    m_saveButton->setText("Load");
    m_saveButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScreen::OnLoadMouseClick, this));

    m_backButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.03f, 0.78f, 0.1f, 0.05f), glm::vec4(0.0f), "BackButton"));
    m_backButton->setText("Back");
    m_backButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScreen::OnBackMouseClick, this));
  }

  {
    //add save window widgets
    m_saveWindow = static_cast<CEGUI::FrameWindow*>(m_gui.CreateWidget("TaharezLook/FrameWindow", glm::vec4(0.1f, 0.3f, 0.4f, 0.4f), glm::vec4(0.0f), "SaveWindow"));
    m_saveWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&EditorScreen::OnSaveCancelClick, this));
    m_saveWindow->setText("Save Level");
    //don't let the user drag the window around
    m_saveWindow->setDragMovingEnabled(false);

    //children of saveWindow
    m_saveWindowCombobox = static_cast<CEGUI::Combobox*>(m_gui.CreateWidget(m_saveWindow, "TaharezLook/Combobox", glm::vec4(0.1f, 0.2f, 0.8f, 0.4f), glm::vec4(0.0f), "SaveCombobox"));
    m_saveWindowSaveButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget(m_saveWindow, "TaharezLook/Button", glm::vec4(0.35f, 0.8f, 0.3f, 0.1f), glm::vec4(0.0f), "SaveCancelButton"));
    m_saveWindowSaveButton->setText("Save");
    m_saveWindowSaveButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScreen::OnSave, this));

    //start disabled
    m_saveWindow->disable();
    m_saveWindow->setAlpha(0.0f);
  }

  {
    //add load window widgets
    m_loadWindow = static_cast<CEGUI::FrameWindow*>(m_gui.CreateWidget("TaharezLook/FrameWindow", glm::vec4(0.1f, 0.3f, 0.4f, 0.4f), glm::vec4(0.0f), "LoadWindow"));
    m_loadWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&EditorScreen::OnLoadCancelClick, this));
    m_loadWindow->setText("Load Level");
    // Don't let user drag window around
    m_loadWindow->setDragMovingEnabled(false);

    // Children of loadWindow
    m_loadWindowCombobox = static_cast<CEGUI::Combobox*>(m_gui.CreateWidget(m_loadWindow, "TaharezLook/Combobox", glm::vec4(0.1f, 0.2f, 0.8f, 0.4f), glm::vec4(0.0f), "LoadCombobox"));
    m_loadWindowLoadButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget(m_loadWindow, "TaharezLook/Button", glm::vec4(0.35f, 0.8f, 0.3f, 0.1f), glm::vec4(0.0f), "LoadCancelButton"));
    m_loadWindowLoadButton->setText("Load");
    m_loadWindowLoadButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScreen::OnLoad, this));

    // Start disabled
    m_loadWindow->disable();
    m_loadWindow->setAlpha(0.0f);
  }
  //set the platform and light widget visibility to false on startup
  SetLightWidgetVisibility(false);
  SetPlatformWidgetVisibility(false);
  //set the custom mouse cursor
  m_gui.SetMouseCursor("TaharezLook/MouseArrow");
  m_gui.ShowMouseCursor();
  //hide the normal mouse cursor
  SDL_ShowCursor(0);
}
void EditorScreen::CheckInput()
{
  SDL_Event evnt;
  //update the input manager
  m_inputManager.Update();
  //handle sdl events
  while (SDL_PollEvent(&evnt))
  {
    m_gui.OnSDLEvent(evnt);
    switch (evnt.type)
    {
    case SDL_QUIT:
      OnExitClicked(CEGUI::EventArgs());
      break;
    case SDL_MOUSEBUTTONDOWN:
      UpdateMouseDown(evnt);
      break;
    case SDL_MOUSEBUTTONUP:
      UpdateMouseUp(evnt);
      break;
    case SDL_MOUSEMOTION:
      UpdateMouseMotion(evnt);
      break;
    case SDL_MOUSEWHEEL:
      // Linear scaling sucks for mouse wheel zoom so we multiply by getScale() instead.
      m_camera.OffsetScale(m_camera.GetScale() * evnt.wheel.y * 0.1f);
      break;
    case SDL_KEYDOWN:
      m_inputManager.PressKey(evnt.key.keysym.sym);
      break;
    case SDL_KEYUP:
      m_inputManager.ReleaseKey(evnt.key.keysym.sym);
      break;
    default:
      break;
    }
  }
}
//Check to see if the user has selected a light
bool inLightSelect(const Light& _l, const glm::vec2& _pos)
{
  //returns true if the user is selecting in the light
  return (glm::length(_pos - _l.position) <= LIGHT_SELECT_RADIUS);
}

//Check to see if the user has selected an enemy
bool inEnemySelect(const EnemyRobot& _enemy, const glm::vec2& _pos)
{
  //returns true if the user is selecting the enemy
  if ( (_pos.x >= _enemy.GetPosition().x - _enemy.GetDrawDims().x / 2.0f && _pos.x <= _enemy.GetPosition().x + _enemy.GetDrawDims().x / 2.0f) &&
       (_pos.y >= _enemy.GetPosition().y - _enemy.GetDrawDims().y / 2.0f && _pos.y <= _enemy.GetPosition().y + _enemy.GetDrawDims().y / 2.0f) )
  {
    return true;
  }
  return false;
}

void EditorScreen::UpdateMouseDown(const SDL_Event& _evnt)
{
  //texture for boxes
  static GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/Tiles/MetalTexture.png");

  glm::vec2 pos;
  glm::vec4 uvRect;

  switch (_evnt.button.button)
  {
  case SDL_BUTTON_LEFT:
    m_mouseButtons[MOUSE_LEFT] = true;
    //don't place object or select things when clicking on UI
    if (isMouseInUI())
    {
      break;
    }

    if (m_selectionMode == SelectionMode::SELECT)
    {
      //select mode
      pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.button.x, _evnt.button.y));
      /* Lights have selection priority, so check lights first */
      /* If a light is already selected, check to see if we clicked it again */
      /******************************
       *         CHECK LIGHTS       *
       ******************************/
      if (m_selectedLight != NO_LIGHT && inLightSelect(m_lights[m_selectedLight], pos))
      {
        //we selected the same, do nothing.
      }
      else
      {
        //unselect
        m_selectedLight = NO_LIGHT;
        //find the light that is selected (if none selected light stays NO_LIGHT)
        for (size_t i = 0; i < m_lights.size(); i++)
        {
          if (inLightSelect(m_lights[i], pos))
          {
            m_selectedLight = i;
            break;
          }
        }
      }
      //if we selected a light
      if (m_selectedLight != NO_LIGHT)
      {
        //get the offset from the center so I can drag correctly
        m_selectedOffset = pos - m_lights[m_selectedLight].position;
        m_selectedBox = NO_LIGHT;
        m_isDragging = true;
        //set the variables first, so refreshing controls works
        m_colorPickerRed = m_lights[m_selectedLight].color.r;
        m_colorPickerGreen = m_lights[m_selectedLight].color.g;
        m_colorPickerBlue = m_lights[m_selectedLight].color.b;
        m_colorPickerAlpha = m_lights[m_selectedLight].color.a;
        m_lightSize = m_lights[m_selectedLight].size;
        //set the widget values
        m_rSpinner->setCurrentValue(m_colorPickerRed);
        m_gSpinner->setCurrentValue(m_colorPickerGreen);
        m_bSpinner->setCurrentValue(m_colorPickerBlue);
        m_aSpinner->setCurrentValue(m_colorPickerAlpha);
        m_sizeSpinner->setCurrentValue(m_lightSize);
        m_lightRadioButton->setSelected(true);
        SetPlatformWidgetVisibility(false);
        SetLightWidgetVisibility(true);
        m_objectMode = ObjectMode::LIGHT;
        break;
      }
      //if a box is already selected, test to see if we just clicked it again
      /******************************
      *         CHECK BOXES         *
      ******************************/
      if (m_selectedBox != NO_BOX && m_boxes[m_selectedBox].TestPoint(pos.x, pos.y))
      {
        //selected an already selected box => do nothings
      }
      else
      {
        //unselect
        m_selectedBox = NO_BOX;
        //find the box that is selected (if none selectedBOx stays NO_BOX)
        for (size_t i = 0; i < m_boxes.size(); i++)
        {
          if (m_boxes[i].TestPoint(pos.x, pos.y))
          {
            m_selectedBox = i;
            break;
          }
        }
      }
      //if a box was newly selected
      if (m_selectedBox != NO_BOX)
      {
        //get the offset from the center so it can be dragged correctly
        m_selectedOffset = pos - m_boxes[m_selectedBox].GetPosition();
        m_isDragging = true;
        // set the correct variables so refreshing the controls work
        m_rotation = m_boxes[m_selectedBox].GetBody()->GetAngle();
        m_boxDims.x = m_boxes[m_selectedBox].GetDimensions().x;
        m_boxDims.y = m_boxes[m_selectedBox].GetDimensions().y;
        m_colorPickerRed = m_boxes[m_selectedBox].GetColor().r;
        m_colorPickerGreen = m_boxes[m_selectedBox].GetColor().g;
        m_colorPickerBlue = m_boxes[m_selectedBox].GetColor().b;
        //set widget values
        m_rotationSpinner->setCurrentValue(m_rotation);
        m_widthSpinner->setCurrentValue(m_boxDims.x);
        m_heightSpinner->setCurrentValue(m_boxDims.y);
        m_rSpinner->setCurrentValue(m_colorPickerRed);
        m_gSpinner->setCurrentValue(m_colorPickerGreen);
        m_bSpinner->setCurrentValue(m_colorPickerBlue);
        if (m_boxes[m_selectedBox].IsDynamic())
        {
          m_dynamicRadioButton->setSelected(true);
          m_physicsMode = PhysicsMode::DYNAMIC;
        }
        else
        {
          m_rigidRadioButton->setSelected(true);
          m_physicsMode = PhysicsMode::RIGID;
        }
        m_platformRadioButton->setSelected(true);
        SetPlatformWidgetVisibility(true);
        SetLightWidgetVisibility(false);
        m_objectMode = ObjectMode::PLATFORM;
      }

      //if an obstacle is already selected, test to see if we just clicked it again
      /******************************
      *        CHECK OBSTACLES      *
      ******************************/
      if (m_selectedObstacle != NO_OBSTACLE && m_obstacles[m_selectedObstacle].TestPoint(pos.x, pos.y))
      {
        //selected an already selected box => do nothings
      }
      else
      {
        //unselect
        m_selectedObstacle = NO_OBSTACLE;
        //find the obstacle that is selected (if none, selectedObstacle stays NO_OBSTACLE)
        for (size_t i = 0; i < m_obstacles.size(); i++)
        {
          if (m_obstacles[i].TestPoint(pos.x, pos.y))
          {
            m_selectedObstacle = i;
            break;
          }
        }
      }
      //if an obstacle was newly selected
      if (m_selectedObstacle != NO_OBSTACLE)
      {
        //get the offset from the center so it can be dragged correctly
        m_selectedOffset = pos - m_obstacles[m_selectedObstacle].GetPosition();
        m_isDragging = true;
        // set the correct variables so refreshing the controls work
        m_rotation = m_obstacles[m_selectedObstacle].GetBody()->GetAngle();
        m_boxDims.x = m_obstacles[m_selectedObstacle].GetDimensions().x;
        m_boxDims.y = m_obstacles[m_selectedObstacle].GetDimensions().y;
        m_colorPickerRed = m_obstacles[m_selectedObstacle].GetColor().r;
        m_colorPickerGreen = m_obstacles[m_selectedObstacle].GetColor().g;
        m_colorPickerBlue = m_obstacles[m_selectedObstacle].GetColor().b;
        //set widget values
        m_rotationSpinner->setCurrentValue(m_rotation);
        m_widthSpinner->setCurrentValue(m_boxDims.x);
        m_heightSpinner->setCurrentValue(m_boxDims.y);
        m_rSpinner->setCurrentValue(m_colorPickerRed);
        m_gSpinner->setCurrentValue(m_colorPickerGreen);
        m_bSpinner->setCurrentValue(m_colorPickerBlue);
        if (m_obstacles[m_selectedObstacle].IsDynamic())
        {
          m_dynamicRadioButton->setSelected(true);
          m_physicsMode = PhysicsMode::DYNAMIC;
        }
        else
        {
          m_rigidRadioButton->setSelected(true);
          m_physicsMode = PhysicsMode::RIGID;
        }
        m_obstacleRadioButton->setSelected(true);
        SetPlatformWidgetVisibility(true);
        SetLightWidgetVisibility(false);
        m_objectMode = ObjectMode::OBSTACLE;
      }

      //if an obstacle is already selected, test to see if we just clicked it again
      /******************************
      *        CHECK COINS          *
      ******************************/
      if (m_selectedCoin != NO_COIN && m_coins[m_selectedCoin].GetBox().TestPoint(pos.x, pos.y))
      {
        //selected an already selected box => do nothings
      }
      else
      {
        //unselect
        m_selectedCoin = NO_COIN;
        //find the obstacle that is selected (if none, selectedObstacle stays NO_OBSTACLE)
        for (size_t i = 0; i < m_coins.size(); i++)
        {
          if (m_coins[i].GetBox().TestPoint(pos.x, pos.y))
          {
            m_selectedCoin = i;
            break;
          }
        }
      }
      //if an obstacle was newly selected
      if (m_selectedCoin != NO_COIN)
      {
        //get the offset from the center so it can be dragged correctly
        m_selectedOffset = pos - m_coins[m_selectedCoin].GetBox().GetPosition();
        m_isDragging = true;
        // set the correct variables so refreshing the controls work
        m_rotation = m_coins[m_selectedCoin].GetBox().GetBody()->GetAngle();
        m_boxDims.x = m_coins[m_selectedCoin].GetDimensions().x;
        m_boxDims.y = m_coins[m_selectedCoin].GetDimensions().y;
        m_colorPickerRed = m_coins[m_selectedCoin].GetColor().r;
        m_colorPickerGreen = m_coins[m_selectedCoin].GetColor().g;
        m_colorPickerBlue = m_coins[m_selectedCoin].GetColor().b;
        //set widget values
        m_rotationSpinner->setCurrentValue(m_rotation);
        m_widthSpinner->setCurrentValue(m_boxDims.x);
        m_heightSpinner->setCurrentValue(m_boxDims.y);
        m_rSpinner->setCurrentValue(m_colorPickerRed);
        m_gSpinner->setCurrentValue(m_colorPickerGreen);
        m_bSpinner->setCurrentValue(m_colorPickerBlue);
        if (m_coins[m_selectedCoin].GetBox().IsDynamic())
        {
          m_dynamicRadioButton->setSelected(true);
          m_physicsMode = PhysicsMode::DYNAMIC;
        }
        else
        {
          m_rigidRadioButton->setSelected(true);
          m_physicsMode = PhysicsMode::RIGID;
        }
        m_coinRadioButton->setSelected(true);
        SetPlatformWidgetVisibility(true);
        SetLightWidgetVisibility(false);
        m_objectMode = ObjectMode::COIN;
      }

      /******************************
      *        CHECK ENEMIES        *
      ******************************/
      if (m_selectedEnemy != NO_ENEMY && inEnemySelect(m_enemies[m_selectedEnemy], pos))
      {
        //selected an already selected box => do nothings
      }
      else
      {
        //unselect
        m_selectedEnemy = NO_ENEMY;
        //find the enemy that is selected (if none, selectedEnemy stays NO_ENEMY)
        for (size_t i = 0; i < m_enemies.size(); i++)
        {
          if (inEnemySelect(m_enemies[i], pos))
          {
            m_selectedEnemy = i;
            break;
          }
        }
      }
      //if an enemy was newly selected
      if (m_selectedEnemy != NO_ENEMY)
      {
        //get the offset from the center so it can be dragged correctly
        m_selectedOffset = pos - m_enemies[m_selectedEnemy].GetPosition();
        m_isDragging = true;
        // set the correct variables so refreshing the controls work
        m_colorPickerRed = m_enemies[m_selectedEnemy].GetColor().r;
        m_colorPickerGreen = m_enemies[m_selectedEnemy].GetColor().g;
        m_colorPickerBlue = m_enemies[m_selectedEnemy].GetColor().b;
        //set widget values
        m_rSpinner->setCurrentValue(m_colorPickerRed);
        m_gSpinner->setCurrentValue(m_colorPickerGreen);
        m_bSpinner->setCurrentValue(m_colorPickerBlue);
        
        m_enemyRadioButton->setSelected(true);
        m_objectMode = ObjectMode::ENEMY;
      }
    }
    else
    {
      /*****************************
      *         PLACE MODE         *
      ******************************/
      Box newBox;
      Light newLight;
      EnemyRobot newEnemy;
      //Place
      switch (m_objectMode)
      {
      case ObjectMode::PLAYER:
        //just remove the current player = easiest way
        if (m_hasPlayer)
        {
          m_player.Destroy(m_world.get());
        }
        //re-init the player
        pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.button.x, _evnt.button.y));

        m_player.Init(m_world.get(), pos, glm::vec2(2.0f), glm::vec2(1.0f, 1.8f),
          GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255));

        m_hasPlayer = true;
        break;
      case ObjectMode::PLATFORM:
        pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.button.x, _evnt.button.y));
        uvRect.x = pos.x;
        uvRect.y = pos.y;
        uvRect.z = m_boxDims.x;
        uvRect.w = m_boxDims.y;
        //initialize the new platform
        newBox.Init(m_world.get(), pos, m_boxDims, texture, GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed, 
          (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255), false, m_physicsMode == PhysicsMode::DYNAMIC, false, m_rotation, uvRect);

        m_boxes.push_back(newBox);
        break;
      case ObjectMode::TRIGGER:
        //just remove the current trigger = easiest way
        if (m_hasTrigger)
        {
          m_trigger.Destroy(m_world.get());
        }
        //re-init the trigger
        pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.button.x, _evnt.button.y));

        m_trigger.Init(m_world.get(), pos, m_boxDims, GameEngine::ResourceManager::GetTexture("Assets/Objects/Switch2.png"),
          GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255),
          false, false, true, m_rotation);

        m_hasTrigger = true;
        break;
      case ObjectMode::LIGHT:
        newLight.position = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.button.x, _evnt.button.y));
        newLight.size = m_lightSize;
        newLight.color = GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, (GLubyte)m_colorPickerAlpha);
        //push back the light
        m_lights.push_back(newLight);
        break;
      case ObjectMode::FINISH:
        //destroy the current exit if there is one
        if (m_hasExit)
        {
          m_exit.Destroy(m_world.get());
        }
        pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.button.x, _evnt.button.y));

        //Init the new exit
        m_exit.Init(m_world.get(), pos, m_boxDims, GameEngine::ResourceManager::GetTexture("Assets/Objects/DoorLocked.png"),
          GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255),
          false, false, true, m_rotation);

        m_hasExit = true;
        break;
      case ObjectMode::OBSTACLE:
        pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.button.x, _evnt.button.y));
        //Initialize the new obstacle
        newBox.Init(m_world.get(), pos, m_boxDims, GameEngine::ResourceManager::GetTexture("Assets/Tiles/Spike.png"),
          GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255),
          false, m_physicsMode == PhysicsMode::DYNAMIC, false, m_rotation);

        m_obstacles.push_back(newBox);
        break;
      case ObjectMode::ENEMY:
        pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.button.x, _evnt.button.y));
        //initialize the new enemy
        newEnemy.Init(m_world.get(), pos, glm::vec2(3.0f), glm::vec2(2.0f, 2.8f),
          GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255));

        m_enemies.push_back(newEnemy);
        break;
      case ObjectMode::COIN:
        pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.button.x, _evnt.button.y));
        //emplace the new coin
        m_coins.emplace_back(m_world.get(), pos, m_boxDims, GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed,
          (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255));
        break;
      default:
        break;
      }
    }
    break;
  case SDL_BUTTON_RIGHT:
    m_mouseButtons[MOUSE_RIGHT] = true;
    break;
  default:
    break;
  }
}
void EditorScreen::UpdateMouseUp(const SDL_Event& _evnt)
{
  //releasing a mouse button
  switch (_evnt.button.button)
  {
  case SDL_BUTTON_LEFT:
    m_mouseButtons[MOUSE_LEFT] = false;
    break;
  case SDL_BUTTON_RIGHT:
    m_mouseButtons[MOUSE_RIGHT] = false;
  default:
    break;
  }
  m_isDragging = false;
}
void EditorScreen::UpdateMouseMotion(const SDL_Event& _evnt)
{
  // if right button is down, drag camera
  const float SPEED = 0.05f;
  if (m_mouseButtons[MOUSE_RIGHT])
  {
    m_camera.OffsetPosition(glm::vec2(-_evnt.motion.xrel, _evnt.motion.yrel * m_camera.GetAspectRatio()) * SPEED);
  }
  //dragging stuff around
  if (m_isDragging && m_mouseButtons[MOUSE_LEFT])
  {
    //Light
    if (m_selectedLight != NO_LIGHT)
    {
      glm::vec2 pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.motion.x, _evnt.motion.y)) - m_selectedOffset;
      RefreshSelectedLight(pos);
    }
    //Box
    if (m_selectedBox != NO_BOX)
    {
      glm::vec2 pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.motion.x, _evnt.motion.y)) - m_selectedOffset;
      RefreshSelectedBox(pos);
    }
    //obstacles
    if (m_selectedObstacle != NO_OBSTACLE)
    {
      glm::vec2 pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.motion.x, _evnt.motion.y)) - m_selectedOffset;
      RefreshSelectedObstacle(pos);
    }
    //coins
    if (m_selectedObstacle != NO_OBSTACLE)
    {
      glm::vec2 pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.motion.x, _evnt.motion.y)) - m_selectedOffset;
      RefreshSelectedCoin(pos);
    }
    //enemies
    if (m_selectedEnemy != NO_ENEMY)
    {
      glm::vec2 pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.motion.x, _evnt.motion.y)) - m_selectedOffset;
      RefreshSelectedEnemy(pos);
    }
    //enemies
    if (m_selectedCoin != NO_COIN)
    {
      glm::vec2 pos = m_camera.ConvertScreenToWorld(glm::vec2(_evnt.motion.x, _evnt.motion.y)) - m_selectedOffset;
      RefreshSelectedCoin(pos);
    }
  }
}
void EditorScreen::RefreshSelectedBox()
{
  if (m_selectedBox == NO_BOX)
  {
    return;
  }
  RefreshSelectedBox(m_boxes[m_selectedBox].GetPosition());
}
void EditorScreen::RefreshSelectedBox(const glm::vec2& _newPos)
{
  if (m_selectedBox == NO_BOX)
  {
    return;
  }
  //Texture for boxes
  static GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/Tiles/MetalTexture.png");
  Box newBox;
  glm::vec4 uvRect;

  uvRect.x = _newPos.x;
  uvRect.y = _newPos.y;
  uvRect.z = m_boxDims.x;
  uvRect.w = m_boxDims.y;

  //init the new box
  newBox.Init(m_world.get(), _newPos, m_boxDims, texture, GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed,
    (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255), false, m_physicsMode == PhysicsMode::DYNAMIC, false, m_rotation, uvRect);

  //destroy old box and replace with new one
  m_boxes[m_selectedBox].Destroy(m_world.get());
  m_boxes[m_selectedBox] = newBox;
}
void EditorScreen::RefreshSelectedLight()
{
  if (m_selectedLight == NO_LIGHT)
  {
    return;
  }
  RefreshSelectedLight(m_lights[m_selectedLight].position);
}
void EditorScreen::RefreshSelectedLight(const glm::vec2& _newPos)
{
  if (m_selectedLight == NO_LIGHT)
  {
    return;
  }
  Light newLight;
  newLight.position = _newPos;
  newLight.size = m_lightSize;
  newLight.color = GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, (GLubyte)m_colorPickerAlpha);
  //set the current light to the new one
  m_lights[m_selectedLight] = newLight;
}
void EditorScreen::RefreshSelectedObstacle()
{
  if (m_selectedObstacle == NO_OBSTACLE)
  {
    return;
  }
  RefreshSelectedObstacle(m_obstacles[m_selectedObstacle].GetPosition());
}
void EditorScreen::RefreshSelectedObstacle(const glm::vec2& _newPos)
{
  if (m_selectedObstacle == NO_OBSTACLE)
  {
    return;
  }
  //Texture for obstacles
  static GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/Tiles/Spike.png");
  Box newObstacle;

  //init the new obstacle
  newObstacle.Init(m_world.get(), _newPos, m_boxDims, texture, GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed,
    (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255), false, m_physicsMode == PhysicsMode::DYNAMIC, false, m_rotation);

  //destroy old pbstacle and replace with new one
  m_obstacles[m_selectedObstacle].Destroy(m_world.get());
  m_obstacles[m_selectedObstacle] = newObstacle;
}
void EditorScreen::RefreshSelectedEnemy()
{
  if (m_selectedEnemy == NO_ENEMY)
  {
    return;
  }
  RefreshSelectedEnemy(m_enemies[m_selectedEnemy].GetPosition());
}
void EditorScreen::RefreshSelectedEnemy(const glm::vec2& _newPos)
{
  if (m_selectedEnemy == NO_ENEMY)
  {
    return;
  }
  EnemyRobot newEnemy;

  //init the new enemy
  newEnemy.Init(m_world.get(), _newPos, glm::vec2(3.0f), glm::vec2(2.0f, 2.8f),
    GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255));

  //destroy the old enemy and replace it with the new one
  m_enemies[m_selectedEnemy].Destroy(m_world.get());
  m_enemies[m_selectedEnemy] = newEnemy;
}
void EditorScreen::RefreshSelectedCoin()
{
  if (m_selectedCoin == NO_COIN)
  {
    return;
  }
  RefreshSelectedCoin(m_boxes[m_selectedBox].GetPosition());
}
void EditorScreen::RefreshSelectedCoin(const glm::vec2& _newPos)
{
  if (m_selectedCoin == NO_COIN)
  {
    return;
  }
  //Create the new updated coin
  Coins newCoin(m_world.get(), _newPos, m_boxDims, GameEngine::ColorRGBA8((GLubyte)m_colorPickerRed,
    (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255));

  //destroy old coin and replace with new one
  m_coins[m_selectedCoin].Destroy(m_world.get());
  m_coins[m_selectedCoin] = newCoin;
}
// not the best way to do this
bool EditorScreen::isMouseInUI()
{
  int x, y;
  SDL_GetMouseState(&x, &y);
  const float SW = (float)m_window->GetScreenWidth();
  const float SH = (float)m_window->GetScreenHeight();
  //first check save window
  if (m_saveWindow->isDisabled() &&
    x >= m_saveWindow->getXPosition().d_scale * SW && x <= m_saveWindow->getXPosition().d_scale * SW + m_saveWindow->getWidth().d_scale  * SW &&
    y >= m_saveWindow->getYPosition().d_scale * SH && y <= m_saveWindow->getYPosition().d_scale * SH + m_saveWindow->getHeight().d_scale * SH)
  {
    return true;
  }
  // Notice there's no conversion to world space, it's staying in screen space because the UI works in screen space.
  return (x >= m_groupBox->getXPosition().d_scale * SW && x <= m_groupBox->getXPosition().d_scale * SW + m_groupBox->getWidth().d_scale  * SW &&
    y >= m_groupBox->getYPosition().d_scale * SH && y <= m_groupBox->getYPosition().d_scale * SH + m_groupBox->getHeight().d_scale * SH);
}
void EditorScreen::SetPlatformWidgetVisibility(bool _visible)
{
  m_rigidRadioButton->setVisible(_visible);
  m_dynamicRadioButton->setVisible(_visible);
  m_rotationSpinner->setVisible(_visible);
  m_widthSpinner->setVisible(_visible);
  m_heightSpinner->setVisible(_visible);
}
void EditorScreen::SetLightWidgetVisibility(bool _visible)
{
  m_aSpinner->setVisible(_visible);
  m_sizeSpinner->setVisible(_visible);
}

bool EditorScreen::OnExitClicked(const CEGUI::EventArgs& e)
{
  m_currentState = GameEngine::ScreenState::EXIT_APPLICATION;
  return true;
}
bool EditorScreen::OnColorPickerRedChanged(const CEGUI::EventArgs& e)
{
  m_colorPickerRed = (float)m_rSpinner->getCurrentValue();
  RefreshSelectedBox();
  RefreshSelectedObstacle();
  RefreshSelectedLight();
  RefreshSelectedEnemy();
  return true;
}
bool EditorScreen::OnColorPickerGreenChanged(const CEGUI::EventArgs& e)
{
  m_colorPickerGreen = (float)m_gSpinner->getCurrentValue();
  RefreshSelectedObstacle();
  RefreshSelectedBox();
  RefreshSelectedEnemy();
  RefreshSelectedLight();
  return true;
}
bool EditorScreen::OnColorPickerBlueChanged(const CEGUI::EventArgs& e)
{
  m_colorPickerBlue = (float)m_bSpinner->getCurrentValue();
  RefreshSelectedObstacle();
  RefreshSelectedBox();
  RefreshSelectedEnemy();
  RefreshSelectedLight();
  return true;
}
bool EditorScreen::OnColorPickerAlphaChanged(const CEGUI::EventArgs& e)
{
  m_colorPickerAlpha = (float)m_aSpinner->getCurrentValue();
  RefreshSelectedObstacle();
  RefreshSelectedBox();
  RefreshSelectedLight();
  RefreshSelectedEnemy();
  return true;
}

bool EditorScreen::OnRigidMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_physicsMode = PhysicsMode::RIGID;
  RefreshSelectedObstacle();
  RefreshSelectedBox();
  return true;
}
bool EditorScreen::OnDynamicMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_physicsMode = PhysicsMode::DYNAMIC;
  RefreshSelectedObstacle();
  RefreshSelectedBox();
  return true;
}
bool EditorScreen::OnPlayerMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  //click on the player button
  m_objectMode = ObjectMode::PLAYER;
  SetPlatformWidgetVisibility(false);
  SetLightWidgetVisibility(false);
  return true;
}
bool EditorScreen::OnPlatformMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_objectMode = ObjectMode::PLATFORM;
  SetPlatformWidgetVisibility(true);
  SetLightWidgetVisibility(false);
  return true;
}
bool EditorScreen::OnTriggerMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_objectMode = ObjectMode::TRIGGER;
  SetPlatformWidgetVisibility(true);
  SetLightWidgetVisibility(false);
  return true;
}
bool EditorScreen::OnFinishMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_objectMode = ObjectMode::FINISH;
  SetPlatformWidgetVisibility(true);
  SetLightWidgetVisibility(false);
  return true;
}
bool EditorScreen::OnLightMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_objectMode = ObjectMode::LIGHT;
  SetPlatformWidgetVisibility(false);
  SetLightWidgetVisibility(true);
  return true;
}
bool EditorScreen::OnObstaleMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_objectMode = ObjectMode::OBSTACLE;
  SetPlatformWidgetVisibility(true);
  SetLightWidgetVisibility(false);
  return true;
}
bool EditorScreen::OnEnemyMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_objectMode = ObjectMode::ENEMY;
  SetPlatformWidgetVisibility(false);
  SetLightWidgetVisibility(false);
  return true;
}
bool EditorScreen::OnCoinMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_objectMode = ObjectMode::COIN;
  SetPlatformWidgetVisibility(true);
  SetLightWidgetVisibility(false);
  return true;
}
bool EditorScreen::OnSelectMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_selectionMode = SelectionMode::SELECT;
  return true;
}
bool EditorScreen::OnPlaceMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_selectionMode = SelectionMode::PLACE;
  m_selectedBox = NO_BOX;
  m_selectedLight = NO_LIGHT;
  m_selectedObstacle = NO_OBSTACLE;

  //set the widget visibility depending on object mode
  if (m_objectMode == ObjectMode::LIGHT)
  {
    SetLightWidgetVisibility(true);
  }
  else if (m_objectMode == ObjectMode::PLATFORM)
  {
    SetPlatformWidgetVisibility(true);
  }
  else if (m_objectMode == ObjectMode::OBSTACLE)
  {
    SetPlatformWidgetVisibility(true);
  }
  else if (m_objectMode == ObjectMode::TRIGGER)
  {
    SetPlatformWidgetVisibility(true);
  }
  else if (m_objectMode == ObjectMode::FINISH)
  {
    SetPlatformWidgetVisibility(true);
  }
  return true;
}
bool EditorScreen::OnSaveMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  //make sure levels dir exists
  GameEngine::IOManager::MakeDirectory("Levels");

  m_saveWindowCombobox->clearAllSelections();

  //remove all items
  for (auto& item : m_saveListBoxItems)
  {
    // We don't have to call delete since removeItem does it for us
    m_saveWindowCombobox->removeItem(item);
  }
  m_saveListBoxItems.clear();

  //Get all directory entries
  std::vector<GameEngine::DirEntry> entries;
  GameEngine::IOManager::GetDirectoryEntries("Levels", entries);

  //add all files to the list box
  for (auto& entry : entries)
  {
    //don't add directories
    if (!entry.isDirectory)
    {
      //remove the "Levels/" substring
      entry.path.erase(0, std::string("Levels/").size());
      m_saveListBoxItems.push_back(new CEGUI::ListboxTextItem(entry.path));
      m_saveWindowCombobox->addItem(m_saveListBoxItems.back());
    }
  }

  m_saveWindow->enable();
  m_saveWindow->setAlpha(1.0f);
  m_loadWindow->disable();
  m_loadWindow->setAlpha(0.0f);
  return true;
}
bool EditorScreen::OnLoadMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_loadWindowCombobox->clearAllSelections();

  //remove all items
  for (auto& item : m_loadListBoxItems)
  {
    m_loadWindowCombobox->removeItem(item);
  }
  m_loadListBoxItems.clear();

  //get all directory entries
  std::vector<GameEngine::DirEntry> entries;
  GameEngine::IOManager::GetDirectoryEntries("Levels", entries);

  //add all files to list box
  for (auto& entry : entries)
  {
    //don't add directories
    if (!entry.isDirectory)
    {
      //remove "Levels/" substring
      entry.path.erase(0, std::string("Levels/").size());
      m_loadListBoxItems.push_back(new CEGUI::ListboxTextItem(entry.path));
      m_loadWindowCombobox->addItem(m_loadListBoxItems.back());
    }
    m_loadWindow->enable();
    m_loadWindow->setAlpha(1.0f);
    m_saveWindow->disable();
    m_saveWindow->setAlpha(0.0f);
  }
  return true;
}
bool EditorScreen::OnBackMouseClick(const CEGUI::EventArgs& e)
{
  m_currentState = GameEngine::ScreenState::CHANGE_PREVIOUS;
  return true;
}
bool EditorScreen::OnRotationValueChange(const CEGUI::EventArgs& e)
{
  m_rotation = (float)m_rotationSpinner->getCurrentValue();
  if (m_selectedBox != NO_BOX)
  {
    RefreshSelectedBox();
  }
  else if (m_selectedObstacle != NO_OBSTACLE)
  {
    RefreshSelectedObstacle();
  }
  return true;
}
bool EditorScreen::OnSizeValueChange(const CEGUI::EventArgs& e)
{
  m_lightSize = (float)m_sizeSpinner->getCurrentValue();
  RefreshSelectedLight();
  return true;
}
bool EditorScreen::OnWidthValueChange(const CEGUI::EventArgs& e)
{
  m_boxDims.x = (float)m_widthSpinner->getCurrentValue();
  if (m_selectedBox != NO_BOX)
  {
    RefreshSelectedBox();
  }
  else if (m_selectedObstacle != NO_OBSTACLE)
  {
    RefreshSelectedObstacle();
  }  return true;
}
bool EditorScreen::OnHeightValueChange(const CEGUI::EventArgs& e)
{
  m_boxDims.y = (float)m_heightSpinner->getCurrentValue();
  if (m_selectedBox != NO_BOX)
  {
    RefreshSelectedBox();
  }
  else if (m_selectedObstacle != NO_OBSTACLE)
  {
    RefreshSelectedObstacle();
  }  return true;
}
bool EditorScreen::OnDebugToggleClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_debugRender = m_debugToggle->isSelected();
  return true;
}
bool EditorScreen::OnSaveCancelClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_saveWindow->disable();
  m_saveWindow->setAlpha(0.0f);
  return true;
}
bool EditorScreen::OnLoadCancelClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_loadWindow->disable();
  m_loadWindow->setAlpha(0.0f);
  return true;
}
bool EditorScreen::OnSave(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  if (!m_hasPlayer)
  {
    puts("Must create player before saving!");
    return true;
  }
  if (!m_hasExit)
  {
    puts("Must create an exit before saving!");
    return true;
  }if (!m_hasTrigger)
  {
    puts("Must create a trigger before saving!");
    return true;
  }
  puts("Saving Game. . .");
  // Make sure the Levels directory exists again
  GameEngine::IOManager::MakeDirectory("levels");

  //save in text mode
  std::string text = "Levels/" + std::string(m_saveWindowCombobox->getText().c_str());
  if (LevelReaderWriter::SaveAsText(text, m_player, m_boxes, m_obstacles, m_coins, m_exit, m_trigger, m_lights, m_enemies))
  {
    m_saveWindow->disable();
    m_saveWindow->setAlpha(0.0f);
    puts("Level successfully saved!");
  }
  else
  {
    puts("Failed to save the level");
  }
  return true;
}
bool EditorScreen::OnLoad(const CEGUI::EventArgs& e)
{
  puts("Loading game. . .");
  std::string path = "Levels/" + std::string(m_loadWindowCombobox->getText().c_str());

  ClearLevel();

  if (LevelReaderWriter::LoadAsText(path, m_world.get(), m_player, m_boxes, m_obstacles, m_coins, m_exit, m_trigger, m_lights, m_enemies))
  {
    m_hasPlayer = true;
    m_hasExit = true;
    m_hasTrigger = true;
  }
  puts("Load successful!");
  m_loadWindow->disable();
  m_loadWindow->setAlpha(0.0f);
  return true;
}

bool EditorScreen::OnMusicVolumeChanged(const CEGUI::EventArgs& e)
{
  //edit the music volume
  int volumeValue = 0;
  volumeValue = (int)m_volumeSpinner->getCurrentValue();

  m_music.EditVolume(volumeValue);

  return true;
}