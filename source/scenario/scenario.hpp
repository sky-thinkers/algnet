#pragma once

#include <memory>
#include <vector>

#include "action/i_action.hpp"
#include "websocket/i_serializable.hpp"

namespace sim {

class Scenario : public virtual websocket::ISerializable {
public:
    Scenario() = default;
    ~Scenario() = default;

    Scenario(const Scenario&) = delete;
    Scenario& operator=(const Scenario&) = delete;

    Scenario(Scenario&&) noexcept = default;
    Scenario& operator=(Scenario&&) noexcept = default;

    // Add a new action to the scenario
    void add_action(std::unique_ptr<IAction> action);

    // Run all actions (schedule them in the simulator)
    void start();
    nlohmann::json to_json() const final;

private:
    std::vector<std::unique_ptr<IAction>> m_actions;
};

}  // namespace sim
