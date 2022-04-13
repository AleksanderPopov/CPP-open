//input_reader.h, input_reader.cpp — чтение запросов на заполнение базы;
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <deque>

#include "transport_catalogue.h"

namespace TRANSPORT {
    
    namespace DETAIL {
    
        struct Query {
            std::string name;
            std::vector<std::string> query_vector;
        };

        std::unordered_map<std::string, TRANSPORT::DETAIL::Query> QueryParser(std::string& query);
        void InputReader(TRANSPORT::TransportCatalogue& trans_cat);
    }
}




