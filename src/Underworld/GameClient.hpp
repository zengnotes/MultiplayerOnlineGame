//
// Underworld - an Ultima Underworld remake project
// Copyright (C) 2002-2014 Michael Fink
//
/// \file Underworld/GameClient.hpp Game client
//
#pragma once

// includes
#include "stdafx.h"
#include "GameClientBase.hpp"
#include "LoadingScene.hpp"
#include "Game.hpp"
#include "DebugClient.hpp"

/// game client
class GameClient : public GameClientBase
{
public:
   /// ctor
   GameClient()
      :GameClientBase(_T("Underworld")),
      m_game(GetGraphicsTaskManager()),
      m_debugClient(m_game)
   {
      GetKeyboardActionBindings().RegisterActionHandler(
         KeyboardActionManager::actionDebugger, [&](bool){
         m_debugClient.RunDebugger();
      });
   }
   /// dtor
   virtual ~GameClient()
   {
   }

   /// starts game client
   void Start()
   {
      GameClientBase::Init(800, 600, false);

      ISceneManager& sceneManager = *this;

      std::shared_ptr<Scene> spScene =
         std::shared_ptr<LoadingScene>(new LoadingScene(sceneManager, *this, m_game));
      sceneManager.ChangeScene(spScene);

      GameClientBase::Run();

      m_game.Done();
   }

   void Decompile()
   {
      m_game.Init();
      m_game.Decompile();
   }

private:
   /// game objects
   Game m_game;

   /// debug client
   DebugClient m_debugClient;
};
