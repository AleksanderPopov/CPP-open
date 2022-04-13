//stat_reader.h, stat_reader.cpp — чтение запросов на вывод и сам вывод;
#pragma once

#include <iostream>
#include <string>
#include <iomanip>

#include "transport_catalogue.h"
#include "input_reader.h"
 
namespace TRANSPORT {
    
    namespace DETAIL {

        void OutputReader(TRANSPORT::TransportCatalogue& trans_cat);

        std::ostream& operator<<(std::ostream& out, const TRANSPORT::DATA::BusInfo& info);
        std::ostream& operator<<(std::ostream& out, const TRANSPORT::DATA::StopInfo& info);

    }
}


