//stat_reader.h, stat_reader.cpp — чтение запросов на вывод и сам вывод;
#pragma once

#include <iostream>
#include <string>
#include <iomanip>

#include "transport_catalogue.h"
#include "input_reader.h"
 
namespace transport {
    
    namespace detail {

        void OutputReader(transport::TransportCatalogue& transport_catalogue);
        
        std::ostream& operator<<(std::ostream& out, const transport::data::BusInfo& info);
        std::ostream& operator<<(std::ostream& out, const transport::data::StopInfo& info);

    }
}


