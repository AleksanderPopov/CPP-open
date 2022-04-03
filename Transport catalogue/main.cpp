//#include <iostream>

#include "geo.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"


int main()
{
    TransportCatalogue transport_cat;
    InputReader(transport_cat);
    OutputReader(transport_cat);
}
