#include <gtest/gtest.h>

#include "../_mocks/mplb_mock.hpp"
#include "connection/connection_impl.hpp"
#include "simulator.hpp"
#include "utils.hpp"

namespace test {
class DeleteTest : public testing::Test {
public:
    void TearDown() override { sim::IdentifierFactory::get_instance().clear(); }
    void SetUp() override { sim::IdentifierFactory::get_instance().clear(); };
};

TEST_F(DeleteTest, DeleteHost) {
    auto host = std::make_shared<sim::Host>("sender");
    sim::Simulator sim;
    ASSERT_NO_THROW(sim.delete_host(host).error());
    ASSERT_TRUE(sim.add_host(host));
    ASSERT_HAS_VALUE(sim.delete_host(host));
}

TEST_F(DeleteTest, DeleteSwitchAfterAdd) {
    auto swtch = std::make_shared<sim::Switch>("switch");
    sim::Simulator sim;
    ASSERT_HAS_ERROR(sim.delete_switch(swtch));
    ASSERT_TRUE(sim.add_switch(swtch));
    ASSERT_HAS_VALUE(sim.delete_switch(swtch));
}

TEST_F(DeleteTest, DeleteLinkAfterAdd) {
    auto device_1 = std::make_shared<sim::Host>("host_1");
    auto device_2 = std::make_shared<sim::Host>("host_2");
    auto link = std::make_shared<sim::Link>("link", device_1, device_2);
    sim::Simulator sim;
    ASSERT_HAS_ERROR(sim.delete_link(link));
    ASSERT_TRUE(sim.add_link(link));
    ASSERT_HAS_VALUE(sim.delete_link(link));
}

TEST_F(DeleteTest, DeleteConnectionAfterAdd) {
    auto mplb_mock = std::make_shared<MPLB_Mock>();
    auto device_1 = std::make_shared<sim::Host>("host_1");
    auto device_2 = std::make_shared<sim::Host>("host_2");
    auto connection = std::make_shared<sim::ConnectionImpl>(
        "connection", device_1, device_2, mplb_mock);
    sim::Simulator sim;
    ASSERT_HAS_ERROR(sim.delete_connection(connection));
    ASSERT_TRUE(sim.add_connection(connection));
    ASSERT_HAS_VALUE(sim.delete_connection(connection));
}

}  // namespace test
