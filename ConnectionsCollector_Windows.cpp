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
/// @brief           Implementation of class ConnectionsCollector for Windows.
///
////////////////////////////////////////////////////////////////////////////////

#include "ConnectionsCollector.hpp"

#if !defined(WIN32)
#	error Target compilation OS is not Windows!
#endif /// !WIN32

#include <Windows.h>
#include <Iphlpapi.h>
#include <WinSock.h>
#include <Psapi.h>
#include <TlHelp32.h>

#pragma comment(lib , "Iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")

#include <memory>
#include <system_error>
#include <iostream>

namespace Darkness::ICV {
	namespace {
		class _HandleGuard final
		{
		public:
			explicit _HandleGuard(HANDLE handle) noexcept
				: m_Handle(handle)
			{
			}

			~_HandleGuard() noexcept {
				if (m_Handle)
				{
					CloseHandle(m_Handle);
				}
			}

		private:
			HANDLE m_Handle;
		};

		std::string _Wc2Mb(std::wstring const& str, UINT cp = CP_UTF8) noexcept {
			int const inSize = static_cast<int>(str.size() + 1);
			auto const sizeNeeded = WideCharToMultiByte(cp, 0, str.c_str(), inSize, 0, 0, 0, 0);

			std::vector<char> buffer(sizeNeeded, '\0');
			WideCharToMultiByte(CP_ACP, 0, str.c_str(), inSize, &buffer[0], sizeNeeded, 0, 0);

			return { buffer.cbegin(), buffer.cend() };
		}

		template<class T>
		inline constexpr bool always_false_v = false;

		template<typename CharT>
		std::string _FromInternalWindowsRepresentationToMultiByteString(CharT(&str)[MAX_PATH]) noexcept {
			if constexpr (std::is_same_v<CharT, char>)
			{
				return str;
			}
			else if constexpr (std::is_same_v<CharT, wchar_t>)
			{
				return _Wc2Mb(str);
			}
			else
			{
				static_assert(always_false_v<CharT>);
			}
		}

		std::string _GetProcessNameByPID(DWORD pid) noexcept(false)
		{
			PROCESSENTRY32 processInfo;
			processInfo.dwSize = sizeof(processInfo);
			auto processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			_HandleGuard const handleGuard(processesSnapshot);

			if (processesSnapshot == INVALID_HANDLE_VALUE)
			{
				throw ConnectionsCollector::Exception("Can not get a process snapshot by PID: " + std::to_string(pid));
			}

			for (auto ok = Process32First(processesSnapshot, &processInfo); ok; ok = Process32Next(processesSnapshot, &processInfo))
			{
				if (pid == processInfo.th32ProcessID)
				{
					return _FromInternalWindowsRepresentationToMultiByteString(processInfo.szExeFile);
				}

			}

			throw ConnectionsCollector::Exception("No process found with given PID: " + std::to_string(pid));
		}

		tIpAddress _IpStr(DWORD ip) noexcept {
			IN_ADDR inAddr;
			inAddr.S_un.S_addr = ip;
			return inet_ntoa(inAddr);
		}

		ConnectionInfo::Udp _MakeUdpConnectionInfo(MIB_UDPROW_OWNER_PID const& udp) {
			return { _IpStr(udp.dwLocalAddr), udp.dwLocalPort, udp.dwOwningPid, _GetProcessNameByPID(udp.dwOwningPid) };
		}

		std::string_view _TcpState2Str(MIB_TCP_STATE stateValue) {
			switch (stateValue)
			{
			case MIB_TCP_STATE_CLOSED:
				return "CLOSED";

			case MIB_TCP_STATE_LISTEN:
				return "LISTEN";

			case MIB_TCP_STATE_SYN_SENT:
				return "SYN_SENT";

			case MIB_TCP_STATE_SYN_RCVD:
				return "SYN_RCVD";

			case MIB_TCP_STATE_ESTAB:
				return "ESTABLISHED";

			case MIB_TCP_STATE_FIN_WAIT1:
				return "FIN_WAIT1";

			case MIB_TCP_STATE_FIN_WAIT2:
				return "FIN_WAIT2";

			case MIB_TCP_STATE_CLOSE_WAIT:
				return "CLOSE_WAIT";

			case MIB_TCP_STATE_CLOSING:
				return "CLOSING";

			case MIB_TCP_STATE_LAST_ACK:
				return "LAST_ACK";

			case MIB_TCP_STATE_TIME_WAIT:
				return "TIME_WAIT";

			case MIB_TCP_STATE_DELETE_TCB:
				return "DELETE_TCB";

			default:
				return "<UNKNOWN-TCP-STATE>";
			}
		}

		ConnectionInfo::Tcp::State _MakeTcpState(MIB_TCP_STATE stateValue) {
			return { uint32_t(stateValue), _TcpState2Str(stateValue) };
		}

		ConnectionInfo::Tcp::State _MakeTcpState(DWORD stateValue) {
			return _MakeTcpState(static_cast<MIB_TCP_STATE>(stateValue));
		}

		[[noreturn]] void _ThrowCollectException(char const* type, DWORD error) {
			throw ConnectionsCollector::Exception(
				std::string("[ ") + type + " Collect Error] " + std::system_category().message(error)
			);
		}

		ConnectionInfo::Tcp _MakeTcpConnectionInfo(MIB_TCPROW_OWNER_PID const& tcp) {
			return {
				_IpStr(tcp.dwLocalAddr), tcp.dwLocalPort
				, _IpStr(tcp.dwRemoteAddr), tcp.dwRemotePort
				, _MakeTcpState(tcp.dwState)
				, tcp.dwOwningPid, _GetProcessNameByPID(tcp.dwOwningPid)
			};
		}
	} /// end unnamed namespace

	ConnectionsCollector::tUdpConnectionsInfo ConnectionsCollector::CollectUdpConnectionsInfo() noexcept(false) {
		DWORD size = 0;
		DWORD error = GetExtendedUdpTable(nullptr, &size, FALSE, AF_INET, UDP_TABLE_OWNER_PID, 0);

		/*if (error != NO_ERROR)
		{
			_ThrowCollectException("UDP", error);
		}*/

		auto udpTable = std::make_unique<MIB_UDPTABLE_OWNER_PID[]>(size);

		error = GetExtendedUdpTable(reinterpret_cast<PVOID>(udpTable.get()), &size, FALSE, AF_INET, UDP_TABLE_OWNER_PID, 0);

		if (error != NO_ERROR)
		{
			_ThrowCollectException("UDP", error);
		}

		tUdpConnectionsInfo result(size_t(udpTable.get()->dwNumEntries));

		for (size_t i = 0; i < result.size(); ++i)
		{
			result[i] = _MakeUdpConnectionInfo(udpTable.get()->table[i]);
		}

		return result;
	}

	ConnectionsCollector::tTcpConnectionsInfo ConnectionsCollector::CollectTcpConnectionsInfo() noexcept(false) {
		DWORD size = 0;
		DWORD error = GetExtendedTcpTable(NULL, &size, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0); /// 1st param must be NULL, not nullptr - to prevent VS warning

		/*if (error != NO_ERROR)
		{
			_ThrowCollectException("TCP", error);
		}*/

		auto tcpTable = std::make_unique<MIB_TCPTABLE_OWNER_PID[]>(size);
		error = GetExtendedTcpTable(reinterpret_cast<PVOID>(tcpTable.get()), &size, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);

		if (error != NO_ERROR)
		{
			_ThrowCollectException("TCP", error);
		}

		tTcpConnectionsInfo result(size_t(tcpTable.get()->dwNumEntries));

		for (size_t i = 0; i < result.size(); ++i)
		{
			result[i] = _MakeTcpConnectionInfo(tcpTable.get()->table[i]);
		}

		return result;
	}
} /// end namespace Darkness::ICV
