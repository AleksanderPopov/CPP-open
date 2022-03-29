#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

template <typename Type>
class SingleLinkedList {

    struct Node {

        Node() = default;

        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }

        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) : node_(node)
        {
        }

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !operator==(rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !operator==(rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
			node_ = node_->next_node;
            return *this;
        }
        
        BasicIterator operator++(int) noexcept {           
            auto old_value(*this);
            ++(*this);
            return old_value;
        }
        
        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }
        
        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            Type* a = &node_->value;
            return a;
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList()
        : size_(0)
    {
    }

    template <typename InputIterator>
    void Assign(InputIterator from, InputIterator to) {
       
        std::vector<Type> temp_other;

        for (auto it_tmp = from; it_tmp != to; it_tmp++) {
            temp_other.push_back(*it_tmp);
        }

        for (auto it = temp_other.crbegin(); it != temp_other.crend(); it++) {
            this->PushFront(*it);
        }
    }

    SingleLinkedList(std::initializer_list<Type> values) {
        Assign(values.begin(), values.end());
    }

    SingleLinkedList(const SingleLinkedList& other) {

        assert(size_ == 0 && head_.next_node == nullptr);
        Assign(other.begin(), other.end());
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{ head_.next_node };
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{ head_.next_node };
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.node_);
        Node* insert = new Node(value, pos.node_->next_node);
        pos.node_->next_node = insert;
        ++size_;

        return Iterator{ insert };
    }
    
    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(!IsEmpty());
        
        auto tmp_it = pos.node_->next_node->next_node;
        delete pos.node_->next_node;
        pos.node_->next_node = tmp_it;
        --size_;

        return Iterator{ tmp_it };
    }

    void PopFront() noexcept {
        auto tmp_head = this->begin().node_->next_node;
        delete head_.next_node;
        this->head_.next_node = tmp_head;
        --size_;
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            Node* next = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = next;
        }
        size_ = 0;
    }


    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;    
    }

    void swap(SingleLinkedList& other) noexcept {

        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {

        if (this->cbegin() != rhs.cbegin()) {
            SingleLinkedList tmp_rhs(rhs);
            this->swap(tmp_rhs);

            return *this;
        }
        return *this;
    }

    ~SingleLinkedList() {
        Clear();
    }
private:

    Node head_;
    size_t size_ = 0;
};


template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {

    SingleLinkedList<Type> tmp;
    tmp.swap(lhs);
    lhs.swap(rhs);
    rhs.swap(tmp);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {

    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {

    return !std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {

    return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {

    return !std::lexicographical_compare(rhs.cbegin(), rhs.cend(), lhs.cbegin(), lhs.cend());
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {

    return std::lexicographical_compare(rhs.cbegin(), rhs.cend(), lhs.cbegin(), lhs.cend());
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {

    return !std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}