#pragma once

#include <cassert>
#include <cstdlib>
#include <utility>
//OK
template <typename Type>
class ArrayPtr {
public:

    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        raw_ptr_ = (size) ? new Type[size] : nullptr;
    }

    explicit ArrayPtr(Type* raw_ptr) 
        noexcept :raw_ptr_(raw_ptr)
    {
    }

    ArrayPtr(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other) {
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);      
    }

    ArrayPtr& operator=(ArrayPtr&& rhs) {
        if (this == &rhs) {
            return *this;
        }    
        raw_ptr_ = std::exchange(rhs.raw_ptr_, nullptr);
        return *this;
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    ArrayPtr& operator=(const ArrayPtr&) = delete;  

    ArrayPtr& operator=(ArrayPtr& other) {
        raw_ptr_ = other.raw_ptr_;
        return *this;
    }

    [[nodiscard]] Type* Release() noexcept {
        Type* raw_ptr_tmp = raw_ptr_;
        raw_ptr_ = nullptr;
        return raw_ptr_tmp;
    }

    Type& operator[](size_t index) noexcept {
        Type* addr = raw_ptr_ + index;
        return *addr;
    }

    const Type& operator[](size_t index) const noexcept {
        const Type* addr = raw_ptr_ + index;
        return *addr;
    }

    explicit operator bool() const {
        return (raw_ptr_ != nullptr);
    }

    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        std::swap(this->raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};