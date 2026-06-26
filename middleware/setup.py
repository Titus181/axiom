from setuptools import setup, Extension
import sys
import os

# 嘗試使用 pybind11.setup_helpers
try:
    from pybind11.setup_helpers import Pybind11Extension, build_ext
except ImportError:
    class build_ext_placeholder:
        def __init__(self, *args, **kwargs):
            pass
    build_ext = build_ext_placeholder
    Pybind11Extension = Extension

# 定義 C++ 源文件
cpp_core_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "core"))

sources = [
    os.path.join(cpp_core_dir, "src/rng.cpp"),
    os.path.join(cpp_core_dir, "src/simulator.cpp"),
    os.path.join(cpp_core_dir, "src/games/card_engine.cpp"),
    os.path.join(cpp_core_dir, "src/games/peek_baccarat.cpp"),
    os.path.join(cpp_core_dir, "binding/python_binding.cpp"),
]

include_dirs = [
    os.path.join(cpp_core_dir, "include"),
]

# 在編譯時強制設定 C++20
ext_modules = [
    Pybind11Extension(
        "pyaxiom",
        sources,
        include_dirs=include_dirs,
        cxx_std=20,  # pyright: ignore[reportCallIssue]
    ),
]

setup(
    name="axiom",
    version="1.0.0",
    description="Axiom middleware simulation engine and data analyzer",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext} if Pybind11Extension is not Extension else {},
    packages=["axiom"],
    zip_safe=False,
    python_requires=">=3.8",
)
