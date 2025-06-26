#pragma once
#include "event.hpp"

namespace sim {

class TcpMetric : public Event {
public:
    TcpMetric(Time a_time, std::weak_ptr<ITcpFlow> a_flow);
    virtual ~TcpMetric() = default;

    void operator()();

private:
    const static Time DELAY = 200;
    std::weak_ptr<ITcpFlow> m_flow;
};

}