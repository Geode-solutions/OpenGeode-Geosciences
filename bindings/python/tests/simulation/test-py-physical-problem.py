# -*- coding: utf-8 -*-
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

import os
import sys
import platform

if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ["PATH"].split(";") if x]:
        os.add_dll_directory(path)

import opengeode
import opengeode_geosciences_py_simulation as simulation

PHASES = [simulation.PHASE.oil, simulation.PHASE.gas, simulation.PHASE.water]


def spe10_dead_oil():
    oil_table = simulation.HydrocarbonPvtTable()
    oil_table.phase = simulation.PHASE.oil
    oil_table.pressures = [1e5, 1e7, 5e7]
    oil_table.formation_volume_factors = [1.02, 1.01, 0.99]
    oil_table.viscosities = [2.9e-3, 3e-3, 3.1e-3]
    gas_table = simulation.HydrocarbonPvtTable()
    gas_table.phase = simulation.PHASE.gas
    gas_table.pressures = [1e5, 1e7, 5e7]
    gas_table.formation_volume_factors = [0.9, 0.012, 0.003]
    gas_table.viscosities = [1e-5, 1.5e-5, 2.5e-5]
    water_pvt = simulation.WaterPvt()
    water_pvt.reference_pressure = 3.0e7
    water_pvt.formation_volume_factor = 1.03
    water_pvt.compressibility = 4.4e-10
    water_pvt.viscosity = 3e-4
    return simulation.DeadOilFluid(
        PHASES,
        [
            simulation.FluidComponent("oil", 0.114),
            simulation.FluidComponent("gas", 0.016),
            simulation.FluidComponent("water", 0.018),
        ],
        [800.0, 0.9907, 1022.0],
        [oil_table, gas_table],
        water_pvt,
    )


def reservoir_material(name, relperm, capillary_pressure=None):
    material = simulation.Material()
    material.name = name
    material.pore_compressibility = 1e-9
    material.reference_pressure = 1e5
    material.porosity = 0.2
    material.permeability = [1e-13, 1e-13, 1e-14]
    material.relative_permeability = relperm
    material.capillary_pressure = capillary_pressure
    return material


def table_capillary_pressure():
    table = simulation.TwoPhaseCapillaryPressureTable()
    table.wetting_phase = simulation.PHASE.water
    table.non_wetting_phase = simulation.PHASE.oil
    table.pressure = simulation.SaturationTable([0.2, 0.6, 1.0], [1e5, 1e4, 0.0])
    return simulation.TableCapillaryPressure(
        [simulation.PHASE.oil, simulation.PHASE.water], [table]
    )


def dead_oil_problem(model_id, block_id):
    problem = simulation.PhysicalProblem()
    builder = simulation.PhysicalProblemBuilder(problem)
    builder.set_name("spe10")
    builder.set_model_id(model_id)
    flow = simulation.MultiphaseFlow(simulation.COMPOSITION_BASIS.mass)
    flow.set_fluid(spe10_dead_oil())
    builder.set_flow(flow)
    relperm = simulation.BrooksCoreyRelativePermeability(
        PHASES, [0.1, 0.0, 0.2], [2.0, 2.0, 2.0], [1.0, 1.0, 1.0]
    )
    top = reservoir_material("top", relperm, table_capillary_pressure())
    top.regions = [block_id]
    builder.add_material(top)
    builder.add_material(reservoir_material("default", relperm))
    builder.add_initial_condition(
        simulation.UniformInitialCondition([], 4.1369e7, 300.0, [0.9995, 0.0, 0.0005])
    )
    builder.set_schedule(simulation.Schedule(0.0, 2e6, 1e3, 5e5))
    output = simulation.OutputSettings()
    output.frequency = 1e5
    output.restart = True
    output.restart_frequency = 1e6
    builder.set_output(output)
    builder.add_source_term(simulation.SourceTerm("injector", "water", 10.0))
    builder.set_solver_option("geos", "nonlinear/newtonTol", "1e-4")
    return problem


def check_multiphase_flow(problem):
    if problem.name() != "spe10":
        raise ValueError("[Test] Wrong problem name")
    if problem.flow().type_name() != simulation.MultiphaseFlow.type_name_static():
        raise ValueError("[Test] Wrong flow type")
    if problem.flow().basis() != simulation.COMPOSITION_BASIS.mass:
        raise ValueError("[Test] Wrong composition basis")


def check_dead_oil_fluid(problem):
    fluid = problem.fluid()
    if not isinstance(fluid, simulation.DeadOilFluid):
        raise ValueError("[Test] Fluid should be returned as a DeadOilFluid")
    if [component.name for component in fluid.components()] != [
        "oil",
        "gas",
        "water",
    ]:
        raise ValueError("[Test] Wrong fluid components")
    if [table.phase for table in fluid.hydrocarbon_pvt()] != [
        simulation.PHASE.oil,
        simulation.PHASE.gas,
    ]:
        raise ValueError("[Test] Wrong hydrocarbon PVT tables")
    if fluid.water_pvt().formation_volume_factor != 1.03:
        raise ValueError("[Test] Wrong water PVT")
    if simulation.phase_component(fluid, simulation.PHASE.water) != 2:
        raise ValueError("[Test] Wrong water component")


def check_top_material(material, block_id):
    if material.regions != [block_id]:
        raise ValueError("[Test] Wrong material regions")
    if material.permeability != [1e-13, 1e-13, 1e-14] or material.porosity != 0.2:
        raise ValueError("[Test] Wrong rock properties")
    if not isinstance(
        material.relative_permeability, simulation.BrooksCoreyRelativePermeability
    ):
        raise ValueError("[Test] Wrong relative permeability type")
    if material.relative_permeability.residual_saturations() != [0.1, 0.0, 0.2]:
        raise ValueError("[Test] Wrong relative permeability values")
    pc_tables = material.capillary_pressure.tables()
    if len(pc_tables) != 1 or pc_tables[0].pressure.values != [1e5, 1e4, 0.0]:
        raise ValueError("[Test] Wrong capillary pressure table")


def check_materials(problem, block_id):
    materials = problem.materials()
    if [material.name for material in materials] != ["top", "default"]:
        raise ValueError("[Test] Wrong material names")
    check_top_material(materials[0], block_id)
    if materials[1].capillary_pressure is not None:
        raise ValueError("[Test] Default material should have no capillary pressure")


def check_dead_oil_initial_condition(problem):
    if problem.nb_initial_conditions() != 1:
        raise ValueError("[Test] Wrong number of initial conditions")
    condition = problem.initial_condition(0)
    if not isinstance(condition, simulation.UniformInitialCondition):
        raise ValueError("[Test] Wrong initial condition type")
    if condition.pressure() != 4.1369e7 or condition.temperature() != 300.0:
        raise ValueError("[Test] Wrong initial condition values")
    if condition.composition() != [0.9995, 0.0, 0.0005]:
        raise ValueError("[Test] Wrong initial composition")


def check_schedule_and_output(problem):
    schedule = problem.schedule()
    if schedule.end_time != 2e6 or schedule.max_time_step != 5e5:
        raise ValueError("[Test] Wrong schedule")
    output = problem.output()
    if output.frequency != 1e5 or not output.restart:
        raise ValueError("[Test] Wrong output settings")


def check_source_terms(problem):
    source_terms = problem.source_terms()
    if len(source_terms) != 1 or source_terms[0].well != "injector":
        raise ValueError("[Test] Wrong source terms")


def check_solver_settings(problem):
    if problem.solver_options().get("geos", "nonlinear/newtonTol") != "1e-4":
        raise ValueError("[Test] Wrong solver option")
    if problem.gravity().value(2) != -9.81:
        raise ValueError("[Test] Wrong default gravity")


def check_dead_oil_problem(problem, block_id):
    check_multiphase_flow(problem)
    check_dead_oil_fluid(problem)
    check_materials(problem, block_id)
    check_dead_oil_initial_condition(problem)
    check_schedule_and_output(problem)
    check_source_terms(problem)
    check_solver_settings(problem)


def check_valid_problem(problem):
    issues = simulation.validate_physical_problem(problem)
    if issues:
        raise ValueError("[Test] Problem should be valid: " + " ".join(issues))


def save_and_reload(problem, filename):
    if not simulation.is_physical_problem_saveable(problem, filename):
        raise ValueError("[Test] Problem should be saveable")
    simulation.save_physical_problem(problem, filename)
    if simulation.is_physical_problem_loadable(filename).value() != 1:
        raise ValueError("[Test] Problem should be loadable")
    return simulation.load_physical_problem(filename)


def check_model_id(problem, model_id):
    if problem.model_id().string() != model_id.string():
        raise ValueError("[Test] Wrong model id")


def test_dead_oil_roundtrip():
    model_id = opengeode.uuid()
    block_id = opengeode.uuid()
    problem = dead_oil_problem(model_id, block_id)
    check_valid_problem(problem)
    check_dead_oil_problem(problem, block_id)
    # Reading Span getters twice checks that they copy the C++ elements
    check_dead_oil_problem(problem, block_id)
    reloaded = save_and_reload(problem, "dead_oil." + problem.native_extension())
    check_dead_oil_problem(reloaded, block_id)
    check_model_id(reloaded, model_id)
    check_dead_oil_problem(reloaded.clone(), block_id)


def thermal_water_fluid():
    thermal = simulation.FluidThermalProperties()
    thermal.specific_heat_capacity = 4180.0
    return simulation.SinglePhaseFluid(
        [simulation.PHASE.water],
        [simulation.FluidComponent("water", 0.018)],
        reference_density=1000.0,
        reference_viscosity=1e-3,
        compressibility=1e-9,
        viscosibility=0.0,
        reference_pressure=1e5,
        thermal=thermal,
    )


def thermal_rock():
    material = simulation.Material()
    material.name = "rock"
    material.porosity = 0.2
    material.permeability = [1e-13, 1e-13, 1e-13]
    rock_thermal = simulation.RockThermalProperties()
    rock_thermal.volumetric_heat_capacity = 2e6
    rock_thermal.thermal_conductivity = [2.0, 2.0, 2.0]
    material.thermal = rock_thermal
    return material


def thermal_single_phase_problem():
    problem = simulation.PhysicalProblem()
    builder = simulation.PhysicalProblemBuilder(problem)
    flow = simulation.SinglePhaseFlow()
    flow.set_fluid(thermal_water_fluid())
    builder.set_flow(flow)
    builder.set_thermal(simulation.ThermalParameters(300.0))
    builder.add_material(thermal_rock())
    builder.add_initial_condition(
        simulation.UniformInitialCondition([], 1.5e7, temperature=320.0)
    )
    builder.set_schedule(simulation.Schedule(0.0, 100.0, 10.0))
    builder.set_gravity(opengeode.Vector3D([0.0, 0.0, 0.0]))
    return problem


def check_thermal_properties(problem):
    if problem.fluid().thermal().specific_heat_capacity != 4180.0:
        raise ValueError("[Test] Wrong fluid thermal properties")
    if problem.thermal().reference_temperature != 300.0:
        raise ValueError("[Test] Wrong thermal parameters")
    if problem.materials()[0].thermal.thermal_conductivity != [2.0, 2.0, 2.0]:
        raise ValueError("[Test] Wrong rock thermal properties")


def check_zero_gravity(problem):
    if problem.gravity().value(2) != 0.0:
        raise ValueError("[Test] Wrong gravity")


def check_porosity_issue(problem):
    builder = simulation.PhysicalProblemBuilder(problem)
    material = problem.materials()[0]
    material.porosity = 1.5
    builder.set_material(0, material)
    if "porosity" not in " ".join(simulation.validate_physical_problem(problem)):
        raise ValueError("[Test] Porosity out of range should be reported")


def test_single_phase_problem():
    problem = thermal_single_phase_problem()
    check_valid_problem(problem)
    check_thermal_properties(problem)
    check_zero_gravity(problem)
    check_porosity_issue(problem)


def co2_brine_fluid():
    return simulation.Co2BrineFluid(
        [simulation.PHASE.gas, simulation.PHASE.water],
        [
            simulation.FluidComponent("co2", 0.044),
            simulation.FluidComponent("water", 0.018),
        ],
        simulation.CO2_BRINE_CORRELATION.phillips,
        1.0,
        simulation.ValidityRange(1e6, 5e7, 1e6),
        simulation.ValidityRange(290.0, 400.0, 5.0),
    )


def check_co2_brine_values(fluid):
    if fluid.pressure_range().max != 5e7 or fluid.salinity() != 1.0:
        raise ValueError("[Test] Wrong CO2-brine fluid")


def check_co2_brine_phases(fluid):
    if simulation.phase_component(fluid, simulation.PHASE.oil) is not None:
        raise ValueError("[Test] CO2-brine fluid should have no oil component")
    if simulation.phase_name(simulation.PHASE.gas) != "gas":
        raise ValueError("[Test] Wrong phase name")


def test_co2_brine_fluid():
    fluid = co2_brine_fluid()
    check_co2_brine_values(fluid)
    check_co2_brine_phases(fluid)


def block_in_collection_model():
    model = opengeode.BRep()
    model_builder = opengeode.BRepBuilder(model)
    block_id = model_builder.add_block()
    collection_id = model_builder.add_block_collection()
    model_builder.add_block_in_block_collection(
        model.block(block_id), model.block_collection(collection_id)
    )
    return model, block_id, collection_id


def set_first_material_regions(problem, regions):
    material = problem.materials()[0]
    material.regions = regions
    simulation.PhysicalProblemBuilder(problem).set_material(0, material)


def check_region_issues(problem, model, nb_issues, message):
    if len(simulation.validate_physical_problem_regions(problem, model)) != nb_issues:
        raise ValueError("[Test] " + message)


def test_region_validation():
    model, block_id, collection_id = block_in_collection_model()
    problem = dead_oil_problem(model.id(), block_id)
    check_region_issues(problem, model, 0, "Block region should be valid")
    set_first_material_regions(problem, [collection_id])
    check_region_issues(problem, model, 0, "Collection region should be valid")
    set_first_material_regions(problem, [opengeode.uuid()])
    check_region_issues(problem, model, 1, "Unknown region should be reported")


if __name__ == "__main__":
    simulation.OpenGeodeGeosciencesSimulationLibrary.initialize()
    test_dead_oil_roundtrip()
    test_single_phase_problem()
    test_co2_brine_fluid()
    test_region_validation()
