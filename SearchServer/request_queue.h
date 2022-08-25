#pragma once

#include "document.h"
#include "read_input_functions.h"
#include "string_processing.h"
#include "search_server.h"

class RequestQueue {
public:

    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;

private:
    struct QueryResult {
        QueryResult() = default;

        QueryResult(int time, std::string requests, std::vector<Document> answer)
            : time(time)
            , requests(requests)
            , answer(answer) {
        }

        int time = 0;                           //время
        std::string requests = {};                  // запрос
        std::vector<Document> answer = {};          //вектор ответов      
    };

    std::deque<QueryResult> requests_;           //очередь запросов
    const static int sec_in_day_ = 1440;    // сутки
    int time_;                               //время запроса    
    int empty_request_;                                //количество пустых ответов
    QueryResult request_;                   //один запрос    
    const SearchServer& search_server_;            // заглушка временно
};
