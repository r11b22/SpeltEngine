#pragma once


#include <format>
#include <stdexcept>
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
    public:
        static Result<V, E> createValue(V value){
            return Result<V, E>(std::move(value));
        }

        static Result<V, E> createError(E error){
            return Result<V, E>(std::move(error));
        }

        bool isValue() const{
            return std::holds_alternative<V>(mData);
        }

        bool isError() const{
            return std::holds_alternative<E>(mData);
        }

        /**
         * Throws an exception if result is error
         */
        V& value() {
            if (auto* val = std::get_if<V>(&mData)) {
                return *val;
            }

            throw ResultError("Tried to get value on Result containing error");
        }

        /**
         * Throws an exception if result is error
         */
        const V& value() const{
            if (const auto* val = std::get_if<V>(&mData)) {
                return *val;
            }

            throw ResultError("Tried to get const value on Result containing error");
        }

        /**
         * Returns a copy of the value
         * If the result is an Error it returns a default value
         */
        V valueOr(V other) const{
            if (const auto* val = std::get_if<V>(&mData)) {
                return *val;
            }

            return other;
        }

        /**
         * Throws an excpetion if result is value
         */
        E& error() {
            if (auto* err = std::get_if<E>(&mData)) {
                return *err;
            }

            throw ResultError("Tried to get error on Result containing value");
        }

        /**
         * Throws an excpetion if result is value
         */
        const E& error() const {
            if (const auto* err = std::get_if<E>(&mData)) {
                return *err;
            }

            throw ResultError("Tried to get const error on Result containing value");
        }

        template <typename FuncV, typename FuncE>
        auto match(FuncV&& valFunc, FuncE&& errFunc) {
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
        auto match(FuncV&& valFunc, FuncE&& errFunc) const {
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

        void throwOnError() const{
            if (isError()){
                throw std::runtime_error("An unexpected error occurred");
            }
        }

        void throwOnError(const std::string& str) const{
            if (isError()){
                throw std::runtime_error(std::format("An unexpected error occurred: {}", str));
            }
        }

    private:
        Result(std::variant<V, E> data)
            : mData(std::move(data))
        {}
    };


    template <typename E>
    class [[nodiscard]] Result<void, E> {
    private:
        // We use std::monostate as a dummy placeholder type for "success with no data"
        Result<std::monostate, E> mResult;

        Result(Result<std::monostate, E> result) : mResult(std::move(result)) {}

    public:
        static Result<void, E> createValue() {
            return Result<void, E>(Result<std::monostate, E>::createValue(std::monostate{}));
        }

        static Result<void, E> createError(E error) {
            return Result<void, E>(Result<std::monostate, E>::createError(error));
        }

        bool isValue() const {
            return mResult.isValue();
        }

        bool isError() const {
            return mResult.isError();
        }

        // value() returns void now, and just acts as an assertion/throw check
        void value() const {
            if (mResult.isValue()) {
                return;
            }
            throw ResultError("Tried to get value on Result containing error");
        }

        E& error() {
            return mResult.error();
        }

        const E& error() const {
            return mResult.error();
        }

        // Match method variations updated because the success lambda takes no arguments
        template <typename FuncV, typename FuncE>
        auto match(FuncV&& valFunc, FuncE&& errFunc) {
            return mResult.match([&](std::monostate) { return std::forward<FuncV>(valFunc)(); }, std::forward<FuncE>(errFunc));
        }

        template <typename FuncV, typename FuncE>
        auto match(FuncV&& valFunc, FuncE&& errFunc) const {
            return mResult.match([&](std::monostate) { return std::forward<FuncV>(valFunc)(); }, std::forward<FuncE>(errFunc));
        }


        void throwOnError() const{
            mResult.throwOnError();
        }

        void throwOnError(const std::string& str) const{
            mResult.throwOnError(str);
        }
    };

}
