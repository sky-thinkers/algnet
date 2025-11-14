#include "simulate.hpp"

#include "metrics/metrics_collector.hpp"

namespace websocket {
Simulate::Simulate(const nlohmann::json& json)
    : m_output_dir(json.at("output_dir")) {}

Response Simulate::apply_to_simulator(sim::Simulator& simulator) {
    simulator.start();
    sim::MetricsCollector::get_instance().draw_metric_plots(m_output_dir);
    return EmptyMessage;
}

}  // namespace websocket
