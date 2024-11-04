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

#include <sese/thread/GlobalThreadPool.h>

DWORD WINAPI sese::GlobalThreadPool::taskRunner1(LPVOID lp_param) {
    auto task1 = static_cast<Task1 *>(lp_param);
    task1->function();
    delete task1;
    return 0;
}

void sese::GlobalThreadPool::postTask(const std::function<void()> &func) {
    auto task1 = new Task1;
    task1->function = func;
    QueueUserWorkItem(taskRunner1, task1, WT_EXECUTEDEFAULT);
}
