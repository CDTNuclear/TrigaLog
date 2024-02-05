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
//TrigaLog.h
#ifndef TRIGA_SERVER
#define TRIGA_SERVER

#include <libModbusSystematomSPU.h>
#include <libOpcMaestecPLC.h>

#include <iostream>
#include <fstream>
#include <thread>
#include <memory>
#include <string>

struct ALL_DATA
{
    PLC_DATA PLC;
    SPU_DATA SPU_CHA;
    SPU_DATA SPU_CHB;
};

class TrigaLog
{
    public:
        TrigaLog(std::string server_ip, short server_port, std::string log_path);
        ~TrigaLog();

        void startRead(int amo);

    private:
        std::string server_ip_;
        short server_port_;
        std::string log_path_;

        const int ALL_DATA_SIZE = sizeof(ALL_DATA);

        void saveToFile(const ALL_DATA& data);
};

#endif
