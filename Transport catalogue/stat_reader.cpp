#include "stat_reader.h"

std::ostream& operator<<(std::ostream& out, const BusInfo& info) {
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

std::ostream& operator<<(std::ostream& out, const StopInfo& info) {
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

void OutputReader(TransportCatalogue& trans_cat) {
	
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

	for (auto queri : all_queries) {
		if (queri.first == "Stop") {			
			std::cout << trans_cat.GetStopInfo(queri.second) << std::endl;
		}
		if (queri.first == "Bus") {			
			std::cout << trans_cat.GetBusInfo(queri.second) << std::endl;
		}
	}
}

