#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "simulator.hpp"
#include "games/peek_baccarat.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pyaxiom, m) {
    m.doc() = "Axiom Monte Carlo Simulation Engine";

    py::class_<axiom::SimulationResult>(m, "SimulationResult")
        .def_readonly("round_id", &axiom::SimulationResult::round_id)
        .def_readonly("bet_amount", &axiom::SimulationResult::bet_amount)
        .def_readonly("peek_fee", &axiom::SimulationResult::peek_fee)
        .def_readonly("raise_amount", &axiom::SimulationResult::raise_amount)
        .def_readonly("win_loss", &axiom::SimulationResult::win_loss)
        .def_readonly("game_status", &axiom::SimulationResult::game_status)
        .def_readonly("detail", &axiom::SimulationResult::detail);

    py::class_<axiom::PeekBaccaratSimulator>(m, "PeekBaccaratSimulator")
        .def(py::init([](int shoe_count, int default_bet) {
            return std::make_unique<axiom::PeekBaccaratSimulator>(shoe_count, static_cast<axiom::BetType>(default_bet));
        }), py::arg("shoe_count") = 8, py::arg("default_bet") = 1)
        .def("run_simulation", &axiom::PeekBaccaratSimulator::RunSimulation, "Run Monte Carlo simulation", py::arg("rounds"))
        .def("set_seed", &axiom::PeekBaccaratSimulator::SetSeed, "Set seed for RNG", py::arg("seed"))
        .def("set_bet_type", &axiom::PeekBaccaratSimulator::SetBetType, "Set active bet type (0=Banker, 1=Player)", py::arg("bet_type"))
        .def("get_bet_type", &axiom::PeekBaccaratSimulator::GetBetType, "Get current bet type")
        .def("set_reveal_weights", &axiom::PeekBaccaratSimulator::SetRevealWeights, "Set reveal count weights (1,2,3,4)", py::arg("weights"));
}
