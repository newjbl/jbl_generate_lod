#!/usr/bin/env python
import os
import sys

env = SConscript("../godot_cpp_dir/godot-cpp/SConstruct")
print(env)
env.Append(CPPPATH=[
	"src/",
	"src/meshoptimizer/"])

sources = Glob("src/*.cpp") +  Glob("/src/meshoptimizer/*.cpp")

if env["platform"] == "windows":
    library = env.SharedLibrary(
        "bin/GDExtension/jbl_generate_lod{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )
Default(library)
