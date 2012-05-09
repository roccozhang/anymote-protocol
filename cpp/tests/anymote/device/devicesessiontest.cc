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

// Tests for DeviceSession.

#include <anymote/device/devicesession.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::InSequence;
using ::testing::Mock;
using ::testing::Return;
using ::testing::StrictMock;

namespace anymote {
namespace device {

// Mock response listener.
class MockAnymoteListener : public AnymoteListener {
 public:
  MOCK_METHOD0(OnAck, void());
  MOCK_METHOD2(OnData, void(const std::string& type,
                                const std::string& data));
  MOCK_METHOD2(OnFlingResult, void(bool success, uint32_t sequence_number));
  MOCK_METHOD0(OnError, void());
};

// Mock wire adapter.
class MockWireAdapter : public wire::WireAdapter {
 public:
  explicit MockWireAdapter(wire::WireInterface* interface)
      : WireAdapter(interface) {}

  MOCK_METHOD0(Init, void());
  MOCK_METHOD1(SendMessage, void(const messages::RemoteMessage& message));
  MOCK_METHOD0(GetNextMessage, void());
  MOCK_METHOD1(OnBytesReceived, void(const std::vector<uint8_t>& data));
  MOCK_METHOD0(OnError, void());
};

// Mock wire interface.
class MockWireInterface : public wire::WireInterface {
 public:
  MOCK_METHOD1(Send, void(const std::vector<uint8_t>& data));
  MOCK_METHOD1(Receive, void(size_t num_bytes));
};

// Test fixture for a DeviceSession test.
class DeviceSessionTest : public ::testing::Test {
 public:
  DeviceSessionTest()
      : interface(),
        adapter(&interface),
        listener(),
        session(&adapter, &listener) {
  }

 protected:
  StrictMock<MockWireInterface> interface;
  StrictMock<MockWireAdapter> adapter;
  MockAnymoteListener listener;
  DeviceSession session;
};

// Defines a matcher for protobuf messages.
MATCHER_P(ProtoMatcher, proto, "") {
  return proto.SerializeAsString() == arg.SerializeAsString();
}

// Tests starting a session.
TEST_F(DeviceSessionTest, TestStartSession) {
  InSequence sequence;

  EXPECT_CALL(adapter, Init());

  session.StartSession();
}

// Tests sending a ping message.
TEST_F(DeviceSessionTest, TestSendPing) {
  InSequence sequence;

  messages::RemoteMessage message1;
  message1.set_sequence_number(1);
  message1.mutable_request_message();

  EXPECT_CALL(adapter, SendMessage(ProtoMatcher(message1)));

  // Verify that the sequence number increments for the next ping.
  messages::RemoteMessage message2;
  message2.set_sequence_number(2);
  message2.mutable_request_message();

  EXPECT_CALL(adapter, SendMessage(ProtoMatcher(message2)));

  session.SendPing();
  session.SendPing();
}

// Tests sending a key event.
TEST_F(DeviceSessionTest, TestSendKeyEvent) {
  messages::RemoteMessage message;
  message.mutable_request_message()->mutable_key_event_message()
      ->set_keycode(messages::KEYCODE_TV_POWER);
  message.mutable_request_message()->mutable_key_event_message()
      ->set_action(messages::DOWN);

  EXPECT_CALL(adapter, SendMessage(ProtoMatcher(message)));

  session.SendKeyEvent(messages::KEYCODE_TV_POWER, messages::DOWN);
}

// Tests sending a mouse move event.
TEST_F(DeviceSessionTest, TestSendMouseMove) {
  messages::RemoteMessage message;
  message.mutable_request_message()->mutable_mouse_event_message()
      ->set_x_delta(-50);
  message.mutable_request_message()->mutable_mouse_event_message()
      ->set_y_delta(100);

  EXPECT_CALL(adapter, SendMessage(ProtoMatcher(message)));

  session.SendMouseMove(-50, 100);
}

// Tests sending a mouse wheel event.
TEST_F(DeviceSessionTest, TestSendMouseWheel) {
  messages::RemoteMessage message;
  message.mutable_request_message()->mutable_mouse_wheel_message()
      ->set_x_scroll(10);
  message.mutable_request_message()->mutable_mouse_wheel_message()
      ->set_y_scroll(-20);

  EXPECT_CALL(adapter, SendMessage(ProtoMatcher(message)));

  session.SendMouseWheel(10, -20);
}

// Tests sending a data message.
TEST_F(DeviceSessionTest, TestSendData) {
  messages::RemoteMessage message;
  message.mutable_request_message()->mutable_data_message()
      ->set_type("foo");
  message.mutable_request_message()->mutable_data_message()
      ->set_data("bar");

  EXPECT_CALL(adapter, SendMessage(ProtoMatcher(message)));

  session.SendData("foo", "bar");
}

// Tests sending a connect message.
TEST_F(DeviceSessionTest, TestSendConnect) {
  messages::RemoteMessage message;
  message.mutable_request_message()->mutable_connect_message()
      ->set_device_name("foo");
  message.mutable_request_message()->mutable_connect_message()
      ->set_version(123);

  EXPECT_CALL(adapter, SendMessage(ProtoMatcher(message)));

  session.SendConnect("foo", 123);
}

// Tests sending a fling message.
TEST_F(DeviceSessionTest, TestSendFling) {
  messages::RemoteMessage message;
  message.mutable_request_message()->mutable_fling_message()
      ->set_uri("http://foo");
  message.set_sequence_number(123);

  EXPECT_CALL(adapter, SendMessage(ProtoMatcher(message)));

  session.SendFling("http://foo", 123);
}

// Tests handling an ack response.
TEST_F(DeviceSessionTest, TestOnMessageAck) {
  messages::RemoteMessage message;
  message.set_sequence_number(123);

  EXPECT_CALL(listener, OnAck());

  session.OnMessage(message);
}

// Tests handling a data response.
TEST_F(DeviceSessionTest, TestOnMessageData) {
  messages::RemoteMessage message;
  message.mutable_response_message()->mutable_data_message()->set_type("foo");
  message.mutable_response_message()->mutable_data_message()->set_data("bar");

  EXPECT_CALL(listener, OnData("foo", "bar"));

  session.OnMessage(message);
}

// Tests handling a fling result response.
TEST_F(DeviceSessionTest, TestOnMessageFlingResult) {
  messages::RemoteMessage message;
  message.mutable_response_message()->mutable_fling_result_message()
      ->set_result(messages::FlingResult_Result_SUCCESS);
  message.set_sequence_number(123);

  EXPECT_CALL(listener, OnFlingResult(true, 123));

  session.OnMessage(message);
}

}  // namespace device
}  // namespace anymote
