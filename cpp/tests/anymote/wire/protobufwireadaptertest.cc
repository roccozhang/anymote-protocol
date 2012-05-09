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

// Tests for ProtobufWireAdapter.

#include <anymote/wire/protobufwireadapter.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "anymote/wire/mocks.h"

using ::testing::InSequence;
using ::testing::Mock;
using ::testing::Return;
using ::testing::StrictMock;

namespace anymote {
namespace wire {

class MockMessageListener : public messages::MessageListener {
 public:
  MOCK_METHOD1(OnMessage, void(const messages::RemoteMessage& message));
  MOCK_METHOD0(OnError, void());
};

// Test fixture for a ProtobufWireAdapter test.
class ProtobufWireAdapterTest : public ::testing::Test {
 public:
  ProtobufWireAdapterTest()
      : interface(),
        adapter(&interface) {
  }

 protected:
  virtual void SetUp() {
    // The adapter will start receiving the next message when initialized.
    EXPECT_CALL(interface, Receive(1));

    adapter.set_listener(&listener);
    adapter.Init();

    Mock::VerifyAndClear(&interface);
  }

  StrictMock<MockWireInterface> interface;
  StrictMock<MockMessageListener> listener;
  ProtobufWireAdapter adapter;
};

// Defines a matcher for protobuf messages.
MATCHER_P(ProtoMatcher, proto, "") {
  return proto.SerializeAsString() == arg.SerializeAsString();
}

// Tests sending an empty remote message as a base case.
TEST_F(ProtobufWireAdapterTest, TestSendMessageEmpty) {
  InSequence sequence;

  messages::RemoteMessage message;

  // Should send a single 0 byte for the message size.
  std::vector<uint8_t> data(1, 0);

  EXPECT_CALL(interface, Send(data));

  adapter.SendMessage(message);
}

// Tests sending a message.
TEST_F(ProtobufWireAdapterTest, TestSendMessage) {
  InSequence sequence;

  messages::RemoteMessage message;
  message.mutable_request_message()->mutable_connect_message()
      ->set_device_name("foo");

  uint8_t bytes[10] = {
      9,  // The message length will be 9 bytes.
      18, 7, 42, 5, 10, 3, 'f', 'o', 'o'};
  std::vector<uint8_t> data(bytes, bytes + 10);

  EXPECT_CALL(interface, Send(data));

  adapter.SendMessage(message);
}

// Tests reading the preamble.
TEST_F(ProtobufWireAdapterTest, TestHandleBytesReceivedPreamble) {
  InSequence sequence;

  // Keep reading the next preamble byte as long as the high bit is set.
  EXPECT_CALL(interface, Receive(1));
  std::vector<uint8_t> preamble1(1, 0x80);
  adapter.OnBytesReceived(preamble1);

  EXPECT_CALL(interface, Receive(1));
  std::vector<uint8_t> preamble2(1, 0x80);
  adapter.OnBytesReceived(preamble2);

  // This will be the last byte of the preamble. The message length will be
  // (0x11 * 2^14) = 278528.
  EXPECT_CALL(interface, Receive(278528));
  std::vector<uint8_t> preamble3(1, 0x11);
  adapter.OnBytesReceived(preamble3);
}

// Tests reading an invalid preamble with too many bytes.
TEST_F(ProtobufWireAdapterTest, TestHandleBytesReceivedInvalidPreamble) {
  InSequence sequence;

  std::vector<uint8_t> preamble(1, 0x80);

  EXPECT_CALL(interface, Receive(1));
  adapter.OnBytesReceived(preamble);

  EXPECT_CALL(interface, Receive(1));
  adapter.OnBytesReceived(preamble);

  EXPECT_CALL(interface, Receive(1));
  adapter.OnBytesReceived(preamble);

  EXPECT_CALL(interface, Receive(1));
  adapter.OnBytesReceived(preamble);

  // This fifth byte will cause an error because it exceeds the varint32 size.
  EXPECT_CALL(listener, OnError());
  adapter.OnBytesReceived(preamble);
}

// Tests that a received message is successfully parsed.
TEST_F(ProtobufWireAdapterTest, TestParseMessage) {
  InSequence sequence;

  EXPECT_CALL(interface, Receive(9));
  std::vector<uint8_t> preamble(1, 0x09);
  adapter.OnBytesReceived(preamble);

  messages::RemoteMessage message;
  message.mutable_request_message()->mutable_connect_message()
      ->set_device_name("foo");

  // The listener will be notified of the message and the adapter will
  // immediately wait for the next message.
  EXPECT_CALL(listener, OnMessage(ProtoMatcher(message)));
  EXPECT_CALL(interface, Receive(1));

  uint8_t bytes[9] = {18, 7, 42, 5, 10, 3, 'f', 'o', 'o'};
  std::vector<uint8_t> data(bytes, bytes + 9);
  adapter.OnBytesReceived(data);
}

}  // namespace wire
}  // namespace anymote
