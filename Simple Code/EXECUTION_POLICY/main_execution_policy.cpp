#include <algorithm>
#include <execution>
#include <iostream>
#include <list>
#include <random>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include<future>
#include<iterator>

#include "log_duration.h"

using namespace std;

template <typename Strings>
void PrintStrings(const Strings& strings) {
    for (string_view s : strings) {
        cout << s << " ";
    }
    cout << endl;
}

template <typename ExecutionPolicy,typename ForwardRange, typename Function>
void ForEach ( ExecutionPolicy&& policy, ForwardRange& range, Function function) {

    if constexpr(is_same_v<decay_t<ExecutionPolicy>, execution::sequenced_policy>) {
        for_each(policy, range.begin(), range.end(), function);
    }

    if constexpr ((is_same_v<decay_t<ExecutionPolicy>, execution::parallel_policy>) &&
        (is_same_v<typename iterator_traits<typename ForwardRange::iterator>::iterator_category, random_access_iterator_tag>)) {
        for_each(policy, range.begin(), range.end(), function);
    }

    if constexpr ((is_same_v<decay_t<ExecutionPolicy>, execution::parallel_policy>) &&
        !(is_same_v<typename iterator_traits<typename ForwardRange::iterator>::iterator_category, random_access_iterator_tag>)) {
        const int N_TASKS = 4;
        const int count = range.size() / N_TASKS;
        auto it_begin = range.begin();
        auto it_end = next(range.begin(), count);
        vector<future<void>> f;
        f.reserve(N_TASKS);
        for (int i = 0; i < N_TASKS; ++i) {
            f.push_back(async([it_begin, it_end, function] {for_each(execution::par, it_begin, it_end, function); }));
            it_begin = next(it_begin, count);
            it_end = (i == N_TASKS - 2) ? range.end() : next(it_end, count);
        }

        for (auto& q : f) {
            q.get();
        }

    };
}


template <typename ForwardRange, typename Function>
void ForEach(ForwardRange& range, Function function) {
    ForEach(execution::seq, range, function);
}


int main() {

    LOG_DURATION("Total");

    auto reverser = [](string& s) { reverse(s.begin(), s.end()); };

    list<string> strings_list = { "cat", "dog", "code" };

    ForEach(strings_list, reverser);
    PrintStrings(strings_list);
    // tac god edoc

    ForEach(execution::seq, strings_list, reverser);
    PrintStrings(strings_list);
    // cat dog code

    ForEach(execution::par, strings_list, reverser);
    PrintStrings(strings_list);
    // tac god edoc
    
    vector<string> strings_vector = { "cat", "dog", "code" };

    ForEach(strings_vector, reverser);
    PrintStrings(strings_vector);
    // tac god edoc

    ForEach(execution::seq, strings_vector, reverser);
    PrintStrings(strings_vector);
    // cat dog code

    ForEach(execution::par, strings_vector, reverser);
    PrintStrings(strings_vector);
    // tac god edoc

    return 0;
}
