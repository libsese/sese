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

#pragma once

#ifndef SQLITE_CONNECTION_STRING
#define SQLITE_CONNECTION_STRING PATH_TO_DB
#endif

#ifndef MYSQL_CONNECTION_STRING
#define MYSQL_CONNECTION_STRING "host=127.0.0.1;port=18806;user=root;pwd=libsese;db=db_test;"
#endif

#ifndef PSQL_CONNECTION_STRING
#define PSQL_CONNECTION_STRING "host=127.0.0.1;port=18080;user=postgres;pwd=libsese;db=db_test;"
#endif