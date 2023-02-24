#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>
#include<future>
#include<iterator>
#include"log_duration.h"

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    // 1. Если диапазон содержит меньше 2 элементов, выходим из функции
    int range_length = range_end - range_begin;
    if (range_length < 2) {
        return;
    }

    vector elements(range_begin, range_end);

    auto mid = elements.begin() + range_length / 2;
    if (range_length >20000) {
        auto merge_left = async(MergeSort<vector<typename iterator_traits<RandomIt>::value_type>::iterator>, elements.begin(), mid);
        auto merge_right = async(MergeSort<vector<typename iterator_traits<RandomIt>::value_type>::iterator>, mid, elements.end());
        merge_left.get();
        merge_right.get();
    }
    else
    {
        MergeSort(elements.begin(), mid);
        MergeSort(mid, elements.end());
    }

    merge(elements.begin(), mid, mid, elements.end(), range_begin);
}

int main() {
    LOG_DURATION("Total");
    mt19937 generator;

    vector<int> test_vector(400000);

    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    iota(test_vector.begin(), test_vector.end(), 1);

    // shuffle   -> https://ru.cppreference.com/w/cpp/algorithm/random_shuffle
    // Перемешивает элементы в случайном порядке
    shuffle(test_vector.begin(), test_vector.end(), generator);

    // Выводим вектор до сортировки
    //PrintRange(test_vector.begin(), test_vector.end());

    // Сортируем вектор с помощью сортировки слиянием
    MergeSort(test_vector.begin(), test_vector.end());

    // Выводим результат
    //PrintRange(test_vector.begin(), test_vector.end());

    // Проверяем, можно ли передать указатели
    MergeSort(test_vector.data(), test_vector.data() + test_vector.size());

    return 0;
}
