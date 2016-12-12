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

#include <cstdint>

#include "../Test.hpp"

template<typename DATA, typename CS>
CS computeFinalChecksum(DATA & checksum);

template<>
uint8_t computeFinalChecksum<uint16_t, uint8_t>(uint16_t & checksum)
{
	auto pChk = reinterpret_cast<uint8_t*> (&checksum);
	return pChk[0] ^ pChk[1];
}

template<>
uint16_t computeFinalChecksum<uint16_t, uint16_t>(uint16_t & checksum)
{
	return checksum;
}

template<>
uint8_t computeFinalChecksum<uint32_t, uint8_t>(uint32_t & checksum)
{
	auto pChk = reinterpret_cast<uint8_t*> (&checksum);
	return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3];
}

template<>
uint32_t computeFinalChecksum<uint32_t, uint32_t>(uint32_t & checksum)
{
	return checksum;
}

// base implementation for all native data types
template<typename T>
bool checksumsDiffer(T checksum1, T checksum2)
{
	return checksum1 != checksum2;
}
