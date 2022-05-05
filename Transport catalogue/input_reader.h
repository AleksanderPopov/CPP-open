//input_reader.h, input_reader.cpp — чтение запросов на заполнение базы;
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <deque> 

#include "transport_catalogue.h"

namespace transport {
    
    namespace detail {
    
        struct Query {
            std::string name;
            std::vector<std::string> query_vector;
        };

        std::unordered_map<std::string, transport::detail::Query> BusQueryParser(const std::string& query);
        std::unordered_map<std::string, transport::detail::Query> StopQueryParser(const std::string& query);
        std::unordered_map<std::string, transport::detail::Query> QueryParser(const std::string& query);
        void InputReader(transport::TransportCatalogue& transport_catalogue);
    }
}




