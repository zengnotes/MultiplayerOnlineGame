//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2014 Michael Fink
//
/// \file MainGameLoop.hpp Main game loop
//
#pragma once

// includes
#include <atomic>
#include <SDL.h>

// forward references

/// main game loop
class MainGameLoop: public boost::noncopyable
{
public:
   /// ctor
   MainGameLoop(bool bUpdateFrameCount = false,
      const CString& cszClientName = _T(""),
      std::function<void(const CString&)> fnUpdateCaption = std::function<void(const CString&)>());

   /// runs main game loop
   void Run();

   /// quits loop (from another thread)
   void QuitLoop();

   // new virtual methods

   /// processes game logic
   virtual void OnTick(){}

   /// renders screen
   virtual void OnRender(){}

   /// called on new event
   virtual void OnEvent(SDL_Event& evt){ evt; }

private:
   /// updates window caption
   void UpdateCaption(double dFramesPerSecond);

   /// called to process events
   void ProcessEvents();

private:
   /// indicates that loop should exit
   std::atomic<bool> m_bExitLoop;

   /// indicates if frame count should be updated in window caption
   bool m_bUpdateFrameCount;

   /// client name; used for window caption
   CString m_cszClientName;

   /// function to update caption of window
   std::function<void(const CString&)> m_fnUpdateCaption;

   /// indicates if application is active or hidden
   bool m_bAppActive;

   /// indicates if app window has mouse focus
   bool m_bMouseFocus;

   /// indicates if app window has keyboard input focus
   bool m_bInputFocus;
};
