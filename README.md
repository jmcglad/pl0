# PL/0

This is a just-for-fun hobby implementation of the [PL/0](https://en.wikipedia.org/wiki/PL/0) programming language. This implementation can parse and evaluate PL/0 source files. It is not a full compiler implementation.

## Build

This project uses [Conan](https://conan.io/) and [CMake](https://cmake.org/). Run the following to perform a release build:

```bash
conan install --build=missing
cmake --preset conan-default
cmake --build --preset conan-release
```

## Run

Once built, you can run the executable directly from the build directory like so:

```bash
./build/src/Release/pl0
```

## Grammar

```ebnf
program = block ".";

block = [ "const" identifier "=" number { "," identifier "=" number } ";" ]
        [ "var" identifier { "," identifier } ";" ]
        { "procedure" identifier ";" block ";" } statement ;

statement = [ identifier ":=" expression
            | "call" expression
            | "?" identifier
            | "!" expression
            | "begin" statement { ";" statement } "end"
            | "if" condition "then" statement
            | "while" condition "do" statement ] ;

condition = "odd" expression
          | expression ( "=" | "#" | "<" | "<=" | ">" | ">=" ) expression ;

expression = [ "+" | "-" ] term { ( "+" | "-" ) term } ;

term = factor { ( "*" | "/" ) factor } ;

factor = identifier | number | "(" expression ")" ;

number = ? [0-9]+ ? ;

identifier = ? [A-Za-z_][A-Za-z0-9_]* ? ;
```

> [!NOTE]
> This implementation treats keywords as case-insensitive and identifiers as case-sensitive.
