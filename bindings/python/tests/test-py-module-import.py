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

"""Each extension module must be importable alone, in a fresh interpreter: it
imports the modules it depends on."""

import subprocess
import sys

import geode_test_env

# Extension modules to check; the other modules are added with their commit
MODULES = [
    "opengeode_geosciences_py_explicit",
    "opengeode_geosciences_py_implicit",
    "opengeode_geosciences_py_simulation",
]


def check_import(module):
    result = subprocess.run(
        [
            sys.executable,
            "-c",
            f"import geode_test_env; import {module}; assert {module}.__doc__",
        ],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        lines = result.stderr.strip().splitlines()
        raise ValueError(
            f"[Test] {module} cannot be imported alone: "
            + (lines[-1] if lines else str(result.returncode))
        )


if __name__ == "__main__":
    for module in MODULES:
        check_import(module)
