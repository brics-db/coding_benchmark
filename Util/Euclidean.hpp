// Copyright (c) 2016 Till Kolditz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

/* 
 * File:   Euclidean.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 6. Dezember 2016, 00:55
 */

#ifndef EUCLIDEAN_HPP
#define EUCLIDEAN_HPP

#include <vector>

template<typename T>
T
ext_euclidean(T b0, size_t codewidth)
{
	T a0(1);
	a0 <<= codewidth;
	std::vector<T> a, b, q, r, s, t;
	a.push_back(a0), b.push_back(b0), s.push_back(T(0)), t.push_back(T(0));
	size_t i = 0;
	do
	{
		q.push_back(a[i] / b[i]);
		r.push_back(a[i] % b[i]);
		a.push_back(b[i]);
		b.push_back(r[i]);
		s.push_back(0);
		t.push_back(0);
	}
	while (b[++i] > 0);
	s[i] = 1;
	t[i] = 0;

	for (size_t j = i; j > 0; --j)
	{
		s[j - 1] = t[j];
		t[j - 1] = s[j] - q[j - 1] * t[j];
	}

	T result = ((b0 * t.front()) % a0);
	result += result < 0 ? a0 : 0;
	if (result == 1)
	{
		return t.front();
	}
	else
	{
		return 0;
	}
}

#endif /* EUCLIDEAN_HPP */
