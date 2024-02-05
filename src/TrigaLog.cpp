/*
TrigaLog is a software for GNU operating system to get the real-time values of the
Nuclear Reator Triga IPR-R1 from the TrigaSever software and storage in hard-drive.
Copyright (C) 2024 Thalles Campagnani

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
// TrigaLog.cpp
#include "TrigaLog.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <iomanip>

TrigaLog::TrigaLog(std::string server_ip, short server_port, std::string log_path)
    : server_ip_(std::move(server_ip)), server_port_(server_port), log_path_(std::move(log_path))
{
}

TrigaLog::~TrigaLog()
{
}

void TrigaLog::startRead(int amo)
{
    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        std::cerr << "[startRead] Error creating socket" << std::endl;
        return;
    }

    // Server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port_);
    inet_pton(AF_INET, server_ip_.c_str(), &server_addr.sin_addr);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "[startRead] Error connecting to server" << std::endl;
        close(client_socket);
        return;
    }

    // Send value of amo to server
    std::string amo_str = std::to_string(amo);
    if (send(client_socket, amo_str.c_str(), amo_str.length(), 0) < 0)
    {
        std::cerr << "[startRead] Error sending data to server" << std::endl;
        close(client_socket);
        return;
    }

    while (true)
    {
        ALL_DATA all_data;
        // Receive binary data of size ALL_DATA_SIZE
        ssize_t bytes_received = recv(client_socket, &all_data, sizeof(all_data), 0);
        if (bytes_received < 0)
        {
            std::cerr << "[startRead] Error receiving data from server" << std::endl;
            break;
        }
        else if (bytes_received == 0)
        {
            std::cerr << "[startRead] Connection closed by server" << std::endl;
            break;
        }

        // Save received data to file
        saveToFile(all_data);
    }

    // Close socket
    close(client_socket);
}

void TrigaLog::saveToFile(const ALL_DATA &data)
{
    // Open file
    std::ofstream file(log_path_,std::ios::app);

    if (file.is_open())
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%Y_%m_%d_%H_%M_%S");
        std::string timestamp = ss.str();

        file << "##### " << timestamp << " ####" << std::endl;
        // Write data to file
        file << "PLC---------------------:" << std::endl;
        file << "PLC_READ: "                << data.PLC.READ                << std::endl;
        file << "PLC_IG01: "                << data.PLC.IG01                << std::endl;
        file << "PLC_IG02: "                << data.PLC.IG02                << std::endl;
        file << "PLC_IG03: "                << data.PLC.IG03                << std::endl;
        file << "PLC_IG04: "                << data.PLC.IG04                << std::endl;
        file << "PLC_JT01: "                << data.PLC.JT01                << std::endl;
        file << "PLC_JT02: "                << data.PLC.JT02                << std::endl;
        file << "PLC_JT03: "                << data.PLC.JT03                << std::endl;
        file << "PLC_KT01: "                << data.PLC.KT01                << std::endl;
        file << "PLC_TT01: "                << data.PLC.TT01                << std::endl;
        file << "PLC_TT02: "                << data.PLC.TT02                << std::endl;
        file << "PLC_TT03: "                << data.PLC.TT03                << std::endl;
        file << "PLC_TT04: "                << data.PLC.TT04                << std::endl;
        file << "PLC_TT05: "                << data.PLC.TT05                << std::endl;
        file << "PLC_CT01: "                << data.PLC.CT01                << std::endl;
        file << "PLC_CT02: "                << data.PLC.CT02                << std::endl;
        file << "PLC_FI01: "                << data.PLC.FI01                << std::endl;
        file << "PLC_FI02: "                << data.PLC.FI02                << std::endl;
        file << "PLC_LT01: "                << data.PLC.LT01                << std::endl;
        file << "PLC_PHT01: "               << data.PLC.PHT01               << std::endl;
        file << "PLC_TTXX: "                << data.PLC.TTXX                << std::endl;
        file << "PLC_TT06: "                << data.PLC.TT06                << std::endl;
        file << "PLC_TT07: "                << data.PLC.TT07                << std::endl;
        file << "PLC_TT08: "                << data.PLC.TT08                << std::endl;
        file << "PLC_RT01: "                << data.PLC.RT01                << std::endl;
        file << "PLC_RT02: "                << data.PLC.RT02                << std::endl;
        file << "PLC_RT03: "                << data.PLC.RT03                << std::endl;
        file << "PLC_RT04: "                << data.PLC.RT04                << std::endl;
        file << "PLC_RT05: "                << data.PLC.RT05                << std::endl;
        file << "PLC_RT06: "                << data.PLC.RT06                << std::endl;
        file << "PLC_Reserva_AI11: "        << data.PLC.Reserva_AI11        << std::endl;
        file << "PLC_Reserva_AI12: "        << data.PLC.Reserva_AI12        << std::endl;
        file << "PLC_Reserva_AI13: "        << data.PLC.Reserva_AI13        << std::endl;
        file << "PLC_Reserva_AI14: "        << data.PLC.Reserva_AI14        << std::endl;
        file << "PLC_Reserva_AI15: "        << data.PLC.Reserva_AI15        << std::endl;
        file << "PLC_Reserva_AI16: "        << data.PLC.Reserva_AI16        << std::endl;
        file << "PLC_Reserva_AI17: "        << data.PLC.Reserva_AI17        << std::endl;
        file << "PLC_Reserva_AI18: "        << data.PLC.Reserva_AI18        << std::endl;
        file << "PLC_Reserva_AI19: "        << data.PLC.Reserva_AI19        << std::endl;
        file << "PLC_Reserva_AI20: "        << data.PLC.Reserva_AI20        << std::endl;
        file << "PLC_Reserva_AI21: "        << data.PLC.Reserva_AI21        << std::endl;
        file << "SPU_CHA------------------" << std::endl;
        file << "SPU_CHA_READ: "            << data.SPU_CHB.READ            << std::endl;
        file << "SPU_CHA_N_DATA_FP: "       << data.SPU_CHB.N_DATA_FP       << std::endl;
        file << "SPU_CHA_T_DATA_FP: "       << data.SPU_CHB.T_DATA_FP       << std::endl;
        file << "SPU_CHA_F1_DATA_FP: "      << data.SPU_CHB.F1_DATA_FP      << std::endl;
        file << "SPU_CHA_F2_DATA_FP: "      << data.SPU_CHB.F2_DATA_FP      << std::endl;
        file << "SPU_CHA_F3_DATA_FP: "      << data.SPU_CHB.F3_DATA_FP      << std::endl;
        file << "SPU_CHA_EMR_N_THRESHOLD: " << data.SPU_CHB.EMR_N_THRESHOLD << std::endl;
        file << "SPU_CHA_WRN_N_THRESHOLD: " << data.SPU_CHB.WRN_N_THRESHOLD << std::endl;
        file << "SPU_CHA_EMR_T_THRESHOLD: " << data.SPU_CHB.EMR_T_THRESHOLD << std::endl;
        file << "SPU_CHA_WRN_T_THRESHOLD: " << data.SPU_CHB.WRN_T_THRESHOLD << std::endl;
        file << "SPU_CHA_EMR_N: "           << data.SPU_CHB.EMR_N           << std::endl;
        file << "SPU_CHA_WRN_N: "           << data.SPU_CHB.WRN_N           << std::endl;
        file << "SPU_CHA_EMR_T: "           << data.SPU_CHB.EMR_T           << std::endl;
        file << "SPU_CHA_WRN_T: "           << data.SPU_CHB.WRN_T           << std::endl;
        file << "SPU_CHA_R1: "              << data.SPU_CHB.R1              << std::endl;
        file << "SPU_CHA_R2: "              << data.SPU_CHB.R2              << std::endl;
        file << "SPU_CHA_R3: "              << data.SPU_CHB.R3              << std::endl;
        file << "SPU_CHA_RDY: "             << data.SPU_CHB.RDY             << std::endl;
        file << "SPU_CHA_TEST: "            << data.SPU_CHB.TEST            << std::endl;
        file << "SPU_CHA_XXXX: "            << data.SPU_CHB.XXXX            << std::endl;
        file << "SPU_CHB------------------" << std::endl;
        file << "SPU_CHB_READ: "            << data.SPU_CHB.READ            << std::endl;
        file << "SPU_CHB_N_DATA_FP: "       << data.SPU_CHB.N_DATA_FP       << std::endl;
        file << "SPU_CHB_T_DATA_FP: "       << data.SPU_CHB.T_DATA_FP       << std::endl;
        file << "SPU_CHB_F1_DATA_FP: "      << data.SPU_CHB.F1_DATA_FP      << std::endl;
        file << "SPU_CHB_F2_DATA_FP: "      << data.SPU_CHB.F2_DATA_FP      << std::endl;
        file << "SPU_CHB_F3_DATA_FP: "      << data.SPU_CHB.F3_DATA_FP      << std::endl;
        file << "SPU_CHB_EMR_N_THRESHOLD: " << data.SPU_CHB.EMR_N_THRESHOLD << std::endl;
        file << "SPU_CHB_WRN_N_THRESHOLD: " << data.SPU_CHB.WRN_N_THRESHOLD << std::endl;
        file << "SPU_CHB_EMR_T_THRESHOLD: " << data.SPU_CHB.EMR_T_THRESHOLD << std::endl;
        file << "SPU_CHB_WRN_T_THRESHOLD: " << data.SPU_CHB.WRN_T_THRESHOLD << std::endl;
        file << "SPU_CHB_EMR_N: "           << data.SPU_CHB.EMR_N           << std::endl;
        file << "SPU_CHB_WRN_N: "           << data.SPU_CHB.WRN_N           << std::endl;
        file << "SPU_CHB_EMR_T: "           << data.SPU_CHB.EMR_T           << std::endl;
        file << "SPU_CHB_WRN_T: "           << data.SPU_CHB.WRN_T           << std::endl;
        file << "SPU_CHB_R1: "              << data.SPU_CHB.R1              << std::endl;
        file << "SPU_CHB_R2: "              << data.SPU_CHB.R2              << std::endl;
        file << "SPU_CHB_R3: "              << data.SPU_CHB.R3              << std::endl;
        file << "SPU_CHB_RDY: "             << data.SPU_CHB.RDY             << std::endl;
        file << "SPU_CHB_TEST: "            << data.SPU_CHB.TEST            << std::endl;
        file << "SPU_CHB_XXXX: "            << data.SPU_CHB.XXXX            << std::endl;
        file << std::endl << std::endl << std::endl << std::endl;
        file.close();
    }
    else
    {
        std::cerr << "[saveToFile] Unable to open file: " << log_path_ << std::endl;
    }
}
