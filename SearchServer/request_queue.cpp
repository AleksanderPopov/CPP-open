#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server)
    : search_server_(search_server) {
    time_ = 0;
    empty_request_ = 0;
}

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    time_++;
    if (time_ == sec_in_day_) {
        time_ = 0;
    }
    request_.time = time_;
    request_.requests = raw_query;
    request_.answer = search_server_.FindTopDocuments(raw_query, document_predicate);

    if (requests_.size() == sec_in_day_) {
        if (requests_.front().answer.empty()) {
            empty_request_--;
        }
        requests_.pop_front();
    }

    requests_.push_back(request_);
    if (request_.answer.empty()) {
        empty_request_++;
    }
    return request_.answer;
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    time_++;
    if (time_ == sec_in_day_) {
        time_ = 0;
    }

    request_.time = time_;
    request_.requests = raw_query;
    request_.answer = search_server_.FindTopDocuments(raw_query, status);


    if (requests_.size() == sec_in_day_) {
        if (requests_.front().answer.empty()) {
            empty_request_--;
        }
        requests_.pop_front();
    }

    requests_.push_back(request_);
    if (request_.answer.empty()) {
        empty_request_++;
    }
    return request_.answer;
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    time_++;
    if (time_ == sec_in_day_) {
        time_ = 0;
    }

    request_.time = time_;
    request_.requests = raw_query;
    request_.answer = search_server_.FindTopDocuments(raw_query);


    if (requests_.size() == sec_in_day_) {
        //c
        if (requests_.front().answer.empty()) {
            empty_request_--;
        }
        requests_.pop_front();
    }

    requests_.push_back(request_);
    if (request_.answer.empty()) {
        empty_request_++;
    }
    return request_.answer;
}

int RequestQueue::GetNoResultRequests() const {
    return empty_request_;
}