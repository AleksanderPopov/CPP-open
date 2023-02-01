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

namespace transport {	
	
	namespace data {
		
		struct Stop {
			Stop(std::string name, double latitude, double longitude)
				: name_(name)
			{
				spot_.lat = latitude;
				spot_.lng = longitude;
			}

			std::string name_;
			geo::Coordinates spot_;
		};

		struct Bus {
			Bus(std::string name, std::deque<Stop*>& stop_to_bus)
				:name_(name), stop_to_bus_(stop_to_bus)
			{
			}

			std::string name_;
			std::deque<Stop*> stop_to_bus_;
			bool busloop_=false;
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
			bool loop_=false;
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

		void AddBus(const data::Bus& new_bus);
		void AddStop(const data::Stop& bus_stop);
		void AddBusToStop(const std::string& bus,const std::set<std::string>& bus_to_stops);
		void SetDistBtwStop(data::Stop& from, data::Stop& to, double dist);
		double GetDistBtwStop(data::Stop& from, data::Stop& to);
		data::Bus* FindBus(const std::string& name_bus);
		data::Stop* FindStop(const std::string& name_stop);
		data::BusInfo GetBusInfo(const std::string& name_bus);
		data::StopInfo GetStopInfo(const std::string& name_stop);

	private:		
		std::unordered_map<std::string, data::Stop> stop_byname;
		std::unordered_map<std::string, data::Bus> bus_byname;
		std::unordered_map<std::string, std::set<std::string>> bus_to_stop_;
		std::unordered_map<std::string, std::unordered_map<std::string, double>> dist_btw_stop_;
	};
}