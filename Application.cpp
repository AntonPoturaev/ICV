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
/// @brief           Implementation of class Application.
///
////////////////////////////////////////////////////////////////////////////////

#include "Application.hpp"
#include "ConnectionsCollector.hpp"

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <algorithm>

namespace Darkness::ICV {
    namespace {
        void _ShowUsage() noexcept {
            std::cout
                << "The InternetConnectionsViewer Application.\n"
                << "\nAvailable (CMD) usage modes: udp | tcp | all.\n"
                << "\tWhere: \'udp\' or \'tcp\' will print accordingly list of process which use there connections.\n"
                << "\tThe 'all' will print all of them (tcp & udp).\n"
                << std::endl;
        }

        void _ShowUdpConnectionTable(ConnectionsCollector::tUdpConnectionsInfo const& udpConnectionInfo) {
            if (udpConnectionInfo.empty())
            {
                std::cout << "\t**** The UDP connection table is empty. ****\n" << std::endl;
            }
            else
            {
                std::cout << "\t**** The UDP connection table:\n";
                std::copy(udpConnectionInfo.cbegin(), udpConnectionInfo.cend(), std::ostream_iterator<ConnectionInfo::Udp>(std::cout));
                std::cout << "\t**** End of UDP connection table.\n" << std::endl;
            }
        }

        void _ShowTcpConnectionTable(ConnectionsCollector::tTcpConnectionsInfo const& tcpConnectionInfo) {
            if (tcpConnectionInfo.empty())
            {
                std::cout << "\t**** The TCP connection table is empty. ****\n" << std::endl;
            }
            else
            {
                std::cout << "\t**** The TCP connection table:\n";
                std::copy(tcpConnectionInfo.cbegin(), tcpConnectionInfo.cend(), std::ostream_iterator<ConnectionInfo::Tcp>(std::cout));
                std::cout << "\t**** End of TCP connection table.\n" << std::endl;
            }
        }
    } /// end unnamed namespace

    Application::Application(int argc, char** argv) {
        switch (argc)
        {
        case 1:
            m_Behavior = Behavior::eEmpty;
            break;

        case 2:
            std::string_view const argCmd(argv[1]);

            if (argCmd == "udp")
            {
                m_Behavior = Behavior::eUdp;
            }
            else if (argCmd == "tcp")
            {
                m_Behavior = Behavior::eTcp;
            }
            else if (argCmd == "all")
            {
                m_Behavior = Behavior::eAll;
            }
            break;
        }
    }

    int Application::Run() noexcept {
        try
        {
            switch (m_Behavior)
            {
            case Behavior::eEmpty:
                _ShowUsage();
                break;

            case Behavior::eUdp:
                _ShowUdpConnectionTable(ConnectionsCollector::CollectUdpConnectionsInfo());
                break;

            case Behavior::eTcp:
                _ShowTcpConnectionTable(ConnectionsCollector::CollectTcpConnectionsInfo());
                break;

            case Behavior::eAll:
                _ShowUdpConnectionTable(ConnectionsCollector::CollectUdpConnectionsInfo());
                _ShowTcpConnectionTable(ConnectionsCollector::CollectTcpConnectionsInfo());
                break;

            default:
                throw std::runtime_error("Unknown value of the application behavior state!");
            }

            return EXIT_SUCCESS;
        }
        catch (ConnectionsCollector::Exception const& ex)
        {
            std::cerr << ex.what();
        }
        catch (std::exception const& ex)
        {
            std::cerr << ex.what();
        }
        catch (...)
        {
            std::cerr << "Unknown exception was caught!";
        }

        return EXIT_FAILURE;
    }

} /// end namespace Darkness::ICV
