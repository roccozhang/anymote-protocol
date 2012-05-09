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

// AnyMote wire adapter implementation that uses protocol buffers for sending
// and receiving messages. The protocol buffers are sent on the wire with
// a varint32 preamble which indicates the size of the message. The varint32
// is encoded using a variable number of bytes (up to 5). For this reason, the
// preamble is read 1 byte at a time until the full varint has been read.

#include "anymote/wire/protobufwireadapter.h"

#include <glog/logging.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

using ::google::protobuf::io::ArrayOutputStream;
using ::google::protobuf::io::CodedOutputStream;

namespace anymote {
namespace wire {

ProtobufWireAdapter::ProtobufWireAdapter(WireInterface* interface)
    : WireAdapter(interface),
      read_state_(kNone),
      preamble_(0),
      preamble_num_bytes_(0) {
}

void ProtobufWireAdapter::GetNextMessage() {
  if (read_state_ != kNone) {
    return;
  }

  VLOG(1) << "Reading first preamble byte";
  read_state_ = kPreamble;
  interface()->Receive(1);
}

void ProtobufWireAdapter::SendMessage(const messages::RemoteMessage& message) {
  VLOG(1) << "SendMessage";
  CHECK(initialized());

  int message_size = message.ByteSize();
  int size = CodedOutputStream::VarintSize32(message_size) + message_size;

  std::vector<uint8_t> buffer(size);

  ArrayOutputStream aos(&buffer[0], buffer.size());
  CodedOutputStream out(&aos);

  out.WriteVarint32(message_size);
  message.SerializeToCodedStream(&out);
  interface()->Send(buffer);
}

void ProtobufWireAdapter::OnBytesReceived(
    const std::vector<uint8_t>& data) {
  VLOG(1) << "OnBytesReceived: " << data.size();

  if (read_state_ == kMessage) {
    // We were waiting for a message, so parse the message and reset the read
    // state.
    read_state_ = kNone;
    ParseMessage(data);
    GetNextMessage();
  } else if (read_state_ == kPreamble && data.size() == 1) {
    HandlePreambleByte(data[0]);
  } else {
    LOG(ERROR) << "Unexpected state: " << read_state_
        << " bytes: " << data.size();
    OnError();
  }
}

void ProtobufWireAdapter::HandlePreambleByte(uint8_t byte) {
  VLOG(1) << "HandlePreambleByte: " << byte;

  // This logic is based on the protobuf code for parsing varint32s.
  preamble_ |= (byte & 0x7F) << (preamble_num_bytes_ * 7);

  preamble_num_bytes_++;
  if (!(byte & 0x80)) {
    // Done reading the preamble.
    uint32_t message_size = preamble_;

    VLOG(1) << "Done reading preamble: " << message_size;

    // Reset the preamble variables for the next read.
    preamble_ = 0;
    preamble_num_bytes_ = 0;

    // Receive the message.
    read_state_ = kMessage;
    interface()->Receive(message_size);
  } else {
    if (preamble_num_bytes_ >= 5) {
      LOG(ERROR) << "Invalid preamble, varint32 more than 5 bytes";

      // TODO(dhawkey): Supply error codes?
      OnError();
    } else {
      VLOG(1) << "Getting next preamble byte";
      // Get the next byte of the preamble.
      interface()->Receive(1);
    }
  }
}

void ProtobufWireAdapter::ParseMessage(const std::vector<uint8_t>& data) {
  messages::RemoteMessage message;
  message.ParseFromArray(&data[0], data.size());

  if (listener()) {
    listener()->OnMessage(message);
  }
}

void ProtobufWireAdapter::OnError() {
  if (listener()) {
    listener()->OnError();
  }
}

}  // namespace anymote
}  // namespace wire
