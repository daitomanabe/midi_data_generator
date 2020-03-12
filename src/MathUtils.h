//
//  RandomUtils.h
//  midi_data_generator
//
//  Created by 2bit on 2020/03/09.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef RandomUtils_h
#define RandomUtils_h

#include <initializer_list>
#include <type_traits>
#include <algorithm>
#include <cmath>
#include <random>

namespace math {
    namespace detail {
        template <typename ... types>
        using common_type_t = typename std::common_type<types ...>::type;
        
        template <typename type, typename tag>
        struct inline_variable {
            template <typename ... types>
            static type &get(types ... args) {
                static type _{args ...};
                return _;
            };
        };
    };
    
    template <typename type1, typename type2>
    inline auto min(type1 a, type2 b)
        -> math::detail::common_type_t<type1, type2>
    { return a < b ? a : b; };

    template <typename ... types>
    inline auto min(types ... vs)
        -> math::detail::common_type_t<types ...>
    {
        using result_t = math::detail::common_type_t<types ...>;
        return std::min<result_t>(std::initializer_list<result_t>({vs ...}));
    };
    
    template <typename type1, typename type2>
    inline auto max(type1 a, type2 b)
        -> math::detail::common_type_t<type1, type2>
    { return a < b ? b : a; };

    template <typename ... types>
    inline auto max(types ... vs)
        -> math::detail::common_type_t<types ...>
    {
        using result_t = math::detail::common_type_t<types ...>;
        return std::max<result_t>(std::initializer_list<result_t>({vs ...}));
    };

    template <typename type1, typename type2, typename type3>
    inline auto clamp(type1 i, type2 min_v, type3 max_v)
        -> math::detail::common_type_t<type1, type2, type3>
    { return std::min(max_v, std::max(min_v, i)); };
    
    template <typename type1, typename type2, typename type3>
    inline auto map(type1 i, type2 out_min, type3 out_max, bool need_clamp = false)
        -> math::detail::common_type_t<type1, type2, type3>
    { return i * (out_max - out_min) + out_min; };
    
    template <typename type1, typename type2, typename type3, typename type4, typename type5>
    inline auto map(type1 i, type2 in_min, type3 in_max, type4 out_min, type5 out_max, bool need_clamp = false)
        -> math::detail::common_type_t<type1, type2, type3, type4, type5>
    { return (out_max - out_min) * (i - in_min) / (double)(in_max - in_min) + out_min; };
    
    template <typename type1, typename type2, typename type3>
    inline auto lerp(type1 from, type2 to, type3 amount)
        -> math::detail::common_type_t<type1, type2, type3>
    { return from * (1.0 - amount) + to * amount; };
    
    template <typename type1, typename type2, typename type3, typename type4>
    inline auto distance_squared(type1 x1, type2 y1, type3 x2, type4 y2)
        -> math::detail::common_type_t<type1, type2, type3, type4>
    { return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2); };
    
    template <typename type1, typename type2, typename type3, typename type4>
    inline auto distance(type1 x1, type2 y1, type3 x2, type4 y2)
        -> math::detail::common_type_t<type1, type2, type3, type4>
    { return std::sqrt(distance_squared(x1, y1, x2, y2)); };
    
    namespace {
        std::random_device &engine_device{detail::inline_variable<std::random_device, struct engine_device_tag>::get()};
        inline std::uint_fast64_t device() {
            return engine_device();
        }

        std::minstd_rand &engine_linear{detail::inline_variable<std::minstd_rand, struct engine_linear_tag>::get()};
        std::minstd_rand &engine_default{engine_linear};

        template <typename Engine>
        inline void set_seed(Engine &engine, typename Engine::result_type seed) {
            engine.seed(seed);
        }

        inline void set_seed_fast(std::uint_fast32_t seed = device()) {
            set_seed(engine_linear, seed);
        }

        inline std::uint_fast32_t fast() {
            return engine_linear();
        }
        
        std::mt19937_64 &engine_mt19937{detail::inline_variable<std::mt19937_64, struct engine_mt19937_tag>::get()};

        inline void set_seed_mt(std::uint_fast32_t seed = device()) {
            set_seed(engine_mt19937, seed);
        }

        inline std::uint_fast32_t mt() {
            return engine_mt19937();
        }

        std::mt19937_64 &engine_mt19937_64{detail::inline_variable<std::mt19937_64, struct engine_mt19937_64_tag>::get()};

        inline void set_seed_mt64(std::uint_fast32_t seed = device()) {
            set_seed(engine_mt19937, seed);
        }

        inline std::uint_fast64_t mt64() {
            return engine_mt19937_64();
        }

#pragma mark floating point random function

        std::uniform_real_distribution<double> &default_dist{detail::inline_variable<std::uniform_real_distribution<double>, struct default_dist_tag>::get(0.0, 1.0)};;

        template <typename Engine>
        inline double random(Engine &engine) {
            return default_dist(engine);
        }
        inline double random() {
            return random(engine_default);
        }

        template <typename Engine>
        inline double random(double max, Engine &engine) {
            return std::uniform_real_distribution<double>(0.0, max)(engine);
        }

        inline double random(double max) {
            return random(max, engine_default);
        }

        template <typename Engine>
        inline double random(double min, double max, Engine &engine) {
            return std::uniform_real_distribution<double>(min, max)(engine);
        }

        inline double random(double min, double max) {
            return random(min, max, engine_default);
        }
    

    
    } // anonymouse namespace for random
    

    #pragma mark vector random function
    template <typename Container>
    inline void shuffle(Container &v) {
        std::shuffle(v.begin(), v.end(), engine_mt19937);
    }

    template <typename T=int>
    inline const T rnd_list(const std::vector<T> mylist) {
      return static_cast<T>(mylist[rand()%mylist.size()]);
    }

    template <typename T=int>
    T cycle_step(std::vector<T> v, int step) {
      return v.at(step%v.size());
    }

    std::vector<int> rotR(std::vector<int>& notes, std::vector<int> scale) {
      std::transform(notes.begin(),notes.end(),notes.begin(),[&scale](int note){
        note += 1;
        note %= scale.size();
        
        return note;
      });
      
      return notes;
    }

    std::vector<int> rotL(std::vector<int>& notes, std::vector<int> scale) {
      transform(notes.begin(),notes.end(),notes.begin(),[&scale](int note){
        note -= 1;
        note %= static_cast<int>(scale.size());
        
        if (note < 0)
          note = static_cast<int>(scale.size()-1);
      
        return note;
      });

      return notes;
    }

}

#endif /* RandomUtils_h */
