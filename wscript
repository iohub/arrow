#!/usr/bin/env python
# -*- coding: utf-8 -*-
from subprocess import Popen, PIPE, PIPE, check_call
from os import path
import os
import webbrowser
from itertools import chain
import sys
import ws.test


WAF_TOOLS = ["compiler_cxx", "boost"]


def options(ctx):
    ctx.load(" ".join(WAF_TOOLS))

    ctx.add_option(
        '--with-llvm-config',
        action='store',
        default='llvm-config'
    )

    ctx.add_option(
        '--release',
        action='store_true',
        default=False
    )


def configure(ctx):
    ctx.load(" ".join(WAF_TOOLS))

    ctx.check(lib='pthread', mandatory=True, uselib_store='PTHREAD')
    ctx.check(lib='dl', mandatory=True, uselib_store='DL')
    ctx.check(lib='ncurses', mandatory=False, uselib_store='TINFO')
    ctx.check(lib='tinfo', mandatory=False, uselib_store='TINFO')
    ctx.check(lib='z', mandatory=True, uselib_store='Z')
    ctx.check(lib='gmp', mandatory=True, uselib_store='GMP')
    ctx.check(lib='ffi', mandatory=True, uselib_store='FFI')
    ctx.check(lib='fmt', mandatory=True, uselib_store='fmt')

    ctx.check_boost(lib="system program_options filesystem")

    ctx.check_cfg(
        path=ctx.options.with_llvm_config, package='',
        args='--ldflags --cflags --libs all',
        uselib_store='LLVM'
    )

    # HACK: Add clang as a llvm lib -- not sure how to do this with check
    ctx.env["LIB_LLVM"].append("clang")

    if ctx.env["CXX_NAME"] in ("gcc", "clang"):
        ctx.env.append_unique("CXXFLAGS", "-std=gnu++1y")

        if ctx.options.release:
            ctx.env.append_unique("CXXFLAGS", "-Ofast")

        else:
            ctx.env.append_unique("CXXFLAGS", "-g")
            ctx.env.append_unique("CXXFLAGS", "-O0")
            ctx.env.append_unique("CXXFLAGS", "--coverage")

            ctx.env.append_unique("LINKFLAGS", "--coverage")

        ctx.env.append_unique("CXXFLAGS", "-Wall")
        ctx.env.append_unique("CXXFLAGS", "-Wextra")
        ctx.env.append_unique("CXXFLAGS", "-Wfatal-errors")

        # ctx.env.append_unique("CXXFLAGS", "-Wsuggest-final-types")
        # ctx.env.append_unique("CXXFLAGS", "-Wsuggest-final-methods")
        # ctx.env.append_unique("CXXFLAGS", "-Wsuggest-override")
        # ctx.env.append_unique("CXXFLAGS", "-Wsuggest-attribute=pure")
        # ctx.env.append_unique("CXXFLAGS", "-Wsuggest-attribute=const")
        # ctx.env.append_unique("CXXFLAGS", "-Wsuggest-attribute=noreturn")
        # ctx.env.append_unique("CXXFLAGS", "-Weffc++")
        ctx.env.append_unique("CXXFLAGS", "-Wpedantic")

        ctx.env.append_unique("CXXFLAGS", "-Woverloaded-virtual")
        ctx.env.append_unique("CXXFLAGS", "-Wno-unused-value")
        ctx.env.append_unique("CXXFLAGS", "-Wno-unused-parameter")
        ctx.env.append_unique("CXXFLAGS", "-Wno-pragmas")

    # Create configurateion file with PREFIX
    ctx.define("WAF_PREFIX", ctx.env["PREFIX"])
    ctx.write_config_header("config.h")


def build(ctx):
    start_dir = ctx.path.find_dir("modules")
    ctx.install_files(
        "${PREFIX}/lib/arrow/modules", start_dir.ant_glob('**/*.as'),
        cwd=start_dir,
        relative_trick=True)

    ctx.program(source=ctx.path.ant_glob("src/**/*.cpp"),
                includes=[
                    "include",
                    "vendor/mach7/code",
                    "vendor/rapidjson/include",
                    # "vendor/utfcpp/source",
                    "build",
                ],
                target="arrow",
                use=["BOOST", "LLVM", "PTHREAD", "DL", "TINFO", "Z", "GMP",
                     "FFI", "fmt", "CLANG"])

    # Delete all *.gcda files
    # BUG: Otherwise lots of chaos happens when running a debug build
    #      several times on macOS
    check_call(["find", "build", "-name", "*.gcda", "-delete"])

def test(ctx):
    result = ws.test.run(ctx)
    if not result:
        sys.exit(1)


# def generate(ctx):
#     ws.test.generate(ctx)


def coverage(ctx):
    # Copy all source files into the build directory
    # HACK: If someone can get this working without this step, be my guest
    check_call(["cp", "-r", "./src", "./build"]);

    # Generate HTML coverage
    check_call(["rm", "-rf", "coverage"])
    check_call(["mkdir", "-p", "coverage"])
    check_call([
        "gcovr", "-r", "build/src", "--html", "--html-details",
        "-o", "coverage/index.html"])

    # Generate coverage report
    check_call(["gcovr", "-r", "build/src", "-s"])
