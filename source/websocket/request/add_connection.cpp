#include "add_connection.hpp"

#include "connection/connection_impl.hpp"
#include "connection/flow/tcp/tahoe/tcp_tahoe_cc.hpp"
#include "connection/flow/tcp/tcp_flow.hpp"
#include "connection/mplb/round_robin_mplb.hpp"
#include "event/add_data_to_connection.hpp"
#include "parser/parse_utils.hpp"
#include "scheduler.hpp"
#include "utils/identifier_factory.hpp"

namespace websocket {

AddConnection::AddConnection(nlohmann::json a_json) : m_json(a_json) {}

Response AddConnection::apply_to_simulator(sim::Simulator& simulator) {
    try {
        // parse json
        Id name = m_json.at("name");
        Id sender_id = m_json.at("sender_id");
        Id receiver_id = m_json.at("receiver_id");

        std::string str_data_to_send = m_json.at("data_to_send");
        utils::StrExpected<SizeByte> maybe_data_to_send =
            sim::parse_size(str_data_to_send);
        if (!maybe_data_to_send.has_value()) {
            return ErrorResponseData(
                fmt::format("Can not parse size from '{}'", str_data_to_send));
        }
        SizeByte data_to_send = maybe_data_to_send.value();
        SizeByte packet_size(1500);

        {
            // use parsed values
            const sim::IdentifierFactory& idf =
                sim::IdentifierFactory::get_instance();
            std::shared_ptr<sim::IHost> sender =
                idf.get_object<sim::IHost>(sender_id);
            if (sender == nullptr) {
                return ErrorResponseData(fmt::format(
                    "Could not find 'sender' device with id {}", sender_id));
            }

            std::shared_ptr<sim::IHost> receiver =
                idf.get_object<sim::IHost>(receiver_id);
            if (receiver == nullptr) {
                return ErrorResponseData(
                    fmt::format("Could not find 'receiver' device with id {}",
                                receiver_id));
            }

            std::shared_ptr<sim::ConnectionImpl> connection =
                std::make_shared<sim::ConnectionImpl>(
                    name, sender, receiver,
                    std::make_shared<sim::RoundRobinMPLB>());

            // ATTENTION: DO NOT PLACE THIS CALL AFTER connection->add_flow
            // (it will trigger data sending)
            connection->add_data_to_send(data_to_send);

            Id flow_name = fmt::format("{}_flow", name);
            std::unique_ptr<sim::TcpTahoeCC> tahoe_cc =
                std::make_unique<sim::TcpTahoeCC>();
            std::shared_ptr<sim::TcpFlow> flow = std::make_shared<sim::TcpFlow>(
                flow_name, connection, std::move(tahoe_cc), packet_size);

            connection->add_flow(flow);

            auto result = simulator.add_connection(connection);
            if (!result.has_value()) {
                return ErrorResponseData(result.error());
            }
            return EmptyMessage;
        }
    } catch (const nlohmann::json::exception& e) {
        return ErrorResponseData(
            fmt::format("Error from json parser: {}", e.what()));
    }
}

}  // namespace websocket
