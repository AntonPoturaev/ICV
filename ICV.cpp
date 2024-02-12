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
/// @brief           Implementation of the entry point of ICV application.
///
////////////////////////////////////////////////////////////////////////////////

#include "Application.hpp"

int main(int argc, char** argv) {
    Darkness::ICV::Application internetConnectionsViewer(argc, argv);
    return internetConnectionsViewer.Run();
}
