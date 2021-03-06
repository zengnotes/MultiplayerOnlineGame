//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ModelViewerScene.hpp Model viewer scene
//
#pragma once

// includes
#include "Scene.hpp"
#include "ModelViewerView.hpp"
#include "ModelViewerViewModel.hpp"
#include "KeyboardActionBindings.hpp"

// forward references
class RenderEngine;
class IWindowManager;
class KeyboardActionManager;

namespace ModelViewer
{
   class Panel;

   /// scene for model viewer
   class Scene: public ::Scene
   {
      /// base class type
      typedef ::Scene BaseClass;

   public:
      /// ctor
      Scene(ISceneManager& sceneManager,
         IWindowManager& windowManager,
         KeyboardActionManager& keyboardActionManager,
         IFileSystem& fileSystem,
         RenderEngine& renderEngine,
         GraphicsTaskManager& taskManager) throw();
      /// dtor
      virtual ~Scene() throw() {}

   private:
      /// sets up UI panel
      void SetupPanel(IWindowManager& windowManager, IFileSystem& fileSystem);

      /// called when animation should change
      void OnChangeAnimation(bool bNextAnim);

   private:
      /// view model
      ViewModel m_viewModel;

      /// view
      std::shared_ptr<View> m_spView;

      /// panel UI
      std::shared_ptr<Panel> m_spPanel;

      /// action bindings
      KeyboardActionBindings m_actionBindings;
   };

} // namespace ModelViewer
