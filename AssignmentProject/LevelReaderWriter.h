#ifndef _LEVELREADERWRITER_
#define _LEVELREADERWRITER_

#include <string>

#include "Player.h"
#include "Box.h"
#include "Light.h"
#include "EnemyRobot.h"
#include "Coins.h"

class LevelReaderWriter
{
public:
  //Static functions for level loading and saving (public ones just call the private ones which actually do the job
  static bool SaveAsText(const std::string& _filePath, const Player& _player, const std::vector<Box>& _boxes,
                         const std::vector<Box>& _obstacles, const std::vector<Coins>& _coins, const Box& _exit,
                        const Box& _trigger, const std::vector<Light>& _lights, const std::vector<EnemyRobot>& _enemies);

  static bool SaveAsBinary(const std::string& _filePath, const Player& _player, const std::vector<Box>& _boxes);

  static bool LoadAsText(const std::string& _filePath, b2World* _world, Player& _player, std::vector<Box>& _boxes,
                          std::vector<Box>& _obstacles, std::vector<Coins>& _coins, Box& _exit,
                          Box& _trigger, std::vector<Light>& _lights, std::vector<EnemyRobot>& _enemies);

  static bool LoadAsBinary(const std::string& _filePath, b2World* _world,  Player& _player, std::vector<Box>& _boxes);

private:
  //Static functions for level loading and saving (the private ones which actually do the job)
  static bool SaveAsTextV0(const std::string& _filePath, const Player& _player, const std::vector<Box>& _boxes,
                            const std::vector<Box>& _obstacles, const std::vector<Coins>& _coins, const Box& _exit,
                            const Box& _trigger, const std::vector<Light>& _lights, const std::vector<EnemyRobot>& _enemies);

  static bool SaveAsBinaryV0(const std::string& _filePath, const Player& _player, const std::vector<Box>& _boxes);

  static bool LoadAsTextV0(std::ifstream& _file, b2World* _world, Player& _player, std::vector<Box>& _boxes,
                            std::vector<Box>& _obstacles, std::vector<Coins>& _coins, Box& _exit,
                            Box& _trigger, std::vector<Light>& _lights, std::vector<EnemyRobot>& _enemies);

  static bool LoadAsBinaryV0(std::fstream& _file, b2World* _world, Player& _player, std::vector<Box>& _boxes);
};

#endif