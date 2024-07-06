#pragma once
#include <memory>
#include <optional>
#include <variant>

using u8   = char;
using i32  = int;
using u32  = unsigned int;
using f32  = float;
using f64  = double;

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename... Args>
constexpr Ref<T> MakeRef(Args&&... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename... Args>
constexpr Scope<T> MakeScope(Args&&... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Option = std::optional<T>;

template<typename T1, typename T2>
using Variant = std::variant<T1, T2>;

#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << __FILE__ << ":" << __LINE__ << " " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) ((void)0)
#endif
