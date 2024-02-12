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
/// @brief           Implementation of common parts for class ConnectionsCollector.
///
////////////////////////////////////////////////////////////////////////////////

#include "ConnectionsCollector.hpp"

#include <iostream>

namespace Darkness::ICV {
	std::ostream& operator<<(std::ostream& os, ConnectionInfo::Udp const& udp) {
		os << "*** [UDP]:\n"
			<< "\tAddress: " << udp.address << ':' << udp.port << '\n'
			<< "\tOwner: " << udp.owner.name << " <PID: " << udp.owner.pid << ">\n"
			<< std::endl;

		return os;
	}

	std::ostream& operator<<(std::ostream& os, ConnectionInfo::Tcp const& tcp) {
		os << "*** [TCP]:\n"
			<< "\tLocal address: " << tcp.local.address << ':' << tcp.local.port << '\n'
			<< "\tRemote address: " << tcp.remote.address << ':' << tcp.remote.port << '\n'
			<< "\tState: " << tcp.state.name << '(' << tcp.state.value << ")\n"
			<< "\tOwner: " << tcp.owner.name << " <PID: " << tcp.owner.pid << ">\n"
			<< std::endl;

		return os;
	}

	ConnectionsCollector::Exception::Exception(std::string const& message)
		: std::runtime_error(message)
	{}
} /// end namespace Darkness::ICV
