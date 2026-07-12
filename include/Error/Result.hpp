#pragma once


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
    class Result {
    private:
        std::variant<V, E> mData;
    public:
        static Result<V, E> createValue(V value){
            return Result<V, E>(value);
        }

        static Result<V, E> createError(E error){
            return Result<V, E>(error);
        }

        bool isValue(){
            return std::holds_alternative<V>(mData);
        }

        bool isError(){
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

    private:
        Result(std::variant<V, E> data)
            : mData(data)
        {}
    };
}
