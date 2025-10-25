#include "summary.hpp"

#include <spdlog/fmt/ranges.h>

#include "filesystem.hpp"

namespace sim {

Summary::Summary(std::map<Id, std::map<Id, FlowSummary>> a_values)
    : m_values(std::move(a_values)) {}

Summary::Summary(
    const std::unordered_set<std::shared_ptr<IConnection>>& connections) {
    for (const auto& conn : connections) {
        Id conn_id = conn->get_id();
        SizeByte expt_data_delivery = conn->get_total_data_added();
        if (expt_data_delivery == SizeByte(0)) {
            m_warnings.emplace_back(fmt::format(
                "Connection {} has 0 expected data delivery", conn_id));
        }
        SizeByte real_data_delivery(0);
        auto flows = conn->get_flows();

        for (const auto& flow : flows) {
            const SizeByte added_from_conn =
                flow->get_total_data_size_added_from_conn();
            const SizeByte delivered = flow->get_delivered_data_size();
            real_data_delivery += delivered;

            if (added_from_conn > delivered) {
                m_errors.emplace_back(fmt::format(
                    "For flow {} of connection {} added from connection {} "
                    "bytes but delivered {} bytes",
                    flow->get_id(), conn_id, added_from_conn.value(),
                    delivered.value()));
            }

            const SizeByte sent = flow->get_sent_data_size();
            if (sent < delivered) {
                m_errors.emplace_back(fmt::format(
                    "For flow {} of connection {} sent {} bytes but delivered "
                    "{} bytes",
                    flow->get_id(), conn_id, sent.value(), delivered.value()));
            }

            double overhead = delivered != SizeByte(0)
                                  ? (sent / delivered - 1) * 100
                                  : std::nan("");

            const TimeNs fct = flow->get_fct();
            SizeByte packet_size = flow->get_packet_size();

            SpeedGbps sending_rate = sent / fct;
            SpeedGbps throughput = delivered / fct;

            uint32_t retransmit_count = flow->retransmit_count();
            SizeByte retransmit_size = retransmit_count * packet_size;

            m_values[conn_id][flow->get_id()] =
                FlowSummary{added_from_conn,  sent,
                            delivered,        packet_size,
                            overhead,         retransmit_size,
                            retransmit_count, sending_rate,
                            throughput,       fct};
        }
        if (expt_data_delivery > real_data_delivery) {
            m_errors.emplace_back(fmt::format(
                "For connection {} expected delivery {} but real is {}",
                conn_id, expt_data_delivery.value(),
                real_data_delivery.value()));
        }
        if (expt_data_delivery == SizeByte(0) &&
            real_data_delivery > SizeByte(0)) {
            m_errors.emplace_back(fmt::format(
                "For connection {} expected delivery 0 but real is {}", conn_id,
                real_data_delivery.value()));
        }
        // Case where expt_data_delivery != real_data_delivery is not
        // always an error, because in our model, if there is less data to send
        // than the size of the flow's packet, the flow still sends a full-sized
        // packet.
        // For example, if there is 1 byte to send and the packet size is 1500
        // bytes, the flow will send a full 1500-byte packet.
        // For this reason we do not consider this case an error.
    }
}

void Summary::write_to_csv(std::filesystem::path& output_path) const {
    utils::create_all_directories(output_path);
    std::ofstream out(output_path);
    if (!out) {
        throw std::runtime_error("Failed to create file for summary");
    }
    out << "Flow id, Packet size (bytes), Data from conn (bytes), Sent "
           "(bytes), Delivered (bytes), "
           "Overhead (%), Retransmit size (bytes), Retransmit count, Sending "
           "rate (Gbps), Throughput (Gbps), FCT (ns)\n";
    for (const auto& [conn_id, flows] : m_values) {
        out << "\n";
        for (const auto& [flow_id, fs] : flows) {
            out << flow_id << ", " << fs.packet_size.value() << ", "
                << fs.added_from_conn.value() << ", " << fs.sent.value() << ", "
                << fs.delivered.value() << ", " << fs.overhead << ", "
                << fs.retransmit_size.value() << ", " << fs.retransmit_count
                << ", " << fs.sending_rate.value() << ", "
                << fs.throughput.value() << ", " << fs.fct.value() << "\n";
        }
    }
    out.close();
}

void Summary::check() const {
    if (!m_warnings.empty()) {
        LOG_WARN(
            fmt::format("Summary warnings: {}", fmt::join(m_warnings, "\n")));
    }
    if (!m_errors.empty()) {
        throw std::runtime_error(
            fmt::format("Summary errors: {}", fmt::join(m_errors, "\n")));
    }
}
}  // namespace sim
