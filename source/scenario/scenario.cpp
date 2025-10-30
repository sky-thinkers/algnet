#include "scenario.hpp"

namespace sim {

void Scenario::add_action(std::unique_ptr<IAction> action) {
    m_actions.emplace_back(std::move(action));
}

void Scenario::start() {
    for (auto& action : m_actions) {
        action->schedule();
    }
}

nlohmann::json Scenario::to_json() const {
    nlohmann::json json = nlohmann::json::array();
    for (const auto& action : m_actions) {
        json.emplace_back(action->to_json());
    }
    return json;
}

}  // namespace sim
