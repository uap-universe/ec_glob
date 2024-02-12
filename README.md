# A POSIX RegEx Implementation for EditorConfig Glob Matching

This repository offers an implementation for the [EditorConfig Glob][0]
matching algorithm. The signature is kept exactly the same as in the
[reference implementation][1] of the EditorConfig Core library:
```C
int ec_glob(const char *pattern, const char *string);
```

## Benefits

This implementation comes with two substantial benefits:

1. Instead of translating the glob pattern to Pearl Compatibile Regular
   Expressions (PCRE), the pattern is translated to POSIX Extended Regular
   Expressions. This removes the requirement for external libraries entirely.
2. If you don't mind having a dependency to libpcre2, this implementation is
   about two times faster than the reference implementation.

### Fixed Bugs

At the time of publishing this implementation, there were still two open
bugs [(1)][2] [(2)][3] in the reference implementation that are fixed in this
implementation, meaning that the behavior is not exactly the same as in the
reference implementation. You can comment the macros at the beginning of the
`testcases.c` to see the differences.

[0]: https://spec.editorconfig.org/#glob-expressions
[1]: https://github.com/editorconfig/editorconfig-core-c/blob/master/src/lib/ec_glob.c
[2]: https://github.com/editorconfig/editorconfig-core-c/issues/101
[3]: https://github.com/editorconfig/editorconfig-core-c/issues/102

## Usage

Just copy the files `ec_glob.c` and `ec_glob.h` to your project and use them
under the terms of the license. If you wish to link the implementation against
standard POSIX regex implementation, there is nothing you need to do.

If you want to link against the libcpre2-posix wrapper, compile the `ec_glob.c`
with the `EC_GLOB_USE_PCRE` macro defined.

## Limitations

This implementation has the following known limitations:

1. The standard POSIX regex implementation is three times slower than the
   reference implementation and six times slower than this implementation
   when linked against the libpcre2-posix wrapper.
2. We only support nesting braces up to a depth of 32.
3. When you want to use the `{num1..num2}` pattern, it must occur in one of
   the first 32 pairs of braces. 
4. The maximum length of the resulting regular expression that fits into stack
   memory is 64. Longer patterns will be allocated on the heap. The program
   is aborted, when heap allocation fails. 

## LICENSE

Copyright 2024 Mike Becker - All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

### License for Reference Implementation

Copyright (c) 2014-2019 Hong Xu <hong AT topbug DOT net>
Copyright (c) 2018 Sven Strickroth <email AT cs-ware DOT de>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

The reference implementation uses `utarray` licensed under the following terms:

Copyright (c) 2008-2014, Troy D. Hanson   http://troydhanson.github.com/uthash/
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
