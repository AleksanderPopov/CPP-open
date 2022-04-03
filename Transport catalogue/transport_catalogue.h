#pragma once

#include "geo.h"

#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <iostream>
#include <string_view>
#include <vector>
#include <functional>
#include <utility>
#include <algorithm>
#include <iterator>
#include <set>

struct Stop {
	Stop(std::string name, double latitude , double longitude )
		: name_(name)
	{
		spot_.lat = latitude;
		spot_.lng = longitude;
	}
		
	std::string name_;
	Coordinates spot_;
};

struct Bus {
	Bus(std::string name, std::deque<Stop*>& stop_to_bus)
		:name_(name),stop_to_bus_(stop_to_bus)
	{
	}
	
	std::string name_;
	std::deque<Stop*> stop_to_bus_;
	
};

struct BusInfo {
	BusInfo(std::string_view name, int number_stop, int uniq_nomber_stop, double distance_route)
		:name_(name),number_stop_(number_stop),uniq_number_stop_(uniq_nomber_stop),distance_route_(distance_route)
	{
	}
	std::string_view name_;
	int number_stop_;
	int uniq_number_stop_;
	double distance_route_;
};

struct StopInfo {
	StopInfo(std::string_view name,std::set<std::string> buses, bool exists)
		:name_(name), buses_(buses), exists_(exists)
	{
	}

	std::string_view name_;
	std::set<std::string> buses_;
	bool exists_;
};

class TransportCatalogue {
public:	
	
	void AddBus(Bus& new_bus);						// добавление маршрута(автобуса) в базу
	void AddStop(Stop& bus_stop);					// добавление остановки в базу
	void AddBusToStop(std::string bus, std::set<std::string> bus_to_stops)	;//Добавление индекса маршрутов через остановку 
													
	Bus* FindeBus(std::string& name_bus);			// поиск маршрута(автобуса) по имени
	Stop* FindeStop(std::string& name_stop);		// поиск остановки по имени
	BusInfo GetBusInfo(std::string& name_bus);		// получение информации о маршруте.
	StopInfo GetStopInfo(std::string& name_stop);

private:

	std::unordered_map<std::string, Stop> stop_;
	std::unordered_map<std::string, Bus> bus_;
	std::unordered_map<std::string, std::set<std::string>> bus_to_stop_;  	
};

