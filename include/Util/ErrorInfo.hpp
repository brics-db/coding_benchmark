// Copyright 2016 Till Kolditz, Stefan de Bruijn
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <exception>
#include <cstddef>
#include <optional>

class ErrorInfo : public std::exception {

public:
    std::string file;
    size_t line;
    size_t i;
    size_t iter;
    std::optional<std::string> message;

    ErrorInfo(
            const char* file,
            size_t line,
            size_t i,
            size_t iter);

    ErrorInfo(
            const char* file,
            size_t line,
            size_t i,
            size_t iter,
            const char* message);

    ErrorInfo(
            const ErrorInfo & other);

    virtual ~ErrorInfo();

    ErrorInfo & operator=(
            const ErrorInfo & other);

    const char* what();
};

