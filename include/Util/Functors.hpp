// Copyright (c) 2017 Till Kolditz
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

/*
 * File:   functors.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 24-05-2017 14:04
 */

#pragma once

#include <cstring>
#include <utility>

namespace coding_benchmark {

    typedef char * str_t;
    typedef const char * cstr_t;
    typedef char * const strc_t;

    struct functor {
    };

    struct arithmetic_functor :
            public functor {
    };

    struct logic_functor :
            public functor {
    };

    struct comparator_functor :
            public functor {
    };

    template<typename _Op = void>
    struct add :
            public arithmetic_functor {
        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp & __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) + _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) + _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) + _Op().operator()(std::forward<_Types>(args)...);
        }

        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp && __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) + _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) + _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) + _Op().operator()(std::forward<_Types>(args)...);
        }
    };

    template<>
    struct add<void> :
            public arithmetic_functor {
        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp & __t,
                _Up & __u) const noexcept(noexcept(std::forward<_Tp>(__t) + std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) + std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) + std::forward<_Up>(__u);
        }

        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp && __t,
                _Up && __u) const noexcept(noexcept(std::forward<_Tp>(__t) + std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) + std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) + std::forward<_Up>(__u);
        }
    };

    template<typename _Op = void>
    struct sub :
            public arithmetic_functor {
        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp & __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) - _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) - _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) - _Op().operator()(std::forward<_Types>(args)...);
        }

        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp && __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) - _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) - _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) - _Op().operator()(std::forward<_Types>(args)...);
        }
    };

    template<>
    struct sub<void> :
            public arithmetic_functor {
        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp & __t,
                _Up & __u) const noexcept(noexcept(std::forward<_Tp>(__t) - std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) - std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) - std::forward<_Up>(__u);
        }
        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp && __t,
                _Up && __u) const noexcept(noexcept(std::forward<_Tp>(__t) - std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) - std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) - std::forward<_Up>(__u);
        }
    };

    template<typename _Op = void>
    struct mul :
            public arithmetic_functor {
        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp & __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) * _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) * _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) * _Op().operator()(std::forward<_Types>(args)...);
        }

        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp && __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) * _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) * _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) * _Op().operator()(std::forward<_Types>(args)...);
        }
    };

    template<>
    struct mul<void> :
            public arithmetic_functor {
        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp & __t,
                _Up & __u) const noexcept(noexcept(std::forward<_Tp>(__t) * std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) * std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) * std::forward<_Up>(__u);
        }

        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp && __t,
                _Up && __u) const noexcept(noexcept(std::forward<_Tp>(__t) * std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) * std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) * std::forward<_Up>(__u);
        }
    };

    template<typename _Op = void>
    struct div :
            public arithmetic_functor {
        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp & __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) / _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) / _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) / _Op().operator()(std::forward<_Types>(args)...);
        }

        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp && __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) / _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) / _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) / _Op().operator()(std::forward<_Types>(args)...);
        }
    };

    template<>
    struct div<void> :
            public arithmetic_functor {
        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp & __t,
                _Up & __u) const noexcept(noexcept(std::forward<_Tp>(__t) / std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) / std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) / std::forward<_Up>(__u);
        }

        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp && __t,
                _Up && __u) const noexcept(noexcept(std::forward<_Tp>(__t) / std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) / std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) / std::forward<_Up>(__u);
        }
    };

    template<typename _Op = void>
    struct and_is :
            public logic_functor {
        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp & __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) & _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) & _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) & _Op().operator()(std::forward<_Types>(args)...);
        }

        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp && __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) & _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) & _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) & _Op().operator()(std::forward<_Types>(args)...);
        }
    };

    template<>
    struct and_is<void> :
            public logic_functor {
        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp & __t,
                _Up & __u) const noexcept(noexcept(std::forward<_Tp>(__t) & std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) & std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) & std::forward<_Up>(__u);
        }

        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp && __t,
                _Up && __u) const noexcept(noexcept(std::forward<_Tp>(__t) & std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) & std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) & std::forward<_Up>(__u);
        }
    };

    template<typename _Op = void>
    struct or_is :
            public logic_functor {
        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp & __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) | _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) | _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) | _Op().operator()(std::forward<_Types>(args)...);
        }

        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp && __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) | _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) | _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) | _Op().operator()(std::forward<_Types>(args)...);
        }
    };

    template<>
    struct or_is<void> :
            public logic_functor {
        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp & __t,
                _Up & __u) const noexcept(noexcept(std::forward<_Tp>(__t) | std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) | std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) | std::forward<_Up>(__u);
        }

        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp && __t,
                _Up && __u) const noexcept(noexcept(std::forward<_Tp>(__t) | std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) | std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) | std::forward<_Up>(__u);
        }
    };

    template<typename _Op = void>
    struct xor_is :
            public logic_functor {
        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp & __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) ^ _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) ^ _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) ^ _Op().operator()(std::forward<_Types>(args)...);
        }

        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp && __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) ^ _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) ^ _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) ^ _Op().operator()(std::forward<_Types>(args)...);
        }
    };

    template<>
    struct xor_is<void> :
            public logic_functor {
        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp & __t,
                _Up & __u) const noexcept(noexcept(std::forward<_Tp>(__t) ^ std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) ^ std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) ^ std::forward<_Up>(__u);
        }

        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp && __t,
                _Up && __u) const noexcept(noexcept(std::forward<_Tp>(__t) | std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) | std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) | std::forward<_Up>(__u);
        }
    };

    template<typename _Op = void>
    struct is_not :
            public logic_functor {
        template<typename ... _Types>
        constexpr
        auto operator()(
                _Types && ... args) const noexcept(noexcept(! _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(! _Op().operator()(std::forward<_Types>(args)...))
                {
            return !_Op().operator()(std::forward<_Types>(args)...);
        }
    };

    template<>
    struct is_not<void> :
            public logic_functor {
        template<typename _Up>
        constexpr
        auto operator()(
                _Up & __u) const noexcept(noexcept(! std::forward<_Up>(__u)))
                -> decltype(! std::forward<_Up>(__u))
                {
            return !std::forward<_Up>(__u);
        }

        template<typename _Up>
        constexpr
        auto operator()(
                _Up && __u) const noexcept(noexcept(! std::forward<_Up>(__u)))
                -> decltype(! std::forward<_Up>(__u))
                {
            return !std::forward<_Up>(__u);
        }
    };

    template<typename _Op = void>
    struct eq :
            public comparator_functor {
        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp & __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) == _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) == _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) == _Op().operator()(std::forward<_Types>(args)...);
        }

        template<typename _Tp, typename ... _Types>
        constexpr
        auto operator()(
                _Tp && __t,
                _Types && ... args) const noexcept(noexcept(std::forward<_Tp>(__t) == _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<_Tp>(__t) == _Op().operator()(std::forward<_Types>(args)...))
                {
            return std::forward<_Tp>(__t) == _Op().operator()(std::forward<_Types>(args)...);
        }

        template<typename ... _Types>
        constexpr
        auto operator()(
                str_t & __t,
                _Types && ... args) const noexcept(noexcept(std::forward<str_t>(__t) == _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<str_t>(__t) == _Op().operator()(std::forward<_Types>(args)...))
                {
            return eq()(std::strcmp(std::forward<str_t>(__t), _Op().operator()(std::forward<_Types>(args)...)), 0);
        }

        template<typename ... _Types>
        constexpr
        auto operator()(
                str_t && __t,
                _Types && ... args) const noexcept(noexcept(std::forward<str_t>(__t) == _Op().operator()(std::forward<_Types>(args)...)))
                -> decltype(std::forward<str_t>(__t) == _Op().operator()(std::forward<_Types>(args)...))
                {
            return eq()(std::strcmp(std::forward<str_t>(__t), _Op().operator()(std::forward<_Types>(args)...)), 0);
        }

        template<typename ... _Types>
        constexpr
        auto operator()(
                cstr_t & __t,
                _Types && ... args) const noexcept(noexcept(eq()(std::strcmp(std::forward<cstr_t>(__t), _Op().operator()(std::forward<_Types>(args)...)), 0)))
                -> decltype(eq()(std::strcmp(std::forward<cstr_t>(__t), _Op().operator()(std::forward<_Types>(args)...)), 0))
                {
            return eq()(std::strcmp(std::forward<cstr_t>(__t), _Op().operator()(std::forward<_Types>(args)...)), 0);
        }

        template<typename ... _Types>
        constexpr
        auto operator()(
                cstr_t && __t,
                _Types && ... args) const noexcept(noexcept(eq()(std::strcmp(std::forward<cstr_t>(__t), _Op().operator()(std::forward<_Types>(args)...)), 0)))
                -> decltype(eq()(std::strcmp(std::forward<cstr_t>(__t), _Op().operator()(std::forward<_Types>(args)...)), 0))
                {
            return eq()(std::strcmp(std::forward<cstr_t>(__t), _Op().operator()(std::forward<_Types>(args)...)), 0);
        }
    };

    template<>
    struct eq<void> :
            public comparator_functor {
        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp & __t,
                _Up & __u) const noexcept(noexcept(std::forward<_Tp>(__t) == std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) == std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) == std::forward<_Up>(__u);
        }

        template<typename _Tp, typename _Up>
        constexpr
        auto operator()(
                _Tp && __t,
                _Up && __u) const noexcept(noexcept(std::forward<_Tp>(__t) == std::forward<_Up>(__u)))
                -> decltype(std::forward<_Tp>(__t) == std::forward<_Up>(__u))
                {
            return std::forward<_Tp>(__t) == std::forward<_Up>(__u);
        }

        constexpr
        auto operator()(
                str_t & __t,
                str_t & __u) const noexcept(noexcept(std::strcmp(std::forward<str_t>(__t), std::forward<str_t>(__u)) == 0))
                -> decltype(std::strcmp(std::forward<str_t>(__t), std::forward<str_t>(__u)) == 0)
                {
            return std::strcmp(std::forward<str_t>(__t), std::forward<str_t>(__u)) == 0;
        }

        constexpr
        auto operator()(
                str_t && __t,
                str_t && __u) const noexcept(noexcept(std::strcmp(std::forward<str_t>(__t), std::forward<str_t>(__u)) == 0))
                -> decltype(std::strcmp(std::forward<str_t>(__t), std::forward<str_t>(__u)) == 0)
                {
            return std::strcmp(std::forward<str_t>(__t), std::forward<str_t>(__u)) == 0;
        }

        constexpr
        auto operator()(
                cstr_t & __t,
                str_t & __u) const noexcept(noexcept(std::strcmp(std::forward<cstr_t>(__t), std::forward<str_t>(__u)) == 0))
                -> decltype(std::strcmp(std::forward<cstr_t>(__t), std::forward<str_t>(__u)) == 0)
                {
            return std::strcmp(std::forward<cstr_t>(__t), std::forward<str_t>(__u)) == 0;
        }

        constexpr
        auto operator()(
                cstr_t && __t,
                str_t && __u) const noexcept(noexcept(std::strcmp(std::forward<cstr_t>(__t), std::forward<str_t>(__u)) == 0))
                -> decltype(std::strcmp(std::forward<cstr_t>(__t), std::forward<str_t>(__u)) == 0)
                {
            return std::strcmp(std::forward<cstr_t>(__t), std::forward<str_t>(__u)) == 0;
        }

        constexpr
        auto operator()(
                str_t & __t,
                cstr_t & __u) const noexcept(noexcept(std::strcmp(std::forward<str_t>(__t), std::forward<cstr_t>(__u)) == 0))
                -> decltype(std::strcmp(std::forward<str_t>(__t), std::forward<cstr_t>(__u)) == 0)
                {
            return std::strcmp(std::forward<str_t>(__t), std::forward<cstr_t>(__u)) == 0;
        }

        constexpr
        auto operator()(
                str_t && __t,
                cstr_t && __u) const noexcept(noexcept(std::strcmp(std::forward<str_t>(__t), std::forward<cstr_t>(__u)) == 0))
                -> decltype(std::strcmp(std::forward<str_t>(__t), std::forward<cstr_t>(__u)) == 0)
                {
            return std::strcmp(std::forward<str_t>(__t), std::forward<cstr_t>(__u)) == 0;
        }

        constexpr
        auto operator()(
                cstr_t & __t,
                cstr_t & __u) const noexcept(noexcept(std::strcmp(std::forward<cstr_t>(__t), std::forward<cstr_t>(__u)) == 0))
                -> decltype(std::strcmp(std::forward<cstr_t>(__t), std::forward<cstr_t>(__u)) == 0)
                {
            return std::strcmp(std::forward<cstr_t>(__t), std::forward<cstr_t>(__u)) == 0;
        }

        constexpr
        auto operator()(
                cstr_t && __t,
                cstr_t && __u) const noexcept(noexcept(std::strcmp(std::forward<cstr_t>(__t), std::forward<cstr_t>(__u)) == 0))
                -> decltype(std::strcmp(std::forward<cstr_t>(__t), std::forward<cstr_t>(__u)) == 0)
                {
            return std::strcmp(std::forward<cstr_t>(__t), std::forward<cstr_t>(__u)) == 0;
        }
    };

}
