#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>


namespace Spelt {
    class OptionError : public std::runtime_error {
    public:
        explicit OptionError(const std::string& message)
            : std::runtime_error(message) {}
    };

    template <typename T>
    class [[nodiscard]] Option {
    private:
        std::optional<T> mData;

        constexpr explicit Option(std::optional<T> data)
            noexcept(std::is_nothrow_move_constructible_v<T>)
            : mData(std::move(data))
        {}

    public:
        static constexpr Option<T> createValue(T value)
            noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            return Option<T>(std::optional<T>(std::move(value)));
        }

        static constexpr Option<T> createNone() noexcept {
            return Option<T>(std::nullopt);
        }

        [[nodiscard]] constexpr bool isValue() const noexcept {
            return mData.has_value();
        }

        [[nodiscard]] constexpr bool isNone() const noexcept {
            return !mData.has_value();
        }

        /**
         * Throws an exception if the option is empty
         */
        constexpr T& value() {
            if (mData.has_value()) [[likely]] {
                return *mData;
            }

            throw OptionError("Tried to get value on an empty Option");
        }

        /**
         * Throws an exception if the option is empty
         */
        constexpr const T& value() const {
            if (mData.has_value()) [[likely]] {
                return *mData;
            }

            throw OptionError("Tried to get const value on an empty Option");
        }

        /**
         * Returns a copy of the value, or `other` if the option is empty
         */
        constexpr T valueOr(T other) const
            noexcept(std::is_nothrow_copy_constructible_v<T> &&
                     std::is_nothrow_move_constructible_v<T>)
        {
            if (mData.has_value()) [[likely]] {
                return *mData;
            }

            return other;
        }

        template <typename FuncV, typename FuncN>
        constexpr auto match(FuncV&& valFunc, FuncN&& noneFunc)
            noexcept(std::is_nothrow_invocable_v<FuncV, T&> &&
                     std::is_nothrow_invocable_v<FuncN>)
        {
            static_assert(std::is_invocable_v<FuncV, T&>,
                "Spelt::Option::match Error: The value lambda must accept 'T&' (or a compatible type).");

            static_assert(std::is_invocable_v<FuncN>,
                "Spelt::Option::match Error: The none lambda must accept no arguments.");

            using ReturnTypeV = std::invoke_result_t<FuncV, T&>;
            using ReturnTypeN = std::invoke_result_t<FuncN>;

            static_assert(std::is_same_v<ReturnTypeV, ReturnTypeN>,
                "Spelt::Option::match Error: Both match lambdas must return the exact same type!");

            if (mData.has_value()) [[likely]] {
                return std::forward<FuncV>(valFunc)(*mData);
            }
            return std::forward<FuncN>(noneFunc)();
        }

        template <typename FuncV, typename FuncN>
        constexpr auto match(FuncV&& valFunc, FuncN&& noneFunc) const
            noexcept(std::is_nothrow_invocable_v<FuncV, const T&> &&
                     std::is_nothrow_invocable_v<FuncN>)
        {
            static_assert(std::is_invocable_v<FuncV, const T&>,
                "Spelt::Option::match Error: The value lambda must accept 'const T&' (or a compatible type).");

            static_assert(std::is_invocable_v<FuncN>,
                "Spelt::Option::match Error: The none lambda must accept no arguments.");

            using ReturnTypeV = std::invoke_result_t<FuncV, const T&>;
            using ReturnTypeN = std::invoke_result_t<FuncN>;

            static_assert(std::is_same_v<ReturnTypeV, ReturnTypeN>,
                "Spelt::Option::match Error: Both match lambdas must return the exact same type!");

            if (mData.has_value()) [[likely]] {
                return std::forward<FuncV>(valFunc)(*mData);
            }
            return std::forward<FuncN>(noneFunc)();
        }

        constexpr void throwOnNone() const {
            if (isNone()) [[unlikely]] {
                throw std::runtime_error("Expected a value but Option was empty");
            }
        }
    };

}
