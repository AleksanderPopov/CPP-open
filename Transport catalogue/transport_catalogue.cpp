#include "transport_catalogue.h"

void TransportCatalogue::AddBus(Bus& new_bus){	
	bus_.emplace(new_bus.name_, new_bus);	
}	

void TransportCatalogue::AddStop(Stop& new_stop){	
	stop_.emplace(new_stop.name_, new_stop);
	bus_to_stop_[new_stop.name_];
}	

void TransportCatalogue::AddBusToStop(std::string name, std::set<std::string> bus_to_stops) {	
	for (auto stops : bus_to_stops) {		
		bus_to_stop_[stops].insert(name);
	}
}

void TransportCatalogue::AddDistBtwStop(Stop& from, Stop& to, double dist) {	
	dist_btw_stop_[from.name_].emplace(to.name_, dist);
}

double TransportCatalogue::GetDistBtwStop(Stop& from, Stop& to) {

	double distance = 0;
	if (dist_btw_stop_.count(from.name_) != 0&& dist_btw_stop_.at(from.name_).count(to.name_)!=0) {
		distance = dist_btw_stop_.at(from.name_).at(to.name_);
	}
	else
	{
		if (dist_btw_stop_.count(to.name_) != 0 && dist_btw_stop_.at(to.name_).count(from.name_) != 0) {
			distance = dist_btw_stop_.at(to.name_).at(from.name_);
		}
	}
	return distance;
}

Bus* TransportCatalogue::FindeBus(std::string& name_bus){	
	if (bus_.count(name_bus)) {
		return  &bus_.at(name_bus);		
	}
	return nullptr;
}	

Stop* TransportCatalogue::FindeStop(std::string& name_stop){
	if (stop_.count(name_stop)) {
		return &stop_.at(name_stop);
	}
	return nullptr;
}	

BusInfo TransportCatalogue::GetBusInfo(std::string& name_bus){
	BusInfo result_info{ name_bus,0,0,0.0,0.0 };
	Bus* bus_info= TransportCatalogue::FindeBus(name_bus);

	if (bus_info == nullptr) {
		return result_info;		
	}

	result_info.number_stop_ = bus_info->stop_to_bus_.size();  

	std::unordered_set< std::string_view> uniq;
	for (auto& x : bus_info->stop_to_bus_) {
		uniq.insert(x->name_);
	}
	result_info.uniq_number_stop_ = uniq.size();				

	if (uniq.size() == 1) {
		result_info.number_stop_ = 1;
	}
		
	double distans=0;
	for (size_t it = 0; it < bus_info->stop_to_bus_.size()-1; it++) {
		distans += ComputeDistance(bus_info->stop_to_bus_[it]->spot_, bus_info->stop_to_bus_[it + 1]->spot_);
	}

	double distans_geo = 0;
	if (uniq.size() != 1) {
		for (size_t it = 0; it < bus_info->stop_to_bus_.size() - 1; it++) {
			distans_geo += GetDistBtwStop(*bus_info->stop_to_bus_[it], *bus_info->stop_to_bus_[it + 1]);
		}
	}
	else
	{
		distans_geo += GetDistBtwStop(*bus_info->stop_to_bus_[0], *bus_info->stop_to_bus_[0]);
	}

	result_info.distance_route_ = distans_geo;			
	result_info.curvature_ = distans_geo / distans;		

	return result_info;
}	

StopInfo TransportCatalogue::GetStopInfo(std::string& name_stop) {
	std::set<std::string> bus_to_stop{};
	StopInfo result{ name_stop,bus_to_stop,false };

	if (bus_to_stop_.count(name_stop) != 0) {		
		result.name_ = name_stop;
		result.buses_ = bus_to_stop_.at(name_stop);
		result.exists_ = true;
	}

	return result;
}

