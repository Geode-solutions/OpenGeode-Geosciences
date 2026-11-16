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

"""The exceptions of OpenGeode-Geosciences reach Python as the OpenGeodeError
hierarchy of opengeode, with the project and the library of the exception."""

import geode_test_env

import opengeode
import opengeode_geosciences as geosciences


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def check_data_error(function, library, extension):
    try:
        function()
    except opengeode.OpenGeodeDataError as error:
        check(isinstance(error, opengeode.OpenGeodeError), "an OpenGeodeError")
        check(isinstance(error, RuntimeError), "a RuntimeError")
        check(error.type == "data", f"Wrong type {error.type}")
        check(error.project == "OpenGeodeGeosciences", f"Wrong project {error.project}")
        check(error.library == library, f"Wrong library {error.library}")
        check(extension in error.message, f"Wrong message {error.message}")
        check(error.message in str(error), "str(error) contains the message")
        check(error.details, "Empty details")
        check(error.__cause__ is None, "Unexpected cause")
        return
    raise ValueError(f"[Test] {library}: OpenGeodeDataError expected")


if __name__ == "__main__":
    check_data_error(
        lambda: geosciences.load_structural_model("file.unknown_extension"),
        "Explicit",
        "unknown_extension",
    )
    check_data_error(
        lambda: opengeode.load(
            geosciences.ImplicitStructuralModel, "file.unknown_extension"
        ),
        "Implicit",
        "unknown_extension",
    )
    check_data_error(
        lambda: opengeode.load(geosciences.PhysicalProblem, "file.unknown_extension"),
        "Simulation",
        "unknown_extension",
    )
