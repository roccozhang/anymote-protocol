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

#include <glog/logging.h>
#include "anymote/device/devicesession.h"
#include "anymote/messages/remote.pb.h"

using ::anymote::messages::RemoteMessage;
using ::anymote::messages::RequestMessage;
using ::anymote::messages::ResponseMessage;

namespace anymote {
namespace device {

DeviceSession::DeviceSession(wire::WireAdapter* adapter,
                             AnymoteListener* listener)
    : adapter_(adapter),
      listener_(listener),
      ping_counter_(0) {
  CHECK_NOTNULL(adapter);
  CHECK_NOTNULL(listener);
}

void DeviceSession::StartSession() {
  adapter_->set_listener(this);
  adapter_->Init();
}

void DeviceSession::SendPing() {
  RequestMessage request;
  SendRequestWithSequence(request, ++ping_counter_);
}

void DeviceSession::SendKeyEvent(messages::Code keycode,
                                 messages::Action action) {
  RequestMessage request;
  request.mutable_key_event_message()->set_keycode(keycode);
  request.mutable_key_event_message()->set_action(action);
  SendRequest(request);
}

void DeviceSession::SendMouseMove(int x_delta, int y_delta) {
  RequestMessage request;
  request.mutable_mouse_event_message()->set_x_delta(x_delta);
  request.mutable_mouse_event_message()->set_y_delta(y_delta);
  SendRequest(request);
}

void DeviceSession::SendMouseWheel(int x_scroll, int y_scroll) {
  RequestMessage request;
  request.mutable_mouse_wheel_message()->set_x_scroll(x_scroll);
  request.mutable_mouse_wheel_message()->set_y_scroll(y_scroll);
  SendRequest(request);
}

void DeviceSession::SendData(const std::string& type,
                             const std::string& data) {
  RequestMessage request;
  request.mutable_data_message()->set_type(type);
  request.mutable_data_message()->set_data(data);
  SendRequest(request);
}

void DeviceSession::SendConnect(const std::string& device_name,
                                int32_t version) {
  RequestMessage request;
  request.mutable_connect_message()->set_device_name(device_name);
  request.mutable_connect_message()->set_version(version);
  SendRequest(request);
}

void DeviceSession::SendFling(std::string uri, int32_t sequence_number) {
  RequestMessage request;
  request.mutable_fling_message()->set_uri(uri);
  SendRequestWithSequence(request, sequence_number);
}

void DeviceSession::SendRequest(const messages::RequestMessage& request) {
  SendRequestWithSequence(request, 0);
}

void DeviceSession::SendRequestWithSequence(
    const messages::RequestMessage& request,
    int32_t sequence_number) {
  CHECK_GE(sequence_number, 0) << "Sequence number must not be negative";

  RemoteMessage message;
  if (sequence_number) {
    message.set_sequence_number(sequence_number);
  }
  message.mutable_request_message()->CopyFrom(request);
  adapter_->SendMessage(message);
}

void DeviceSession::OnMessage(const messages::RemoteMessage& message) {
  const ResponseMessage& response = message.response_message();
  uint32_t sequence_number = message.has_sequence_number() ?
      message.sequence_number() : 0;
  bool empty = true;

  // Invoke the listener if the response has any of these messages.
  if (response.has_data_message()) {
    empty = false;
    const messages::Data& data = response.data_message();
    listener_->OnData(data.type(), data.data());
  }

  if (response.has_fling_result_message()) {
    empty = false;
    bool success = response.fling_result_message().result()
        == messages::FlingResult_Result_SUCCESS;
    listener_->OnFlingResult(success, sequence_number);
  }

  // If the response was empty and there was a sequence number, treat it as an
  // ack.
  if (empty && sequence_number) {
    listener_->OnAck();
  }
}

void DeviceSession::OnError() {
  listener_->OnError();
}

}  // namespace device
}  // namespace anymote
