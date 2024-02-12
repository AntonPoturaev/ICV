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
/// @brief           Declaration of class Application.
///
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace Darkness::ICV {
    /// @brief The InternetConnectionsViewer Application class
    class Application
    {
    public:
        enum class Behavior
        {
            eEmpty
            , eUdp
            , eTcp
            , eAll
        };

    public:
        explicit Application(int argc, char** argv);

        int Run() noexcept;

    private:
        Behavior m_Behavior =
            //Behavior::eEmpty
            Behavior::eAll
            ;
    };
} /// end namespace Darkness::ICV
