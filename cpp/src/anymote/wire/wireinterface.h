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

#ifndef ANYMOTE_WIRE_WIREINTERFACE_H_
#define ANYMOTE_WIRE_WIREINTERFACE_H_

#include <glog/logging.h>
#include <vector>
#include "anymote/wire/wirelistener.h"

namespace anymote {
namespace wire {

// A generic interface for sending and receiving raw data for an Anymote
// session. Implementations will use asynchronous I/O for sending and receiving
// data. All I/O operations will be performed from a single dispatch thread, and
// the listener will be invoked from the dispatch thread.
class WireInterface {
 public:
  WireInterface() {}
  virtual ~WireInterface() {}

  // Sets the listener that will receive incoming data and error notifications.
  // @param listener The wire listener. No ownership is taken and the pointer
  //                 must be valid for the duration of the existence of this
  //                 instance.
  void set_listener(WireListener* listener) {
    CHECK_NOTNULL(listener);
    listener_ = listener;
  }

  // Sends data over the interface asynchronously. This function is thread-safe
  // and will queue the message to be sent, returning immediately.
  virtual void Send(const std::vector<uint8_t>& data) = 0;

  // Receives the given number of bytes from the interface asynchronously. The
  // request will be queued if there is already a pending receive operation.
  // This function will return immediately and the listener will be notified
  // from the dispatch thread when the data is received.
  // @param num_bytes The number of bytes to receive over the interface.
  virtual void Receive(size_t num_bytes) = 0;

 protected:
  WireListener* listener() const { return listener_; }

 private:
  // This pointer is not owned by this adapter but should remain valid for
  // the duration of the existence of this instance.
  WireListener* listener_;

  // Disallow copy and assign.
  WireInterface(const WireInterface&);
  void operator=(const WireInterface&);
};

}  // namespace wire
}  // namespace anymote

#endif  // ANYMOTE_WIRE_WIREINTERFACE_H_
