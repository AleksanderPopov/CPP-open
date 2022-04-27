#include "geo.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

int main()
{
    transport::TransportCatalogue transport_cat;
    transport::detail::InputReader(transport_cat);   
    transport::detail::OutputReader(transport_cat);
}
