//
// Created by scott on 22/01/2020.
//

#ifndef SXCLIENT_APICLIENT_H
#define SXCLIENT_APICLIENT_H

#include <3rd-party/SimpleWeb/client_https.hpp>
#include <api_data/MarketCatalogue.h>
#include <api_data/PlaceExecutionReport.h>
#include <api_data/PlaceInstruction.h>
#include <api_data/MarketVersion.h>
#include <api_data/PriceProjection.h>
#include <api_data/MarketBook.h>
#include <api_data/TimeRange.h>
#include <api_data/CurrentOrderSummaryReport.h>
#include <api_data/CancelInstruction.h>
#include <api_data/CancelExecutionReport.h>
#include <api_data/UpdateInstruction.h>
#include <api_data/UpdateExecutionReport.h>
#include <api_data/ReplaceInstruction.h>
#include <api_data/ReplaceExecutionReport.h>
#include <api_data/MarketFilter.h>
#include <set>


class APIClient {
    using HttpsClient = SimpleWeb::Client<SimpleWeb::HTTPS>;

    std::string url;
    SimpleWeb::CaseInsensitiveMultimap header;
public:
    std::forward_list<API::MarketCatalogue>  listMarketCatalogue(const API::MarketFilter &filter, const std::set<std::string>& marketProjection, const std::string& sort,int maxResults, const std::string& locale);
    API::PlaceExecutionReport                placeOrders(const std::string& marketId, const std::vector<API::PlaceInstruction>& instructions, std::optional<std::string> customerRef,std::optional<API::MarketVersion> marketVersion, std::optional<std::string> customerStrategyRef, std::optional<bool> async);
    std::forward_list<API::MarketBook>       listMarketBook(const std::forward_list<std::string>& marketIds, std::optional<API::PriceProjection> priceProjection = std::nullopt, std::optional<std::string> orderProjection = std::nullopt, std::optional<std::string> matchProjection = std::nullopt, std::optional<bool> includeOverallPosition = std::nullopt, std::optional<bool> partitionMatchedByStrategyRef = std::nullopt, std::optional<std::vector<std::string>> customerStrategyRefs = std::nullopt, std::optional<std::string> currencyCode = std::nullopt, std::optional<std::string> locale = std::nullopt, std::optional<std::string> matchedSince = std::nullopt, std::optional<std::vector<std::string>> betIds = std::nullopt);
    API::CurrentOrderSummaryReport           listCurrentOrders(std::optional<std::set<std::string>> betIds, std::optional<std::set<std::string>> marketIds,
                                                            std::optional<std::string> orderProjection,
                                                            std::optional<std::set<std::string>> customerOrderRefs,
                                                            std::optional<std::set<std::string>> customerStrategyRefs, std::optional<API::TimeRange> dateRange,
                                                            std::optional<std::string> orderBy, std::optional<std::string> sortDir,
                                                            std::optional<int> fromRecord,
                                                            std::optional<int> recordCount);
    API::CancelExecutionReport               cancelOrders(std::optional<std::string> marketId, std::optional<std::forward_list<API::CancelInstruction>> instructions, std::optional<std::string> customerRef);
    API::UpdateExecutionReport               updateOrders(std::string marketId, std::forward_list<API::UpdateInstruction> instructions, std::optional<std::string> customerRef);
    API::ReplaceExecutionReport              replaceOrders(const std::string& marketId, std::forward_list<API::ReplaceInstruction> instructions, std::optional<std::string> customerRef, std::optional<API::MarketVersion> marketVersion, std::optional<bool> async);
    APIClient();
};


#endif //SXCLIENT_APICLIENT_H
