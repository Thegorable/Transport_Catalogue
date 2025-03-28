#pragma once
#include "domain.h"
#include "json.h"
#include "request_handler.h"

std::vector<std::shared_ptr<Request>>  ReadBaseJsonRequests(const json::Document& doc);
std::vector<std::shared_ptr<Stat>>  ReadStatJsonRequests(const json::Document& doc);
json::Document BuildStatJsonOutput(const std::vector<std::shared_ptr<Stat>>& answers);