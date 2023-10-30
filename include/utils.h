
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <iomanip>

namespace utils {
    template<typename Container>
    std::string join(const Container &elements, const std::string &delimiter) {
        std::stringstream result;
        auto iter = elements.begin();
        if (iter != elements.end()) {
            result << *iter;
            ++iter;
        }
        for (; iter != elements.end(); ++iter) {
            result << delimiter << *iter;
        }
        return result.str();
    }


    template<typename InputIterator>
    std::string join(InputIterator first, InputIterator last, const std::string &delimiter = "") {
        std::stringstream result;
        if (first != last) {
            result << *first;
            ++first;
        }
        for (; first != last; ++first) {
            result << delimiter << *first;
        }
        return result.str();
    }

    template<typename T>
    std::set<T> unionSet(const std::set<T> &set1, const std::set<T> &set2) {
        std::set<T> resultSet;
        std::set_union(set1.begin(), set1.end(),
                       set2.begin(), set2.end(),
                       std::inserter(resultSet, resultSet.begin()));
        return resultSet;
    }

    template<typename Container, typename T>
    bool contains(const Container &container, const T &element) {
        return container.find(element) != container.end();
    }

    template<typename T>
    void insert(std::set<T> &set, T &element, bool &newElement) {
        auto result = set.insert(element);
        if (result.second) {
            newElement = true;
        }
    }

    template<typename K,typename V>
    bool findKey(std::map<K,V> map,V value, K & key) {
        for (const auto &pair:map) {
            if (pair.second==value) {
                key=pair.first;
                return true;
            }
        }
        return false;
    }
}

#endif //UTILS_H
