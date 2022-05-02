#include "input_reader.h"

std::unordered_map<std::string, transport::detail::Query> transport::detail::BusQueryParser(std::string& query) {
	std::unordered_map<std::string, Query> query_processing;
	Query complit_parsing_query;

	int name_start = query.find_first_of(" ");
	int name_stop = query.find_first_of(":") - 1;
	std::string name = query.substr(name_start + 1, name_stop - name_start);
	complit_parsing_query.name = name;

	if (query.find(">") != std::string::npos) {
		int start_tmp = query.find_first_not_of(" ", name_stop + 2);
		int stop_tmp = 0;
		while (stop_tmp != std::string::npos)
		{
			std::string bus_tmp;
			stop_tmp = query.find_first_of(">", start_tmp);
			bus_tmp = query.substr(start_tmp, stop_tmp - 1 - start_tmp);
			start_tmp = stop_tmp + 2;
			complit_parsing_query.query_vector.push_back(std::move(bus_tmp));
		}
		query_processing.emplace("Bus", std::move(complit_parsing_query));
	}

	if (query.find("-") != std::string::npos) {
		int start_tmp = query.find_first_not_of(" ", name_stop + 2);
		int stop_tmp = 0;
		while (stop_tmp != std::string::npos)
		{
			std::string bus_tmp;
			stop_tmp = query.find_first_of("-", start_tmp);
			bus_tmp = query.substr(start_tmp, stop_tmp - 1 - start_tmp);
			start_tmp = stop_tmp + 2;
			complit_parsing_query.query_vector.push_back(std::move(bus_tmp));
		}

		int ptr = complit_parsing_query.query_vector.size() - 1;
		while (ptr != 0) {
			complit_parsing_query.query_vector.push_back(complit_parsing_query.query_vector[ptr - 1]);
			ptr--;
		}
	}

	if (query.find(">") == std::string::npos && query.find("-") == std::string::npos) {
		int start_tmp = query.find_first_not_of(" ", name_stop + 2);
		std::string bus_tmp;
		bus_tmp = query.substr(start_tmp);
		complit_parsing_query.query_vector.push_back(std::move(bus_tmp));
	}
	query_processing.emplace("Bus", std::move(complit_parsing_query));

	return query_processing;
}

std::unordered_map<std::string, transport::detail::Query> transport::detail::StopQueryParser(std::string& query) {
	std::unordered_map<std::string, Query> query_processing;
	Query complit_parsing_query;

	int name_start = query.find_first_of(" ");
	int name_stop = query.find_first_of(":") - 1;
	std::string name = query.substr(name_start + 1, name_stop - name_start);

	int start_lat = query.find_first_of(" ", name_stop) + 1;
	int stop_lat = query.find_first_of(" ", start_lat) - 1;
	int start_lon = query.find_first_of(",", name_stop) + 2;
	int stop_lon = query.find_first_of(" ", start_lon);

	std::string lat_s = query.substr(start_lat, stop_lat - start_lat);
	std::string lon_s = query.substr(start_lon, stop_lon - 1 - start_lon);

	transport::detail::Query distance_queryuery;

	if (stop_lon != std::string::npos) {

		std::string dist_complit_parsing_query = query.substr(stop_lon - 1);
		std::string dist_str;
		std::string stop_to;
		distance_queryuery.name = name;

		int start_dist;
		int stop_dist;
		int start_to_stop;
		int stop_to_stop = 0;

		while (stop_to_stop != std::string::npos) {
			start_dist = dist_complit_parsing_query.find_first_of(",") + 2;
			stop_dist = dist_complit_parsing_query.find("m to ");
			dist_str = dist_complit_parsing_query.substr(start_dist, stop_dist - start_dist);

			start_to_stop = stop_dist + 5;
			stop_to_stop = dist_complit_parsing_query.find_first_of(",", start_to_stop);
			stop_to = dist_complit_parsing_query.substr(start_to_stop, stop_to_stop - start_to_stop);

			if (stop_to_stop != std::string::npos) {
				dist_complit_parsing_query = dist_complit_parsing_query.substr(stop_to_stop);
			}
			std::string result_q_d = stop_to + ">" + dist_str;
			distance_queryuery.query_vector.push_back(std::move(result_q_d));

		}
	}

	complit_parsing_query.name = name;
	complit_parsing_query.query_vector.push_back(std::move(lat_s));
	complit_parsing_query.query_vector.push_back(std::move(lon_s));

	query_processing.emplace("Stop", std::move(complit_parsing_query));

	if (!distance_queryuery.query_vector.empty()) {
		query_processing.emplace("Dist", std::move(distance_queryuery));
	}

	return query_processing;
}

std::unordered_map<std::string, transport::detail::Query> transport::detail::QueryParser(std::string& query) {
	
	std::unordered_map<std::string, Query> query_processing;

	if (query.find("Bus") == 0) {		
		query_processing = BusQueryParser(query);

	};
	
	if (query.find("Stop") == 0) {		
		query_processing = StopQueryParser(query);

	};

	return query_processing;
}

void transport::detail::InputReader(transport::TransportCatalogue& transport_catalogue) {

	int query_count;
	std::cin >> query_count;
	std::cin.get();

	std::unordered_map<std::string,Query> tmp_queries;
	std::vector<Query> stop_query; 
	std::vector<Query> bus_query;	
	std::vector<Query> distance_query;	

	for (int i = 0; i < query_count; ++i) {
		std::string temp_queries;		
		std::getline(std::cin, temp_queries);
		tmp_queries = QueryParser(temp_queries);

		if (tmp_queries.count("Stop") != 0) {
			stop_query.push_back(std::move(tmp_queries.at("Stop")));
		}

		if (tmp_queries.count("Bus") != 0) {
			bus_query.push_back(std::move(tmp_queries.at("Bus")));
		}

		if (tmp_queries.count("Dist") != 0) {
			distance_query.push_back(std::move(tmp_queries.at("Dist")));
		}
	}

	for (auto queri : stop_query) {

		transport::data::Stop tmp(queri.name ,std::stod(queri.query_vector[0]) ,std::stod(queri.query_vector[1]) );
		transport_catalogue.AddStop(tmp);
	}

	for (auto queri : bus_query) {
		std::deque<transport::data::Stop*> stop_v;
		
		std::set<std::string> to_stops;
		
		for (auto rq : queri.query_vector) {
			stop_v.push_back(transport_catalogue.FindeStop(rq));
			to_stops.insert(transport_catalogue.FindeStop(rq)->name_);			
		}
		transport::data::Bus bus_tmp(queri.name, stop_v);
		transport_catalogue.AddBus(bus_tmp);

		transport_catalogue.AddBusToStop(queri.name,to_stops);
	}

	for (auto query : distance_query) {

		for (auto rq_d : query.query_vector) {
			std::string to_stop;
			std::string dist_str;
			double btw_dist;
			int ptr;
			ptr = rq_d.find(">");
			to_stop = rq_d.substr(0,ptr);
			dist_str = rq_d.substr(ptr + 1);
			btw_dist = std::stod(dist_str);
			transport_catalogue.SetDistBtwStop(*transport_catalogue.FindeStop(query.name), *transport_catalogue.FindeStop(to_stop),btw_dist);
		}
	}
	std::cout << std::flush;
}
