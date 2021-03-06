//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2014 Michael Fink
//
/// \file BaseFileSystem.cpp Base file system
//

// includes
#include "StdAfx.h"
#include "BaseFileSystem.hpp"
#include "Filesystem.hpp"
#include "Path.hpp"
#include <ulib/stream/FileStream.hpp>

CString BaseFileSystem::UserDataFolder() const
{
   // TODO return user account data folder
   return Filesystem().BaseFolder();
}

CString BaseFileSystem::AppDataFolder() const
{
#ifdef _DEBUG
   return Filesystem().BaseFolder();
#else
   // TODO return Windows AppData folder
   return Filesystem().BaseFolder();
#endif
}

bool BaseFileSystem::IsFileAvail(const CString& cszFilename) const
{
   return Path(cszFilename).FileExists();
}

std::shared_ptr<Stream::IStream> BaseFileSystem::OpenFile(const CString& cszFilename, bool bForReading)
{
   // TODO check that the path is inside the app data folder

   using Stream::FileStream;

   std::shared_ptr<Stream::IStream> spStream(
      new Stream::FileStream(cszFilename,
         bForReading ? Stream::FileStream::modeOpen : Stream::FileStream::modeCreate,
         bForReading ? Stream::FileStream::accessRead : Stream::FileStream::accessWrite,
         Stream::FileStream::shareRead // always disallow writing
      ));

   return spStream;
}
