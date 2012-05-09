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

#ifndef ANYMOTE_DEVICE_DEVICESESSION_H_
#define ANYMOTE_DEVICE_DEVICESESSION_H_

#include <string>
#include "anymote/device/anymotelistener.h"
#include "anymote/messages/keycodes.pb.h"
#include "anymote/messages/messagelistener.h"
#include "anymote/wire/wireadapter.h"

namespace anymote {
namespace device {

// Anymote device session used to communicate with an Anymote server. This is
// used to send mouse, key, fling, and data messages.
//
// The session must be initialized before sending any other messages. Example:
//
//   DeviceSession session(wire_adapter, anymote_listener);
//   session.StartSession();
//   session.SendConnect(device_name, version);
//
// Now the session can be used to call SendPing, SendKeyEvent, etc.
class DeviceSession : public messages::MessageListener {
 public:
  // Creates a new Anymote device session.
  //
  // @param adapter The wire adapter used to send and receive Anymote messages.
  //        The adapter must not be NULL and must exist for the duration of this
  //        session. No ownership is taken.
  // @param listener The listener that will be notified Anymote responses. The
  //        listener must not be NULL and must exist for the duration of this
  //        session. No ownership is taken.
  DeviceSession(wire::WireAdapter* adapter, AnymoteListener* listener);

  virtual ~DeviceSession() {}

  // Starts the session. This must be called before sending any messages on
  // this session.
  void StartSession();

  // Sends a "ping" message that should receive an ack.
  void SendPing();

  // Sends a key event.
  //
  // @param keycode The keycode of the event.
  // @param action The action of the event (up or down).
  void SendKeyEvent(messages::Code keycode, messages::Action action);

  // Sends a relative mouse movement.
  //
  // @param x_delta The relative movement along the x-axis (horizontal).
  // @param y_delta The relative movement along the y-axis (vertical).
  void SendMouseMove(int x_delta, int y_delta);

  // Sends a mouse wheel movement.
  //
  // @param x_scroll The scroll amount along the x-axis (horizontal).
  // @param y_scroll The scroll amount along the y-axis (vertical).
  void SendMouseWheel(int x_scroll, int y_scroll);

  // Sends generic data to the server.
  //
  // @param type The data type identifier.
  // @param data The data payload.
  void SendData(const std::string& type, const std::string& data);

  // Sends a connection message. This message should be sent after starting
  // the session, before sending any other messages.
  //
  // @param device_name The device name.
  // @param version The device version.
  void SendConnect(const std::string& device_name, int32_t version);

  // Sends a fling event.
  //
  // @param uri The uri to fling.
  // @param sequence_number The fling sequence number which must not be
  //        negative.
  void SendFling(std::string uri, int32_t sequence_number);

  // @override
  virtual void OnMessage(const messages::RemoteMessage& message);

  // @override
  virtual void OnError();

 private:
  // Sends a request without a sequence number.
  //
  // @param request The request to send.
  void SendRequest(const messages::RequestMessage& request);

  // Sends a request with a sequence number.
  //
  // @param request The request to send.
  // @param sequence_number The sequence number, or 0 if there is none. Must be
  //        non-negative.
  void SendRequestWithSequence(const messages::RequestMessage& request,
                               int32_t sequence_number);

  // The wire adapter used to send and receive Anymote messages. The adapter
  // must not be NULL and must exist for the duration of this session. No
  // ownership is taken.
  wire::WireAdapter* adapter_;

  // The listener that will be notified Anymote responses. The listener must not
  // be NULL and must exist for the duration of this session. No ownership is
  // taken.
  AnymoteListener* listener_;

  // Counter that is incremented and used as the sequence number for each ping
  // message sent.
  int32_t ping_counter_;

  // Disallow copy and assign.
  DeviceSession(const DeviceSession&);
  void operator=(const DeviceSession&);
};

}  // namespace device
}  // namespace anymote

#endif  // ANYMOTE_DEVICE_DEVICESESSION_H_
