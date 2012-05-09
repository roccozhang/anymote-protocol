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

#ifndef ANYMOTE_WIRE_PROTOBUFWIREADAPTER_H_
#define ANYMOTE_WIRE_PROTOBUFWIREADAPTER_H_

#include <glog/logging.h>
#include <vector>
#include "anymote/wire/wireadapter.h"

namespace anymote {
namespace wire {

// Anymote wire adapter that uses protocol buffers to send/receive messages.
// This class is not thread-safe, so SendMessage and GetNextMessage should only
// be invoked from a single thread.
class ProtobufWireAdapter : public WireAdapter {
 public:
  // Creates a new Protobuf adapter on the given interface.
  // @param interface The interface used to send/receive data. No ownership is
  //                  taken and the pointer must be valid for the duration of
  //                  the existence of this instance.
  explicit ProtobufWireAdapter(WireInterface* interface);
  virtual ~ProtobufWireAdapter() {}

  // @override
  virtual void GetNextMessage();

  // @override
  virtual void SendMessage(const messages::RemoteMessage& message);

  // @override
  virtual void OnBytesReceived(const std::vector<uint8_t>& data);

  // @override
  virtual void OnError();

 private:
  // The current read state.
  enum ReadState {
    // There is no read operation in progress.
    kNone,

    // Waiting to read the message preamble which is a varint32 representing
    // the size of the next message.
    kPreamble,

    // Waiting to read the message.
    kMessage,
  };

  // Handles a byte received as part of the message preamble.
  // @param byte The preamble byte.
  void HandlePreambleByte(uint8_t byte);

  // Parses an Anymote message from the given data.
  // @param data The data containing an Anymote message.
  void ParseMessage(const std::vector<uint8_t>& data);

  ReadState read_state_;
  uint32_t preamble_;
  uint8_t preamble_num_bytes_;
};

}  // namespace anymote
}  // namespace wire

#endif  // ANYMOTE_WIRE_PROTOBUFWIREADAPTER_H_
