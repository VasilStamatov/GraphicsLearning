#include "GameplayScreen.h"
#include <SDL/SDL.h>
#include <GameEngine/IMainGame.h>
#include <GameEngine/ResourceManager.h>
#include <GameEngine\IOManager.h>
#include <GameEngine/GameEngineErrors.h>
#include <random>

#include "ScreenIndices.h"
#include "LevelReaderWriter.h"
#include "LevelPathContainer.h"
#include "GameStats.h"

const b2Vec2 GRAVITY(0.0f, -25.0);

GameplayScreen::GameplayScreen(GameEngine::Window* _window) : m_window(_window)
{
  m_screenIndex = SCREEN_INDEX_GAMEPLAY;
}


GameplayScreen::~GameplayScreen()
{
}

int GameplayScreen::GetNextScreenIndex() const
{
  return m_nextScreen;
}

int GameplayScreen::GetPreviousScreenIndex() const
{
  return SCREEN_INDEX_MAINMENU;
}

void GameplayScreen::Build()
{
}

void GameplayScreen::Destroy()
{

}

void GameplayScreen::OnEntry()
{
  //Initialize the audio
  m_audio.Init();
  //set up the world
  m_world = std::make_unique<b2World>(GRAVITY);

  //Initialize the camera
  m_camera.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  m_camera.SetScale(32.0f);
  m_HUDCamera.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  m_HUDCamera.SetPosition(glm::vec2(m_window->GetScreenWidth() / 2.0f, m_window->GetScreenHeight() / 2.0f));
  //Initialize the debug renderer
  m_debugRenderer.Init();

  //Initialize the UI
  InitUI();

  //Initialize the spritebatch
  m_spriteBatch.Init();

  //Initialize the spritefont
  m_spriteFont.init("Fonts/chintzy.ttf", 32);
  // Shader init
  // Compile our texture shader
  std::vector<GameEngine::Shader> textureShaders =
  {
    { GL_VERTEX_SHADER, "Shaders/textureShading.vert", "Texture Vertex Shader" },
    { GL_FRAGMENT_SHADER, "Shaders/textureShading.frag", "Texture Fragment Shader" },
  };
  m_textureProgram.CompileShaders(textureShaders);
  m_textureProgram.AddAttribute("vertexPosition");
  m_textureProgram.AddAttribute("vertexColor");
  m_textureProgram.AddAttribute("vertexUV");
  m_textureProgram.LinkShaders();
  // Compile our light shader
  std::vector<GameEngine::Shader> lightingShaders =
  {
    { GL_VERTEX_SHADER, "Shaders/lightShading.vert", "Lighting Vertex Shader" },
    { GL_FRAGMENT_SHADER, "Shaders/lightShading.frag", "Lighting Fragment Shader" },
  };
  m_lightProgram.CompileShaders(lightingShaders);
  m_lightProgram.AddAttribute("vertexPosition");
  m_lightProgram.AddAttribute("vertexColor");
  m_lightProgram.AddAttribute("vertexUV");
  m_lightProgram.LinkShaders();
  /************************************
  *        Loading the game           *
  *************************************/

  puts("Loading game");
  //Clear the level just in case
  ClearLevel();
  //Load the game
  if (LevelReaderWriter::LoadAsText(levelPath, m_world.get(), m_player, m_boxes, m_obstacles, m_coins, m_exit, m_trigger, m_lights, m_enemies))
  {
    m_enemiesKilled = 0;
    m_coinsCollected = 0;
    m_hasPlayer = true;
    m_hasTrigger = true;
    m_hasExit = true;
    //Start the ingmae music
    m_music = m_audio.LoadMusic("Sound/neocrey - Last Cyber Dance.ogg");
    m_music.Play(-1);
    m_elapsedTime = 0.0f;

    /////////////////////////////////Set up the background layers
    //center the position of the camera to the player for the background setups
    m_camera.SetPosition(m_player.GetPosition());
    //bottomleft position of the screen in world coords
    glm::vec2 bottomLeft = m_camera.ConvertScreenToWorld(glm::vec2(0.0f, m_window->GetScreenHeight()));
    //screen dimension in world coords
    glm::vec2 screenDims = glm::vec2(m_window->GetScreenWidth() / m_camera.GetScale(), m_window->GetScreenHeight() / m_camera.GetScale());
    //3 foreground layers positions at the center and 1 to the right and 1 to the left
    m_backgroundLayers.emplace_back(0.1f, -1.0f, "Assets/layers/skill-desc_0000_foreground.png",
      bottomLeft, screenDims, m_player.GetPosition().x, 0);

    m_backgroundLayers.emplace_back(0.1f, -1.0f, "Assets/layers/skill-desc_0000_foreground.png",
      glm::vec2(bottomLeft.x + screenDims.x, bottomLeft.y), screenDims, m_player.GetPosition().x, 1);

    m_backgroundLayers.emplace_back(0.1f, -1.0f, "Assets/layers/skill-desc_0000_foreground.png",
      glm::vec2(bottomLeft.x - screenDims.x, bottomLeft.y), screenDims, m_player.GetPosition().x, -1);
    //////////////////////////////////////////////////////////////////////////////////

    //3 buildings layers positions at the center and 1 to the right and 1 to the left
    m_backgroundLayers.emplace_back(0.3f, -2.0f, "Assets/layers/skill-desc_0001_buildings.png",
      bottomLeft, screenDims, m_player.GetPosition().x, 0);

    m_backgroundLayers.emplace_back(0.1f, -2.0f, "Assets/layers/skill-desc_0001_buildings.png",
      glm::vec2(bottomLeft.x + screenDims.x, bottomLeft.y), screenDims, m_player.GetPosition().x, 1);

    m_backgroundLayers.emplace_back(0.1f, -2.0f, "Assets/layers/skill-desc_0001_buildings.png",
      glm::vec2(bottomLeft.x - screenDims.x, bottomLeft.y), screenDims, m_player.GetPosition().x, -1);
    //////////////////////////////////////////////////////////////////////////////////

    //3 far buildings layers positions at the center and 1 to the right and 1 to the left
    m_backgroundLayers.emplace_back(0.5f, -3.0f, "Assets/layers/skill-desc_0002_far-buildings.png",
      bottomLeft, screenDims, m_player.GetPosition().x, 0);

    m_backgroundLayers.emplace_back(0.1f, -3.0f, "Assets/layers/skill-desc_0002_far-buildings.png",
      glm::vec2(bottomLeft.x + screenDims.x, bottomLeft.y), screenDims, m_player.GetPosition().x, 1);

    m_backgroundLayers.emplace_back(0.1f, -3.0f, "Assets/layers/skill-desc_0002_far-buildings.png",
      glm::vec2(bottomLeft.x - screenDims.x, bottomLeft.y), screenDims, m_player.GetPosition().x, -1);
    //////////////////////////////////////////////////////////////////////////////////

    //Only 1 background needed as it always follows the player
    m_backgroundLayers.emplace_back(1.0f, -4.0f, "Assets/layers/skill-desc_0003_bg.png",
      bottomLeft, screenDims, m_player.GetPosition().x, 0);

    /////////////////////////////////////////////////////////////
  }
  puts("Load successful!");
}

void GameplayScreen::OnExit()
{
  for (auto& item : m_saveListBoxItems)
  {
    // We don't have to call delete since removeItem does it for us
    m_saveWindowCombobox->removeItem(item);
  }
  m_saveListBoxItems.clear();
  for (auto& item : m_loadListBoxItems)
  {
    // We don't have to call delete since removeItem does it for us
    m_loadWindowCombobox->removeItem(item);
  }
  m_loadListBoxItems.clear();

  //Destroy all objects and clear all the vectors
  for (auto& enemy : m_enemies)
  {
    enemy.Destroy(m_world.get());
  }
  m_enemies.clear();
  for (auto& box : m_boxes)
  {
    box.Destroy(m_world.get());
  }
  m_boxes.clear();
  for (auto& obs : m_obstacles)
  {
    obs.Destroy(m_world.get());
  }
  m_obstacles.clear();
  for (auto& projectile : m_projectiles)
  {
    projectile.Destroy(m_world.get());
  }
  m_projectiles.clear();

  //Dispose and Destroy all the programs
  m_textureProgram.Dispose();
  m_lightProgram.Dispose();
  m_audio.Destroy();

  m_spriteBatch.Dispose();
  m_spriteFont.dispose();

  //Destroy the player
  m_player.Destroy(m_world.get());
  //Clear the level
  ClearLevel();
  //Reset the world
  m_world.reset();
  m_debugRenderer.Dispose();
  m_lights.clear();
  m_isUnlocked = false;
  //DEstroy the gui
  m_gui.DestroyGUI();
}

void GameplayScreen::Update()
{
  /////////Update the background
  for (auto& bg : m_backgroundLayers)
  {
    //update the parallaxing
    bg.Update(m_player.GetPosition(), m_camera.ConvertScreenToWorld(glm::vec2(0.0f, m_window->GetScreenHeight())));
  }

  //Check if the player is alive
  if (m_player.GetIsDead())
  {
    OnGameOver(CEGUI::EventArgs());
  }
  // Make sure the camera is bound to the player position
  m_camera.SetPosition(m_player.GetPosition());
  //Update the camera
  m_camera.Update();
  //Check for user input
  CheckInput();
  //update the player
  m_player.Update(m_game->inputManager);
  //increase the elapsed time
  m_elapsedTime += 1.0f / m_game->GetFPS();
  //Update the projectiles
  for (int i = m_projectiles.size() - 1; i >= 0; i--)
  {
    //Check if the projectile has been destroyed
    if (m_projectiles[i].GetIsDestroyed())
    {
      //Destroy the projectile and increment the possible shots
      m_projectiles[i].Destroy(m_world.get());
      m_projectiles[i] = m_projectiles.back();
      m_projectiles.pop_back();
      m_player.IncrementShots();
    }
    //if Not, then update it
    else
    {
      m_projectiles[i].Update();
    }
  }
  //Check if player shot
  if (m_player.GetPlayerShot())
  {
    //Instantiate the new projectile
    m_projectiles.emplace_back(m_world.get(), glm::vec2(m_player.GetPosition().x + 2.0f * m_player.GetDirection(), m_player.GetPosition().y),
      glm::vec2(1.0f, 1.0f), glm::vec2(0.9f, 0.9f), GameEngine::ColorRGBA8(255, 255, 255, 255), m_player.GetDirection(), 0.008f);
    m_player.SetPlayerShot(false);
  }
  //Update all alive enemies
  for (int i = m_enemies.size() - 1; i >= 0; i--)
  {
    //check if the enemy is dead, if so remove him
    if (m_enemies[i].GetIsDead())
    {
      m_enemies[i].Destroy(m_world.get());
      m_enemies[i] = m_enemies.back();
      m_enemies.pop_back();
    }
    //if the enemy is alive, update him
    else
    {
      m_enemies[i].Update(m_player.GetPosition());
      //if the enemy has spawned, check its collision
      if (m_enemies[i].GetSpawn())
      {
        for (b2ContactEdge* ce = m_enemies[i].GetCapsule().GetBody()->GetContactList(); ce != nullptr; ce = ce->next)
        {
          b2Contact* c = ce->contact;
          if (c->GetFixtureB()->GetBody() == m_player.GetCapsule().GetBody())
          {
            //if the enemy collides with the player, apply some damage to the player and push him away
            m_player.ApplyDamage(m_enemies[i].GetDmg());
            m_player.GetCapsule().GetBody()->ApplyForceToCenter(b2Vec2(100.0f * (-m_enemies[i].GetDirection()), 0.0f), true);
          }
          if (m_projectiles.size() > 0)
          {
            for (auto& projectile : m_projectiles)
            {
              //if the enemy collides with a player projectile, apply damage to the enemy
              if (c->GetFixtureB()->GetBody() == projectile.GetBulletBody().GetBody())
              {
                m_enemies[i].ApplyDamage(m_player.GetDmg());
              }
            }
          }
        }
      }
    }
  }
  //////////////obstacle collission
  for (auto& obs : m_obstacles)
  {
    for (b2ContactEdge* ce = obs.GetBody()->GetContactList(); ce != nullptr; ce = ce->next)
    {
      b2Contact* c = ce->contact;
      if (c->GetFixtureB()->GetBody() == m_player.GetCapsule().GetBody())
      {
        //if the player collides with an obstacle, deal some damage to him and push him a bit up to give him time to react
        m_player.ApplyDamage(0.25f);
        m_player.GetCapsule().GetBody()->ApplyForceToCenter(b2Vec2(0.0f, 100.0f), true);
      }
    }
  }
  //////////////coins collission
  for (int i = m_coins.size() - 1; i >= 0; i--)
  {
    for (b2ContactEdge* ce = m_coins[i].GetBox().GetBody()->GetContactList(); ce != nullptr; ce = ce->next)
    {
      b2Contact* c = ce->contact;
      if (c->GetFixtureB()->GetBody() == m_player.GetCapsule().GetBody())
      {
        //if the player collides with a coin, increment the coin counter and play the sound
        GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Pickup_Coin4.ogg");
        soundEffect.Play();

        m_coinsCollected++;
        //then remove the coin
        m_coins[i].Destroy(m_world.get());
        m_coins[i] = m_coins.back();
        m_coins.pop_back();
        break;
      }
    }
  }

  if (m_isUnlocked)
  {
    //if the exit is unlocked, change the locked door sprite with an unlocked door one
    glm::vec2 pos = m_exit.GetPosition();
    glm::vec2 dims = m_exit.GetDimensions();
    glm::vec4 uvRect = m_exit.GetUvRect();
    GameEngine::ColorRGBA8 color = m_exit.GetColor();
    bool fixedRotation = m_exit.GetFixedRotation();
    bool isDynamic = m_exit.GetIsDynamic();
    bool isSensor = m_exit.GetIsSensor();
    float angle = m_exit.GetAngle();
    GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/Objects/DoorOpen.png");
    //destroy the current exit
    m_exit.Destroy(m_world.get());
    //Initialize the new exit
    m_exit.Init(m_world.get(), pos, dims, texture, color, fixedRotation, isDynamic, isSensor, angle, uvRect);
    //set isUnlocked to false, so that this change doesn;t happen every frame
    m_isUnlocked = false;
  }
  for (b2ContactEdge* ce = m_exit.GetBody()->GetContactList(); ce != nullptr; ce = ce->next)
  {
    b2Contact* c = ce->contact;
    if (c->GetFixtureB()->GetBody() == m_player.GetCapsule().GetBody())
    {
      //if the player collides with the exit, the level is cleared
      if (m_exit.GetTexture().filePath == "Assets/Objects/DoorOpen.png")
      {
        OnLevelClear(CEGUI::EventArgs());
        break;
      }
    }
  }

  for (b2ContactEdge* ce = m_trigger.GetBody()->GetContactList(); ce != nullptr; ce = ce->next)
  {
    b2Contact* c = ce->contact;
    //Check if the player has collided with the trigger to unlock the door
    if (c->GetFixtureB()->GetBody() == m_player.GetCapsule().GetBody())
    {
      //check if it's not unlocked
      if (m_exit.GetTexture().filePath != "Assets/Objects/DoorOpen.png")
      {
        glm::vec2 pos = m_trigger.GetPosition();
        glm::vec2 dims = m_trigger.GetDimensions();
        glm::vec4 uvRect = m_trigger.GetUvRect();
        GameEngine::ColorRGBA8 color = m_trigger.GetColor();
        bool fixedRotation = m_trigger.GetFixedRotation();
        bool isDynamic = m_trigger.GetIsDynamic();
        bool isSensor = m_trigger.GetIsSensor();
        float angle = m_trigger.GetAngle();
        GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/Objects/Switch1.png");
        //Destroy the current red trigger
        m_trigger.Destroy(m_world.get());
        //and initialize the new green trigger to show that the door has been unlocked
        m_trigger.Init(m_world.get(), pos, dims, texture, color, fixedRotation, isDynamic, isSensor, angle, uvRect);
        //set isUnlocked to true, because it is
        m_isUnlocked = true;
        break;

      }
    }
  }
  //update the physics simulation (using fixed deltatime, because everything else looks bad with box2d)
  m_world->Step(1.0f / 60.0f, 6, 2);
}
void GameplayScreen::Draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.125f, 0.298f, 0.137f, 1.0f);
  //use the texture program
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
    //draw everything except lights, because they use a different shader program
    m_spriteBatch.Begin(GameEngine::GlyphSortType::FRONT_TO_BACK);

    //Draw the background
    for (auto& bg : m_backgroundLayers)
    {
      bg.Draw(m_spriteBatch);
    }

    //Draw the boxes
    for (auto& box : m_boxes)
    {
      box.Draw(m_spriteBatch);
    }
    //draw the obstacles
    for (auto& obstacle : m_obstacles)
    {
      obstacle.Draw(m_spriteBatch);
    }
    for (auto& coin : m_coins)
    {
      //apply camera culling for optimization
      if (m_camera.IsBoxInView(coin.GetPosition(), coin.GetDimensions()))
      {
        coin.Draw(m_spriteBatch);
      }
    }
    for (auto& enemy : m_enemies)
    {
      //apply camera culling for optimization
      if (m_camera.IsBoxInView(enemy.GetPosition(), enemy.GetDrawDims()))
      {
        enemy.Draw(m_spriteBatch);
      }
    }
    for (auto& projectile : m_projectiles)
    {
      projectile.Draw(m_spriteBatch);
    }
    //apply camera culling for optimization
    if (m_camera.IsBoxInView(m_exit.GetPosition(), m_exit.GetDimensions()))
    {
      m_exit.Draw(m_spriteBatch);
    }
    //apply camera culling for optimization
    if (m_camera.IsBoxInView(m_trigger.GetPosition(), m_trigger.GetDimensions()))
    {
      m_trigger.Draw(m_spriteBatch);
    }
    m_player.Draw(m_spriteBatch);

    m_spriteBatch.End();
    m_spriteBatch.RenderBatch();

    DrawHUD();

    m_textureProgram.UnUse();
  }

  {
    //Draw the lights

    m_lightProgram.Use();
    pUniform = m_textureProgram.GetUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    //additive blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    //begin the spritebatch
    m_spriteBatch.Begin();

    //draw all the lights
    for (auto& light : m_lights)
    {
      light.Draw(m_spriteBatch);
    }
    //end the spritebatch
    m_spriteBatch.End();
    //render the batch
    m_spriteBatch.RenderBatch();
    //unuse the lights shader program
    m_lightProgram.UnUse();

    //restore alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  }

  //Debug rendering
  if (m_renderDebug)
  {
    //use destRect for the box outlining
    glm::vec4 destRect;
    {
      //Draw the exit outlines
      destRect.x = m_exit.GetBody()->GetPosition().x - m_exit.GetDimensions().x / 2.0f;
      destRect.y = m_exit.GetBody()->GetPosition().y - m_exit.GetDimensions().y / 2.0f;
      destRect.z = m_exit.GetDimensions().x;
      destRect.w = m_exit.GetDimensions().y;
      m_debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(255, 255, 255, 255), m_exit.GetBody()->GetAngle());
    }
    {
      //Draw the trigger outlines
      destRect.x = m_trigger.GetBody()->GetPosition().x - m_trigger.GetDimensions().x / 2.0f;
      destRect.y = m_trigger.GetBody()->GetPosition().y - m_trigger.GetDimensions().y / 2.0f;
      destRect.z = m_trigger.GetDimensions().x;
      destRect.w = m_trigger.GetDimensions().y;
      m_debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(255, 255, 255, 255), m_trigger.GetBody()->GetAngle());
    }
    //draw all the platforms outlines
    for (auto& box : m_boxes)
    {
      destRect.x = box.GetBody()->GetPosition().x - box.GetDimensions().x / 2.0f;
      destRect.y = box.GetBody()->GetPosition().y - box.GetDimensions().y / 2.0f;
      destRect.z = box.GetDimensions().x;
      destRect.w = box.GetDimensions().y;

      m_debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(255, 255, 255, 255), box.GetBody()->GetAngle());
    }
    //draw all the obstacles outlines
    for (auto& obs : m_obstacles)
    {
      destRect.x = obs.GetBody()->GetPosition().x - obs.GetDimensions().x / 2.0f;
      destRect.y = obs.GetBody()->GetPosition().y - obs.GetDimensions().y / 2.0f;
      destRect.z = obs.GetDimensions().x;
      destRect.w = obs.GetDimensions().y;

      m_debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(255, 255, 255, 255), obs.GetBody()->GetAngle());
    }
    //draw all the enemies outlines
    for (auto& enemy : m_enemies)
    {
      enemy.DrawDebug(m_debugRenderer);
    }
    for (auto& coin : m_coins)
    {
      coin.DrawDebug(m_debugRenderer);
    }
    //draw the projectile outlines
    for (auto& projectile : m_projectiles)
    {
      projectile.DrawDebug(m_debugRenderer);
    }
    m_player.DrawDebug(m_debugRenderer);
    //render player outlines
    m_debugRenderer.End();
    m_debugRenderer.Render(projectionMatrix, 2.0f);
  }
  //Draw the gui
  m_gui.Draw();
}

void GameplayScreen::InitUI()
{
  // Init the UI
  m_gui.Init("GUI");
  m_gui.LoadScheme("TaharezLook.scheme");
  m_gui.SetFont("DejaVuSans-10");
  /************
  *EXIT BUTTON*
  *************/
  m_exitButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.01f, 0.01f, 0.1f, 0.05f), glm::vec4(0.0f), "exitButton"));
  m_exitButton->setText("Exit");
  // Set the event to be called when we click
  m_exitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameplayScreen::OnExitClicked, this));

  /************
  *LOAD BUTTON*
  *************/
  m_loadButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.01, 0.06, 0.1f, 0.05f), glm::vec4(0.0f), "loadButton"));
  m_loadButton->setText("Load");
  m_loadButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameplayScreen::OnLoadMouseClick, this));

  //load window widgets
  m_loadWindow = static_cast<CEGUI::FrameWindow*>(m_gui.CreateWidget("TaharezLook/FrameWindow", glm::vec4(0.35, 0.2f, 0.4f, 0.4f), glm::vec4(0.0f), "LoadWindow"));
  m_loadWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&GameplayScreen::OnLoadCancelClick, this));
  m_loadWindow->setText("Load Game");
  //don't let the window be dragged
  m_loadWindow->setDragMovingEnabled(false);

  //childrent of load window
  m_loadWindowCombobox = static_cast<CEGUI::Combobox*>(m_gui.CreateWidget(m_loadWindow, "TaharezLook/Combobox", glm::vec4(0.05, 0.25, 0.8, 0.4), glm::vec4(0.0f), "LoadCombobox"));
  m_loadWindowLoadButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget(m_loadWindow, "TaharezLook/Button", glm::vec4(0.25, 0.6, 0.3, 0.1), glm::vec4(0.0f), "LoadCancelButton"));
  m_loadWindowLoadButton->setText("Load");
  m_loadWindowLoadButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameplayScreen::OnLoad, this));

  //has to start disabled
  m_loadWindow->setAlpha(0.0f);
  m_loadWindow->disable();

  /*************
   *SAVE BUTTON*
   *************/
  m_saveButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.01f, 0.11f, 0.1f, 0.05f), glm::vec4(0.0f), "SaveButton"));
  m_saveButton->setText("Save");
  m_saveButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameplayScreen::OnSaveMouseClick, this));

  //save window widgets
  m_saveWindow = static_cast<CEGUI::FrameWindow*>(m_gui.CreateWidget("TaharezLook/FrameWindow", glm::vec4(0.35, 0.2, 0.4, 0.4), glm::vec4(0.0f), "SaveWIndow"));
  m_saveWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&GameplayScreen::OnSaveCancelClick, this));
  m_saveWindow->setText("Save Game");
  //don't let the window be dragged
  m_saveWindow->setDragMovingEnabled(false);

  //children of save window
  m_saveWindowCombobox = static_cast<CEGUI::Combobox*>(m_gui.CreateWidget(m_saveWindow, "TaharezLook/Combobox", glm::vec4(0.05, 0.25, 0.8, 0.4), glm::vec4(0.0f), "SaveCombobox"));
  m_saveWindowSaveButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget(m_saveWindow, "TaharezLook/Button", glm::vec4(0.25, 0.6, 0.3, 0.1), glm::vec4(0.0f), "SaveCancelButton"));
  m_saveWindowSaveButton->setText("Save");
  m_saveWindowSaveButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameplayScreen::OnSave, this));
  //has to start disabled
  m_saveWindow->setAlpha(0.0f);
  m_saveWindow->disable();

  /************
  *BACK BUTTON*
  *************/
  m_backButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.01f, 0.16f, 0.1f, 0.05f), glm::vec4(0.0f), "BackButton"));
  m_backButton->setText("Back");
  m_backButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameplayScreen::OnBackClicked, this));

  /***************
  *VOLUME SPINNER*
  ****************/
  m_volumeSpinner = static_cast<CEGUI::Spinner*>(m_gui.CreateWidget("TaharezLook/Spinner", glm::vec4(0.0322f, 0.9f, 0.1f, 0.05f), glm::vec4(0.0f), "VolumeSpinner"));
  m_volumeSpinner->setMinimumValue(0.0f);
  m_volumeSpinner->setMaximumValue(128.0f);
  m_volumeSpinner->setCurrentValue(128.0f);
  m_volumeSpinner->setTextInputMode(CEGUI::Spinner::Integer);
  m_volumeSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&GameplayScreen::OnMusicVolumeChanged, this));
  m_volumeSpinner->setStepSize(1.0f);

  //set custom mouse cursor and hide the normal one
  m_gui.SetMouseCursor("TaharezLook/MouseArrow");
  m_gui.ShowMouseCursor();
  SDL_ShowCursor(0);
}
void GameplayScreen::CheckInput()
{
  //handle user inputs
  SDL_Event evnt;
  while (SDL_PollEvent(&evnt))
  {
    m_game->OnSDLEvent(evnt);
    m_gui.OnSDLEvent(evnt);
    switch (evnt.type)
    {
    case SDL_QUIT:
      OnExitClicked(CEGUI::EventArgs());
      break;
    default:
      break;
    }
  }
}

void GameplayScreen::ClearLevel()
{
  //clear the current level
  m_boxes.clear();
  m_obstacles.clear();
  m_coins.clear();
  m_lights.clear();
  m_enemies.clear();
  m_isUnlocked = false;
  m_hasPlayer = false;
  m_player.SetHealth(100.0f);
  m_player.SetIsDead(false);
  m_hasExit = false;
  m_hasTrigger = false;
  m_world.reset();
  m_backgroundLayers.clear();
  m_world = std::make_unique<b2World>(GRAVITY);
}
void GameplayScreen::DrawHUD()
{
  //Draw the HUD
  //use the hud camera matrix for the projection matrix of the HUD (so it's in 1 place during the gameplay)
  glm::mat4 projectionMatrix = m_HUDCamera.GetCameraMatrix();
  GLint pUniform = m_textureProgram.GetUniformLocation("P");
  glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
  //begin using the spritebatch
  m_spriteBatch.Begin();
  //all the strings 
  std::string fps = "FPS: " + std::to_string((int)m_game->GetFPS()); //< fps text
  std::string playerHealth = "HP: " + std::to_string((int)m_player.GetHP()); //< player HP text
  std::string musicVolume = "Music Volume"; //< music volume text
  std::string elapsedTime = "Time elapsed: " + std::to_string((int)m_elapsedTime); //< time elapsed text
  std::string enemiesKilled = "Enemies killed: " + std::to_string((m_enemiesKilled)); //< enemies killed text
  std::string coinsCollected = "Coins collected: " + std::to_string((m_coinsCollected)); //< coins collected text


  //Draw all the texts
  m_spriteFont.draw(m_spriteBatch, fps.c_str(), glm::vec2(-1.0f, -1.0f), glm::vec2(0.002f), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255));
  m_spriteFont.draw(m_spriteBatch, playerHealth.c_str(), glm::vec2(0.6, 0.9), glm::vec2(0.0015f), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255));
  m_spriteFont.draw(m_spriteBatch, musicVolume.c_str(), glm::vec2(-0.99f, -0.8f), glm::vec2(0.0015f), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255));
  m_spriteFont.draw(m_spriteBatch, elapsedTime.c_str(), glm::vec2(0.5, 0.85), glm::vec2(0.0015f), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255));
  m_spriteFont.draw(m_spriteBatch, enemiesKilled.c_str(), glm::vec2(0.5, 0.8), glm::vec2(0.0015f), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255));
  m_spriteFont.draw(m_spriteBatch, coinsCollected.c_str(), glm::vec2(0.5, 0.75), glm::vec2(0.0015f), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255));
  //end the spritebatch
  m_spriteBatch.End();
  //render the batch
  m_spriteBatch.RenderBatch();
}
bool GameplayScreen::OnExitClicked(const CEGUI::EventArgs& e)
{
  //exit the game
  m_currentState = GameEngine::ScreenState::EXIT_APPLICATION;
  return true;
}
bool GameplayScreen::OnBackClicked(const CEGUI::EventArgs& e)
{
  //change to the previous screen
  m_currentState = GameEngine::ScreenState::CHANGE_PREVIOUS;
  return true;
}
bool GameplayScreen::OnLoadMouseClick(const CEGUI::EventArgs& e)
{
  //play the click sound effect
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();
  //clear all the selections in the combobox
  m_loadWindowCombobox->clearAllSelections();
  //remove the items
  for (auto& item : m_loadListBoxItems)
  {
    //delete doesnt need to be called since removeItem does this for me
    m_loadWindowCombobox->removeItem(item);
  }
  //clear the vector of items in the load list box
  m_loadListBoxItems.clear();
  //the vector of entries of the directory
  std::vector<GameEngine::DirEntry> entries;
  //getting all the entries(files) in the directory "levels" and putting them in the entries vector
  GameEngine::IOManager::GetDirectoryEntries("Levels", entries);

  //add all files to list box
  for (auto& e : entries)
  {
    //Don't add directories
    if (!e.isDirectory)
    {
      //Remove "Levels/" substring
      e.path.erase(0, std::string("Levels/").size());
      //push back the new list box item
      m_loadListBoxItems.push_back(new CEGUI::ListboxTextItem(e.path));
      //add the last added list box item to the combobox
      m_loadWindowCombobox->addItem(m_loadListBoxItems.back());
    }
  }
  //show the load window
  m_loadWindow->enable();
  m_loadWindow->setAlpha(1.0f);
  //hide the save window
  m_saveWindow->disable();
  m_saveWindow->setAlpha(0.0f);
  return true;
}
bool GameplayScreen::OnLoadCancelClick(const CEGUI::EventArgs& e)
{
  //play the sound effect on click
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();
  //hide the load window
  m_loadWindow->disable();
  m_loadWindow->setAlpha(0.0f);
  return true;
}
bool GameplayScreen::OnLoad(const CEGUI::EventArgs& e)
{
  //load th game
  puts("Loading game...");
  std::string path = "Levels/" + std::string(m_loadWindowCombobox->getText().c_str());

  levelPath = path;

  ClearLevel();

  if (LevelReaderWriter::LoadAsText(levelPath, m_world.get(), m_player, m_boxes, m_obstacles, m_coins, m_exit, m_trigger, m_lights, m_enemies))
  {
    m_enemiesKilled = 0;
    m_coinsCollected = 0;
    m_hasPlayer = true;
    m_hasTrigger = true;
    m_hasExit = true;
    m_elapsedTime = 0.0f;

    ////////////////////////////////////////////////////////////Set up the background layers
    //center the camera position to the player
    m_camera.SetPosition(m_player.GetPosition());
    //the bottom left position of the screen in world coordinates
    glm::vec2 bottomLeft = m_camera.ConvertScreenToWorld(glm::vec2(0.0f, m_window->GetScreenHeight()));
    //the screen dimension in world coordinates
    glm::vec2 screenDims = glm::vec2(m_window->GetScreenWidth() / m_camera.GetScale(), m_window->GetScreenHeight() / m_camera.GetScale());
    //3 foreground layers positions at the center and 1 to the right and 1 to the left
    m_backgroundLayers.emplace_back(0.1f, -1.0f, "Assets/layers/skill-desc_0000_foreground.png",
      bottomLeft, screenDims, m_player.GetPosition().x, 0);

    m_backgroundLayers.emplace_back(0.1f, -1.0f, "Assets/layers/skill-desc_0000_foreground.png",
      glm::vec2(bottomLeft.x + screenDims.x, bottomLeft.y), screenDims, m_player.GetPosition().x, 1);

    m_backgroundLayers.emplace_back(0.1f, -1.0f, "Assets/layers/skill-desc_0000_foreground.png",
      glm::vec2(bottomLeft.x - screenDims.x, bottomLeft.y), screenDims, m_player.GetPosition().x, -1);
    //////////////////////////////////////////////////////////////////////////////////

    //3 buildings layers positions at the center and 1 to the right and 1 to the left
    m_backgroundLayers.emplace_back(0.3f, -2.0f, "Assets/layers/skill-desc_0001_buildings.png",
      bottomLeft, screenDims, m_player.GetPosition().x, 0);

    m_backgroundLayers.emplace_back(0.1f, -2.0f, "Assets/layers/skill-desc_0001_buildings.png",
      glm::vec2(bottomLeft.x + screenDims.x, bottomLeft.y), screenDims, m_player.GetPosition().x, 1);

    m_backgroundLayers.emplace_back(0.1f, -2.0f, "Assets/layers/skill-desc_0001_buildings.png",
      glm::vec2(bottomLeft.x - screenDims.x, bottomLeft.y), screenDims, m_player.GetPosition().x, -1);
    //////////////////////////////////////////////////////////////////////////////////

    //3 far buildings layers positions at the center and 1 to the right and 1 to the left
    m_backgroundLayers.emplace_back(0.5f, -3.0f, "Assets/layers/skill-desc_0002_far-buildings.png",
      bottomLeft, screenDims, m_player.GetPosition().x, 0);

    m_backgroundLayers.emplace_back(0.1f, -3.0f, "Assets/layers/skill-desc_0002_far-buildings.png",
      glm::vec2(bottomLeft.x + screenDims.x, bottomLeft.y), screenDims, m_player.GetPosition().x, 1);

    m_backgroundLayers.emplace_back(0.1f, -3.0f, "Assets/layers/skill-desc_0002_far-buildings.png",
      glm::vec2(bottomLeft.x - screenDims.x, bottomLeft.y), screenDims, m_player.GetPosition().x, -1);
    //////////////////////////////////////////////////////////////////////////////////

    //Only 1 background needed as it always follows the player
    m_backgroundLayers.emplace_back(1.0f, -4.0f, "Assets/layers/skill-desc_0003_bg.png",
      bottomLeft, screenDims, m_player.GetPosition().x, 0);

    /////////////////////////////////////////////////////////////
  }
  puts("Load successful!");

  //hide the load window
  m_loadWindow->disable();
  m_loadWindow->setAlpha(0.0f);
  return true;
}
bool GameplayScreen::OnSaveMouseClick(const CEGUI::EventArgs& e)
{
  //play the on-click sound effect
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  //make sure the directory exists
  GameEngine::IOManager::MakeDirectory("Levels");
  //clear all the current selection in the save window combobox
  m_saveWindowCombobox->clearAllSelections();
  //remove the items
  for (auto& item : m_saveListBoxItems)
  {
    //delete doesnt need to be called since removeItem does this for me
    m_saveWindowCombobox->removeItem(item);
  }
  //clear the save list box items vector
  m_saveListBoxItems.clear();

  // vector of directory entries
  std::vector<GameEngine::DirEntry> entries;
  // get all the entries of the "levels" directory and put them in entries
  GameEngine::IOManager::GetDirectoryEntries("Levels", entries);

  //add all files to list box
  for (auto& e : entries)
  {
    //Don't add directories
    if (!e.isDirectory)
    {
      //Remove "Levels/" substring
      e.path.erase(0, std::string("Levels/").size());
      //push back the new list box text item (the filepath of the current entry)
      m_saveListBoxItems.push_back(new CEGUI::ListboxTextItem(e.path));
      //add the last save list box item to the save window combobox
      m_saveWindowCombobox->addItem(m_saveListBoxItems.back());
    }
  }
  //Show the save window
  m_saveWindow->enable();
  m_saveWindow->setAlpha(1.0f);
  //Hide the load window
  m_loadWindow->disable();
  m_loadWindow->setAlpha(0.0f);

  return true;
}
bool GameplayScreen::OnSaveCancelClick(const CEGUI::EventArgs& e)
{
  //play the on-click sound effect
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  //hide the save window
  m_saveWindow->disable();
  m_saveWindow->setAlpha(0.0f);
  return true;
}
bool GameplayScreen::OnSave(const CEGUI::EventArgs& e)
{
  //play the on-click sound effect
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  puts("saving game. . .");
  //make sure the levels directory exists again just in case
  GameEngine::IOManager::MakeDirectory("Levels");
  //save in text mode
  std::string text = "Levels/" + std::string(m_saveWindowCombobox->getText().c_str());
  if (LevelReaderWriter::SaveAsText(text, m_player, m_boxes, m_obstacles, m_coins, m_exit, m_trigger, m_lights, m_enemies))
  {
    m_saveWindow->disable();
    m_saveWindow->setAlpha(0.0f);
    puts("Save successful !");
  }
  else
  {
    puts("Failed to save file =[");
  }
  return true;
}

bool GameplayScreen::OnGameOver(const CEGUI::EventArgs& e)
{
  //transition to game over screen
  m_nextScreen = SCREEN_INDEX_GAMEOVER;
  m_currentState = GameEngine::ScreenState::CHANGE_NEXT;
  return true;
}

bool GameplayScreen::OnLevelClear(const CEGUI::EventArgs& e)
{
  //transition to level clear screen
  m_nextScreen = SCREEN_INDEX_CLEARLEVEL;
  m_currentState = GameEngine::ScreenState::CHANGE_NEXT;
  return true;
}

bool GameplayScreen::OnMusicVolumeChanged(const CEGUI::EventArgs& e)
{
  //Music volume editting
  int volumeValue = 0;
  //get the current value of the volume spinner as an int
  volumeValue = (int)m_volumeSpinner->getCurrentValue();
  //use the edit volume function from the audio engine(takes an integer)
  m_music.EditVolume(volumeValue);
  return true;
}