#ifndef FIXED_VECTOR_HPP
#define FIXED_VECTOR_HPP


#include <iostream>
#include <vector>

template<typename T>
class FixedSizeVector {
private:
    std::vector<T> vec;
    size_t max_size;
    bool size_set = false; // Indique si la taille a été définie
public:
    FixedSizeVector() : max_size(0), size_set(false) {} // Constructeur par défaut sans taille
    FixedSizeVector(size_t max) : max_size(max) {}

    void set_size(size_t max) {
        if (!size_set) {
            max_size = max;
            size_set = true;

            for (int i = 0; i < max_size; i++) { vec.push_back(0); }
        }
        else {
            throw std::logic_error("Taille déjà définie !");
        }
    }

    void set_at(size_t index, const T& value) {
        if (index >= max_size) {
            throw std::out_of_range("Index hors limites ! Index: " + index);
        }
        if (index >= vec.size()) {
            vec.resize(index + 1); // Remplit avec des valeurs par défaut
        }
        vec[index] = value;
    }

    void push_back(const T& value) {
        if (vec.size() < max_size) {
            vec.push_back(value);
        }
        else {
            throw std::length_error("Taille maximale du vector atteinte !");
        }
    }

    size_t size() const { return vec.size(); }
    const T& operator[](size_t index) const { return vec[index]; }
};

#endif