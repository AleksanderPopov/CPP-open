#include "json_reader.h"

using namespace transport;
using namespace json;
using namespace std::literals;

void JsonReader::Reader() {
    Node document_json = Load(std::cin).GetRoot();
    Dict root = document_json.AsMap();
    Array content_base = root.at("base_requests"s).AsArray();
    Array content_state = root.at("stat_requests"s).AsArray();

    CompletingCatalog(content_base);
    ProcessingRequest(content_state);
    PrintResult();
}

////////// base_requests //////////
void JsonReader::CompletingCatalog(Array& value) {

    std::vector<json::Node> bus_descriptions;
    std::map<std::string, Dict> road_distances;
    std::vector<std::string> bus_names;
    for (auto& description : value) {
        if (description.AsMap().at("type"s) == "Stop"s) {
            AddingStop(description.AsMap());           
            road_distances[description.AsMap().at("name"s).AsString()] = description.AsMap().at("road_distances"s).AsMap();
        }
        else if (description.AsMap().at("type"s) == "Bus"s) {
            bus_descriptions.push_back(description.AsMap());
            bus_names.push_back(description.AsMap().at("name"s).AsString());
        }
    }

    for (Node& description : bus_descriptions) {
        AddingBus(description.AsMap());
    }

    for (auto& [stop_one, other] : road_distances) { 
        for (auto& [stop_two, distance] : other) { 
            catalog_.SetDistBtwStop(*catalog_.FindStop(stop_one), *catalog_.FindStop(stop_two), distance.AsInt()); //FIX
        }
    }

}

void JsonReader::AddingStop(const Dict& stop) {
    std::string name;
    double latitude;
    double longitude;

    name = stop.at("name"s).AsString();
    latitude = stop.at("latitude"s).AsDouble();
    longitude = stop.at("longitude"s).AsDouble();

    transport::data::Stop cat_stop{ name, latitude, longitude };

    catalog_.AddStop(cat_stop); 
}

void JsonReader::AddingBus(const Dict& bus) {
    std::string name;
    Array stope;

    name = bus.at("name"s).AsString();;
    stope = bus.at("stops"s).AsArray();

    std::deque<transport::data::Stop*> stop_to_bus;
    std::set<std::string> to_stops;
    if (bus.at("is_roundtrip"s).AsBool()){
        for (auto& kek : stope) {
            stop_to_bus.push_back(catalog_.FindStop(kek.AsString()));
            to_stops.insert(catalog_.FindStop(kek.AsString())->name_);
           
        }
        stop_to_bus.push_back(stop_to_bus[0]);
    }
    else
    {
        for (auto& kek : stope) {           
            stop_to_bus.push_back(catalog_.FindStop(kek.AsString()));
            to_stops.insert(catalog_.FindStop(kek.AsString())->name_);
           
        }
        int ptr = stop_to_bus.size() - 1;
        while (ptr != 0) {
            stop_to_bus.push_back(stop_to_bus[ptr - 1]);
            ptr--;
        }
    }

    transport::data::Bus cat_bus{ name,stop_to_bus };
    catalog_.AddBus(cat_bus);

    if (bus.at("is_roundtrip"s).AsBool()) {
        catalog_.FindBus(name)->busloop_ = true;
    }
    
    catalog_.AddBusToStop(name, to_stops);

}
//-------- base_requests //--------

////////// stat_requests //////////
void JsonReader::ProcessingRequest(Array& value) {
    for (auto& description : value) {
        if (description.AsMap().at("type"s) == "Stop"s) {
            StopInformationPrinting(description.AsMap());
        }
        else if (description.AsMap().at("type"s) == "Bus"s) {
            BusInformationPrinting(description.AsMap());
        }
    }
}

void JsonReader::StopInformationPrinting(const Dict& value) {
    int id;
    std::string name;
    Array bus_names;

    id = value.at("id"s).AsInt();
    name = value.at("name"s).AsString();

    std::set<std::string> buses;

    if (catalog_.FindStop(name) == nullptr) {
        print_.emplace_back(Dict{ {"request_id"s, id}, {"error_message"s, "not found"s} });
    }
    else {
        try {
            transport::data::StopInfo stop_info=catalog_.GetStopInfo(name);
            buses = stop_info.buses_;
            for (auto& bus : buses) {
                bus_names.push_back(bus);
            }
        }
        catch (...) {}
        print_.emplace_back(Dict{ {"buses"s, bus_names}, {"request_id"s, id} });
    }
}

void JsonReader::BusInformationPrinting(const Dict& value) {
    std::string name;
    double curvature;
    int id;
    double route_length;
    size_t stop_count;
    size_t unique_stop_count;

    name = value.at("name"s).AsString();
    id = value.at("id"s).AsInt();
  
    if (catalog_.FindBus(name) == nullptr) {
        print_.emplace_back(Dict{ {"request_id"s, id}, {"error_message"s, "not found"s} });
    }
    else {
        transport::data::BusInfo info = catalog_.GetBusInfo(value.at("name"s).AsString());
        curvature = info.curvature_;
        route_length = info.distance_route_;
        if (info.loop_) {
            stop_count = info.number_stop_-1;
        }
        else
        {
            stop_count = info.number_stop_;
        }
        
        unique_stop_count = info.uniq_number_stop_;
        print_.emplace_back(Dict{ {"curvature"s, curvature}, {"request_id"s, id}, {"route_length"s, route_length}, {"stop_count"s, int(stop_count)}, {"unique_stop_count"s, int(unique_stop_count)} });
    }
}

void JsonReader::PrintResult() {
    Print(Document{ print_ }, output_);
}

std::ostringstream& JsonReader::Result() {
    return output_;
}