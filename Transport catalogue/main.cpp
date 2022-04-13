#include "geo.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"


int main()
{
    TRANSPORT::TransportCatalogue transport_cat;
    TRANSPORT::DETAIL::InputReader(transport_cat);   
    TRANSPORT::DETAIL::OutputReader(transport_cat);
}
