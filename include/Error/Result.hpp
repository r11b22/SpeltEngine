#pragma once

#include <format>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include "Error/Panic.hpp"
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
        constexpr V& value(std::source_location loc = std::source_location::current()) {
            if (auto* val = std::get_if<V>(&mData)) [[likely]] {
                return *val;
            }

            fatalPanic("Tried to get value on Result containing error", loc);
        }

        /**
         * Throws an exception if result is error
         */
        constexpr const V& value(std::source_location loc = std::source_location::current()) const {
            if (const auto* val = std::get_if<V>(&mData)) [[likely]] {
                return *val;
            }

            fatalPanic("Tried to get const value on Result containing error", loc);
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
        constexpr E& error(std::source_location loc = std::source_location::current()) {
            if (auto* err = std::get_if<E>(&mData)) [[likely]] {
                return *err;
            }

            fatalPanic("Tried to get error on Result containing value", loc);
        }

        /**
         * Throws an excpetion if result is value
         */
        constexpr const E& error(std::source_location loc = std::source_location::current()) const {
            if (const auto* err = std::get_if<E>(&mData)) [[likely]] {
                return *err;
            }

            fatalPanic("Tried to get const error on Result containing value", loc);
        }

        /**
         * If this Result contains an error, replaces it with a new error value.
         * Otherwise, forwards the current value.
         */
        template <typename NewE>
        constexpr Result<V, NewE> replaceError(NewE newErr) &&
            noexcept(std::is_nothrow_move_constructible_v<V> && std::is_nothrow_move_constructible_v<NewE>)
        {
            if (isError()) {
                return Result<V, NewE>::createError(std::move(newErr));
            }
            return Result<V, NewE>::createValue(std::move(value()));
        }

        /**
         * If this Result contains an error, replaces it with a new error value.
         * Otherwise, forwards the current value.
         */
        template <typename NewE>
        constexpr Result<V, NewE> replaceError(NewE newErr) const &
            noexcept(std::is_nothrow_copy_constructible_v<V> && std::is_nothrow_move_constructible_v<NewE>)
        {
            if (isError()) {
                return Result<V, NewE>::createError(std::move(newErr));
            }
            return Result<V, NewE>::createValue(value());
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

        constexpr void panicOnError(std::source_location loc = std::source_location::current()) const {
            if (isError()) [[unlikely]] {
                fatalPanic("An unexpected error occurred", loc);
            }
        }

        void panicOnError(const std::string& str, std::source_location loc = std::source_location::current()) const {
            if (isError()) [[unlikely]] {
                fatalPanic(std::format("An unexpected error occurred: {}", str), loc);
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

        constexpr void value(std::source_location loc = std::source_location::current()) const {
            if (mResult.isValue()) [[likely]] {
                return;
            }
            fatalPanic("Tried to get value on Result containing error", loc);
        }

        constexpr E& error(std::source_location loc = std::source_location::current()) {
            return mResult.error(loc);
        }

        constexpr const E& error(std::source_location loc = std::source_location::current()) const {
            return mResult.error(loc);
        }

        /**
         * If this void Result contains an error, replaces it with a new error value.
         * Otherwise, forwards the success state.
         */
        template <typename NewE>
        constexpr Result<void, NewE> replaceError(NewE newErr) const &
            noexcept(std::is_nothrow_move_constructible_v<NewE>)
        {
            if (isError()) {
                return Result<void, NewE>::createError(std::move(newErr));
            }
            return Result<void, NewE>::createValue();
        }

        /**
         * If this void Result contains an error, replaces it with a new error value.
         * Otherwise, forwards the success state.
         */
        template <typename NewE>
        constexpr Result<void, NewE> replaceError(NewE newErr) &&
            noexcept(std::is_nothrow_move_constructible_v<NewE>)
        {
            if (isError()) {
                return Result<void, NewE>::createError(std::move(newErr));
            }
            return Result<void, NewE>::createValue();
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



        constexpr void throwOnError(std::source_location loc = std::source_location::current()) const {
            mResult.panicOnError(loc);
        }

        void throwOnError(const std::string& str, std::source_location loc = std::source_location::current()) const {
            mResult.panicOnError(str, loc);
        }
    };

}
