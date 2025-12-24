#include "simulate.hpp"

#include "metrics/metrics_collector.hpp"

namespace websocket {
Simulate::Simulate(const nlohmann::json& json)
    : m_output_dir(json.at("output_dir")) {}

Response Simulate::apply_to_simulator(sim::Simulator& simulator) {
    sim::MetricsCollector::get_instance().clear();
    sim::Scheduler::get_instance().clear();
    nlohmann::json backup = simulator.to_json();
    simulator.start();
    sim::MetricsCollector::get_instance().draw_metric_plots(m_output_dir);
    if (auto build_res = simulator.build_from_json(backup);
        !build_res.has_value()) {
        LOG_ERROR(fmt::format("Error while rebuilding from backup: {}",
                              build_res.error()));
    }
    return EmptyMessage;
}

}  // namespace websocket
