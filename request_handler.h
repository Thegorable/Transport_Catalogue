#pragma once
#include <memory>
#include "transport_catalogue.h"

enum class RequestType {Bus, Stop, Error};

struct NeighborStop {
	std::string_view name_;
	int distance_;
};

struct Request {
    Request() = default;
    Request(RequestType type = RequestType::Bus);

    RequestType type_;
    std::string_view name_;

	virtual ~Request() = default;
};

struct RequestBaseStop : public Request {
    RequestBaseStop(RequestType type = RequestType::Bus, Geo::Coordinates coords = {0.0, 0.0});

    Geo::Coordinates coords_;
	std::vector<NeighborStop> neighbor_stops_;
};

struct RequestBaseBus : public Request {
    RequestBaseBus(RequestType type = RequestType::Bus, bool is_round_trip = false);

    std::vector<std::string_view> route_;
    bool is_round_trip_;
};

struct Stat : public Request {
    Stat(RequestType type = RequestType::Bus, int id = 0);
    int id_ = 0;
};

struct StatStop : public Stat {
    StatStop(RequestType type = RequestType::Bus, int id = 0);

    const BusPtrsSet* buses_;
};

struct StatBus : public RouteStatistics, public Stat {
    StatBus(const RouteStatistics& parent, RequestType type = RequestType::Bus, int id = 0);
};

void ProvideInputRequests(const std::vector<std::shared_ptr<Request>>& input_requests, TransportCatalogue& transport_c);
std::vector<std::shared_ptr<Stat>> GetStats(const std::vector<std::shared_ptr<Stat>>& out_requests, const TransportCatalogue& transport_c);

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
/*
class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};
*/