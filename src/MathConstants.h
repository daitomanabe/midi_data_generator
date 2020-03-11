#include <type_traits>

#pragma once

#ifndef MATH_CONSTANTS
#   define MATH_CONSTANTS

#ifdef PI
#   undef PI
#endif

static constexpr double PI{3.14159265358979323846};

#ifdef TWO_PI
#   undef TWO_PI
#endif

static constexpr double TWO_PI{6.28318530717958647693};

#ifdef M_TWO_PI
#   undef M_TWO_PI
#endif

static constexpr double M_TWO_PI{6.28318530717958647693};

#ifdef FOUR_PI
#   undef FOUR_PI
#endif

static constexpr double FOUR_PI{12.56637061435917295385};

#ifdef HALF_PI
#   undef HALF_PI
#endif

static constexpr double HALF_PI{1.57079632679489661923};

#ifdef DEG_TO_RAD
#   undef DEG_TO_RAD
#endif

static constexpr double DEG_TO_RAD{PI / 180.0};

#ifndef RAD_TO_DEG
#   undef RAD_TO_DEG
#endif

static constexpr double RAD_TO_DEG{180.0 / PI};

#ifdef MIN
#   undef MIN
#endif

template <typename t1, typename t2>
static inline constexpr typename std::common_type<t1, t2>::type MIN(t1 x, t2 y) {
    return (x < y) ? x : y;
}

#ifdef MAX
#   undef MAX
#endif

template <typename t1, typename t2>
static inline constexpr typename std::common_type<t1, t2>::type MAX(t1 x, t2 y) {
    return (x < y) ? y : x;
}

#ifdef CLAMP
#   undef CLAMP
#endif

template <typename t1, typename t2, typename t3>
static inline constexpr typename std::common_type<t1, t2, t3>::type CLAMP(t1 val, t2 min_v, t3 max_v) {
    return MIN(max_v, MAX(min_v, val));
}

#ifdef ABS
#   undef ABS
#endif

template <typename type>
static inline constexpr type ABS(type x) {
    return (x < 0) ? -x : x;
}

#endif
