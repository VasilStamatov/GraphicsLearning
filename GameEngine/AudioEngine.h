#pragma once

#include <SDL\SDL_mixer.h>
#include <string>
#include <map>

namespace GameEngine
{

  class SoundEffect
  {
  public:
    friend class AudioEngine;
    /// plays the effect file
    /// @param loops: if loops == -1, loop forever,
    ///  otherwise it loops + 1 times
    void Play(int _loops = 0);

  private:
    Mix_Chunk* m_chunk = nullptr;
  };

  class Music
  {
  public:
    friend class AudioEngine;
    /// plays the music file
    /// @param loops: if loops == -1, loop forever,
    ///  otherwise it loops x amount of times
    void Play(int _loops = 1);

    /// Pauses whatever song is currently playing
    static void Pause();
    /// Stops whatever song is currently playing
    static void Stop();
    /// Resumes whatever song is currently playing
    static void Resume();
    ///edit the music volue
    static void EditVolume(int _volume);
  private:
    Mix_Music* m_music = nullptr;
  };

  class AudioEngine
  {
  public:
    AudioEngine();
    ~AudioEngine();

    void Init();
    void Destroy();

    SoundEffect LoadSoundEffect(const std::string& _filePath);
    Music LoadMusic(const std::string& _filePath);
  private:

    std::map<std::string, Mix_Chunk*> m_effectMap; ///< Effects cache
    std::map<std::string, Mix_Music*> m_musicMap; ///< Music cache

    bool m_isInitialized = false;
  };
}



