#include "AudioEngine.h"
#include "GameEngineErrors.h"

namespace GameEngine
{
  void SoundEffect::Play(int _loops /* = 0 */)
  {
    //play the mix and error check
    if (Mix_PlayChannel(-1, m_chunk, _loops) == -1)
    {
      if (Mix_PlayChannel(0, m_chunk, _loops) == -1)
      {
        FatalError("Mix_PlayChannel error: " + std::string(Mix_GetError()));
      }
    }
  }

  void Music::Play(int _loops /* = 1 */)
  {
    //play music
    Mix_PlayMusic(m_music, _loops);
  }

  void Music::Pause()
  {
    //pause music
    Mix_PauseMusic();
  }

  void Music::Stop()
  {
    //stop music
    Mix_HaltMusic();
  }

  void Music::Resume()
  {
    //resume music
    Mix_ResumeMusic();
  }

  void Music::EditVolume(int _volume)
  {
    //edit the music volume with the passed int
    Mix_VolumeMusic(_volume);
  }

  AudioEngine::AudioEngine()
  {
    //empty
  }


  AudioEngine::~AudioEngine()
  {
    Destroy();
  }

  void AudioEngine::Init()
  {
    if (m_isInitialized)
    {
      FatalError("Tried to initialize audio engine twice");
    }
    // Parameter can be a bitwise combinations of MIX_INIT_FAC,
    // MIX_INIT_MOD, MIX_INIT_MP3, MIX_INIT_OGG 
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == -1)
    {
      FatalError("MIX_INIT error: " + std::string(Mix_GetError()));
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
    {
      FatalError("Mix_OpenAudio error: " + std::string(Mix_GetError()));
    }
    m_isInitialized = true;
  }
  void AudioEngine::Destroy()
  {
    if (m_isInitialized)
    {
      m_isInitialized = false;
      //iterate through the effectmap and free the chunks
      for (auto& it : m_effectMap)
      {
        Mix_FreeChunk(it.second);
      }
      //iterate through the musicmap and free the music
      for (auto& it : m_musicMap)
      {
        Mix_FreeMusic(it.second);
      }
      m_effectMap.clear();
      m_musicMap.clear();

      Mix_CloseAudio();

      Mix_Quit();
    }
  }

  SoundEffect AudioEngine::LoadSoundEffect(const std::string& _filePath)
  {
    //try to find the audio in the cache
    auto it = m_effectMap.find(_filePath);

    SoundEffect effect;

    if (it == m_effectMap.end())
    {
      //failed to find it, must load
      Mix_Chunk* chunk = Mix_LoadWAV(_filePath.c_str());
      //check for errors
      if (chunk == nullptr)
      {
        FatalError("Mix_LoadWAV error: " + std::string(Mix_GetError()));
      }

      effect.m_chunk = chunk;
      m_effectMap[_filePath] = chunk;
    }
    else
    {
      //it's already cached
      effect.m_chunk = it->second;
    }
    return effect;
  }
  Music AudioEngine::LoadMusic(const std::string& _filePath)
  {
    //try to find the audio in the cache
    auto it = m_musicMap.find(_filePath);

    Music music;

    if (it == m_musicMap.end())
    {
      //failed to find it, must load
      Mix_Music* mixMusic = Mix_LoadMUS(_filePath.c_str());
      //check for errors
      if (mixMusic == nullptr)
      {
        FatalError("Mix_LoadMUS error: " + std::string(Mix_GetError()));
      }

      music.m_music = mixMusic;
      m_musicMap[_filePath] = mixMusic;
    }
    else
    {
      //it's already cached
      music.m_music = it->second;
    }
    return music;
  }
}
