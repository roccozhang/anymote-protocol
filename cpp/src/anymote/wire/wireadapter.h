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

#ifndef ANYMOTE_WIRE_WIREADAPTER_H_
#define ANYMOTE_WIRE_WIREADAPTER_H_

#include <glog/logging.h>
#include "anymote/messages/messagelistener.h"
#include "anymote/wire/wireinterface.h"
#include "anymote/wire/wirelistener.h"

namespace anymote {
namespace wire {

// Abstract class for protocol adapters that send and receive Anymote messages.
// The adapter is responsible for serializing and deserializing messages sent
// and received from the supplied WireInterface. The listener may call back to
// this adapter, but it must not perform any blocking I/O.
class WireAdapter : public WireListener {
 public:
  // Creates a new adapter on the given interface.
  // @param interface The interface used to send/receive data. No ownership is
  //                  taken and the pointer must be valid for the duration of
  //                  the existence of this instance.
  explicit WireAdapter(WireInterface* interface)
      : interface_(interface),
        listener_(NULL),
        initialized_(false) {
    CHECK_NOTNULL(interface);
    interface_->set_listener(this);
  }

  virtual ~WireAdapter() {}

  // Sets the listener that will receive incoming Anymote messages.
  // @param listener The Anymote message listener. No ownership is taken and the
  //                 pointer must be valid for the duration of the existence of
  //                 this instance.
  void set_listener(messages::MessageListener* listener) {
    CHECK_NOTNULL(listener);
    listener_ = listener;
  }

  // Initializes the wire adapter. This must be called once before sending
  // a message. This will cause the adapter to start receiving new messages
  // which will be forwarded to the listener.
  virtual void Init() {
    CHECK(!initialized_) << "Adapter already initialized";
    GetNextMessage();
    initialized_ = true;
  }

  // Sends a remote message asynchronously. This function is thread-safe and
  // will return immediately. Init must be invoked once before sending
  // a message to start receiving incoming messages.
  // @param message The message to send.
  virtual void SendMessage(const messages::RemoteMessage& message) = 0;

  bool initialized() { return initialized_; }

 protected:
  // Asynchronously receives the next message. The listener will be invoked
  // when a message has been received. Once a message is received, this function
  // will automatically be invoked again to continue receiving subsequent
  // messages. This function is not thread-safe.
  virtual void GetNextMessage() = 0;

  messages::MessageListener* listener() { return listener_; }
  WireInterface* interface() { return interface_; }

 private:
  // These pointers are not owned by this adapter but should remain valid for
  // the duration of the existence of this instance.
  WireInterface* interface_;
  messages::MessageListener* listener_;

  bool initialized_;

  // Disallow copy and assign.
  WireAdapter(const WireAdapter&);
  void operator=(const WireAdapter&);
};

}  // namespace wire
}  // namespace anymote

#endif  // ANYMOTE_WIRE_WIREADAPTER_H_
