// Copyright 2012 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ANYMOTE_WIRE_MESSAGELISTENER_H_
#define ANYMOTE_WIRE_MESSAGELISTENER_H_

#include "anymote/messages/remote.pb.h"

namespace anymote {
namespace messages {

// Interface for listeners that receive AnyMote messages.
class MessageListener {
 public:
  virtual ~MessageListener() {}

  // Handles a received Anymote message.
  //
  // @param message The received message.
  virtual void OnMessage(const messages::RemoteMessage& message) = 0;

  // Handles an AnyMote error. This should be treated as a fatal error and the
  // session should be aborted.
  virtual void OnError() = 0;
};

}  // namespace messages
}  // namespace anymote

#endif  // ANYMOTE_WIRE_MESSAGELISTENER_H_
