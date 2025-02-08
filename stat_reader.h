#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

void ParseFullRequest(const TransportCatalogue& tansport_catalogue, std::istream& in,
     std::ostream& output);

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
std::ostream& output);