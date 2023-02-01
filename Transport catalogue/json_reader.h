#pragma once

#include <sstream>

#include "json.h"
#include "transport_catalogue.h"

class JsonReader {
public:
    JsonReader(std::ostringstream& output)
        : output_(output)
    {
    }

    void Reader();

    std::ostringstream& Result();

private:
    transport::TransportCatalogue catalog_;
    json::Array print_;
    std::ostringstream& output_;

    // base_requests 
    void CompletingCatalog(json::Array& value);
    void AddingStop(const json::Dict& stop);
    void AddingBus(const json::Dict& bus);

    //stat_requests
    void ProcessingRequest(json::Array& value);
    void StopInformationPrinting(const json::Dict& value);
    void BusInformationPrinting(const json::Dict& value);

    void PrintResult();
};
