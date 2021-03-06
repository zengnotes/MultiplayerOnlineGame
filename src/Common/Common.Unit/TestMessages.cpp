//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2014 Michael Fink
//
//! \file TestMessages.cpp Unit tests for Message classes
//

// includes
#include "stdafx.h"
#include "LogoutMessage.hpp"
#include "ByteStream.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{

/// tests all Message classes
TEST_CLASS(TestMessages)
{
   TEST_METHOD(TestAllMessages)
   {
      TestMessage<LogoutMessage>();
   }

   /// tests a message type
   template<typename TMessage>
   void TestMessage()
   {
      VectorStream in;
      unsigned int uiMessageId = 0;
      {
         TMessage msg;
         uiMessageId = msg.MessageId();
         msg.Serialize(in);
      }

      ConstVectorRefStream out(in.Data());
      {
         TMessage msg;
         msg.Deserialize(out);

         Assert::AreEqual(uiMessageId, msg.MessageId(), _T("Message IDs must be equal"));
      }
   }
};
