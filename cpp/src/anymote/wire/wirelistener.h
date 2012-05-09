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

#ifndef ANYMOTE_WIRE_WIRELISTENER_H_
#define ANYMOTE_WIRE_WIRELISTENER_H_

#include <stdint.h>
#include <vector>

namespace anymote {
namespace wire {

// Interface for a listener that receives data over a wire interface. Listeners
// must not perform any blocking I/O.
class WireListener {
 public:
  virtual ~WireListener() {}

  // Handles data received over the interface.
  // @param data The data received.
  virtual void OnBytesReceived(const std::vector<uint8_t>& data) = 0;

  // Handles a protocol error from the wire interface if there was an error
  // sending or receiving data. This should be treated as a fatal error and the
  // session should be aborted.
  virtual void OnError() = 0;
};

}  // namespace wire
}  // namespace anymote

#endif  // ANYMOTE_WIRE_WIRELISTENER_H_
