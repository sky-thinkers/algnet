#include "event.hpp"

#include "metrics/metrics_collector.hpp"
#include "scheduler.hpp"

namespace sim {

Event::Event(Time a_time) : m_time(a_time) {};

Time Event::get_time() const { return m_time; }

}  // namespace sim
