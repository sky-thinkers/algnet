#pragma once

#include <expected>
#include <initializer_list>
#include <memory>
#include <utility>

#include "simulator.hpp"

namespace test {

using two_way_link_t =
    std::pair<std::shared_ptr<sim::IDevice>, std::shared_ptr<sim::IDevice>>;

[[nodiscard]] sim::Simulator::AddResult add_two_way_links(
    sim::Simulator& sim, std::initializer_list<two_way_link_t> links);

bool check_reachability(std::shared_ptr<sim::IDevice> src_device,
                        sim::Packet packet_to_dest);

template <typename TVal>
auto inline ASSERT_HAS_VALUE(std::expected<TVal, std::string> expect) {
    ASSERT_TRUE(expect.has_value()) << expect.error();
}

template <typename TVal, typename TErr>
auto inline ASSERT_HAS_ERROR(std::expected<TVal, TErr> expect) {
    ASSERT_NO_THROW(expect.error());
}

}  // namespace test
