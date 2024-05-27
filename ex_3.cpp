#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <future>
#include <unordered_map>
#include <algorithm>

void computeSum(std::promise<int> &&prms, const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        prms.set_exception(std::make_exception_ptr(std::runtime_error("Failed to open file")));
        return;
    }

    int sum = 0;
    char c;
    while (file.get(c)) {
        sum += static_cast<int>(c);
    }

    prms.set_value(sum);
}

int main(int argc, char *argv[]) {
    std::vector<std::thread> threads;
    std::vector<std::future<int>> futures;
    std::unordered_multimap<int, std::string> sums;

    for (int i = 1; i < argc; ++i) {
        std::promise<int> prms;
        futures.push_back(prms.get_future());
        threads.push_back(std::thread(computeSum, std::move(prms), argv[i]));
    }

    for (int i = 0; i < threads.size(); ++i) {
        threads[i].join();
        try {
            sums.insert({futures[i].get(), argv[i + 1]});
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << '\n';
        }
    }

    for (auto it = sums.begin(); it != sums.end(); ++it) {
        auto range = sums.equal_range(it->first);
        if (std::distance(range.first, range.second) > 1) {
            std::cout << "Files with the same sum " << it->first << ": ";
            for (auto jt = range.first; jt != range.second; ++jt) {
                std::cout << jt->second << ' ';
            }
            std::cout << '\n';
        }
    }

    return 0;
}