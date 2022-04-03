#include "input_reader.h"

std::unordered_map<std::string,Query> QueryParser(std::string& query) {
	
	std::unordered_map<std::string,Query> query_processing;		
	Query qr;

	if (query.find("Bus") == 0) {
				
		int name_start = query.find_first_of(" ");
		int name_stop = query.find_first_of(":") - 1;
		std::string name = query.substr(name_start + 1, name_stop - name_start);		
		qr.name = name;
			
		if (query.find(">") != std::string::npos) {			
			int start_tmp = query.find_first_not_of(" ", name_stop + 2);
			int stop_tmp = 0;
			while (stop_tmp != std::string::npos)
			{
				std::string bus_tmp;
				stop_tmp = query.find_first_of(">", start_tmp);
				bus_tmp = query.substr(start_tmp, stop_tmp - 1 - start_tmp);
				start_tmp = stop_tmp + 2;
				qr.query_vector.push_back(std::move(bus_tmp));
			}						
			query_processing.emplace("Bus", std::move(qr));
		}

		if (query.find("-") != std::string::npos) {			
			int start_tmp=query.find_first_not_of(" ",name_stop+2);
			int stop_tmp=0;
			while (stop_tmp!=std::string::npos)
			{
				std::string bus_tmp;
				stop_tmp = query.find_first_of("-",start_tmp);
				bus_tmp = query.substr(start_tmp, stop_tmp-1 - start_tmp);				
				start_tmp = stop_tmp+2;				
				qr.query_vector.push_back(std::move(bus_tmp));
			}

			int ptr = qr.query_vector.size()-1;
			while (ptr != 0) {				
				qr.query_vector.push_back(qr.query_vector[ptr-1]);
				ptr--;
			}
		}
		query_processing.emplace("Bus", std::move(qr));

	};

	if (query.find("Stop") == 0) {		
		int name_start = query.find_first_of(" ");
		int name_stop = query.find_first_of(":")-1;
		std::string name = query.substr(name_start+1,name_stop-name_start);

		int start_lat;
		int start_lon;
		int stop_lat;
		//int stop_lon;
		start_lat = query.find_first_of(" ", name_stop)+1;	
		stop_lat = query.find_first_of(" ", start_lat)-1;
		start_lon = query.find_first_of(",", name_stop)+2;
		

		std::string lat_s = query.substr(start_lat,stop_lat-start_lat);
		std::string lon_s = query.substr(start_lon);
		
		qr.name = name;
		qr.query_vector.push_back(std::move(lat_s));
		qr.query_vector.push_back(std::move(lon_s));		
		query_processing.emplace("Stop", std::move(qr));
	};

	return query_processing;
}

void InputReader(TransportCatalogue& trans_cat) {

	int query_count;
	std::cin >> query_count;
	std::cin.get();

	std::unordered_map<std::string,Query> tmp_queries;		
	std::vector<Query> stop_q;
	std::vector<Query> bus_q;

	for (int i = 0; i < query_count; ++i) {
		std::string temp_queries;		
		std::getline(std::cin, temp_queries);
		tmp_queries = QueryParser(temp_queries);

		if (tmp_queries.count("Stop") != 0) {
			stop_q.push_back(std::move(tmp_queries.at("Stop")));
		}

		if (tmp_queries.count("Bus") != 0) {
			bus_q.push_back(std::move(tmp_queries.at("Bus")));
		}
	}

	for (auto queri : stop_q) {

		Stop tmp(queri.name ,std::stod(queri.query_vector[0]) ,std::stod(queri.query_vector[1]) );
		trans_cat.AddStop(tmp);
	}

	for (auto queri : bus_q) {
		std::deque<Stop*> stop_v;		
		std::set<std::string> to_stops;
		
		for (auto rq : queri.query_vector) {
			stop_v.push_back(trans_cat.FindeStop(rq));
			to_stops.insert(trans_cat.FindeStop(rq)->name_);
		}
		Bus bus_tmp(queri.name, stop_v);
		trans_cat.AddBus(bus_tmp);

		trans_cat.AddBusToStop(queri.name,to_stops);
	}

	std::cout << std::flush;
}
