#pragma once
#include "i_packet_reordering.hpp"

#ifdef _MSC_VER
#include <vector>
#else
#include "utils/ordered_set.hpp"
#endif

#include "utils/statistics.hpp"

namespace sim {
class SimplePacketReordering : public IPacketReordering {
public:
    explicit SimplePacketReordering(std::size_t a_max_packets = 1'000'000);
    ~SimplePacketReordering() = default;

    void add_record(PacketNum packet_num) final;
    PacketReordering value() const final;

private:
    // maximal count of packets in m_packet_num_set
    // When m_packet_num_set.size() growth up it, the smallest element deletes
    std::size_t m_max_packets;
    uint64_t m_inversions_count;
#ifdef _MSC_VER
    std::vector<PacketNum> m_packet_num_set;
#else
    utils::ordered_set<PacketNum> m_packet_num_set;
#endif
};
}  // namespace sim
