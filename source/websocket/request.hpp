#pragma once
#include <expected>
#include <string>
#include <variant>

#include "simulator.hpp"
#include "types.hpp"

namespace websocket {

using ErrorType = std::string;
using SuccessType = std::string;
using ApplyResult = std::expected<SuccessType, ErrorType>;

class IRequest {
    [[nodiscard]] virtual ApplyResult apply_to_simulator(
        sim::Simulator& simlator) = 0;
};

class AddHost : public IRequest {
public:
    explicit AddHost(Id a_host_id);

    [[nodiscard]] ApplyResult apply_to_simulator(
        sim::Simulator& simulator) final;

private:
    Id m_host_id;
};

struct AddSwitch : public IRequest {
public:
    explicit AddSwitch(Id a_switch_id);

    [[nodiscard]] ApplyResult apply_to_simulator(
        sim::Simulator& simulator) final;

private:
    Id switch_id;
};

using Request = std::variant<AddHost, AddSwitch>;

Request parse_request(const std::string& request);

}  // namespace websocket
