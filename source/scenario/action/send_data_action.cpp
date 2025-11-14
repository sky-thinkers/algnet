#include "send_data_action.hpp"

#include "event/add_data_to_connection.hpp"

namespace sim {

SendDataAction::SendDataAction(TimeNs a_when, SizeByte a_size,
                               std::vector<std::weak_ptr<IConnection>> a_conns,
                               int a_repeat_count, TimeNs a_repeat_interval,
                               TimeNs a_jitter)
    : m_when(a_when),
      m_size(a_size),
      m_conns(std::move(a_conns)),
      m_repeat_count(a_repeat_count),
      m_repeat_interval(a_repeat_interval),
      m_jitter(a_jitter) {}

void SendDataAction::schedule() {
    const bool use_jitter = m_jitter > TimeNs(0);

    for (auto& weak : m_conns) {
        auto conn = weak.lock();
        if (!conn) throw std::runtime_error("Expired connection in action");

        std::uint64_t seed = std::hash<std::string>{}(conn->get_id());
        std::mt19937_64 rng(seed);
        std::uniform_int_distribution<uint64_t> dist(
            0, m_jitter.value_nanoseconds());

        for (size_t i = 0; i < m_repeat_count; ++i) {
            TimeNs jitter_gap = use_jitter ? TimeNs(dist(rng)) : TimeNs(0);
            Scheduler::get_instance().add<AddDataToConnection>(
                m_when + i * m_repeat_interval + jitter_gap, conn, m_size);
        }
    }
}

nlohmann::json SendDataAction::to_json() const {
    nlohmann::json json;
    nlohmann::json connection_ids = nlohmann::json::array();
    for (const auto& connection : m_conns) {
        if (connection.expired()) {
            continue;
        }
        connection_ids.emplace_back(connection.lock()->get_id());
    }

    json["connection_ids"] = connection_ids;
    json["size"] =
        fmt::format("{}b", std::uint32_t(std::round(m_size.value())));
    return json;
}

}  // namespace sim
