#pragma once
#include "connection/flow/tcp/i_tcp_cc.hpp"

// Represents no congestion control
namespace sim {
class BasicCC : public ITcpCC {
public:
    void on_ack(TimeNs rtt, TimeNs avg_rtt, bool ecn_flag) final;
    void on_timeout() final;
    TimeNs get_pacing_delay() const final;
    double get_cwnd() const final;
    std::string to_string() const final;
};
}  // namespace sim