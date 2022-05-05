#include "stat_reader.h"

std::ostream& transport::detail::operator<<(std::ostream& out, const transport::data::BusInfo& info) {
	if (info.number_stop_ == 0) {
		out << "Bus " << info.name_ << ": not found";
		return out;
	}
	else
	{
		out << "Bus " << info.name_ << ": ";
		out << info.number_stop_ << " stops on route, ";
		out << info.uniq_number_stop_ << " unique stops, ";
		out << std::setprecision(6) << info.distance_route_ << " route length, ";
		out << std::setprecision(6) << info.curvature_ << " curvature";
	}
    	
	return out;
}

std::ostream& transport::detail::operator<<(std::ostream& out, const transport::data::StopInfo& info) {
	if (!info.exists_) {
		out << "Stop " << info.name_ << ": not found";
		return out;
	}

	if (info.buses_.empty())
		out << "Stop " << info.name_ << ": no buses";
	else {
		out << "Stop " << info.name_ << ": ";
		out << "buses";
		for (auto str : info.buses_) {
			out <<" "<< str;
		}
	}
	return out;
}

void transport::detail::PrintBusInfo(transport::TransportCatalogue& transport_catalogue,const std::string& query_info){
	std::cout << transport_catalogue.GetBusInfo(query_info) << std::endl;

}

void transport::detail::PrintStopInfo(transport::TransportCatalogue& transport_catalogue,const std::string& query_info) {
	std::cout << transport_catalogue.GetStopInfo(query_info) << std::endl;
}


void transport::detail::OutputReader(TransportCatalogue& transport_catalogue) {
	
	int query_count;
	std::cin >> query_count;
	std::cin.get();

	std::vector<std::pair<std::string, std::string>> all_queries;

	for (int i = 0; i < query_count; ++i) {
		std::string query;
		std::getline(std::cin, query);

		if (query.find("Bus") == 0) {			
			int name_start = query.find_first_of(" ");
			std::string str = query.substr(name_start + 1);						
			all_queries.push_back({ "Bus",str });
		}
		if (query.find("Stop") == 0) {			
			int name_start = query.find_first_of(" ");
			std::string str = query.substr(name_start + 1);			
			all_queries.push_back({ "Stop",str });
		}
		
	}

	std::cout << std::flush;
	
	for (auto [query_type, query_info] : all_queries) {

		if (query_type == "Stop") {						
			PrintStopInfo(transport_catalogue,query_info);
		}
		if (query_type == "Bus") {			
			PrintBusInfo(transport_catalogue, query_info);
		}
	}
}

