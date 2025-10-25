#pragma once
#include <expected>
#include <variant>

#include "../response.hpp"
#include "simulator.hpp"
#include "types.hpp"

namespace websocket {

class IRequest {
    [[nodiscard]] virtual Response apply_to_simulator(
        sim::Simulator& simlator) = 0;
};

}  // namespace websocket
