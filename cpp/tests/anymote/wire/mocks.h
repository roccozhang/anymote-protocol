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

// Mocks for the Anymote wire classes.

#ifndef TV_GTVREMOTE_TESTS_ANYMOTE_WIRE_MOCKS_H_
#define TV_GTVREMOTE_TESTS_ANYMOTE_WIRE_MOCKS_H_

#include <anymote/wire/wireinterface.h>
#include <gmock/gmock.h>
#include <vector>

namespace anymote {
namespace wire {

// Mock wire interface.
class MockWireInterface : public wire::WireInterface {
 public:
  MOCK_METHOD1(Send, void(const std::vector<uint8_t>& data));
  MOCK_METHOD1(Receive, void(size_t num_bytes));
};

// Mock wire listener.
class MockWireListener : public wire::WireListener {
 public:
  MOCK_METHOD1(OnBytesReceived, void(const std::vector<uint8_t>& data));
  MOCK_METHOD0(OnError, void());
};

}  // namespace wire
}  // namespace anymote

#endif  // TV_GTVREMOTE_TESTS_ANYMOTE_WIRE_MOCKS_H_
