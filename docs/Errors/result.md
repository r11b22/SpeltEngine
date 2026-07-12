# Result

This is the standard Error type used in Spelt. Just like in rust it holds either a value or an error value.

## Creating the result

Result has 2 named constructors
* `Result<V, E> Result<V, E>::createValue(V value)`
* `Result<V, E> Result<V, E>::createError(E error)`

Like the names suggest they create either a Result with a value or a Result with an error.

## Getting the data

To access a value contained inside a Result you can use the `value()` function.

To access an error contained inside a Result you can use the `error()` function.

Both these functions will throw an exception if the type that was requested is not the type that is contained in the Result. It is not recommended to use this as a way to check what type is contained in Result.

## Checking the type

Accessing a value or error inside of a Result is not safe without knowing what type is actually contained within. To check before calling either `V value()` or `E error()` use:
* `bool isValue()`
* `bool isError()`

Like their name suggests they either check if a value or an error is contained within the Result.


## Matching

If both cases could occurr and have different ways of handeling using the match function can greatly reduce the size of error handeling code. Match takes in 2 lambdas. One lambda takes in the Value type, the other the Error type. 

```cpp
result.match(
        [](int val) { /* Handle value case */},
        [&reached](std::string err) { /* Handle error case */ }
    );
```

`result.match()` Can actually return a value. The important thing to keep in mind here is that each lambda must return the same type for this to compile.

```cpp
float value = result.match(
        [](int val) { return static_cast<float>(val); },
        [&reached](std::string err) { return 0.0f; }
    );
```
