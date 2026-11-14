# Copyright (c) 2019 - 2026 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""Naming conventions of the bindings: the enums are CamelCase classes whose
members are only reachable through the class (no export_values), the
dimension suffix of a function is 2D or 3D (never 2d or 3d), and the static
methods are static."""

import enum
import inspect
import re

import geode_test_env

import opengeode_geosciences_py_explicit as explicit
import opengeode_geosciences_py_implicit as implicit

# Extension modules to check; the other modules are added with their commit
MODULES = [explicit, implicit]
LOWERCASE_DIMENSION = re.compile(r"[a-z_][23]d(_|$)")
# OpenGeode appends the dimension: load_horizons_stack2D, not ..._2D
UNDERSCORE_DIMENSION = re.compile(r"_[23]D(_|$)")
CAMEL_CASE = re.compile(r"^[A-Z][A-Za-z0-9]*$")


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def enums():
    for module in MODULES:
        for name, member in vars(module).items():
            if inspect.isclass(member) and issubclass(member, enum.Enum):
                yield module, None, name, member
            if inspect.isclass(member) and member.__module__ == module.__name__:
                for member_name, value in vars(member).items():
                    if inspect.isclass(value) and issubclass(value, enum.Enum):
                        yield module, member, member_name, value


def test_enum_class_names():
    count = 0
    for module, owner, name, cls in enums():
        count += 1
        check(CAMEL_CASE.match(name), f"enum {name} should be a CamelCase class name")
        check(cls.__doc__ and cls.__doc__.strip(), f"enum {name} has no docstring")
        check(
            len(cls) > 1,
            f"enum {name} should have members",
        )
    check(count >= 2, f"{count} enums found, expected at least 2")


def test_enum_values_not_exported():
    # export_values() would put every member at the scope of its enum
    for module, owner, name, cls in enums():
        scope = owner if owner is not None else module
        for member in cls:
            check(
                not hasattr(scope, member.name) or getattr(scope, member.name) is not member,
                f"{member.name} of {name} should not be exported in its scope",
            )


def test_dimension_suffix():
    for module in MODULES:
        for name, member in vars(module).items():
            if name.startswith("_"):
                continue
            check(
                not LOWERCASE_DIMENSION.search(name),
                f"{module.__name__}.{name}: use 2D/3D, not 2d/3d",
            )
            check(
                not UNDERSCORE_DIMENSION.search(name),
                f"{module.__name__}.{name}: append the dimension (names2D)",
            )
            if inspect.isclass(member) and member.__module__ == module.__name__:
                for member_name in vars(member):
                    check(
                        not LOWERCASE_DIMENSION.search(member_name),
                        f"{name}.{member_name}: use 2D/3D, not 2d/3d",
                    )


def test_static_methods():
    for module in MODULES:
        for name, member in vars(module).items():
            if inspect.isclass(member) and member.__module__ == module.__name__:
                for member_name, value in vars(member).items():
                    if member_name.endswith("_static"):
                        check(
                            isinstance(value, staticmethod),
                            f"{name}.{member_name} should be a static method",
                        )


if __name__ == "__main__":
    test_enum_class_names()
    test_enum_values_not_exported()
    test_dimension_suffix()
    test_static_methods()
