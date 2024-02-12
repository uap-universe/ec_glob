# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
#
# Copyright 2024 Mike Becker - All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   1. Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#   2. Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

all: prog pcreprog refprog

prog: ec_glob.o testcases.o
	$(CC) -o $@ $+

pcreprog: ec_glob_pcre.o testcases.o
	$(CC) -o $@ `pkg-config --libs libpcre2-posix libpcre2-8` $+

refprog: ec_glob_ref.o testcases.o
	$(CC) -o $@ `pkg-config --libs libpcre2-8` $+

ec_glob_pcre.o: ec_glob.c
	$(CC) -O3 -DEC_GLOB_USE_PCRE -o $@ -c $<

%.o: %.c
	$(CC) -O3 -o $@ -c $<

check: all
	@./prog > /dev/null && ./refprog > /dev/null && ./pcreprog > /dev/null
	@echo OK

check-impl: prog
	perf stat -e instructions ./$<

check-pcre: pcreprog
	perf stat -e instructions ./$<

check-ref: refprog
	perf stat -e instructions ./$<

clean:
	rm -f *.o prog refprog
