#pragma once

#include <CEGUI\CEGUI.h>
#include <CEGUI\RendererModules\OpenGL\GL3Renderer.h>
#include <glm\glm.hpp>
#include <SDL\SDL_events.h>
namespace GameEngine
{
  class GUI
  {
  public:
    //initialize the gui
    void Init(const std::string& _resourceDirectory);
    //destroy the gui
    void DestroyGUI();
    //draw the gui
    void Draw();
    //update the gui
    void Update();
    //sets the mouse cursor to a custom image
    void SetMouseCursor(const std::string& _imageFile);
    //shows teh mouse cursor
    void ShowMouseCursor();
    //hides the mouse cursor
    void HideMouseCursor();
    //handles SDL events
    void OnSDLEvent(SDL_Event& _evnt);
    //Loads a scheme file
    void LoadScheme(const std::string& _schemeFile);
    //Sets the font the gui uses
    void SetFont(const std::string& _fontFile);
    //create a widget
    CEGUI::Window* CreateWidget(const std::string& type, const glm::vec4& destRectPerc, const glm::vec4& destRectPix, const std::string& name = "");
    //create a child widget (has a parent)
    CEGUI::Window* CreateWidget(CEGUI::Window* parent, const std::string& type, const glm::vec4& destRectPerc, const glm::vec4& destRectPix, const std::string& name = "");
    //sets the widget destination rectangle
    static void SetWidgetDestRect(CEGUI::Window* widget, const glm::vec4& destRectPerc, const glm::vec4& destRectPix);

    //Getters
    static CEGUI::OpenGL3Renderer* GetRenderer() noexcept { return m_renderer; }
    const CEGUI::GUIContext* GetContext()  const noexcept { return m_context; }

  private:
    static CEGUI::OpenGL3Renderer* m_renderer;
    CEGUI::GUIContext* m_context = nullptr;
    CEGUI::Window* m_root = nullptr;
    unsigned int m_lastTime = 0;
  };
}

