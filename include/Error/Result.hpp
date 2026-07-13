#pragma once

#include <format>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include "Utilities/VariantVisitHelper.hpp"



namespace Spelt {
    class ResultError : public std::runtime_error {
    public:
        explicit ResultError(const std::string& message)
            : std::runtime_error(message) {}
    };

    template <typename V, typename E>
    class [[nodiscard]] Result {
    private:
        std::variant<V, E> mData;

        constexpr Result(std::in_place_index_t<0>, V value)
            noexcept(std::is_nothrow_move_constructible_v<V>)
            : mData(std::in_place_index<0>, std::move(value))
        {}

        constexpr Result(std::in_place_index_t<1>, E error)
            noexcept(std::is_nothrow_move_constructible_v<E>)
            : mData(std::in_place_index<1>, std::move(error))
        {}

    public:
        static constexpr Result<V, E> createValue(V value)
            noexcept(std::is_nothrow_move_constructible_v<V>)
        {
            return Result<V, E>(std::in_place_index<0>, std::move(value));
        }

        static constexpr Result<V, E> createError(E error)
            noexcept(std::is_nothrow_move_constructible_v<E>)
        {
            return Result<V, E>(std::in_place_index<1>, std::move(error));
        }

        [[nodiscard]] constexpr bool isValue() const noexcept {
            return std::holds_alternative<V>(mData);
        }

        [[nodiscard]] constexpr bool isError() const noexcept {
            return std::holds_alternative<E>(mData);
        }

        /**
         * Throws an exception if result is error
         */
        constexpr V& value() {
            if (auto* val = std::get_if<V>(&mData)) [[likely]] {
                return *val;
            }

            throw ResultError("Tried to get value on Result containing error");
        }

        /**
         * Throws an exception if result is error
         */
        constexpr const V& value() const {
            if (const auto* val = std::get_if<V>(&mData)) [[likely]] {
                return *val;
            }

            throw ResultError("Tried to get const value on Result containing error");
        }

        /**
         * Returns a copy of the value
         * If the result is an Error it returns a default value
         */
        constexpr V valueOr(V other) const
            noexcept(std::is_nothrow_copy_constructible_v<V> &&
                     std::is_nothrow_move_constructible_v<V>)
        {
            if (const auto* val = std::get_if<V>(&mData)) [[likely]] {
                return *val;
            }

            return other;
        }

        /**
         * Throws an excpetion if result is value
         */
        constexpr E& error() {
            if (auto* err = std::get_if<E>(&mData)) [[likely]] {
                return *err;
            }

            throw ResultError("Tried to get error on Result containing value");
        }

        /**
         * Throws an excpetion if result is value
         */
        constexpr const E& error() const {
            if (const auto* err = std::get_if<E>(&mData)) [[likely]] {
                return *err;
            }

            throw ResultError("Tried to get const error on Result containing value");
        }

        template <typename FuncV, typename FuncE>
        constexpr auto match(FuncV&& valFunc, FuncE&& errFunc)
            noexcept(std::is_nothrow_invocable_v<FuncV, V&> &&
                     std::is_nothrow_invocable_v<FuncE, E&>)
        {
            static_assert(std::is_invocable_v<FuncV, V&>,
                "Spelt::Result::match Error: The value lambda must accept 'V&' (or a compatible type).");

            static_assert(std::is_invocable_v<FuncE, E&>,
                "Spelt::Result::match Error: The error lambda must accept 'E&' (or a compatible type).");

            using ReturnTypeV = std::invoke_result_t<FuncV, V&>;
            using ReturnTypeE = std::invoke_result_t<FuncE, E&>;

            static_assert(std::is_same_v<ReturnTypeV, ReturnTypeE>,
                "Spelt::Result::match Error: Both match lambdas must return the exact same type!");

            return std::visit(Overloaded{
                std::forward<FuncV>(valFunc),
                std::forward<FuncE>(errFunc)
            }, mData);
        }

        template <typename FuncV, typename FuncE>
        constexpr auto match(FuncV&& valFunc, FuncE&& errFunc) const
            noexcept(std::is_nothrow_invocable_v<FuncV, const V&> &&
                     std::is_nothrow_invocable_v<FuncE, const E&>)
        {
            static_assert(std::is_invocable_v<FuncV, const V&>,
                "Spelt::Result::match Error: The value lambda must accept 'const V&' (or a compatible type).");

            static_assert(std::is_invocable_v<FuncE, const E&>,
                "Spelt::Result::match Error: The error lambda must accept 'const E&' (or a compatible type).");

            using ReturnTypeV = std::invoke_result_t<FuncV, const V&>;
            using ReturnTypeE = std::invoke_result_t<FuncE, const E&>;

            static_assert(std::is_same_v<ReturnTypeV, ReturnTypeE>,
                "Spelt::Result::match Error: Both match lambdas must return the exact same type!");

            return std::visit(Overloaded{
                std::forward<FuncV>(valFunc),
                std::forward<FuncE>(errFunc)
            }, mData);
        }

        constexpr void throwOnError() const {
            if (isError()) [[unlikely]] {
                throw std::runtime_error("An unexpected error occurred");
            }
        }

        void throwOnError(const std::string& str) const {
            if (isError()) [[unlikely]] {
                throw std::runtime_error(std::format("An unexpected error occurred: {}", str));
            }
        }
    };


    template <typename E>
    class [[nodiscard]] Result<void, E> {
    private:
        Result<std::monostate, E> mResult;

        constexpr explicit Result(Result<std::monostate, E> result)
            noexcept(std::is_nothrow_move_constructible_v<Result<std::monostate, E>>)
            : mResult(std::move(result))
        {}

    public:
        static constexpr Result<void, E> createValue()
            noexcept(noexcept(Result<std::monostate, E>::createValue(std::monostate{})))
        {
            return Result<void, E>(Result<std::monostate, E>::createValue(std::monostate{}));
        }

        static constexpr Result<void, E> createError(E error)
            noexcept(std::is_nothrow_move_constructible_v<E>)
        {
            return Result<void, E>(Result<std::monostate, E>::createError(std::move(error)));
        }

        [[nodiscard]] constexpr bool isValue() const noexcept {
            return mResult.isValue();
        }

        [[nodiscard]] constexpr bool isError() const noexcept {
            return mResult.isError();
        }

        constexpr void value() const {
            if (mResult.isValue()) [[likely]] {
                return;
            }
            throw ResultError("Tried to get value on Result containing error");
        }

        constexpr E& error() {
            return mResult.error();
        }

        constexpr const E& error() const {
            return mResult.error();
        }

        // Match method variations updated because the success lambda takes no arguments
        template <typename FuncV, typename FuncE>
        constexpr auto match(FuncV&& valFunc, FuncE&& errFunc)
            noexcept(std::is_nothrow_invocable_v<FuncV> &&
                     std::is_nothrow_invocable_v<FuncE, E&>)
        {
            return mResult.match(
                [&](std::monostate) { return std::forward<FuncV>(valFunc)(); },
                std::forward<FuncE>(errFunc));
        }

        template <typename FuncV, typename FuncE>
        constexpr auto match(FuncV&& valFunc, FuncE&& errFunc) const
            noexcept(std::is_nothrow_invocable_v<FuncV> &&
                     std::is_nothrow_invocable_v<FuncE, const E&>)
        {
            return mResult.match(
                [&](std::monostate) { return std::forward<FuncV>(valFunc)(); },
                std::forward<FuncE>(errFunc));
        }


        constexpr void throwOnError() const {
            mResult.throwOnError();
        }

        void throwOnError(const std::string& str) const {
            mResult.throwOnError(str);
        }
    };

}
