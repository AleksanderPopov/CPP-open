#include <iostream>

#include "json_reader.h"

int main() {
    std::ostringstream output;
    JsonReader processing(output);
    processing.Reader();
    std::cout << processing.Result().str();
}