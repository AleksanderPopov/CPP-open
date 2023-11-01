#pragma once
#include "request_queue.h"
#include "log_duration.h"
#include "remove_duplicates.h"
#include "test_example_functions.h"
#include <numeric>

template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const std::string& t_str, const std::string& u_str, const std::string& file,
    const std::string& func, unsigned line, const std::string& hint) {
    if (t != u) {
        std::cout << std::boolalpha;
        std::cout << file << "("s << line << "): "s << func << ": "s;
        std::cout << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        std::cout << t << " != "s << u << "."s;
        if (!hint.empty()) {
            std::cout << " Hint: "s << hint;
        }
        std::cout << std::endl;
        abort();
    }
}

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

void AssertImpl(bool value, const std::string& expr_str, const std::string& file, const std::string& func, unsigned line,
    const std::string& hint) {
    if (!value) {
        std::cout << file << "("s << line << "): "s << func << ": "s;
        std::cout << "ASSERT("s << expr_str << ") failed."s;
        if (!hint.empty()) {
            std::cout << " Hint: "s << hint;
        }
        std::cout << std::endl;
        abort();
    }
}

#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))


template <typename T>
void RunTestImpl(T test, std::string func_name) {
    test();
    std::cerr << func_name << " OK" << std::endl;
}

#define RUN_TEST(func) RunTestImpl((func), #func)

void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const std::string content = "cat in the city"s;
    const std::vector<int> ratings = { 1, 2, 3 };
    {
        SearchServer server("the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server("in the"s);        
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(), "Stop words must be excluded from documents"s);
        ASSERT_HINT(server.FindTopDocuments("the"s).empty(), "Stop words must be excluded from documents"s);
    }
}

void TestAddDocuments() {

    std::string content = "";
    int doc_id = 0;
    std::vector<int> ratings = {};

    {
        SearchServer server(""s);
        doc_id = 42;
        content = "cat in the city"s;
        std::vector<int> ratings = { 1, 2, 3 };
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s, DocumentStatus::ACTUAL);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server(""s);
        content = "off on in the"s;
        doc_id = 42;
        std::vector<int> ratings = { 1, 2, 3 };
        server.AddDocument(doc_id, content, DocumentStatus::BANNED, ratings);
        ASSERT_EQUAL(server.GetDocumentCount(), 1u);
    }

    {
        SearchServer server(""s);
        server.AddDocument(0, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, { 8, -3 });
        server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
        server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
        server.AddDocument(3, "ухоженный скворец евгений"s, DocumentStatus::BANNED, { 9 });
        ASSERT_EQUAL(server.GetDocumentCount(), 4u);
        ASSERT_EQUAL(server.FindTopDocuments("кот"s).size(), 2u);
        ASSERT_EQUAL(server.FindTopDocuments("ухоженный"s).size(), 1u);
    }

}

void TestStopWords() {
    SearchServer server("и в на кот"s);

    server.AddDocument(10, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, { 8, -3 });
    server.AddDocument(5, "пушистый кот пушистый хвост кот"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    server.AddDocument(8, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
    server.AddDocument(12, "на и в"s, DocumentStatus::ACTUAL, { 8, -1, 6, 7 });

    const auto docs = server.FindTopDocuments("ухоженный кот"s);
    int doc_id = 8;
    const Document& doc0 = docs[0];
    ASSERT_EQUAL(docs.size(), 1u);
    ASSERT_EQUAL(doc0.id, doc_id);

}

void TestMatching() {                                       
    SearchServer server("in the on off"s);
    const std::vector<int> ratings = { 1, 2, 3 };

    server.AddDocument(12, "cat on street"s, DocumentStatus::ACTUAL, ratings);
    server.AddDocument(22, "dog in home"s, DocumentStatus::ACTUAL, ratings);

    ASSERT_EQUAL(std::get<0>(server.MatchDocument("dog street"s, 12)).size(), 1u);
    ASSERT_EQUAL(std::get<0>(server.MatchDocument("dog street"s, 22)).size(), 1u);
    ASSERT_HINT(std::get<0>(server.MatchDocument("dog home"s, 12)).empty(), "empty Matching");
    ASSERT_EQUAL(std::get<0>(server.MatchDocument("dog home"s, 22)).size(), 2u);
    ASSERT_HINT(std::get<0>(server.MatchDocument("-cat home"s, 12)).empty(), "empty Matching");
    ASSERT_EQUAL(std::get<0>(server.MatchDocument("-cat home"s, 22)).size(), 1u);
    ASSERT_HINT(std::get<0>(server.MatchDocument("in on"s, 22)).empty(), "empty Matching");
}

void TestMinusWord() {
    SearchServer server("и в на"s);

    server.AddDocument(10, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, { 8, -3 });
    server.AddDocument(5, "пушистый кот пушистый хвост кот"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    server.AddDocument(8, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });

    const auto docs = server.FindTopDocuments("ухоженный кот -пёс"s);
    ASSERT_EQUAL(docs.size(), 2u);

    const Document& doc0 = docs[0];
    const Document& doc1 = docs[1];
    ASSERT_EQUAL(doc0.id, 5);
    ASSERT_EQUAL(doc1.id, 10);

}

void TestReiting() {
    SearchServer server(""s);
    std::vector<int> rating1 = { 8, -3 };
    server.AddDocument(10, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, rating1);

    std::vector<int> rating2 = { 7, 2, 7 };
    server.AddDocument(5, "пушистый кот пушистый хвост кот"s, DocumentStatus::ACTUAL, rating2);

    std::vector<int> rating3 = { 5, -12, 2, 1 };
    server.AddDocument(8, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, rating3);

    std::vector<int> rating4 = { 9 };
    server.AddDocument(3, "ухоженный скворец евгений"s, DocumentStatus::ACTUAL, rating4);


    int sum1 = std::accumulate(rating1.begin(), rating1.end(), 0);
    int rait1 = sum1 / static_cast<int>(rating1.size());

    int sum2 = std::accumulate(rating2.begin(), rating2.end(), 0);
    int rait2 = sum2 / static_cast<int>(rating2.size());

    int sum3 = std::accumulate(rating3.begin(), rating3.end(), 0);
    int rait3 = sum3 / static_cast<int>(rating3.size());

    int sum4 = std::accumulate(rating4.begin(), rating4.end(), 0);
    int rait4 = sum4 / static_cast<int>(rating4.size());


    const auto docs2 = server.FindTopDocuments("ухоженный кот"s);
    ASSERT_EQUAL(docs2.size(), 4u);
    ASSERT_EQUAL(docs2[0].rating, rait2);
    ASSERT_EQUAL(docs2[1].rating, rait4);
    ASSERT_EQUAL(docs2[2].rating, rait3);
    ASSERT_EQUAL(docs2[3].rating, rait1);
}

void TestStatus() {

    SearchServer server(""s);

    server.AddDocument(10, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, { 8, -3 });
    server.AddDocument(5, "пушистый кот пушистый хвост кот"s, DocumentStatus::REMOVED, { 7, 2, 7 });
    server.AddDocument(8, "ухоженный пёс выразительные глаза"s, DocumentStatus::REMOVED, { 5, -12, 2, 1 });
    server.AddDocument(3, "ухоженный скворец евгений"s, DocumentStatus::BANNED, { 9 });
    server.AddDocument(15, "ухоженный скворец евгений"s, DocumentStatus::IRRELEVANT, { 5 });

    ASSERT_EQUAL(server.FindTopDocuments("ухоженный кот"s, DocumentStatus::BANNED).size(), 1u);
    ASSERT_EQUAL(server.FindTopDocuments("евгений"s, DocumentStatus::IRRELEVANT).size(), 1u);

    const auto docs1 = server.FindTopDocuments("ухоженный кот"s, DocumentStatus::ACTUAL);
    const Document& doc0 = docs1[0];
    ASSERT((docs1.size() == 1) && (doc0.id == 10));

    const auto docs2 = server.FindTopDocuments("ухоженный кот"s, DocumentStatus::REMOVED);
    ASSERT_EQUAL(docs2.size(), 2u);
    ASSERT_EQUAL(docs2[0].id, 5u);
    ASSERT_EQUAL(docs2[1].id, 8u);

}

void TestRelevance() {                                      

    SearchServer search_server("и в на"s);
    const double TestValues[3] = { 0.274653, 0.162186, 0.101366 };                      

    search_server.AddDocument(10, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, { 8, -3 });
    search_server.AddDocument(5, "пушистый кот пушистый хвост кот"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(8, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });

    auto Docs = search_server.FindTopDocuments("ухоженный кот"s);
    ASSERT_EQUAL(Docs.size(), 3u);
    int i;
    for (i = 0; i < 3; i++) {
        ASSERT(std::abs(Docs[i].relevance - TestValues[i]) < 1e-6);
    }

    ASSERT(!(std::abs(Docs[1].relevance - TestValues[2]) < 1e-6));
}


void TestSearchServer() {
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(TestAddDocuments);
    RUN_TEST(TestStopWords);
    RUN_TEST(TestMatching);
    RUN_TEST(TestMinusWord);
    RUN_TEST(TestReiting);  
    RUN_TEST(TestStatus);
    RUN_TEST(TestRelevance);
}


