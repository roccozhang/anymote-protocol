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

#ifndef ANYMOTE_DEVICE_ANYMOTELISTENER_H_
#define ANYMOTE_DEVICE_ANYMOTELISTENER_H_

#include <stdint.h>
#include <string>
#include "anymote/messages/remote.pb.h"

namespace anymote {
namespace device {

// Interface for a listener that handles AnyMote responses.
class AnymoteListener {
 public:
  virtual ~AnymoteListener() {}

  // Handles an AnyMote ack response.
  virtual void OnAck() = 0;

  // Handles an Anymote response containing generic data.
  //
  // @param type The data type identifier.
  // @param data The data payload.
  virtual void OnData(const std::string& type,
                      const std::string& data) = 0;

  // Handles an AnyMote fling result.
  //
  // @param success Whether the fling was successful.
  // @param sequence_number The fling sequence number, or 0 if there is no
  //                        sequence number.
  virtual void OnFlingResult(bool success, uint32_t sequence_number) = 0;

  // Handles an Anymote error. This should be treated as a fatal error and the
  // session should be aborted.
  virtual void OnError() = 0;
};

}  // namespace device
}  // namespace anymote

#endif  // ANYMOTE_DEVICE_ANYMOTELISTENER_H_
