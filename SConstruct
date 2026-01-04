#!/usr/bin/env python
import os
import sys

env = SConscript("../godot_cpp_dir/godot-cpp/SConstruct")
print(env)
env.Append(CPPPATH=[
	"src/",
	"src/meshoptimizer/"])

sources = Glob("src/*.cpp") +  Glob("src/meshoptimizer/simplifier.cpp")

print("📌 编译文件列表：", [str(f) for f in sources])

# 过滤掉 allocator.cpp
sources = [f for f in sources if not f.name.endswith("allocator.cpp")]

print("📌 编译文件列表：", [str(f) for f in sources])


if env["platform"] == "windows":
    library = env.SharedLibrary(
        "bin/GDExtension/jbl_generate_lod{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )
Default(library)
