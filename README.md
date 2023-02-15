# wlanbridge project

## Introduction
This is a userland layer-2 bridge application that integrates with `hostapd` to provide VLAN tagging based on the PSK the user enters when wireless association occurs.

## Building
The system is built using `make`. This creates the binary `bridge` that can then be used to bridge traffic between an ethernet adapter

## Use
The command to use this program is: `./bridge <wired adapter> <wireless adapter>`

For example: `./bridge eth0 wlan1`

**Note**: The ethernet adapter MUST be the first adapter. The first adapter is passing tagged traffic, while the second adapter is always passing untagged traffic.

## Operation
This operates as a normal bridge between to interfaces when run.  However, it has a thread running that waits for messages on a fifo (named pipe) containing VLANID/MAC address pairs.  When a pair is recieved, the bridge begins to tag all matching packets with the assigned VLAN tag when bridging from the wireless interface to the wired interface.  When the bridge recieves tagged packets on the wired interface, it removes the VLAN tag before forwarding the packet to the proper interface (at this point always a one wireless interface)
