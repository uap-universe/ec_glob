/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 * Copyright 2024 Mike Becker - All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "ec_glob.h"

#include "test.h"

// https://github.com/editorconfig/editorconfig-core-c/issues/101
#define TEST_EXCLUDE_EDITORCONFIG_CORE_C_BUG_101

// https://github.com/editorconfig/editorconfig-core-c/issues/102
#define TEST_EXCLUDE_EDITORCONFIG_CORE_C_BUG_102

#define assert_ec_glob_true(str) CX_TEST_ASSERT(0 == ec_glob(pattern, str))
#define assert_ec_glob_false(str) CX_TEST_ASSERT(0 != ec_glob(pattern, str))

CX_TEST(test_match_all) {
    const char *pattern = "**/*";
    CX_TEST_DO {
        assert_ec_glob_true("/LICENSE");
        assert_ec_glob_true("/README.txt");
        assert_ec_glob_true("/Makefile");
        assert_ec_glob_true("/.hgignore");
        assert_ec_glob_true("/src/code.c");
        assert_ec_glob_true("/src/header.h");
        assert_ec_glob_true("/~backup");
    }
}

CX_TEST(test_single_extension) {
    const char *pattern = "**/*.txt";
    CX_TEST_DO {
        assert_ec_glob_true("/README.txt");
        assert_ec_glob_true("/docs/help.txt");
        assert_ec_glob_true("/~backup.txt");
        assert_ec_glob_true("/foo/bar/baz.txt");

        assert_ec_glob_false("/Makefile");
        assert_ec_glob_false("/help.md");
        assert_ec_glob_false("/help.txt.in");
        assert_ec_glob_false("/CAPS.TXT");
    }
}

CX_TEST(test_multi_extension) {
    const char *pattern = "**/*.{diff,md}";
    CX_TEST_DO {
        assert_ec_glob_true("/help.md");
        assert_ec_glob_true("/changes.diff");
        assert_ec_glob_true("/changes.diff.md");
        assert_ec_glob_true("/changes.txt.diff");
        assert_ec_glob_true("/DOC/help.md");

        assert_ec_glob_false("/README.txt");
        assert_ec_glob_false("/Makefile");
        assert_ec_glob_false("/somediff");
        assert_ec_glob_false("/changes.diff/source.c");
        assert_ec_glob_false("/housemd");
        assert_ec_glob_false("/pattern.{diff,md}");
    }
}

CX_TEST(test_mix_extension_and_special_file) {
    const char *pattern = "**/{*.yaml,*.yml,.clang-format,.clang-tidy,_clang-format}";
    CX_TEST_DO {
        assert_ec_glob_true("/docs.yaml");
        assert_ec_glob_true("/_clang-format");
        assert_ec_glob_true("/.clang-format");
        assert_ec_glob_true("/.clang-tidy");
        assert_ec_glob_true("/src/.clang-format");
        assert_ec_glob_true("/rest/api.yml");

        assert_ec_glob_false("/file._clang-format");
        assert_ec_glob_false("/file.clang-format");
        assert_ec_glob_false("/file.clang-tidy");
        assert_ec_glob_false("/yaml.text");
        assert_ec_glob_false("/yml");
    }
}

CX_TEST(test_choice_within_choice) {
    const char *pattern = "**/*.{y{ml,aml},json}";
    CX_TEST_DO {
        assert_ec_glob_true("/config.yml");
        assert_ec_glob_true("/config.yaml");
        assert_ec_glob_true("/config.json");

        assert_ec_glob_false("/json/help.md");
        assert_ec_glob_false("/test.y");
        assert_ec_glob_false("/test.yjson");
    }
}

CX_TEST(test_arbitrary_subdir) {
    const char *pattern = "**/*/*.c";
    CX_TEST_DO {
        assert_ec_glob_true("/src/code.c");
        assert_ec_glob_true("/lib/func.c");

        assert_ec_glob_false("/code.c");
        assert_ec_glob_false("/func.c");
    }
}

CX_TEST(test_double_double_wildcard) {
    const char *pattern = "**/**/*.c";
    CX_TEST_DO {
        assert_ec_glob_true("/src/code.c");
        assert_ec_glob_true("/lib/func.c");
        assert_ec_glob_true("/code.c");
        assert_ec_glob_true("/func.c");
        assert_ec_glob_true("/src/lib/func.c");

        assert_ec_glob_false("code.c");
        assert_ec_glob_false("func.c");
    }
}

CX_TEST(test_escaped_opening_brace) {
    const char *pattern = "a\\{b,c,}.d";
    CX_TEST_DO {
        assert_ec_glob_true("a{b,c,}.d");

        assert_ec_glob_false("ab.d");
        assert_ec_glob_false("ac.d");
        assert_ec_glob_false("a.d");
        assert_ec_glob_false("a{b}.d");
        assert_ec_glob_false("a{c}.d");
    }
}

CX_TEST(test_single_nested_single) {
    const char *pattern = "{sin{nested}gle}.b";
    CX_TEST_DO {
        assert_ec_glob_true("{sin{nested}gle}.b");

        assert_ec_glob_false("single.b");
    }
}

CX_TEST(test_single_nested_choice) {
    const char *pattern = "{sin{nested,choice}gle}.b";
    CX_TEST_DO {
        // looks wrong, but this is what the reference implementation does
        assert_ec_glob_true("sinnestedgle.b");
        assert_ec_glob_true("sinchoicegle.b");

        assert_ec_glob_false("sin{nested.b");
        assert_ec_glob_false("choice}gle.b");
        assert_ec_glob_false("{sin{nested,choice}gle}.b");

        // this is what we actually expected
        assert_ec_glob_false("{sinnestedgle}.b");
        assert_ec_glob_false("{sinchoicegle}.b");
    }
}

CX_TEST(test_escaped_comma_outside_choice) {
    const char *pattern = "no\\,choice";
    CX_TEST_DO {
        assert_ec_glob_true("no,choice");

        assert_ec_glob_false("no\\,choice");
        assert_ec_glob_false("nochoice");
        assert_ec_glob_false("no");
        assert_ec_glob_false("choice");
    }
}

CX_TEST(test_num_range_in_file_extension) {
    const char *pattern = "**/*.orig.{0..9}";
    CX_TEST_DO {
#ifndef TEST_EXCLUDE_EDITORCONFIG_CORE_C_BUG_102
        assert_ec_glob_true("/README.orig.0");
#endif
        assert_ec_glob_true("/README.orig.1");
        assert_ec_glob_true("/src/code.c.orig.4");

        assert_ec_glob_false("/src/code.c.orig.10");
    }
}

CX_TEST(test_escaped_slash_in_brackets) {
    const char *pattern = "ab[e\\/]cd.i";
    CX_TEST_DO {
        // the specification does not say that slash
        // must be escaped within brackets to work like
        // a single character, but the reference
        // implementation has special treatment for
        // slash within brackets
        assert_ec_glob_true("abecd.i");
        assert_ec_glob_true("ab/cd.i");

        assert_ec_glob_false("ab\\cd.i");
        assert_ec_glob_false("ab[e\\/]cd.i");
        assert_ec_glob_false("abcd.i");
    }
}

CX_TEST(test_auto_escape_bracket_and_minus_in_brackets) {
    const char *pattern = "ab[-\\]z]cd.i";
    CX_TEST_DO {
        assert_ec_glob_true("ab]cd.i");
        assert_ec_glob_true("ab-cd.i");
        assert_ec_glob_true("abzcd.i");

        assert_ec_glob_false("ab-z]cd.i");
    }
}

#ifndef TEST_EXCLUDE_EDITORCONFIG_CORE_C_BUG_101
CX_TEST(test_wildcard_in_brackets) {
    const char *pattern = "ab[e*]cd.i";
    CX_TEST_DO {
        // to match what the buggy reference implementation does,
        // you have to swap true and false
        assert_ec_glob_true("abecd.i");
        assert_ec_glob_true("ab*cd.i");

        assert_ec_glob_false("ab[e]cd.i");
        assert_ec_glob_false("ab[[]cd.i");
        assert_ec_glob_false("ab[^]cd.i");
        assert_ec_glob_false("ab[/]cd.i");
        assert_ec_glob_false("ab[[]cd.i");
        assert_ec_glob_false("ab[e[^/e[e]cd.i");
    }
}
#endif

// Create some test cases based on the patterns given in the core tests repo:
// https://github.com/editorconfig/editorconfig-core-test/tree/master/glob

CX_TEST(test_core_star_star_0) {
    const char *pattern = "a**z.c";
    CX_TEST_DO {
        assert_ec_glob_true("alcatraz.c");
        assert_ec_glob_true("a/s/d/f/z.c");
        assert_ec_glob_true("a/s/z.c/fz.c");
        assert_ec_glob_true("az.c");
        assert_ec_glob_true("a/z.c");

        assert_ec_glob_false("src/alcatraz.c");
        assert_ec_glob_false("a.c");
    }
}

CX_TEST(test_core_star_star_1) {
    const char *pattern = "b/**z.c";
    CX_TEST_DO {
        assert_ec_glob_true("b/alcatraz.c");
        assert_ec_glob_true("b/a/r/z.c");
        assert_ec_glob_true("b/fuzz.c");
        assert_ec_glob_true("b/z.c");

        assert_ec_glob_false("src/b/fuzz.c");
        assert_ec_glob_false("/bar/fuzz.c");
    }
}

CX_TEST(test_core_star_star_2) {
    const char *pattern = "c**/z.c";
    CX_TEST_DO {
        assert_ec_glob_true("charlie/z.c");
        assert_ec_glob_true("c/z.c");
        assert_ec_glob_true("crazy/dir/z.c");
        assert_ec_glob_true("c/d/e/z.c");
    }
}

CX_TEST(test_core_star_star_3) {
    const char *pattern = "d/**/z.c";
    CX_TEST_DO {
        assert_ec_glob_true("d/foo/bar/z.c");
        assert_ec_glob_true("d/foo/z.c");
        assert_ec_glob_true("d/z.c");

        assert_ec_glob_false("dz.c");
    }
}

CX_TEST(test_core_star_0) {
    const char *pattern = "a*e.c";
    CX_TEST_DO {
        assert_ec_glob_true("afterlife.c");
        assert_ec_glob_true("ae.c");
        assert_ec_glob_true("a4711e.c");

        assert_ec_glob_false("after/life.c");
        assert_ec_glob_false("a/e.c");
    }
}

CX_TEST(test_core_star_1) {
    const char *pattern = "Bar/*";
    CX_TEST_DO {
        assert_ec_glob_true("Bar/code.c");
        assert_ec_glob_true("Bar/");

        assert_ec_glob_false("Bar");
        assert_ec_glob_false("bar/code.c");
        assert_ec_glob_false("Bar/foo/code.c");
    }
}

CX_TEST(test_core_star_2) {
    const char *pattern = "*";
    CX_TEST_DO {
        assert_ec_glob_true("code.c");
        assert_ec_glob_true("crazy.f90");

        assert_ec_glob_false("src/stuff.c");
    }
}

CX_TEST(test_core_utf8) {
    const char *pattern = "中德.txt";
    CX_TEST_DO {
        assert_ec_glob_true("中德.txt");

        assert_ec_glob_false("德中.txt");
    }
}

CX_TEST(test_core_question) {
    const char *pattern = "som?.c";
    CX_TEST_DO {
        assert_ec_glob_true("some.c");
        assert_ec_glob_true("somE.c");
        assert_ec_glob_true("som3.c");

        assert_ec_glob_false("somad.c");
        assert_ec_glob_false("som.c");
    }
}

CX_TEST(test_core_brackets_0) {
    const char *pattern = "[ab].a";
    CX_TEST_DO {
        assert_ec_glob_true("a.a");
        assert_ec_glob_true("b.a");

        assert_ec_glob_false("A.a");
        assert_ec_glob_false("B.a");
        assert_ec_glob_false("c.a");
        assert_ec_glob_false("ab.a");
        assert_ec_glob_false("[ab].a");
        assert_ec_glob_false(".a");
    }
}

CX_TEST(test_core_brackets_1) {
    const char *pattern = "[!ab].b";
    CX_TEST_DO {
        assert_ec_glob_true("c.b");
        assert_ec_glob_true("d.b");
        assert_ec_glob_true("A.b");
        assert_ec_glob_true("B.b");
        assert_ec_glob_true("/.b");
        assert_ec_glob_true("{.b");
        assert_ec_glob_true("}.b");
        assert_ec_glob_true("?.b");
        assert_ec_glob_true("(.b");
        assert_ec_glob_true(").b");
        assert_ec_glob_true("\\.b");
        assert_ec_glob_true("5.b");
        assert_ec_glob_true("\t.b");

        assert_ec_glob_false("a.b");
        assert_ec_glob_false("b.b");
        assert_ec_glob_false("[!ab].b");
        assert_ec_glob_false(".b");
    }
}

CX_TEST(test_core_brackets_2) {
    const char *pattern = "[d-g].c";
    CX_TEST_DO {
        assert_ec_glob_true("d.c");
        assert_ec_glob_true("e.c");
        assert_ec_glob_true("f.c");
        assert_ec_glob_true("g.c");

        assert_ec_glob_false("a.c");
        assert_ec_glob_false("b.c");
        assert_ec_glob_false("c.c");
        assert_ec_glob_false("h.c");
        assert_ec_glob_false("D.c");
        assert_ec_glob_false("E.c");
        assert_ec_glob_false("F.c");
        assert_ec_glob_false("G.c");
    }
}

CX_TEST(test_core_brackets_3) {
    const char *pattern = "[!d-g].d";
    CX_TEST_DO {
        assert_ec_glob_true("a.d");
        assert_ec_glob_true("b.d");
        assert_ec_glob_true("c.d");
        assert_ec_glob_true("h.d");
        assert_ec_glob_true("D.d");
        assert_ec_glob_true("E.d");
        assert_ec_glob_true("F.d");
        assert_ec_glob_true("G.d");

        assert_ec_glob_false("d.d");
        assert_ec_glob_false("e.d");
        assert_ec_glob_false("f.d");
        assert_ec_glob_false("g.d");
    }
}

CX_TEST(test_core_brackets_4) {
    const char *pattern = "[abd-g].e";
    CX_TEST_DO {
        assert_ec_glob_true("a.e");
        assert_ec_glob_true("b.e");
        assert_ec_glob_true("d.e");
        assert_ec_glob_true("e.e");
        assert_ec_glob_true("f.e");
        assert_ec_glob_true("g.e");

        assert_ec_glob_false("c.e");
        assert_ec_glob_false("h.e");
        assert_ec_glob_false("D.e");
        assert_ec_glob_false("E.e");
        assert_ec_glob_false("F.e");
        assert_ec_glob_false("G.e");
    }
}

CX_TEST(test_core_brackets_5) {
    const char *pattern = "[-ab].f";
    CX_TEST_DO {
        assert_ec_glob_true("a.f");
        assert_ec_glob_true("b.f");
        assert_ec_glob_true("-.f");

        assert_ec_glob_false("A.f");
        assert_ec_glob_false("B.f");
        assert_ec_glob_false("z.f");
        assert_ec_glob_false("0.f");
        assert_ec_glob_false("9.f");
        assert_ec_glob_false(".f");
    }
}

CX_TEST(test_core_brackets_6) {
    const char *pattern = "[\\]ab].g";
    CX_TEST_DO {
        assert_ec_glob_true("a.g");
        assert_ec_glob_true("b.g");
        assert_ec_glob_true("].g");

        assert_ec_glob_false("\\.g");
    }
}

CX_TEST(test_core_brackets_7) {
    const char *pattern = "[ab]].g";
    CX_TEST_DO {
        assert_ec_glob_true("a].g");
        assert_ec_glob_true("b].g");

        assert_ec_glob_false("a.g");
        assert_ec_glob_false("b.g");
    }
}

CX_TEST(test_core_brackets_8) {
    const char *pattern = "[!\\]ab].g";
    CX_TEST_DO {
        assert_ec_glob_true("z.g");
        assert_ec_glob_true("[.g");
        assert_ec_glob_true("0.g");

        assert_ec_glob_false("zz.g");
        assert_ec_glob_false("a.g");
        assert_ec_glob_false("b.g");
        assert_ec_glob_false("].g");
    }
}

CX_TEST(test_core_brackets_9) {
    const char *pattern = "[!ab]].g";
    CX_TEST_DO {
        assert_ec_glob_true("c].g");
        assert_ec_glob_true("d].g");

        assert_ec_glob_false("a].g");
        assert_ec_glob_false("b].g");
        assert_ec_glob_false("a.g");
        assert_ec_glob_false("b.g");
    }
}

CX_TEST(test_core_brackets_10) {
    const char *pattern = "ab[e/]cd.i";
    CX_TEST_DO {
        assert_ec_glob_true("ab[e/]cd.i");

        assert_ec_glob_false("abecd.i");
        assert_ec_glob_false("ab/cd.i");
        assert_ec_glob_false("abcd.i");
    }
}

CX_TEST(test_core_brackets_11) {
    const char *pattern = "ab[/c";
    CX_TEST_DO {
        assert_ec_glob_true("ab[/c");

        assert_ec_glob_false("ab[c");
        assert_ec_glob_false("ab/c");
    }
}

CX_TEST(test_core_braces_0) {
    const char *pattern = "*.{py,js,html}";
    CX_TEST_DO {
        assert_ec_glob_true("cobra.py");
        assert_ec_glob_true("web.html");
        assert_ec_glob_true("script.js");

        assert_ec_glob_false("src/cobra.py");
        assert_ec_glob_false("src/web.html");
        assert_ec_glob_false("src/script.js");
    }
}

CX_TEST(test_core_braces_1) {
    const char *pattern = "{single}.b";
    CX_TEST_DO {
        assert_ec_glob_true("{single}.b");

        assert_ec_glob_false("single.b");
    }
}

CX_TEST(test_core_braces_2) {
    const char *pattern = "{}.c";
    CX_TEST_DO {
        assert_ec_glob_true("{}.c");

        assert_ec_glob_false(".c");
    }
}

CX_TEST(test_core_braces_3) {
    const char *pattern = "a{b,c,}.d";
    CX_TEST_DO {
        assert_ec_glob_true("ab.d");
        assert_ec_glob_true("ac.d");
        assert_ec_glob_true("a.d");

        assert_ec_glob_false("a{b,c,}.d");
        assert_ec_glob_false("a{b}.d");
        assert_ec_glob_false("a{c}.d");
    }
}

CX_TEST(test_core_braces_4) {
    const char *pattern = "a{,b,,c,}.e";
    CX_TEST_DO {
        assert_ec_glob_true("ab.e");
        assert_ec_glob_true("ac.e");
        assert_ec_glob_true("a.e");

        assert_ec_glob_false("abc.e");
        assert_ec_glob_false("ab c.e");
        assert_ec_glob_false("a{,b,,c}.e");
    }
}

CX_TEST(test_core_braces_5) {
    const char *pattern = "{.f";
    CX_TEST_DO {
        assert_ec_glob_true("{.f");

        assert_ec_glob_false(".f");
    }
}

CX_TEST(test_core_braces_6) {
    const char *pattern = "{word,{also},this}.g";
    CX_TEST_DO {
        assert_ec_glob_true("word.g");
        assert_ec_glob_true("{also}.g");
        assert_ec_glob_true("this.g");

        assert_ec_glob_false("{word}.g");
        assert_ec_glob_false("also.g");
        assert_ec_glob_false("{this}.g");
    }
}

CX_TEST(test_core_braces_7) {
    const char *pattern = "{{a,b},c}.k";
    CX_TEST_DO {
        assert_ec_glob_true("a.k");
        assert_ec_glob_true("b.k");
        assert_ec_glob_true("c.k");

        assert_ec_glob_false("ac.k");
        assert_ec_glob_false("bc.k");
        assert_ec_glob_false("{a,b}.k");
    }
}

CX_TEST(test_core_braces_8) {
    const char *pattern = "{a,{b,c}}.l";
    CX_TEST_DO {
        assert_ec_glob_true("a.l");
        assert_ec_glob_true("b.l");
        assert_ec_glob_true("c.l");

        assert_ec_glob_false("ab.l");
        assert_ec_glob_false("ac.l");
        assert_ec_glob_false("{b,c}.l");
    }
}

CX_TEST(test_core_braces_9) {
    const char *pattern = "{},b}.h";
    CX_TEST_DO {
        assert_ec_glob_true("{},b}.h");

        assert_ec_glob_false("}.h");
        assert_ec_glob_false("b.h");
    }
}

CX_TEST(test_core_braces_10) {
    const char *pattern = "{{,b,c{d}.i";
    CX_TEST_DO {
        assert_ec_glob_true("{{,b,c{d}.i");

        assert_ec_glob_false("{.i");
        assert_ec_glob_false("b.i");
        assert_ec_glob_false("c{d}.i");
        assert_ec_glob_false("c.i");
        assert_ec_glob_false("d.i");
        assert_ec_glob_false("c{d.i");
    }
}

CX_TEST(test_core_braces_11) {
    const char *pattern = "{a\\,b,cd}.txt";
    CX_TEST_DO {
        assert_ec_glob_true("a,b.txt");
        assert_ec_glob_true("cd.txt");

        assert_ec_glob_false("a.txt");
        assert_ec_glob_false("b.txt");
    }
}

CX_TEST(test_core_braces_12) {
    const char *pattern = "{e,\\},f}.txt";
    CX_TEST_DO {
        assert_ec_glob_true("e.txt");
        assert_ec_glob_true("}.txt");
        assert_ec_glob_true("f.txt");

        assert_ec_glob_false("\\}.txt");
        assert_ec_glob_false("e,f}.txt");
        assert_ec_glob_false("\\,f}.txt");
    }
}

CX_TEST(test_core_braces_13) {
    const char *pattern = "{g,\\\\,i}.txt";
    CX_TEST_DO {
        assert_ec_glob_true("g.txt");
        assert_ec_glob_true("i.txt");
        assert_ec_glob_true("\\.txt");

        assert_ec_glob_false(",i.txt");
        assert_ec_glob_false("\\,i.txt");
    }
}

CX_TEST(test_core_braces_14) {
    const char *pattern = "{some,a{*c,b}[ef]}.j";
    CX_TEST_DO {
        assert_ec_glob_true("some.j");
        assert_ec_glob_true("abe.j");
        assert_ec_glob_true("abf.j");
        assert_ec_glob_true("ace.j");
        assert_ec_glob_true("acf.j");
        assert_ec_glob_true("afooce.j");
        assert_ec_glob_true("abarcf.j");

        assert_ec_glob_false("af/ooce.j");
        assert_ec_glob_false("ab/arcf.j");
        assert_ec_glob_false("acef.j");
    }
}

CX_TEST(test_core_braces_15) {
    const char *pattern = "{3..120}";
    CX_TEST_DO {
        assert_ec_glob_true("3");
        assert_ec_glob_true("17");
        assert_ec_glob_true("42");
        assert_ec_glob_true("84");
        assert_ec_glob_true("100");
        assert_ec_glob_true("111");
        assert_ec_glob_true("120");

        assert_ec_glob_false("-5");
        assert_ec_glob_false("-3");
        assert_ec_glob_false("-0");
        assert_ec_glob_false("1");
        assert_ec_glob_false("2");
        assert_ec_glob_false("121");
        assert_ec_glob_false("122");
        assert_ec_glob_false("1200");
        assert_ec_glob_false("{3..120}");
        assert_ec_glob_false("3..120");
        assert_ec_glob_false("3.");
        assert_ec_glob_false(".120");
    }
}

CX_TEST(test_core_braces_16) {
    const char *pattern = "{aardvark..antelope}";
    CX_TEST_DO {
        assert_ec_glob_true("{aardvark..antelope}");
        assert_ec_glob_false("{aardvark00antelope}");
    }
}


int main(void) {

    CxTestSuite *suite = cx_test_suite_new("ec_glob");

    cx_test_register(suite, test_match_all);
    cx_test_register(suite, test_single_extension);
    cx_test_register(suite, test_multi_extension);
    cx_test_register(suite, test_mix_extension_and_special_file);
    cx_test_register(suite, test_choice_within_choice);
    cx_test_register(suite, test_arbitrary_subdir);
    cx_test_register(suite, test_double_double_wildcard);
    cx_test_register(suite, test_escaped_opening_brace);
    cx_test_register(suite, test_single_nested_single);
    cx_test_register(suite, test_single_nested_choice);
    cx_test_register(suite, test_escaped_comma_outside_choice);
    cx_test_register(suite, test_num_range_in_file_extension);
    cx_test_register(suite, test_escaped_slash_in_brackets);
    cx_test_register(suite, test_auto_escape_bracket_and_minus_in_brackets);
#ifndef TEST_EXCLUDE_EDITORCONFIG_CORE_C_BUG_101
    cx_test_register(suite, test_wildcard_in_brackets);
#endif

    cx_test_register(suite, test_core_star_star_0);
    cx_test_register(suite, test_core_star_star_1);
    cx_test_register(suite, test_core_star_star_2);
    cx_test_register(suite, test_core_star_star_3);
    cx_test_register(suite, test_core_star_0);
    cx_test_register(suite, test_core_star_1);
    cx_test_register(suite, test_core_star_2);
    cx_test_register(suite, test_core_question);
    cx_test_register(suite, test_core_utf8);
    cx_test_register(suite, test_core_brackets_0);
    cx_test_register(suite, test_core_brackets_1);
    cx_test_register(suite, test_core_brackets_2);
    cx_test_register(suite, test_core_brackets_3);
    cx_test_register(suite, test_core_brackets_4);
    cx_test_register(suite, test_core_brackets_5);
    cx_test_register(suite, test_core_brackets_6);
    cx_test_register(suite, test_core_brackets_7);
    cx_test_register(suite, test_core_brackets_8);
    cx_test_register(suite, test_core_brackets_9);
    cx_test_register(suite, test_core_brackets_10);
    cx_test_register(suite, test_core_brackets_11);
    cx_test_register(suite, test_core_braces_0);
    cx_test_register(suite, test_core_braces_1);
    cx_test_register(suite, test_core_braces_2);
    cx_test_register(suite, test_core_braces_3);
    cx_test_register(suite, test_core_braces_4);
    cx_test_register(suite, test_core_braces_5);
    cx_test_register(suite, test_core_braces_6);
    cx_test_register(suite, test_core_braces_7);
    cx_test_register(suite, test_core_braces_8);
    cx_test_register(suite, test_core_braces_9);
    cx_test_register(suite, test_core_braces_10);
    cx_test_register(suite, test_core_braces_11);
    cx_test_register(suite, test_core_braces_14);
    cx_test_register(suite, test_core_braces_15);
    cx_test_register(suite, test_core_braces_16);

    cx_test_run_stdout(suite);
    int result = suite->failure > 0;
    cx_test_suite_free(suite);

    return result;
}
