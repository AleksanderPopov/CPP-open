//stat_reader.h, stat_reader.cpp — чтение запросов на вывод и сам вывод;
#pragma once

#include <iostream>
#include <string>
#include <iomanip>

#include "transport_catalogue.h"
#include "input_reader.h"
 
namespace transport {
    
    namespace detail {

        std::ostream& operator<<(std::ostream& out, const transport::data::BusInfo& info);
        std::ostream& operator<<(std::ostream& out, const transport::data::StopInfo& info);
        
        void OutputReader(transport::TransportCatalogue& transport_catalogue);
        void PrintBusInfo(transport::TransportCatalogue& transport_catalogue,const std::string& query_info);
        void PrintStopInfo(transport::TransportCatalogue& transport_catalogue,const std::string& query_info);

    }
}


