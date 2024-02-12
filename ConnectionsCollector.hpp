///////////////////////////////////////////////////////////////////////////////
///
/// Project           Internet Connections Viewer application
/// Copyright (c)     Poturaiev Anton, 2024
///
////////////////////////////////////////////////////////////////////////////////
/// @file            DataCoder.cpp
/// @authors         Poturaiev Anton
/// @date            12.02.2024
///
/// @brief           Declaration of class ConnectionsCollector.
///
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <string_view>
#include <iosfwd>

namespace Darkness::ICV {

    using tIpAddress = std::string;
    using tPort = uint32_t;
    using tPID = uint32_t;

    struct OwnerProcessInfo final {
        tPID pid;
        std::string name;
    };

    struct AddressInfo {
        tIpAddress address;
        tPort port;
    };

    struct ConnectionInfo final {
        struct Udp final : public AddressInfo {
            OwnerProcessInfo owner;
        };

        struct Tcp final {
            struct State final {
                uint32_t value;
                std::string_view name;
            };

        public:
            AddressInfo local;
            AddressInfo remote;
            State state;
            OwnerProcessInfo owner;
        };
    };

    std::ostream& operator<<(std::ostream& os, ConnectionInfo::Udp const& udp);
    std::ostream& operator<<(std::ostream& os, ConnectionInfo::Tcp const& tcp);

    class ConnectionsCollector
    {
    public:
        using tUdpConnectionsInfo = std::vector<ConnectionInfo::Udp>;
        using tTcpConnectionsInfo = std::vector<ConnectionInfo::Tcp>;

        struct Exception final : public std::runtime_error {
            explicit Exception(std::string const& message);
        };

    public:
        ConnectionsCollector() = delete;

        static tUdpConnectionsInfo CollectUdpConnectionsInfo() noexcept(false);
        static tTcpConnectionsInfo CollectTcpConnectionsInfo() noexcept(false);
    };
} /// end namespace Darkness::ICV

