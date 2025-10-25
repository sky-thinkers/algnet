#include "tcp_flow_parser.hpp"

#include "cc/tcp_cc_parser.hpp"
#include "connection/i_connection.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

std::shared_ptr<TcpFlow> TcpFlowParser::parse_tcp_flow(
    const ConfigNode& node, std::shared_ptr<IConnection> connection) {
    Id flow_id = connection->get_id() + "_" + node.get_name_or_throw();
    std::unique_ptr<ITcpCC> cc =
        TcpCCParser::parse_i_tcp_cc(node["cc"].value_or_throw());

    SizeByte packet_size =
        SizeByte(parse_size(node["packet_size"].value_or_throw()));

    return std::make_shared<TcpFlow>(flow_id, connection, std::move(cc),
                                     packet_size);
}

}  // namespace sim
