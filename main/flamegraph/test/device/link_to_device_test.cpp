#include <gtest/gtest.h>

#include "../utils/fake_packet.hpp"
#include "device/routing_module.hpp"
#include "utils.hpp"

namespace test {

class LinkToDevice : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(LinkToDevice, NoLinkToDevice) {
    auto source = std::make_shared<TestDevice>();
    auto dest = std::make_shared<TestDevice>();
    auto link = std::make_shared<TestLink>(source, dest);
    dest->add_inlink(link);

    auto destPacket = FakePacket(dest);
    EXPECT_EQ(source->get_link_to_destination(destPacket), nullptr);
}

TEST_F(LinkToDevice, LinkIsPresent) {
    auto source = std::make_shared<TestDevice>("s1");
    auto neighbour = std::make_shared<TestDevice>("s2");
    auto dest = std::make_shared<TestDevice>("d1");
    auto another_dest = std::make_shared<TestDevice>("d2");

    auto link_neighbour =
        std::make_shared<TestLink>(TestLink(source, neighbour));
    source->update_routing_table(neighbour->get_id(), link_neighbour);
    auto link_dest = std::make_shared<TestLink>(TestLink(source, dest));
    source->update_routing_table(dest->get_id(), link_dest);

    EXPECT_EQ(source->get_link_to_destination(FakePacket(dest)), link_dest);
    EXPECT_EQ(source->get_link_to_destination(FakePacket(neighbour)),
              link_neighbour);
    EXPECT_EQ(source->get_link_to_destination(FakePacket(another_dest)),
              nullptr);
    source->update_routing_table(another_dest->get_id(), link_neighbour);

    EXPECT_EQ(source->get_link_to_destination(FakePacket(another_dest)),
              link_neighbour);
}

}  // namespace test
