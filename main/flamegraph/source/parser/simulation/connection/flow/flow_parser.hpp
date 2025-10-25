#pragma once
#include <spdlog/fmt/fmt.h>

#include <memory>

#include "connection/flow/tcp/tcp_flow.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

class FlowParser {
public:
    // ATTENTION: do not change std::shared_ptr with std::unique_ptr (because
    // TcpFlow is inherit from std::enable_shared_from_this)
    static std::shared_ptr<IFlow> parse_i_flow(
        const ConfigNode& node, std::shared_ptr<IConnection> connection);
};

}  // namespace sim
