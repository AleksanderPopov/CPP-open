#pragma once

#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>

#include "array_ptr.h"

class ReserveProxyObj {

public:

    ReserveProxyObj(const size_t capacity_to_reserve)
        :capacity_to_reserve_(capacity_to_reserve)
    {
    }

    size_t GetSize() { return capacity_to_reserve_; }

private:
    size_t capacity_to_reserve_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}


template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;
    using Array = ArrayPtr<Type>;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size)
        : size_(size)
        , capacity_(size) 
    {
        Array array_tmp(size);
        std::fill(array_tmp.Get(), array_tmp.Get() + size_, Type{}); 
        array_.swap(array_tmp);
    }

    SimpleVector(size_t size, const Type& value)
        : array_(size)  
        , size_(size)
        , capacity_(size) 
    {
        std::fill(array_.Get(), array_.Get() + size_, value);  
    }


    SimpleVector(std::initializer_list<Type> init) {
        Array tmp_array(init.size());
        std::copy(init.begin(), init.end(), &tmp_array[0]);
        array_.swap(tmp_array);
        size_ = init.size();
        capacity_ = init.size();
    }

    SimpleVector(const SimpleVector& other) {
        SimpleVector<Type> tmp_vector(other.size_);
        std::copy(other.begin(), other.end(), tmp_vector.begin());
        tmp_vector.capacity_ = other.capacity_;
        this->swap(tmp_vector);
    }

    SimpleVector(SimpleVector&& other) {
        array_ = std::move(other.array_);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
    }

    SimpleVector(ReserveProxyObj obj) {
        SimpleVector<Type> vector_reserv;
        vector_reserv.Reserve(obj.GetSize());        
        swap(vector_reserv);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            SimpleVector<Type> new_vector(new_capacity);
            std::move(std::make_move_iterator(cbegin()),std::make_move_iterator( cend()), new_vector.begin());
            new_vector.size_ = size_;
            new_vector.capacity_ = new_capacity;
            swap(new_vector);
        }
    };
    
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (rhs == *this) {
            return *this;
        }
        else {
            SimpleVector<Type> tmp_vector(rhs.size_);
            std::copy(rhs.begin(), rhs.end(), tmp_vector.begin());
            tmp_vector.capacity_ = rhs.capacity_;
            this->swap(tmp_vector);
        }
        return *this;;
    }

    SimpleVector& operator=(SimpleVector&& rhs) {            
        if (this != &rhs) {            
            array_ = std::move(rhs.array_);
            size_ = std::exchange(rhs.size_, 0);
            capacity_ = std::exchange(rhs.capacity_, 0);
        }
        return *this;
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        assert(index <= size_);
        return array_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index <= size_);
        return array_[index];
    }

    Type& At(size_t index) {
        if (index >= size_)
        {
            throw std::out_of_range("At error : out_of_range");
        }
        else
        {
            return array_[index];
        }
    }

    const Type& At(size_t index) const {
        if (index >= size_)
        {
            throw std::out_of_range("At error : out_of_range");
        }
        else
        {
            return array_[index];
        }
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
        }
        else if (new_size <= capacity_) {           
            for (auto it = end(); it != &array_[new_size]; ++it)
                *(it) = std::move(Type{});
            size_ = new_size;
        }
        else {
            Array new_arr_ptr(new_size);
            std::move(std::make_move_iterator(begin()), std::make_move_iterator(end()), new_arr_ptr.Get());            
            for (auto it = &new_arr_ptr[size_]; it != &new_arr_ptr[new_size]; ++it) {
                *(it) = std::move(Type{});
            }                
            array_.swap(new_arr_ptr);
            size_ = new_size;
            capacity_ = new_size;
        }
    }
    //array_.Get(), array_.Get() + size_
    Iterator begin() noexcept {
        return array_.Get();
    }

    Iterator end() noexcept {
        return array_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return const_cast<ConstIterator>(array_.Get());
    }
    ConstIterator end() const noexcept {
        return const_cast<ConstIterator>(array_.Get() + size_);
    }

    ConstIterator cbegin() const noexcept {
        return const_cast<ConstIterator>(array_.Get());
    }
    ConstIterator cend() const noexcept {
        return const_cast<ConstIterator>(array_.Get() + size_);
    }

    void PushBack(const Type& item) {
        
        if (capacity_ == 0) {
            SimpleVector<Type> tmp_vector(1);
            tmp_vector[0] = item;
            this->swap(tmp_vector);
        }
        else if (capacity_ == size_) {
            SimpleVector<Type> new_vector(size_ * 2);
            new_vector.size_ = (size_ + 1);
            std::copy(begin(), end(), new_vector.begin());
            new_vector[size_] = item;
            swap(new_vector);
        }
        else {
            array_[size_] = item;
            size_++;
        }

    }

    void PushBack(Type&& item) {
               
        if (capacity_ == 0) {
            SimpleVector<Type> tmp_vector(1);
            tmp_vector[0] = std::move(item);
            *this = std::move(tmp_vector);
        }
        else if (size_ == capacity_) {
            SimpleVector<Type> new_vector(2 * size_);
            std::move(std::make_move_iterator(begin()), std::make_move_iterator(end()), new_vector.begin());
            new_vector.size_ = size_ + 1;
            new_vector[size_] = std::move(item);
            *this = std::move(new_vector);
        }
        else {
            array_[size_] = std::move(item);
            size_++;
        }
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        size_t pos_insert = std::distance(cbegin(), pos);
        if (capacity_ == 0) {
            Array tmp_array(1);
            tmp_array[0] = value;
            size_ = 1;
            capacity_ = 1;
            array_.swap(tmp_array);
            return &array_[pos_insert];
        }
        else if (capacity_ == size_) {
            Array tmp_array(size_ * 2);
            std::copy(cbegin(), pos, &tmp_array[0]);
            std::copy_backward(pos,cend(), &tmp_array[(size_ + 1)]);
            tmp_array[pos_insert] = value;
            capacity_ = (size_ * 2);
            size_++;
            array_.swap(tmp_array);
            return &array_[pos_insert];
        }
        else {
            std::copy_backward(pos,cend(), &array_[(size_ + 1)]);
            array_[pos_insert] = value;
            size_++;
            return &array_[pos_insert];
        }
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos>=begin() && pos <= end());
        auto* pos_tmp = const_cast<Iterator>(pos);
        size_t pos_insert = std::distance(begin(), pos_tmp);
        if (capacity_ == 0) {
            SimpleVector<Type> new_vector(1);
            new_vector[pos_insert] = std::move(value);
            *this = std::move(new_vector);
        }
        else if (size_ == capacity_) {
            SimpleVector<Type> new_vector(2 * size_);
            std::move(std::make_move_iterator(begin()), std::make_move_iterator(pos_tmp), new_vector.begin());
            std::move_backward(std::make_move_iterator(pos_tmp), std::make_move_iterator(end()), &new_vector[pos_insert + 1]);
            new_vector.size_ = size_ + 1;
            new_vector[pos_insert] = std::move(value);
            *this = std::move(new_vector);
        }
        else {
            
            for (int i = pos_insert; i < std::distance(pos_tmp, end()); ++i)
                array_[size_ - i - pos_insert] = std::move(array_[size_ - i - pos_insert - 1]);
            ++size_;
            array_[pos_insert] = std::move(value);
        }
        return &array_[pos_insert];

    }

    void PopBack() noexcept {
        if (!IsEmpty()) {
            --size_;
        }       
    }


    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() && pos <= end());
        auto* pos_tmp = const_cast<Iterator>(pos);
        std::move(pos_tmp + 1, end(), &array_[std::distance(begin(), pos_tmp)]);
        --size_;
        return &array_[std::distance(begin(), pos_tmp)];
    }

    void swap(SimpleVector& other) noexcept {
        this->array_.swap(other.array_);
        std::swap(this->size_, other.size_);
        std::swap(this->capacity_, other.capacity_);
    }

private:
    Array array_;
    size_t size_ = 0;
    size_t capacity_ = 0;

};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs > lhs);
}