#include "LevelReaderWriter.h"

#include <GameEngine/ResourceManager.h>
#include <fstream>

// When you want to make a new version, add it here
const unsigned int TEXT_VERSION_0 = 100;
const unsigned int BINARY_VERSION_0 = 200;

// Make sure this is set to the current version
const unsigned int TEXT_VERSION = TEXT_VERSION_0;
const unsigned int BINARY_VERSION = BINARY_VERSION_0;

bool LevelReaderWriter::SaveAsText(const std::string& _filePath, const Player& _player,
  const std::vector<Box>& _boxes, const std::vector<Box>& _obstacles, const std::vector<Coins>& _coins, const Box& _exit,
  const Box& _trigger, const std::vector<Light>& _lights, const std::vector<EnemyRobot>& _enemies)
{
  //always up to date with newest version
  return SaveAsTextV0(_filePath, _player, _boxes, _obstacles, _coins, _exit, _trigger, _lights, _enemies);
}

bool LevelReaderWriter::SaveAsBinary(const std::string& _filePath, const Player& _player, const std::vector<Box>& _boxes)
{
  //always up to date with newest version
  return SaveAsBinaryV0(_filePath, _player, _boxes);
}

bool LevelReaderWriter::LoadAsText(const std::string& _filePath, b2World* _world, Player& _player, std::vector<Box>& _boxes,
  std::vector<Box>& _obstacles, std::vector<Coins>& _coins, Box& _exit, Box& _trigger, std::vector<Light>& _lights, std::vector<EnemyRobot>& _enemies)
{
  //open file and error check
  std::ifstream file(_filePath);
  if (file.fail())
  {
    perror(_filePath.c_str());
    return false;
  }

  // Get version
  unsigned int version;
  file >> version;

  // Parse based on version (only 1 so far, because the game isn't published)
  switch (version)
  {
  case TEXT_VERSION_0:
    LoadAsTextV0(file, _world, _player, _boxes, _obstacles, _coins, _exit, _trigger, _lights, _enemies);
    break;
  default:
    puts("Unknown version number in level file. File may be corrupted...");
    return false;
  }

  return true;
}

bool LevelReaderWriter::LoadAsBinary(const std::string& _filePath, b2World* _world, Player& _player, std::vector<Box>& _boxes)
{
  //not implemented
  std::fstream file(_filePath + ".bin", std::ios::out | std::ios::binary);
  if (file.fail())
  {
    perror((_filePath + ".bin").c_str());
    return false;
  }
  //get version
  unsigned int version = NULL;
  //read the version
  file.read((char*)version, sizeof(unsigned int));
  //read level based on version
  switch (version)
  {
  case BINARY_VERSION_0:
  {
    LoadAsBinaryV0(file, _world, _player, _boxes);
    break;
  }
  default: puts("Unknown version number in level file.File may be corrupted...");
    return false;
    break;
  }
  return true;
}

bool LevelReaderWriter::SaveAsTextV0(const std::string& _filePath, const Player& _player, const std::vector<Box>& _boxes,
  const std::vector<Box>& _obstacles, const std::vector<Coins>& _coins, const Box& _exit, const Box& _trigger,
  const std::vector<Light>& _lights, const std::vector<EnemyRobot>& _enemies)
{
  std::ofstream file(_filePath);
  if (file.fail())
  {
    perror(_filePath.c_str());
    return false;
  }

  //write version
  file << TEXT_VERSION << "\n";
  //write player
  file << _player.GetPosition().x << ' ' << _player.GetPosition().y << ' '
    << _player.GetDrawDims().x << ' ' << _player.GetDrawDims().y << ' '
    << _player.GetColDims().x << ' ' << _player.GetColDims().y << ' '
    << _player.GetColor().r << ' ' << _player.GetColor().g << ' '
    << _player.GetColor().b << ' ' << _player.GetColor().a << '\n';

  //write the exit
  file << _exit.GetPosition().x << ' ' << _exit.GetPosition().y << ' '
    << _exit.GetDimensions().x << ' ' << _exit.GetDimensions().y << ' '
    << _exit.GetColor().r << ' ' << _exit.GetColor().g << ' '
    << _exit.GetColor().b << ' ' << _exit.GetColor().a << ' '
    << _exit.GetUvRect().x << ' ' << _exit.GetUvRect().y << ' '
    << _exit.GetUvRect().z << ' ' << _exit.GetUvRect().w << ' '
    << _exit.GetAngle() << ' ' << _exit.GetTexture().filePath << ' '
    << _exit.GetIsDynamic() << ' ' << _exit.GetFixedRotation() << ' '
    << _exit.GetIsSensor() << '\n';

  //write the trigger(switch)
  file << _trigger.GetPosition().x << ' ' << _trigger.GetPosition().y << ' '
    << _trigger.GetDimensions().x << ' ' << _trigger.GetDimensions().y << ' '
    << _trigger.GetColor().r << ' ' << _trigger.GetColor().g << ' '
    << _trigger.GetColor().b << ' ' << _trigger.GetColor().a << ' '
    << _trigger.GetUvRect().x << ' ' << _trigger.GetUvRect().y << ' '
    << _trigger.GetUvRect().z << ' ' << _trigger.GetUvRect().w << ' '
    << _trigger.GetAngle() << ' ' << _trigger.GetTexture().filePath << ' '
    << _trigger.GetIsDynamic() << ' ' << _trigger.GetFixedRotation() << ' '
    << _trigger.GetIsSensor() << '\n';

  //write the number of boxes
  file << _boxes.size() << "\n";

  //write all the boxes
  for (auto& b : _boxes)
  {
    file << b.GetPosition().x << ' ' << b.GetPosition().y << ' '
      << b.GetDimensions().x << ' ' << b.GetDimensions().y << ' '
      << b.GetColor().r << ' ' << b.GetColor().g << ' '
      << b.GetColor().b << ' ' << b.GetColor().a << ' '
      << b.GetUvRect().x << ' ' << b.GetUvRect().y << ' '
      << b.GetUvRect().z << ' ' << b.GetUvRect().w << ' '
      << b.GetAngle() << ' ' << b.GetTexture().filePath << ' '
      << b.GetIsDynamic() << ' ' << b.GetFixedRotation() << ' '
      << b.GetIsSensor() << '\n';
  }

  //write the number of obstacles
  file << _obstacles.size() << "\n";
  //write all the obstacles
  for (auto& obs : _obstacles)
  {
    file << obs.GetPosition().x << ' ' << obs.GetPosition().y << ' '
      << obs.GetDimensions().x << ' ' << obs.GetDimensions().y << ' '
      << obs.GetColor().r << ' ' << obs.GetColor().g << ' '
      << obs.GetColor().b << ' ' << obs.GetColor().a << ' '
      << obs.GetUvRect().x << ' ' << obs.GetUvRect().y << ' '
      << obs.GetUvRect().z << ' ' << obs.GetUvRect().w << ' '
      << obs.GetAngle() << ' ' << obs.GetTexture().filePath << ' '
      << obs.GetIsDynamic() << ' ' << obs.GetFixedRotation() << ' '
      << obs.GetIsSensor() << '\n';
  }

  //write the number of lights
  file << _lights.size() << "\n";
  for (auto& light : _lights)
  {
    file << light.position.x << ' ' << light.position.y << ' ' << light.size << ' ' <<
      light.color.r << ' ' << light.color.g << ' ' << light.color.b << ' ' << light.color.a << '\n';
  }

  //write the number of enemies
  file << _enemies.size() << "\n";
  for (auto& enemy : _enemies)
  {
    //write player
    file << enemy.GetPosition().x << ' ' << enemy.GetPosition().y << ' '
      << enemy.GetDrawDims().x << ' ' << enemy.GetDrawDims().y << ' '
      << enemy.GetColDims().x << ' ' << enemy.GetColDims().y << ' '
      << enemy.GetColor().r << ' ' << enemy.GetColor().g << ' '
      << enemy.GetColor().b << ' ' << enemy.GetColor().a << '\n';
  }

  //write the number of obstacles
  file << _coins.size() << "\n";
  //write all the obstacles
  for (auto& coin : _coins)
  {
    file << coin.GetPosition().x << ' ' << coin.GetPosition().y << ' '
      << coin.GetDimensions().x << ' ' << coin.GetDimensions().y << ' '
      << coin.GetColor().r << ' ' << coin.GetColor().g << ' '
      << coin.GetColor().b << ' ' << coin.GetColor().a << ' ' << '\n';
  }
  file.close();
  return true;
}

bool LevelReaderWriter::LoadAsTextV0(std::ifstream& _file, b2World* _world, Player& _player, std::vector<Box>& _boxes,
         std::vector<Box>& _obstacles, std::vector<Coins>& _coins, Box& _exit, Box& _trigger, std::vector<Light>& _lights, std::vector<EnemyRobot>& _enemies)
{
  {
    //read the player
    glm::vec2 pos;
    glm::vec2 drawDims;
    glm::vec2 colDims;
    GameEngine::ColorRGBA8 color;
    _file >> pos.x >> pos.y >> drawDims.x >> drawDims.y >> colDims.x >> colDims.y >> color.r >> color.g >> color.b >> color.a;

    _player.Init(_world, pos, drawDims, colDims, color);
  }

  {
    //read the exit
    //read exits
    glm::vec2 pos;
    glm::vec2 dims;
    glm::vec4 uvRect;
    GameEngine::ColorRGBA8 color;
    bool fixedRotation;
    bool isDynamic;
    bool isSensor;
    float angle;

    GameEngine::GLTexture texture;
    std::string texturePath;

      _file >> pos.x >> pos.y >> dims.x >> dims.y >> color.r >> color.g >> color.b >> color.a
        >> uvRect.x >> uvRect.y >> uvRect.z >> uvRect.w
        >> angle >> texturePath >> isDynamic >> fixedRotation >> isSensor;

      texture = GameEngine::ResourceManager::GetTexture(texturePath);
      _exit.Init(_world, pos, dims, texture, color, fixedRotation, isDynamic, isSensor, angle, uvRect);
  }
  
  {
    //read the trigger(switch)
    //read triggers
    glm::vec2 pos;
    glm::vec2 dims;
    glm::vec4 uvRect;
    GameEngine::ColorRGBA8 color;
    bool fixedRotation;
    bool isDynamic;
    bool isSensor;
    float angle;

    GameEngine::GLTexture texture;
    std::string texturePath;

      _file >> pos.x >> pos.y >> dims.x >> dims.y >> color.r >> color.g >> color.b >> color.a
        >> uvRect.x >> uvRect.y >> uvRect.z >> uvRect.w
        >> angle >> texturePath >> isDynamic >> fixedRotation >> isSensor;

      texture = GameEngine::ResourceManager::GetTexture(texturePath);
      _trigger.Init(_world, pos, dims, texture, color, fixedRotation, isDynamic, isSensor, angle, uvRect);
  }

  {
    //read boxes
    glm::vec2 pos;
    glm::vec2 dims;
    glm::vec4 uvRect;
    GameEngine::ColorRGBA8 color;
    bool fixedRotation;
    bool isDynamic;
    bool isSensor;
    float angle;

    GameEngine::GLTexture texture;
    std::string texturePath;
    size_t numBoxes;
    _file >> numBoxes;

    for (size_t i = 0; i < numBoxes; i++)
    {
      _file >> pos.x >> pos.y >> dims.x >> dims.y >> color.r >> color.g >> color.b >> color.a
        >> uvRect.x >> uvRect.y >> uvRect.z >> uvRect.w
        >> angle >> texturePath >> isDynamic >> fixedRotation >> isSensor;

      texture = GameEngine::ResourceManager::GetTexture(texturePath);
      _boxes.emplace_back();
      _boxes.back().Init(_world, pos, dims, texture, color, fixedRotation, isDynamic, isSensor, angle, uvRect);
    }
  }

  {
    //read obstacles
    glm::vec2 pos;
    glm::vec2 dims;
    glm::vec4 uvRect;
    GameEngine::ColorRGBA8 color;
    bool fixedRotation;
    bool isDynamic;
    bool isSensor;
    float angle;

    GameEngine::GLTexture texture;
    std::string texturePath;
    size_t numObstacles;
    _file >> numObstacles;

    for (size_t i = 0; i < numObstacles; i++)
    {
      _file >> pos.x >> pos.y >> dims.x >> dims.y >> color.r >> color.g >> color.b >> color.a
        >> uvRect.x >> uvRect.y >> uvRect.z >> uvRect.w
        >> angle >> texturePath >> isDynamic >> fixedRotation >> isSensor;

      texture = GameEngine::ResourceManager::GetTexture(texturePath);
      _obstacles.emplace_back();
      _obstacles.back().Init(_world, pos, dims, texture, color, fixedRotation, isDynamic, isSensor, angle, uvRect);
    }
  }

  {
    //read lights
    glm::vec2 pos;
    GameEngine::ColorRGBA8 color;
    float size;

    size_t numLights;
    _file >> numLights;

    for (size_t i = 0; i < numLights; i++)
    {
      _file >> pos.x >> pos.y >> size >> color.r >> color.g >> color.b >> color.a;

      _lights.emplace_back();
      _lights.back().color = color;
      _lights.back().position = pos;
      _lights.back().size = size;
    }
  }

  {
    //read the enemies
    glm::vec2 pos;
    glm::vec2 drawDims;
    glm::vec2 colDims;
    GameEngine::ColorRGBA8 color;

    size_t numEnemies;
    _file >> numEnemies;

    for (size_t i = 0; i < numEnemies; i++)
    {
      _file >> pos.x >> pos.y >> drawDims.x >> drawDims.y >> colDims.x >> colDims.y >> color.r >> color.g >> color.b >> color.a;
      _enemies.emplace_back();
      _enemies.back().Init(_world, pos, drawDims, colDims, color);
    }
  }

  {
    //read coins
    glm::vec2 pos;
    glm::vec2 dims;
    GameEngine::ColorRGBA8 color;

    GameEngine::GLTexture texture;
    size_t numCoins;
    _file >> numCoins;

    for (size_t i = 0; i < numCoins; i++)
    {
      _file >> pos.x >> pos.y >> dims.x >> dims.y >> color.r >> color.g >> color.b >> color.a ;

      _coins.emplace_back(_world, pos, dims, color);
    }
  }
  _file.close();
  return true;
}

bool LevelReaderWriter::LoadAsBinaryV0(std::fstream& _file, b2World* _world, Player& _player, std::vector<Box>& _boxes)
{
  {
    //raed the player
    glm::vec2 pos;
    glm::vec2 ddims;
    glm::vec2 cdims;
    GameEngine::ColorRGBA8 color;

    _file.read((char*)&pos, sizeof(glm::vec2));
    _file.read((char*)&ddims, sizeof(glm::vec2));
    _file.read((char*)&cdims, sizeof(glm::vec2));
    _file.read((char*)&color, sizeof(GameEngine::ColorRGBA8));

    _player.Init(_world, pos, ddims, cdims, color);
  }

  {
    //read the boxes
    glm::vec2 pos;
    glm::vec2 dims;
    glm::vec4 uvRect;
    GameEngine::ColorRGBA8 color;
    bool fixedRotation;
    bool isDynamic;
    bool isSensor;
    float angle;

    GameEngine::GLTexture texture;
    std::string texturePath;
    size_t numBoxes;

    _file.read((char*)&numBoxes, sizeof(size_t));

    for (size_t i = 0; i < numBoxes; i++)
    {
      _file.read((char*)&pos, sizeof(glm::vec2));
      _file.read((char*)&dims, sizeof(glm::vec2));
      _file.read((char*)&color, sizeof(GameEngine::ColorRGBA8));
      _file.read((char*)&uvRect, sizeof(glm::vec4));
      _file.read((char*)&angle, sizeof(float));
      _file.read((char*)&texturePath, sizeof(std::string));
      _file.read((char*)&fixedRotation, sizeof(bool));
      _file.read((char*)&isDynamic, sizeof(bool));
      _file.read((char*)&isSensor, sizeof(bool));

      texture = GameEngine::ResourceManager::GetTexture(texturePath);
      _boxes.emplace_back();
      _boxes.back().Init(_world, pos, dims, texture, color, fixedRotation, isDynamic, isSensor, angle, uvRect);
    }
  }
  _file.close();
  return true;
}

bool LevelReaderWriter::SaveAsBinaryV0(const std::string& _filePath, const Player& _player, const std::vector<Box>& _boxes)
{
  //not implemented
  std::fstream file(_filePath + ".bin", std::ios::in | std::ios::binary);
  if (file.fail())
  {
    perror((_filePath + ".bin").c_str());
    return false;
  }
  std::string newLine = "\n";
  //write version
  file.write((char*)&BINARY_VERSION, sizeof(unsigned int));
  //newline
  file.write((char*)&newLine, sizeof(std::string));
  //player
  file.write((char*)&_player, sizeof(Player));
  //newline
  file.write((char*)&newLine, sizeof(std::string));
  //Number of boxes
  file.write((char*)_boxes.size(), sizeof(size_t));
  //boxes
  for (Box box : _boxes)
  {
    file.write((char*)&box, sizeof(Box));
  }
  file.close();
  return true;
}
