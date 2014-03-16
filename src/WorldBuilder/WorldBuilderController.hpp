//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2014 Michael Fink
//
/// \file WorldBuilderController.hpp World builder controller
//
#pragma once

// includes
#include "KeyboardActionManager.hpp"

// forward references
class OverviewController;
class PerspectiveCamera;

/// controller for the WorldBuilder application
/// translates messages to keyboard manager and mouse events
class WorldBuilderController
{
public:
   /// ctor
   WorldBuilderController(std::shared_ptr<PerspectiveCamera> spCamera);
   /// dtor
   ~WorldBuilderController() throw();

   /// translates messages to keyboard and mouse events
   BOOL PreTranslateMessage(MSG* pMsg);

   /// does idle processing
   void OnIdle();

private:
   /// keyboard action manager
   KeyboardActionManager m_keyboardActionManager;

   /// controller
   std::shared_ptr<OverviewController> m_spController;

   /// indicates if the last mouse position is valid
   bool m_bLastMousePosValid;

   /// last mouse position
   CPoint m_ptLastMousePos;
};