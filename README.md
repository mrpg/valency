Valency
=======

Valency is a imperative programming language following some functional rules. Valency is also the name of its reference implementation.


Programming language
--------------------

* Valency is line-based. One line contains _one_ function call.
* Everything consists of one of these variable types:
 * a numeric type for integers (at least 64 bits of precision)
 * a numeric type for floating point operations (at least double precision)
 * a null type (not otherwise specified)
 * dynamic strings
 * dynamic lists
 * function
* Key features are implemented as functions in the normal variable space so that they can be re-defined by the programmer.
* There are no operators, only functions.
* Functions can be given a copy or, if explicitly specified, a reference to a function argument.
* There is nothing like a _return_ statement, results are normally stored in the last function argument (which is then passed as a reference).

### Standard-defined functions

_Optional arguments are enclosed by [ and ]_.

> set &ref value

The set function call instructs the interpreter to copy _value_'s value to the reference pointed to by _&ref_.

> print a [...]

Writes all given values and the newline character to the standard output.

> write a [...]

Like _print_, but without the newline character.

> mul a b [...] &result

> **Alias**: * a b [...] &result

The mul function call will multiply all the given values and write the result to _&result_. It is possible to pass more than 2+1 arguments. In this case, all given values will be multiplied.

> add a b [...] &result

> **Alias**: + a b [...] &result

The mul function call will add up all the given values and write the result to _&result_. It is possible to pass more than 2+1 arguments. In this case, all given values will be added up.

If all arguments except _&result_ are of the type string, the strings are concatenated.

> sub a b [...] &result

> **Alias**: - a b [...] &result

Like add, but with subtraction (and without string concatenation).

> div a b [...] &result

> **Alias**: / a b [...] &result

Like mul, but with division.

> mod a b [...] &result

> **Alias**: % a b [...] &result

Like div, but with modulus instead of division.

> not a &result

If a == 0, _&result_ will be non-zero. If a is non-zero, _&result_ will be 0.

> is a b &result

> **Alias**: == a b &result

If a == b, _&result_ will be non-zero. If not, _&result_ will be 0.

> and a b [...] &result

> **Alias**: && a b [...] &result

Checks whether all given values are non-zero and, if yes, writes a non-zero value to _&result_.
If not, then not.

> or a b [...] &result

> **Alias**: || a b [...] &result

If at least one value of the given arguments is non-zero, the result will also be non-zero. If none of the given
values is non-zero, a value equal to 0 will be written to _&result_.

> gt a b &result

> **Alias**: > a b &result

If a > b, _&result_ will be non-zero. If not, _&result_ will be 0.

> gte a b &result

> **Alias**: >= a b &result

If a >= b, _&result_ will be non-zero. If not, _&result_ will be 0.

> lt a b &result

> **Alias**: < a b &result

If a < b, _&result_ will be non-zero. If not, _&result_ will be 0.

> lte a b &result

> **Alias**: <= a b &result

If a <= b, _&result_ will be non-zero. If not, _&result_ will be 0.

> gamma x &result

Apply gamma function (tgamma in C++) to x and store the result in _&result_.

> factorial x &result

> **Alias**: ! x &result

Calculate factorial of x and store the result in _&result_.

> while &cond func

> while f func

Prototype 1: While _&cond_ is non-zero, _func_ is executed. Note that _&cond_ **must always**
be a reference.

Prototype 2: While _f_ 'return's any non-zero value, _func_ is executed. See examples/mean.txt
and examples/integration.txt.

> for\_each list &key &val func

> for\_each list func

Prototype 1: Iterate over _list_, current key will be written to the variable referenced by _&key_ and
current value will be written to the variable referenced by _&val_. _func_ is executed (in the current namespace).

See example.

Prototype 2: Iterate over _list_, execute _func_ with the current key as the first function argument and the current
value as the second function argument.

> for\_each\_value list &val func

> for\_each\_value list func

Prototype 1: Iterate over _list_, current value will be written to the variable referenced by _&val_.
Current key will be left out. _func_ is executed (in the current namespace).

Prototype 2: Iterate over _list_, execute _func_ with the current value as the first function argument.

> for\_each\_key list &val func

> for\_each\_key list func

Prototype 1: Iterate over _list_, current key will be written to the variable referenced by _&key_.
Current value will be left out. _func_ is executed (in the current namespace).

Prototype 2: Iterate over _list_, execute _func_ with the current key as the first function argument. 

> if cond func [else]

If _cond_ is non-zero, _func_ is executed. If _else_ is given and _cond_ is equal to
0 it is executed instead of _func_.

> pow a b &result

> **Alias**: ^ a b &result

Calculates _a_ raised to the power of _b_ and writes the result to _&result_.

> log a &result

> **Alias**: ln a &result

Calculates the natural logarithm of _a_ and writes the result to _&result_. Note that log == ln.

> sin a &result

Calculates the sine of _a_ and writes the result to _&result_. All trigonometric functions use radians
as their units.

> cos a &result

Calculates the cosine of _a_ and writes the result to _&result_.

> tostring a &result

Converts integers, floats and user-defined functions to strings and writes it to _&result_. If a user-defined function
is given, the function code will become a string.

> tonum a &result

Converts floats and strings to a integer and writes it to _&result_.

> tofloat a &result

Converts integers and strings to a float and writes it to _&result_.

> tovar str &ref

Treats the given string _str_ as a variable name and writes the reference to this variable to _&ref_.

> replace str from to &result

Replaces all occurences of _from_ in _str_ with _to_ and writes the newly replaced string to _&result_. Does not
change the original _str_.

> makelist &where

Creates an empty list in _&where_.

> list_add &list key value

Adds _value_ with the key _key_ to the list _&list_.

> push &list value [...]

Appends all given values to the list _&list_.

> delete &list key

Deletes the key _key_ from its list _&list_.

> find &list key &result

Finds the key _key_ in _&list_ and writes a reference to the resulting element in the list to
_&result_.

If _&list_ is a string instead of a list, _key_ must be an integer. It is then used as character index,
where 0 stands for the first character in the string. The resulting character is, however, written
to _&result_.

> readstring &result [&success]

Reads one 'word' from the standard input and writes it to _&result_. This word is separated by a space
character, a newline character or a tab character.

If _&success_ is given, it will contain a non-zero value if the operation succeeded or a value equal to 0
if the operation failed. It will, for example, fail when the user uses Ctrl+D.

> export name value

Exports a variable with the name _name_ and the value _value_ for exactly one following user-defined function, where
this variable can be used as a closure. After this single function has been defined, the list of exported variable names
will be emptied.

> export_clear

Clears the list of exported variable names.

> length &list &result

Writes the number of elements in the list _&list_ to _&result_. If _&list_ is a string, _&result_ will contain the number
of characters in the string.

> type a &result

> **Alias**: : a &result

Determines the type of _a_ and writes a string to _&result_. Depending on the type, the result string can be one of the following:

* _float_ -- for a float
* _num_ -- for an integer
* _function_ -- for a function
* _list_ -- for a list
* _string_ -- for a string
* _undefined_ and _null_ for corresponding types

> split str delimiter &result

Split _str_ into a list. These substrings of _str_ are delimited by their _delimiter_. If _delimiter_ is longer than one character, each of the delimiter's characters work as a valid delimiter.

If _delimiter_ is a number, _str_ will be cut in substrings and converted to a list. Each substring will have a length not greater than _delimiter_.

See examples/split.txt.

> readlines [file] &result

If _file_ is given, this entire file will be read into _&result_, a list of line strings. If the file argument is not given, standard input will be read into a list of line strings.

See examples/for\_each\_file.txt.

> range from to [step] &result

Create a list consisting of numeric elements starting from _from_ and ending by no greater than _to_. If _step_ is given, this will be the incremental value between the elements; if it is not given, we will use 1.

See examples/range.txt.

> sum [f] list &result

If _f_ is given: For every element in _list_, call _f_ with the current element as its first argument. Sum up the results and store the result in _result_. The function prototype for _f_ must be: _f x &result_.

If _f_ is not given: Sum up the elements in _list_ and store the result in _result_.

See examples/sum.txt.

> move from to

> **Alias**: -> from to

Move the variable with the name _from_ to _to_. _from_ and _to_ must be strings. The variable _from_ will be deleted from the current scope.

_move_ provides a very simple, convenient and efficient way to give readable names to function arguments.

See examples/move.txt.

> do f list [list...] &result

For every element of the first list, calls _f_ with the first argument being the current element of the first list.
If more than 1 list is supplied, appends the current elements of the other lists to the argument list of the current
_f_ function call. Calls _f_ with an additional result argument, stores the result in the _result_ list and goes to
the next element in each list.

All supplied lists must have the same size (i.e. length).

You can perform very efficient and simple vector and matrix calculations with this function.

See examples/do\_1.txt and examples/do\_2.txt.

### Notation

One line consists of this scheme:

> [function] [arg1] [arg2] [...] {&result}

The actual scheme depends on the function.

#### Integer notation

> NonzeroNumber = "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9";

> Number = "0" | NonzeroNumber;

> Integer = "0" | ["-"], NonzeroNumber {Number};

#### Float notation

Everything that is legal in C regarding float or double notation is also legal in Valency.

#### Function notation

Starts with { and ends with }. Contains Valency program code.

#### String notation

Starts with " and ends with ". Contains characters.

The ASCII character 0x09 (TAB) can be escaped with \t.
The ASCII character 0x0A (LF) can be escaped with \n.
The ASCII character 0x0D (CR) can be escaped with \r.
The ASCII character 0x22 (") can be escaped with \".

#### List notation

You can create an anonymous list by calling the function "list" in a subexpression.

See examples/list.txt and its output. The syntax is quite obvious:

> list [el1]... &result

### Subexpressions

When one Valency function call and/or argument is enclosed by ( and ), it is regarded as a Subexpression.
This Subexpression is then evaluated and substituted for the actual evaluation result. That's
why the last argument, representing a reference to the result variable, must not be specified
in a Subexpression.

Example:

_Without Subexpressions:_

> add 3 4 &xy

> div 3 6 &uv

> mul 2 xy uv &res

> print res

_With Subexpressions:_

> print (mul 2 (add 3 4) (div 3 6))

(The result is, however, _7_ in both cases.)

Subexpressions are very good, in nearly every situation. But: They are evaluated only once, namely
when the interpreter finds the Subexpression in a function call. That's why Subexpressions cannot
be used as conditions for the _while_ function, because they would be evaluated only once.

See also examples/unnamed.txt for an example of function substitution.

### Function arguments

By default, all function arguments are passed as value. If an argument is the name of a variable,
it can be passed with the ampersand character _&_. Then, the specified variable is passed as reference.

Because of Subexpressions, it is common policy to use the last function argument as reference if the
function returns a value. This value is then written into the specified destination. Only functions
that follow this syntax can be used with Subexpressions.

Inside an user-defined function, function arguments can be accessed by using _#_ in front of the argument number,
where _#1_ is the first argument, _#2_ the second, etc. The number of arguments is in _#n_.

### Currying/Partial Application

See en.wikipedia.org/wiki/Partial%5Fapplication

You can use the _curry_ command to produce a function-like list from another function. This function-like
list will have a smaller arity than the original function.

You can use the _curry_ command like that:

> curry func [args...] &out

To fix (bind) an argument to the function, specify it. To leave it open, type "..".

The new function-like list will be put into &out. It is possible to call and use it as a normal function.

A self-explanatory example can be found in examples/curry.txt.

Building the interpreter
------------------------

You need:

* g++ 4.6 or higher

_or_

* clang++ 3.1 or higher

Building steps:

> cd src

> make all

> sudo make install

Invoking the interpreter
------------------------

> ./valency file.txt

_or_

> ./valency -i

Note
----

In 2012, Max R. P. Grossmann started this project as a simple and straightforward interpreter for a simple
and straightforward but yet Turing complete programming language.

This project is still in a very primitive and pre-pre-alpha stage. Please be aware of that.

There is still great room for improvement, both for the specification of programming language and
for this reference implementation of the interpreter. You are invited to participate in this project.

Todo
----

_(no particular order)_

* eliminating function call overhead
* adding natural list notation
* better error handling for builtin functions (see example mean.txt)
* file handling
* cleaning up source files
* splitting code
* etc.

Known bugs
----------

* Windows newline problem (solution: don't use Windows)
* Function calls are somewhat expensive (solution: use more mathematical approaches)
* A few non-critical memory leaks (solution: use less memory)

Licensing
---------

This program is free software. It comes without any warranty, to the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.
