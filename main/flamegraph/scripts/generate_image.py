import os
import sys

import yaml
import argparse
from graphviz import Digraph

"""
Generates topology image by given path to topology config file
"""


def generate_topology(config_file, output_file, picture_label="Network Topology"):
    # Load the YAML configuration
    with open(config_file) as f:
        config = yaml.safe_load(f)

    # Create a directed graph with top-to-bottom layout
    graph = Digraph(engine="dot")
    graph.attr(
        rankdir="TB",
        bgcolor="#F5F5F5",
        fontname="Helvetica",
        label=picture_label,
        fontsize="20",
        pad="0.5",
        nodesep="0.8",
        ranksep="1.5",
    )

    # Common style parameters
    node_style = {
        "style": "filled,rounded",
        "fontname": "Helvetica",
        "fontsize": "12",
        "penwidth": "2",
    }

    edge_style = {"fontname": "Helvetica", "fontsize": "10", "penwidth": "2"}

    def add_node(device_id, style):
        nonlocal graph

        color = style["color"]
        icon = style["icon"]
        shape = style["shape"]

        label = f"""<
            <table border="0" cellborder="0">
                <tr><td><font face="Arial" point-size="14">{icon}</font></td></tr>
                <tr><td><b>{device_id}</b></td></tr>
            </table>
        >"""

        graph.node(
            device_id,
            label=label,
            shape=shape,
            color=color,
            fillcolor=f"{color}20",
            **node_style,
        )

    key_lambda = lambda key_value : key_value[0]

    # Add hosts with vertical alignment
    hosts = config.get("hosts", {})
    host_items = sorted(hosts.items(), key = key_lambda)

    switches = config.get("switches", {})
    switch_items = sorted(switches.items(), key= key_lambda)
    
    DEVICE_STYLES = {
        "host": {"color": "#2E7D32", "icon": "🖥️", "shape": "none"},
        "switch": {"color": "#1565C0", "icon": "🌐", "shape": "box3d"},
    }

    # Define default layers for device types
    DEFAULT_LAYERS = {
        "host": 0,
        "switch": 1
    }

    # Collect all devices with their layers
    all_devices = []
    
    # Process hosts
    for host_id, host_info in hosts.items():
        layer = host_info["layer"] if host_info is not None and "layer" in host_info else DEFAULT_LAYERS["host"]
        all_devices.append((host_id, "host", layer, host_info))
    
    # Process switches
    for switch_id, switch_info in switches.items():
        layer = switch_info["layer"] if switch_info is not None and "layer" in switch_info else DEFAULT_LAYERS["switch"]
        all_devices.append((switch_id, "switch", layer, switch_info))

    # Group devices by layer
    layer_groups = {}
    for device_id, dev_type, layer, _ in all_devices:
        if layer not in layer_groups:
            layer_groups[layer] = []
        layer_groups[layer].append((device_id, dev_type))


    for layer, devices in sorted(layer_groups.items()):
        sorted_devices = sorted(devices, key = key_lambda)
        for device_id, dev_type in sorted_devices:
            add_node(device_id, DEVICE_STYLES[dev_type])

    presets = config.get("presets", {})
    link_presets = presets.get("link", {})

    def get_with_preset(node : dict, preset_node : dict, field_name):
        if field_name in node:
            return node[field_name]
        return preset_node[field_name]

    # Add styled links with metrics
    links = config.get("links", {})
    for link_id, link_info in links.items():
        from_node = link_info["from"]
        to_node = link_info["to"]
        preset_name = link_info.get("preset-name", "default")
        link_preset = link_presets.get(preset_name)
        latency = get_with_preset(link_info, link_preset, 'latency')
        throughput = get_with_preset(link_info, link_preset, 'throughput')
        label = f"{link_id}\n"\
                f"⏱ {latency}\n" \
                f"📶 {throughput}\n"

        graph.edge(
            from_node,
            to_node,
            label=label,
            color="#616161",
            arrowhead="open",
            arrowsize="0.8",
            fontcolor="#424242",
            **edge_style,
        )

    print(layer_groups)

    # Create subgraphs for each layer
    for layer, devices in sorted(layer_groups.items()):
        sorted_devices = sorted(devices, key = key_lambda)
        with graph.subgraph() as s:
            s.attr(rank="same")
            for device_id, dev_type in sorted_devices:
                # Add node with appropriate style
                s.node(device_id)


    directory = os.path.dirname(output_file)
    file_name, extension = os.path.splitext(os.path.basename(output_file))
    # Delete . from the extension beginning
    extension = extension[1:]
    
    # Render the graph
    graph.render(file_name, directory, format=extension, cleanup=True)
    print(f"Generated topology image: {output_file}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate topology image by its configuration file."
    )
    parser.add_argument("-c", "--config", help="Path to the topology configuration file", required=True)
    parser.add_argument("-o", "--output", help="Path to the output image", required=True)
    args = parser.parse_args()

    
    generate_topology(args.config, args.output)
