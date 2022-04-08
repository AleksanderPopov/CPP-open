//input_reader.h, input_reader.cpp — чтение запросов на заполнение базы;
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <deque>

#include "transport_catalogue.h"

struct Query{
    std::string name; 
    std::vector<std::string> query_vector; 
};

std::unordered_map<std::string,Query> QueryParser(std::string& query);

void InputReader(TransportCatalogue& trans_cat);
