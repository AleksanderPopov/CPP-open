//stat_reader.h, stat_reader.cpp — чтение запросов на вывод и сам вывод;
#pragma once

#include <iostream>
#include <string>
#include <iomanip>

#include "transport_catalogue.h"
#include "input_reader.h"

std::ostream& operator<<(std::ostream& out, const BusInfo& info); 
std::ostream& operator<<(std::ostream& out, const StopInfo& info);
 
void OutputReader(TransportCatalogue& trans_cat);
