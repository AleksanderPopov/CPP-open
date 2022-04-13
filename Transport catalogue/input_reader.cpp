#include "input_reader.h"

std::unordered_map<std::string, TRANSPORT::DETAIL::Query> TRANSPORT::DETAIL::QueryParser(std::string& query) {
	
	std::unordered_map<std::string, TRANSPORT::DETAIL::Query> query_processing;
	TRANSPORT::DETAIL::Query qr;

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

		if (query.find(">") == std::string::npos && query.find("-") == std::string::npos) {
			int start_tmp = query.find_first_not_of(" ", name_stop + 2);
			std::string bus_tmp;
			bus_tmp = query.substr(start_tmp);
			qr.query_vector.push_back(std::move(bus_tmp));
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
		int stop_lon;

		start_lat = query.find_first_of(" ", name_stop)+1;	
		stop_lat = query.find_first_of(" ", start_lat)-1;
		start_lon = query.find_first_of(",", name_stop)+2;
		stop_lon = query.find_first_of(" ", start_lon);		

		std::string lat_s = query.substr(start_lat,stop_lat-start_lat);
		std::string lon_s = query.substr(start_lon,stop_lon-1-start_lon);  

		TRANSPORT::DETAIL::Query dist_query;

		if (stop_lon != std::string::npos) {

			std::string dist_qr = query.substr(stop_lon - 1);
			std::string dist_str;
			std::string stop_to;
			dist_query.name = name;

			int start_dist;
			int stop_dist;
			int start_to_stop;
			int stop_to_stop = 0;

			while (stop_to_stop != std::string::npos) {
				start_dist = dist_qr.find_first_of(",") + 2;
				stop_dist = dist_qr.find("m to ");
				dist_str = dist_qr.substr(start_dist, stop_dist - start_dist);

				start_to_stop = stop_dist + 5;
				stop_to_stop = dist_qr.find_first_of(",", start_to_stop);
				stop_to = dist_qr.substr(start_to_stop, stop_to_stop - start_to_stop);

				if (stop_to_stop != std::string::npos) {
					dist_qr = dist_qr.substr(stop_to_stop);
				}
				std::string result_q_d = stop_to + ">" + dist_str;
				dist_query.query_vector.push_back(std::move(result_q_d));

			}
		}

		qr.name = name;
		qr.query_vector.push_back(std::move(lat_s));
		qr.query_vector.push_back(std::move(lon_s));

		query_processing.emplace("Stop", std::move(qr));

		if (!dist_query.query_vector.empty()) {
			query_processing.emplace("Dist", std::move(dist_query));
		}
				
	};

	return query_processing;
}

void TRANSPORT::DETAIL::InputReader(TRANSPORT::TransportCatalogue& trans_cat) {

	int query_count;
	std::cin >> query_count;
	std::cin.get();

	std::unordered_map<std::string, TRANSPORT::DETAIL::Query> tmp_queries;
	std::vector<TRANSPORT::DETAIL::Query> stop_q;
	std::vector<TRANSPORT::DETAIL::Query> bus_q;
	std::vector<TRANSPORT::DETAIL::Query> dist_q;

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

		if (tmp_queries.count("Dist") != 0) {
			dist_q.push_back(std::move(tmp_queries.at("Dist")));
		}
	}

	for (auto queri : stop_q) {

		TRANSPORT::DATA::Stop tmp(queri.name ,std::stod(queri.query_vector[0]) ,std::stod(queri.query_vector[1]) );
		trans_cat.AddStop(tmp);
	}

	for (auto queri : bus_q) {
		std::deque<TRANSPORT::DATA::Stop*> stop_v;
		
		std::set<std::string> to_stops;
		
		for (auto rq : queri.query_vector) {
			stop_v.push_back(trans_cat.FindeStop(rq));
			to_stops.insert(trans_cat.FindeStop(rq)->name_);			
		}
		TRANSPORT::DATA::Bus bus_tmp(queri.name, stop_v);
		trans_cat.AddBus(bus_tmp);

		trans_cat.AddBusToStop(queri.name,to_stops);
	}

	for (auto query : dist_q) {

		for (auto rq_d : query.query_vector) {
			std::string to_stop;
			std::string dist_str;
			double btw_dist;
			int ptr;
			ptr = rq_d.find(">");
			to_stop = rq_d.substr(0,ptr);
			dist_str = rq_d.substr(ptr+1);
			btw_dist = std::stod(dist_str);
			trans_cat.AddDistBtwStop(*trans_cat.FindeStop(query.name), *trans_cat.FindeStop(to_stop),btw_dist);
		}
	}
	std::cout << std::flush;
}
