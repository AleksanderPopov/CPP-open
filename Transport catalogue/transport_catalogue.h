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

namespace TRANSPORT {	
	
	namespace DATA {
		
		struct Stop {
			Stop(std::string name, double latitude, double longitude)
				: name_(name)
			{
				spot_.lat = latitude;
				spot_.lng = longitude;
			}

			std::string name_;
			GEO::Coordinates spot_;
		};

		struct Bus {
			Bus(std::string name, std::deque<Stop*>& stop_to_bus)
				:name_(name), stop_to_bus_(stop_to_bus)
			{
			}

			std::string name_;
			std::deque<Stop*> stop_to_bus_;

		};

		struct BusInfo {
			BusInfo(std::string_view name, int number_stop, int uniq_nomber_stop, double distance_route, double curvature)
				:name_(name), number_stop_(number_stop), uniq_number_stop_(uniq_nomber_stop),
				distance_route_(distance_route), curvature_(curvature)
			{
			}
			std::string_view name_;
			int number_stop_;
			int uniq_number_stop_;
			double distance_route_;
			double curvature_;
		};

		struct StopInfo {
			StopInfo(std::string_view name, std::set<std::string> buses, bool exists)
				:name_(name), buses_(buses), exists_(exists)
			{
			}

			std::string_view name_;
			std::set<std::string> buses_;
			bool exists_;
		};
	}
	
	class TransportCatalogue {
	public:

		void AddBus(DATA::Bus& new_bus);
		void AddStop(DATA::Stop& bus_stop);
		void AddBusToStop(std::string bus, std::set<std::string> bus_to_stops);
		void AddDistBtwStop(DATA::Stop& from, DATA::Stop& to, double dist);
		double GetDistBtwStop(DATA::Stop& from, DATA::Stop& to);
		DATA::Bus* FindeBus(std::string& name_bus);
		DATA::Stop* FindeStop(std::string& name_stop);
		DATA::BusInfo GetBusInfo(std::string& name_bus);
		DATA::StopInfo GetStopInfo(std::string& name_stop);

	private:

		std::unordered_map<std::string, DATA::Stop> stop_;
		std::unordered_map<std::string, DATA::Bus> bus_;
		std::unordered_map<std::string, std::set<std::string>> bus_to_stop_;
		std::unordered_map<std::string, std::unordered_map<std::string, double>> dist_btw_stop_;
	};
}





