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

#include <sstream>
#include <cstring>

#include "ErrorInfo.hpp"

ErrorInfo::ErrorInfo (const char* file, size_t line, size_t i, size_t iter)
        : file (file), line (line), i (i), iter (iter) {
}

ErrorInfo::ErrorInfo (const ErrorInfo & other) : file (other.file), line (other.line), i (other.i), iter (other.iter) {
}

ErrorInfo::~ErrorInfo () {
}

ErrorInfo & ErrorInfo::operator= (const ErrorInfo & other) {
    this->~ErrorInfo();
    new (this) ErrorInfo(other);
    return *this;
}

const char*
ErrorInfo::what () {
    std::stringstream ss;
    ss << "[" << file << "@" << line << "]: i=" << i << " iter=" << iter;
    std::string s = ss.str();
    char* msg = new char[s.length() + 1];
    strncpy(msg, s.c_str(), s.length() + 1);
    return msg;
}
