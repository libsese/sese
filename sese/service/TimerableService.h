// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// \file TimerableService.h
/// \author kaoru
/// \date September 16, 2023
/// \brief Timerable Service
/// \version 0.1.0

#pragma once

#include <sese/service/TimerableService_V1.h>
#include <sese/service/TimerableService_V2.h>

namespace sese::service {
/// Timeout event struct
typedef v2::TimeoutEvent TimeoutEvent;

/// Timerable Service
typedef v2::TimerableService TimerableService;
} // namespace sese::service