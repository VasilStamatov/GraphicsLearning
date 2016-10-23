#include "GameScreen.h"
#include "ScreenIndices.h"

GameScreen::GameScreen(GameEngine::Window * _window) : m_window(_window)
{
		m_screenIndex = SCREEN_INDEX_GAMEPLAY;
}

GameScreen::~GameScreen()
{
}

int GameScreen::GetNextScreenIndex() const
{
		return SCREEN_INDEX_NO_SCREEN;
}

int GameScreen::GetPreviousScreenIndex() const
{
		return SCREEN_INDEX_NO_SCREEN;
}

void GameScreen::Build()
{
}

void GameScreen::Destroy()
{
}

void GameScreen::OnEntry()
{
}

void GameScreen::OnExit()
{
}

void GameScreen::Update()
{
		SDL_Event evnt;
		while (SDL_PollEvent(&evnt))
		{
		}
}

void GameScreen::Draw()
{
}
