//
// Underworld - an Ultima Underworld remake project
// Copyright (C) 2002-2013 Michael Fink
//
//! \file ClientLogic/GameClientBase.hpp Game client base class
//
#pragma once

// includes
#include "ClientLogic.hpp"
#include "MainGameLoop.hpp"
#include "ISceneManager.hpp"
#include "KeyboardActionBindings.hpp"
#include <functional>

// forward references
class RenderEngine;
class IWindowManager;

/// game client base class
class CLIENTLOGIC_DECLSPEC GameClientBase :
   public MainGameLoop,
   public ISceneManager
{
public:
   /// ctor
   GameClientBase(const CString& cszClientName);
   /// dtor
   virtual ~GameClientBase();

   /// initializes client window
   void Init(unsigned int uiWidth, unsigned int uiHeight, bool bFullscreen);

   /// runs game
   void Run();

   /// returns render engine
   RenderEngine& GetRenderEngine();

   /// returns keyboard action manager
   KeyboardActionManager& GetKeyboardActionManager() throw() { return m_keyboardActionManager; }

private:
   // virtual methods from MainGameLoop

   virtual void OnRender() override;
   virtual void OnEvent(SDL_Event& evt) override;

   // virtual methods from ISceneManager

   virtual void ChangeScene(std::shared_ptr<Scene> spScene) override;
   virtual void DispatchInEventLoop(std::function<void()> fn) override;
   virtual Point GetMousePos() override;
   virtual void SetMousePos(const Point& pt) override;
   virtual void ShowCursor(bool bShow) override;
   virtual Size GetScreenSize() const throw() override;

private:
   /// render engine
   std::shared_ptr<RenderEngine> m_spRenderEngine;

   /// window manager
   std::shared_ptr<IWindowManager> m_spWindowManager;

   /// keyboard action manager
   /// \note this variable must be above the Scene ptr so that it gets destroyed after scene
   KeyboardActionManager m_keyboardActionManager;

   /// keyboard action bindings
   KeyboardActionBindings m_actionBindings;

   /// scene
   std::shared_ptr<Scene> m_spScene;
   /// previous scene; used to store scene ptr when changing scenes
   std::shared_ptr<Scene> m_spPreviousScene;

   /// indicates that next mouse motion event should be ignored
   bool m_bIgnoreNextMouseMotion;
   /// mouse motion x pos to ignore
   unsigned int m_iMouseMotionPosToIgnoreX;
   /// mouse motion y pos to ignore
   unsigned int m_iMouseMotionPosToIgnoreY;
};