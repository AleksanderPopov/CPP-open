#include "test_example_functions.h"

using namespace std::literals;

void PrintDocument(const Document& document) {
    std::cout << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s << std::endl;
}

void PrintMatchDocumentResult(int document_id, const std::vector<std::string>& words, DocumentStatus status) {
    std::cout << "{ "s
        << "document_id = "s << document_id << ", "s
        << "status = "s << static_cast<int>(status) << ", "s
        << "words ="s;
    for (const std::string& word : words) {
        std::cout << ' ' << word;
    }
    std::cout << "}"s << std::endl;
}

void AddDocument(SearchServer& search_server, int document_id, const std::string& document, DocumentStatus status,
    const std::vector<int>& ratings) {
    try {
        search_server.AddDocument(document_id, document, status, ratings);
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Error Add Document "s << document_id << ": "s << e.what() << std::endl;
    }
}

void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query) {

    LOG_DURATION_STREAM("Operation time", std::cout);

    std::cout << "Search results: "s << raw_query << std::endl;
    try {
        for (const Document& document : search_server.FindTopDocuments(raw_query)) {
            PrintDocument(document);
        }
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Search error: "s << e.what() << std::endl;
    }
}

void MatchDocuments(const SearchServer& search_server, const std::string& query) {

    LOG_DURATION_STREAM("Operation time", std::cout);

    try {
        std::cout << "Document matching on request: "s << query << std::endl;

        for (auto id : search_server) {
            const auto [words, status] = search_server.MatchDocument(query, id);
            PrintMatchDocumentResult(id, words, status);
        }
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Error matching documents to query "s << query << ": "s << e.what() << std::endl;
    }
}
