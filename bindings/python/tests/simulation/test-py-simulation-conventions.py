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

"""Conventions of the simulation bindings: enums, representations, keyword
arguments, checked indices, lifetimes and the report of the checks."""

import gc
import weakref

import geode_test_env

import opengeode
import opengeode_geosciences_py_simulation as simulation


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def check_raises(function, error, message):
    try:
        function()
    except error:
        return
    raise ValueError(f"[Test] {message} should raise {error.__name__}")


def test_enums():
    expected = {
        simulation.Phase: ["oil", "gas", "water"],
        simulation.CompositionBasis: ["mole", "mass"],
        simulation.Co2BrineCorrelation: ["phillips", "ezrokhi"],
        simulation.SolidMechanicsLaw: ["linear_elastic_isotropic"],
        simulation.StressSignConvention: ["compression_positive", "tension_positive"],
        simulation.SimulationIssueSeverity: ["error", "warning"],
        simulation.SimulationCheckStage: [
            "problem",
            "regions",
            "mesh",
            "properties",
            "simulator",
        ],
        simulation.SimulationElementType: ["none", "vertex", "polygon", "polyhedron"],
    }
    for enum_class, names in expected.items():
        check([member.name for member in enum_class] == names, enum_class.__name__)
    check(simulation.phase_name(simulation.Phase.gas) == "gas", "phase_name")
    check(
        simulation.phase_name(phase=simulation.Phase.water) == "water", "phase_name"
    )
    check(
        simulation.simulation_check_stage_string(simulation.SimulationCheckStage.regions)
        == "Region assignment",
        "simulation_check_stage_string",
    )
    mechanics = simulation.SolidMechanics(law=simulation.SolidMechanicsLaw.linear_elastic_isotropic)
    check(
        mechanics.sign_convention == simulation.StressSignConvention.compression_positive,
        "default sign convention",
    )
    check(
        "StressSignConvention.compression_positive"
        in simulation.SolidMechanics.__init__.__doc__,
        "the default value is shown with the enum name",
    )


def test_evaluable_representations():
    values = [
        simulation.FluidComponent(name="CO2", molar_weight=0.044),
        simulation.ValidityRange(min=1.0, max=2.0, step=0.5),
        simulation.Schedule(start_time=0.0, end_time=10.0, initial_time_step=1.0),
        simulation.Schedule(
            start_time=0.0, end_time=10.0, initial_time_step=1.0, max_time_step=2.0
        ),
        simulation.SourceTerm(well="W1", component="oil", rate=-1.5),
        simulation.ThermalParameters(reference_temperature=300.0),
        simulation.SaturationTable(saturations=[0.0, 1.0], values=[0.0, 0.5]),
    ]
    for value in values:
        text = repr(value)
        copy = eval(text, vars(simulation))
        check(repr(copy) == text, f"repr of {type(value).__name__} is evaluable: {text}")
    check(
        repr(values[0]) == "FluidComponent(name='CO2', molar_weight=0.044)",
        repr(values[0]),
    )
    check(
        repr(values[2])
        == "Schedule(start_time=0.0, end_time=10.0, initial_time_step=1.0, max_time_step=None)",
        repr(values[2]),
    )


def test_descriptive_representations():
    material = simulation.Material()
    material.name = "Sand"
    material.regions = [opengeode.uuid(), opengeode.uuid()]
    check(repr(material) == "Material(name='Sand', nb_regions=2)", repr(material))
    table = simulation.HydrocarbonPvtTable()
    table.pressures = [1.0, 2.0]
    check("phase=" in repr(table) and "nb_pressures=2" in repr(table), repr(table))
    check(repr(simulation.WaterPvt()).startswith("WaterPvt(reference_pressure=0.0"), "WaterPvt")
    check("specific_heat_capacity=0.0" in repr(simulation.FluidThermalProperties()), "thermal")
    check("volumetric_heat_capacity=0.0" in repr(simulation.RockThermalProperties()), "rock")
    check("binary=True" in repr(simulation.OutputSettings()), "OutputSettings")
    check("law=" in repr(simulation.SolidMechanics()), "SolidMechanics")
    check("wetting_phase=" in repr(simulation.TwoPhaseRelativePermeabilityTable()), "kr")
    check("pressure=" in repr(simulation.TwoPhaseCapillaryPressureTable()), "pc")
    problem = simulation.PhysicalProblem()
    text = repr(problem)
    check(
        text.startswith("PhysicalProblem(id='")
        and "has_flow=False" in text
        and "nb_materials=0" in text,
        text,
    )


def test_fields_have_named_setters():
    # def_readwrite exposes the setter argument as arg0
    for cls in [simulation.FluidComponent, simulation.Schedule, simulation.Material]:
        for name, member in vars(cls).items():
            if isinstance(member, property) and member.fset is not None:
                check("arg0" not in (member.fset.__doc__ or ""), f"{cls.__name__}.{name}")
                check("value" in (member.fset.__doc__ or ""), f"{cls.__name__}.{name}")


def build_problem():
    problem = simulation.PhysicalProblem()
    builder = simulation.PhysicalProblemBuilder(problem=problem)
    flow = simulation.SinglePhaseFlow()
    flow.set_fluid(
        fluid=simulation.SinglePhaseFluid(
            phases=[simulation.Phase.water],
            components=[simulation.FluidComponent("water", 0.018)],
            reference_density=1000.0,
            reference_viscosity=1e-3,
            compressibility=1e-9,
            viscosibility=0.0,
            reference_pressure=1e5,
        )
    )
    builder.set_flow(flow=flow)
    builder.set_thermal(thermal=simulation.ThermalParameters(300.0))
    builder.set_solid_mechanics(
        mechanics=simulation.SolidMechanics(simulation.SolidMechanicsLaw.linear_elastic_isotropic)
    )
    builder.set_schedule(schedule=simulation.Schedule(0.0, 1.0, 0.1))
    builder.set_output(output=simulation.OutputSettings())
    builder.set_gravity(gravity=opengeode.Vector3D([0.0, 0.0, -1.0]))
    builder.set_model_id(model_id=opengeode.uuid())
    builder.set_solver_option(simulator="geos", key="a", value="1")
    return problem, builder


def test_builder_arguments_and_indices():
    problem, builder = build_problem()
    check(problem.solver_options().get(simulator="geos", key="a") == "1", "solver")
    check(problem.solver_options().options(simulator="geos") == {"a": "1"}, "options")
    check(problem.solver_options().get("geos", "b") is None, "missing option")
    material_id = builder.add_material(material=simulation.Material())
    builder.set_material(material_id=material_id, material=simulation.Material())
    source_id = builder.add_source_term(source_term=simulation.SourceTerm("w", "water", 1.0))
    condition_id = builder.add_initial_condition(
        initial_condition=simulation.UniformInitialCondition([], 1e5)
    )
    check((material_id, source_id, condition_id) == (0, 0, 0), "indices")
    check(
        problem.initial_condition(initial_condition_id=0).pressure() == 1e5,
        "initial_condition",
    )
    # Out of range indices raise a data error
    for call in [
        lambda: problem.initial_condition(5),
        lambda: builder.set_material(5, simulation.Material()),
        lambda: builder.remove_material(5),
        lambda: builder.remove_initial_condition(5),
        lambda: builder.remove_source_term(5),
    ]:
        check_raises(call, opengeode.OpenGeodeDataError, "an index out of range")
    check_raises(
        lambda: simulation.PhysicalProblem().flow(),
        opengeode.OpenGeodeDataError,
        "flow of a problem without flow",
    )
    check_raises(
        lambda: simulation.PhysicalProblem().fluid(),
        opengeode.OpenGeodeDataError,
        "fluid of a problem without flow",
    )
    check_raises(
        lambda: simulation.SinglePhaseFlow().fluid(),
        opengeode.OpenGeodeDataError,
        "fluid of a flow without fluid",
    )
    check_raises(
        lambda: simulation.SinglePhaseFlow().set_fluid(None),
        opengeode.OpenGeodeDataError,
        "set_fluid with None",
    )
    builder.remove_material(material_id=0)
    builder.remove_source_term(source_term_id=0)
    builder.remove_initial_condition(initial_condition_id=0)
    builder.remove_solid_mechanics()
    check(problem.solid_mechanics() is None, "remove_solid_mechanics")
    check(problem.thermal().reference_temperature == 300.0, "thermal")
    other = simulation.PhysicalProblem()
    simulation.PhysicalProblemBuilder(other).copy(other=problem)
    check(other.has_flow() and other.thermal() is not None, "copy")
    check(problem.clone().has_flow(), "clone")


def test_builder_keeps_problem_alive():
    problem = simulation.PhysicalProblem()
    reference = weakref.ref(problem)
    builder = simulation.PhysicalProblemBuilder(problem)
    del problem
    gc.collect()
    check(reference() is not None, "The builder keeps its problem alive")
    del builder
    gc.collect()
    check(reference() is None, "The problem is released with its builder")


def test_checker_factory():
    check(isinstance(simulation.SimulationCheckerFactory.list_creators(), list), "list")
    check(
        not simulation.SimulationCheckerFactory.has_creator(key="unknown_simulator"),
        "has_creator",
    )
    problem = simulation.PhysicalProblem()
    check_raises(
        lambda: simulation.SimulationCheckerFactory.create(
            key="unknown_simulator", problem=problem, model=opengeode.BRep()
        ),
        opengeode.OpenGeodeError,
        "create a checker for an unknown simulator",
    )


def test_report():
    report = simulation.SimulationReport()
    report.set_simulator(simulator="GEOS")
    report.set_model_name(model_name="model")
    location = simulation.SimulationIssueLocation()
    location.name = "Block"
    location.element_type = simulation.SimulationElementType.polyhedron
    location.elements = [1, 2]
    location.nb_elements = 10
    report.add_issue(
        severity=simulation.SimulationIssueSeverity.warning,
        stage=simulation.SimulationCheckStage.mesh,
        subject="porosity",
        message="ignored",
        fix="remove it",
        locations=[location],
    )
    report.add_error(
        stage=simulation.SimulationCheckStage.problem, subject="Material", message="m"
    )
    report.add_warning(
        stage=simulation.SimulationCheckStage.problem, subject="Schedule", message="s"
    )
    check(report.nb_errors() == 1 and report.nb_warnings() == 2, "counts")
    check(not report.is_simulable(), "not simulable")
    check(len(report.issues()) == 3, "issues")
    check(
        len(report.stage_issues(stage=simulation.SimulationCheckStage.problem)) == 2,
        "stage_issues",
    )
    issue = report.stage_issues(simulation.SimulationCheckStage.mesh)[0]
    check(issue.fix == "remove it" and not issue.is_error(), "issue")
    check(issue.locations[0].elements == [1, 2], "locations")
    check(issue.severity == simulation.SimulationIssueSeverity.warning, "severity")
    text = repr(report)
    check(
        text == "SimulationReport(simulator='GEOS', model_name='model', nb_errors=1, nb_warnings=2)",
        text,
    )
    check(str(issue.locations[0]).startswith("Block") or "Block" in str(issue.locations[0]), "location str")
    check(
        repr(issue).startswith("SimulationIssue(severity="), repr(issue)
    )
    check(report.string(), "string")
    check(
        repr(issue.locations[0]).startswith("SimulationIssueLocation(component="),
        repr(issue.locations[0]),
    )


if __name__ == "__main__":
    simulation.OpenGeodeGeosciencesSimulationLibrary.initialize()
    test_enums()
    test_evaluable_representations()
    test_descriptive_representations()
    test_fields_have_named_setters()
    test_builder_arguments_and_indices()
    test_builder_keeps_problem_alive()
    test_checker_factory()
    test_report()
