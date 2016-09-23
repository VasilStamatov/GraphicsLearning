#include <GL/glew.h> // Include BEFORE GUI.h

#include "GUI.h"

#include <SDL\SDL_timer.h>
#include <utf8/utf8.h>

namespace GameEngine
{
  CEGUI::OpenGL3Renderer* GUI::m_renderer = nullptr;

  void GUI::Init(const std::string& _resourceDirectory)
  {
    // Check if the renderer and system were not already initialized
    if (m_renderer == nullptr)
    {
      m_renderer = &CEGUI::OpenGL3Renderer::bootstrapSystem();
    }
    //set up the resource group directories
    CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
    rp->setResourceGroupDirectory("imagesets", _resourceDirectory + "/imagesets/");
    rp->setResourceGroupDirectory("schemes", _resourceDirectory + "/schemes/");
    rp->setResourceGroupDirectory("fonts", _resourceDirectory + "/fonts/");
    rp->setResourceGroupDirectory("layouts", _resourceDirectory + "/layouts/");
    rp->setResourceGroupDirectory("looknfeels", _resourceDirectory + "/looknfeel/");
    rp->setResourceGroupDirectory("lua_scripts", _resourceDirectory + "/lua_scripts/");
    //set the default resource groups
    CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
    CEGUI::Scheme::setDefaultResourceGroup("schemes");
    CEGUI::Font::setDefaultResourceGroup("fonts");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
    CEGUI::WindowManager::setDefaultResourceGroup("layouts");
    CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
    //set up the context
    m_context = &CEGUI::System::getSingleton().createGUIContext(m_renderer->getDefaultRenderTarget());
    m_root = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "root");
    m_context->setRootWindow(m_root);
  }

  void GUI::DestroyGUI()
  {
    //destroy the GUI
    CEGUI::System::getSingleton().destroyGUIContext(*m_context);
    CEGUI::WindowManager::getSingleton().destroyWindow(m_root);
    m_context = nullptr;
    m_root = nullptr;
  }

  void GUI::Draw()
  {
    //Render the GUI
    glDisable(GL_DEPTH_TEST);
    m_renderer->beginRendering();
    m_context->draw();
    m_renderer->endRendering();
    // Clean up after CEGUI
    glBindVertexArray(0);
    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  void GUI::Update()
  {
    //Update gets called each frame
    unsigned int elapsed;
    if (m_lastTime == 0)
    {
      elapsed = 0;
      m_lastTime = SDL_GetTicks();
    }
    else
    {
      //get current ticks
      unsigned int nextTime = SDL_GetTicks();
      //calculate elapsed ticks
      elapsed = nextTime - m_lastTime;
      //last ticks = current ticks so for the next frame the last ticks is this
      m_lastTime = nextTime;
    }
    m_context->injectTimePulse((float)elapsed / 1000.0f);
  }

  void GUI::SetMouseCursor(const std::string& imageFile)
  {
    //set the image for the mouse cursor
    m_context->getMouseCursor().setDefaultImage(imageFile);
  }

  void GUI::ShowMouseCursor()
  {
    //show the mouse cursor
    m_context->getMouseCursor().show();
  }

  void GUI::HideMouseCursor()
  {
    //hide the mouse cursor
    m_context->getMouseCursor().hide();
  }

  CEGUI::Key::Scan SDLKeyToCEGUIKey(SDL_Keycode _key)
  {
    //Convert the SDL keys to CEGUI keys
    //Reference: http://cegui.org.uk/wiki/SDL_to_CEGUI_keytable
    using namespace CEGUI;
    switch (_key)
    {
    case SDLK_BACKSPACE:    return Key::Backspace;
    case SDLK_TAB:          return Key::Tab;
    case SDLK_RETURN:       return Key::Return;
    case SDLK_PAUSE:        return Key::Pause;
    case SDLK_ESCAPE:       return Key::Escape;
    case SDLK_SPACE:        return Key::Space;
    case SDLK_COMMA:        return Key::Comma;
    case SDLK_MINUS:        return Key::Minus;
    case SDLK_PERIOD:       return Key::Period;
    case SDLK_SLASH:        return Key::Slash;
    case SDLK_0:            return Key::Zero;
    case SDLK_1:            return Key::One;
    case SDLK_2:            return Key::Two;
    case SDLK_3:            return Key::Three;
    case SDLK_4:            return Key::Four;
    case SDLK_5:            return Key::Five;
    case SDLK_6:            return Key::Six;
    case SDLK_7:            return Key::Seven;
    case SDLK_8:            return Key::Eight;
    case SDLK_9:            return Key::Nine;
    case SDLK_COLON:        return Key::Colon;
    case SDLK_SEMICOLON:    return Key::Semicolon;
    case SDLK_EQUALS:       return Key::Equals;
    case SDLK_LEFTBRACKET:  return Key::LeftBracket;
    case SDLK_BACKSLASH:    return Key::Backslash;
    case SDLK_RIGHTBRACKET: return Key::RightBracket;
    case SDLK_a:            return Key::A;
    case SDLK_b:            return Key::B;
    case SDLK_c:            return Key::C;
    case SDLK_d:            return Key::D;
    case SDLK_e:            return Key::E;
    case SDLK_f:            return Key::F;
    case SDLK_g:            return Key::G;
    case SDLK_h:            return Key::H;
    case SDLK_i:            return Key::I;
    case SDLK_j:            return Key::J;
    case SDLK_k:            return Key::K;
    case SDLK_l:            return Key::L;
    case SDLK_m:            return Key::M;
    case SDLK_n:            return Key::N;
    case SDLK_o:            return Key::O;
    case SDLK_p:            return Key::P;
    case SDLK_q:            return Key::Q;
    case SDLK_r:            return Key::R;
    case SDLK_s:            return Key::S;
    case SDLK_t:            return Key::T;
    case SDLK_u:            return Key::U;
    case SDLK_v:            return Key::V;
    case SDLK_w:            return Key::W;
    case SDLK_x:            return Key::X;
    case SDLK_y:            return Key::Y;
    case SDLK_z:            return Key::Z;
    case SDLK_DELETE:       return Key::Delete;
    case SDLK_KP_PERIOD:    return Key::Decimal;
    case SDLK_KP_DIVIDE:    return Key::Divide;
    case SDLK_KP_MULTIPLY:  return Key::Multiply;
    case SDLK_KP_MINUS:     return Key::Subtract;
    case SDLK_KP_PLUS:      return Key::Add;
    case SDLK_KP_ENTER:     return Key::NumpadEnter;
    case SDLK_KP_EQUALS:    return Key::NumpadEquals;
    case SDLK_UP:           return Key::ArrowUp;
    case SDLK_DOWN:         return Key::ArrowDown;
    case SDLK_RIGHT:        return Key::ArrowRight;
    case SDLK_LEFT:         return Key::ArrowLeft;
    case SDLK_INSERT:       return Key::Insert;
    case SDLK_HOME:         return Key::Home;
    case SDLK_END:          return Key::End;
    case SDLK_PAGEUP:       return Key::PageUp;
    case SDLK_PAGEDOWN:     return Key::PageDown;
    case SDLK_F1:           return Key::F1;
    case SDLK_F2:           return Key::F2;
    case SDLK_F3:           return Key::F3;
    case SDLK_F4:           return Key::F4;
    case SDLK_F5:           return Key::F5;
    case SDLK_F6:           return Key::F6;
    case SDLK_F7:           return Key::F7;
    case SDLK_F8:           return Key::F8;
    case SDLK_F9:           return Key::F9;
    case SDLK_F10:          return Key::F10;
    case SDLK_F11:          return Key::F11;
    case SDLK_F12:          return Key::F12;
    case SDLK_F13:          return Key::F13;
    case SDLK_F14:          return Key::F14;
    case SDLK_F15:          return Key::F15;
    case SDLK_RSHIFT:       return Key::RightShift;
    case SDLK_LSHIFT:       return Key::LeftShift;
    case SDLK_RCTRL:        return Key::RightControl;
    case SDLK_LCTRL:        return Key::LeftControl;
    case SDLK_RALT:         return Key::RightAlt;
    case SDLK_LALT:         return Key::LeftAlt;
    case SDLK_SYSREQ:       return Key::SysRq;
    case SDLK_MENU:         return Key::AppMenu;
    case SDLK_POWER:        return Key::Power;
    default:                return Key::Unknown;
    }
  }

  CEGUI::MouseButton SDLButtonToCEGUIButton(Uint8 _sdlButton)
  {
    //convert SDL mouse buttons to CEGUI mouse buttons
    switch (_sdlButton)
    {
    case SDL_BUTTON_LEFT:   return CEGUI::MouseButton::LeftButton;
    case SDL_BUTTON_MIDDLE: return CEGUI::MouseButton::MiddleButton;
    case SDL_BUTTON_RIGHT:  return CEGUI::MouseButton::RightButton;
    case SDL_BUTTON_X1:     return CEGUI::MouseButton::X1Button;
    case SDL_BUTTON_X2:     return CEGUI::MouseButton::X2Button;
    }
    return CEGUI::MouseButton::NoButton;
  }

  void GUI::OnSDLEvent(SDL_Event& _evnt)
  {
    //handle SDL events
    CEGUI::utf32 codePoint;
    switch (_evnt.type)
    {
      //inject the mouse position
    case SDL_MOUSEMOTION:
    {
      m_context->injectMousePosition((float)_evnt.motion.x, (float)_evnt.motion.y);
      break;
    }
    //inject the key down
    case SDL_KEYDOWN:
    {
      m_context->injectKeyDown(SDLKeyToCEGUIKey(_evnt.key.keysym.sym));
      break;
    }
    //inject the key up
    case SDL_KEYUP:
    {
      m_context->injectKeyUp(SDLKeyToCEGUIKey(_evnt.key.keysym.sym));
      break;
    }
    //inject the text input
    case SDL_TEXTINPUT:
    {
      std::string evntText = std::string(_evnt.text.text);
      std::vector<int> utf32result;
      utf8::utf8to32(_evnt.text.text, _evnt.text.text + evntText.size(),

        std::back_inserter(utf32result));
      codePoint = (CEGUI::utf32)utf32result[0];
      m_context->injectChar(codePoint);
      break;
    }
    //inject the mouse button down
    case SDL_MOUSEBUTTONDOWN:
    {
      m_context->injectMouseButtonDown(SDLButtonToCEGUIButton(_evnt.button.button));
      break;
    }
    //inject the mouse button up
    case SDL_MOUSEBUTTONUP:
    {
      m_context->injectMouseButtonUp(SDLButtonToCEGUIButton(_evnt.button.button));
      break;
    }
    }
  }
  void GUI::LoadScheme(const std::string& _schemeFile)
  {
    //load the scheme file
    CEGUI::SchemeManager::getSingleton().createFromFile(_schemeFile);
  }

  CEGUI::Window* GUI::CreateWidget(const std::string& _type, const glm::vec4& _destRectPerc, const glm::vec4& _destRectPix, const std::string& _name /*= ""*/)
  {
    //create a new widget
    CEGUI::Window* newWindow = CEGUI::WindowManager::getSingleton().createWindow(_type, _name);
    m_root->addChild(newWindow);
    SetWidgetDestRect(newWindow, _destRectPerc, _destRectPix);
    return newWindow;
  }

  CEGUI::Window* GUI::CreateWidget(CEGUI::Window* _parent, const std::string& _type, const glm::vec4& _destRectPerc, const glm::vec4& _destRectPix, const std::string& _name /*= ""*/)
  {
    //create a child widget
    CEGUI::Window* newWindow = CEGUI::WindowManager::getSingleton().createWindow(_type, _name);
    _parent->addChild(newWindow);
    SetWidgetDestRect(newWindow, _destRectPerc, _destRectPix);
    return newWindow;
  }

  void GUI::SetWidgetDestRect(CEGUI::Window* _widget, const glm::vec4& _destRectPerc, const glm::vec4& _destRectPix)
  {
    //set the widget destination rectangle
    _widget->setPosition(CEGUI::UVector2(CEGUI::UDim(_destRectPerc.x, _destRectPix.x), CEGUI::UDim(_destRectPerc.y, _destRectPix.y)));
    _widget->setSize(CEGUI::USize(CEGUI::UDim(_destRectPerc.z, _destRectPix.z), CEGUI::UDim(_destRectPerc.w, _destRectPix.w)));
  }

  void GUI::SetFont(const std::string& _fontFile)
  {
    //set the font
    CEGUI::FontManager::getSingleton().createFromFile(_fontFile + ".font");
    m_context->setDefaultFont(_fontFile);
  }
}