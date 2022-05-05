#include "transport_catalogue.h"

void transport::TransportCatalogue::AddBus(const transport::data::Bus& new_bus){	
	bus_byname.emplace(new_bus.name_, new_bus);
}	

void transport::TransportCatalogue::AddStop(const transport::data::Stop& new_stop){
	stop_byname.emplace(new_stop.name_, new_stop);
	bus_to_stop_[new_stop.name_];
}	

void transport::TransportCatalogue::AddBusToStop(const std::string& name,const std::set<std::string>& bus_to_stops) {
	for (auto stops : bus_to_stops) {		
		bus_to_stop_[stops].insert(name);
	}
}

void transport::TransportCatalogue::SetDistBtwStop(transport::data::Stop& from, transport::data::Stop& to, double dist) {
	dist_btw_stop_[from.name_].emplace(to.name_, dist);
}

double transport::TransportCatalogue::GetDistBtwStop(transport::data::Stop& from, transport::data::Stop& to) {

	double distance = 0;
	if (dist_btw_stop_.count(from.name_) != 0 && dist_btw_stop_.at(from.name_).count(to.name_) != 0) {
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

transport::data::Bus* transport::TransportCatalogue::FindBus(const std::string& name_bus){
	if (bus_byname.count(name_bus)) {
		return  &bus_byname.at(name_bus);
	}
	return nullptr;
}	

transport::data::Stop* transport::TransportCatalogue::FindStop(const std::string& name_stop){
	if (stop_byname.count(name_stop)) {
		return &stop_byname.at(name_stop);
	}
	return nullptr;
}	

transport::data::BusInfo transport::TransportCatalogue::GetBusInfo(const std::string& name_bus){
	transport::data::BusInfo result_info{ name_bus,0,0,0.0,0.0 };
	transport::data::Bus* bus_info= TransportCatalogue::FindBus(name_bus);

	if (bus_info == nullptr) {
		return result_info;		
	}

	result_info.number_stop_ = bus_info->stop_to_bus_.size();  

	std::unordered_set< std::string_view> uniq;
	for (const auto& uniq_stop : bus_info->stop_to_bus_) {
		uniq.insert(uniq_stop->name_);
	}
	result_info.uniq_number_stop_ = uniq.size();				

	if (uniq.size() == 1) {
		result_info.number_stop_ = 1;
	}
		
	double distance=0;
	for (size_t it = 0; it < bus_info->stop_to_bus_.size() - 1; it++) {
		distance += ComputeDistance(bus_info->stop_to_bus_[it]->spot_, bus_info->stop_to_bus_[it + 1]->spot_);
	}

	double distance_geo = 0;
	if (uniq.size() != 1 && uniq.size()!=0) {
		for (size_t it = 0; it < bus_info->stop_to_bus_.size() - 1; it++) {
			distance_geo += GetDistBtwStop(*bus_info->stop_to_bus_[it], *bus_info->stop_to_bus_[it + 1]);
		}
	}
	else
	{
		distance_geo += GetDistBtwStop(*bus_info->stop_to_bus_[0], *bus_info->stop_to_bus_[0]);
	}

	result_info.distance_route_ = distance_geo;			
	result_info.curvature_ = distance_geo / distance;		

	return result_info;
}	

transport::data::StopInfo transport::TransportCatalogue::GetStopInfo(const std::string& name_stop) {
	std::set<std::string> bus_to_stop{};
	transport::data::StopInfo result{ name_stop,bus_to_stop,false };

	if (bus_to_stop_.count(name_stop) != 0) {		
		result.name_ = name_stop;
		result.buses_ = bus_to_stop_.at(name_stop);
		result.exists_ = true;
	}

	return result;
}

