#include <algorithm>

enum class Colour {BLACK, RED};

template <class ValueType>
struct Node;

template <class ValueType>
class Set;

template <class ValueType>
class SetIterator;

template <class ValueType>
struct Node {
    Node* left;
    Node* right;
    Node* parent;
    Colour colour;
    ValueType value;

    Node()
        : left(nullptr)
        , right(nullptr)
        , parent(nullptr)
        , colour(Colour::BLACK)
        , value(ValueType())
    {}

    Node(const ValueType& value)
        : left(nullptr)
        , right(nullptr)
        , parent(nullptr)
        , colour(Colour::BLACK)
        , value(value)
    {}

    Node(const Node& other)
        : left(other.left)
        , right(other.right)
        , parent(other.parent)
        , colour(other.colour)
        , value(other.value)
    {}

    Node* next(Node* nil_) {
        Node* tmp = this;
        if (tmp->right != nil_) {
            tmp = tmp->right;
            while (tmp->left != nil_) {
                tmp = tmp->left;
            }
            return tmp;
        }
        Node* par = parent;
        while (par != nil_ && par->right == tmp) {
            tmp = par;
            par = par->parent;
        }
        return par;
    }

    Node* prev(Node* nil_) {
        Node* tmp = this;
        if (tmp->left != nil_) {
            tmp = tmp->left;
            while (tmp->right != nil_) {
                tmp = tmp->right;
            }
            return tmp;
        }
        Node* par = parent;
        while (par != nil_ && par->left == tmp) {
            tmp = par;
            par = par->parent;
        }
        return par;
    }
};

template <class ValueType>
class SetIterator: public std::iterator<std::bidirectional_iterator_tag, ValueType> {
 private:
    friend class Set<ValueType>;
    Node<ValueType>* node;
    const Set<ValueType>* set;

 public:
    SetIterator() = default;

    SetIterator(const SetIterator& other)
        : node(other.node)
        , set(other.set)
    {}

    SetIterator(Node<ValueType>* node, const Set<ValueType>* set)
        : node(node)
        , set(set)
    {}

    SetIterator(SetIterator&& other)
        : node(std::move(other.node))
        , set(std::move(other.set))
    {}

    SetIterator& operator = (const SetIterator& other) {
        SetIterator tmp(other);
        std::swap(tmp.node, (*this).node);
        std::swap(tmp.set, (*this).set);
        return (*this);
    }

    SetIterator& operator++ () {
        node = (*node).next(set->nil_);
        return (*this);
    }

    SetIterator operator++ (int) {
        SetIterator tmp(*this);
        ++(*this);
        return tmp;
    }

    SetIterator& operator-- () {
        if (node == set->nil_) {
            node = set->maximum_;
            return (*this);
        }
        node = (*node).prev(set->nil_);
        return (*this);
    }

    SetIterator operator-- (int) {
        SetIterator tmp(*this);
        --(*this);
        return tmp;
    }

    bool operator== (const SetIterator& other) const {
        return node == other.node;
    }

    bool operator!= (const SetIterator& other) const {
        return !((*this) == other);
    }

    const ValueType& operator* () const {
        return node->value;
    }

    const ValueType* operator-> () const {
        return &node->value;
    }
};

template <class ValueType>
class Set {
 public:
    friend class SetIterator<ValueType>;
    using iterator = SetIterator<ValueType>;

 private:
    Node<ValueType>* root_;
    Node<ValueType>* nil_;
    Node<ValueType>* maximum_;
    Node<ValueType>* minimum_;
    size_t size_;

    void rotate_left(Node<ValueType>* x) {
        Node<ValueType>* y = x->right;
        x->right = y->left;
        if (y->left != nil_) {
            y->left->parent = x;
        }
        if (y != nil_) {
            y->parent = x->parent;
        }
        if (x->parent != nil_) {
            if (x == x->parent->left) {
                x->parent->left = y;
            } else {
                x->parent->right = y;
            }
        } else {
            root_ = y;
        }
        y->left = x;
        if (x != nil_) {
            x->parent = y;
        }
    }

    void rotate_right(Node<ValueType>* x) {
        Node<ValueType>* y = x->left;
        x->left = y->right;
        if (y->right != nil_) {
            y->right->parent = x;
        }
        if (y != nil_) {
            y->parent = x->parent;
        }
        if (x->parent != nil_) {
            if (x == x->parent->left) {
                x->parent->left = y;
            } else {
                x->parent->right = y;
            }
        } else {
            root_ = y;
        }
        y->right = x;
        if (x != nil_) {
            x->parent = y;
        }
    }

    void rebalance_insertion(Node<ValueType>* x) {
        while (x != root_ && x->parent->colour == Colour::RED) {
            if (x->parent == x->parent->parent->left) {
                Node<ValueType>* y = x->parent->parent->right;
                if (y->colour == Colour::RED) {
                    x->parent->colour = Colour::BLACK;
                    y->colour = Colour::BLACK;
                    x->parent->parent->colour = Colour::RED;
                    x = x->parent->parent;
                } else {
                    if (x == x->parent->right) {
                        x = x->parent;
                        rotate_left(x);
                    }
                    x->parent->colour = Colour::BLACK;
                    x->parent->parent->colour = Colour::RED;
                    rotate_right(x->parent->parent);
                }
            } else {
                Node<ValueType>* y = x->parent->parent->left;
                if (y->colour == Colour::RED) {
                    x->parent->colour = Colour::BLACK;
                    y->colour = Colour::BLACK;
                    x->parent->parent->colour = Colour::RED;
                    x = x->parent->parent;
                } else {
                    if (x == x->parent->left) {
                        x = x->parent;
                        rotate_right(x);
                    }
                    x->parent->colour = Colour::BLACK;
                    x->parent->parent->colour = Colour::RED;
                    rotate_left(x->parent->parent);
                }
            }
        }
        root_->colour = Colour::BLACK;
    }

    Node<ValueType>* get_min(Node<ValueType>* z) {
        while (z->left != nil_) {
            z = z->left;
        }
        return z;
    }

    Node<ValueType>* get_max(Node<ValueType>* z) {
        while (z->right != nil_) {
            z = z->right;
        }
        return z;
    }

    void transplant(Node<ValueType>* u, Node<ValueType>* v) {
        if (u->parent == nil_) {
            root_ = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    void rebalance_erasing(Node<ValueType>* x) {
        while (x != root_ && x->colour == Colour::BLACK) {
            if (x == x->parent->left) {
                Node<ValueType>* w = x->parent->right;
                if (w->colour == Colour::RED) {
                    w->colour = Colour::BLACK;
                    x->parent->colour = Colour::RED;
                    rotate_left(x->parent);
                    w = x->parent->right;
                }
                if (w->left->colour == Colour::BLACK && w->right->colour == Colour::BLACK) {
                    w->colour = Colour::RED;
                    x = x->parent;
                } else {
                    if (w->right->colour == Colour::BLACK) {
                        w->left->colour = Colour::BLACK;
                        w->colour = Colour::RED;
                        rotate_right(w);
                        w = x->parent->right;
                    }
                    w->colour = x->parent->colour;
                    x->parent->colour = Colour::BLACK;
                    w->right->colour = Colour::BLACK;
                    rotate_left(x->parent);
                    x = root_;
                }
            } else {
                Node<ValueType>* w = x->parent->left;
                if (w->colour == Colour::RED) {
                    w->colour = Colour::BLACK;
                    x->parent->colour = Colour::RED;
                    rotate_right(x->parent);
                    w = x->parent->left;
                }
                if (w->right->colour == Colour::BLACK && w->left->colour == Colour::BLACK) {
                    w->colour = Colour::RED;
                    x = x->parent;
                } else {
                    if (w->left->colour == Colour::BLACK) {
                        w->right->colour = Colour::BLACK;
                        w->colour = Colour::RED;
                        rotate_left(w);
                        w = x->parent->left;
                    }
                    w->colour = x->parent->colour;
                    x->parent->colour = Colour::BLACK;
                    w->left->colour = Colour::BLACK;
                    rotate_right(x->parent);
                    x = root_;
                }
            }
        }
        x->colour = Colour::BLACK;
    }

    void erase(Node<ValueType>* z) {
        Node<ValueType>* y = z;
        Node<ValueType>* x;
        if (maximum_ == z) {
            maximum_ = maximum_->prev(nil_);
        }
        if (minimum_ == z) {
            minimum_ = minimum_->next(nil_);
        }
        Colour original_colour = y->colour;
        if (z->left == nil_) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == nil_) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = get_min(z->right);
            original_colour = y->colour;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->colour = z->colour;
        }
        if (original_colour == Colour::BLACK) {
           rebalance_erasing(x);
        }
        delete z;
        --size_;
    }

    void swap(Set<ValueType>& other) {
        std::swap(nil_, other.nil_);
        std::swap(root_, other.root_);
        std::swap(size_, other.size_);
        std::swap(minimum_, other.minimum_);
        std::swap(maximum_, other.maximum_);
    }

 public:
    Set() {
        nil_ = new Node<ValueType>();
        nil_->left = nil_;
        nil_->right = nil_;
        nil_->parent = nil_;
        root_ = nil_;
        maximum_ = nil_;
        minimum_ = nil_;
        size_ = 0;
    }

    ~Set() {
        while (!empty()) {
            erase(minimum_);
        }
        nil_->parent = nullptr;
        nil_->left = nullptr;
        nil_->right = nullptr;
        delete nil_;
    }

    void insert(ValueType value) {
        if (find(value) != end()) {
            return;
        }
        Node<ValueType>* z = new Node<ValueType>(value);
        z->parent = nil_;
        z->left = nil_;
        z->right = nil_;
        Node<ValueType>* y = nil_;
        Node<ValueType>* x = root_;
        while (x != nil_) {
            y = x;
            if (value < x->value) {
                x = x->left;
            } else {
                x = x->right;
            }
        }
        z->parent = y;
        if (y == nil_) {
            root_ = z;
        } else if (value < y->value) {
            y->left = z;
        } else {
            y->right = z;
        }
        z->left = nil_;
        z->right = nil_;
        z->colour = Colour::RED;
        rebalance_insertion(z);
        if (maximum_ == nil_ || maximum_->value < value) {
            maximum_ = z;
        }
        if (minimum_ == nil_ || value < minimum_->value) {
            minimum_ = z;
        }
        ++size_;
    }

    template <typename Iterator>
    Set(Iterator first, Iterator last)
        : Set() {
        while (first != last) {
            insert(*first);
            ++first;
        }
    }

    Set(std::initializer_list<ValueType> input)
        : Set(input.begin(), input.end())
    {}

    Set(const Set& other)
        : Set(other.begin(), other.end())
    {}

    Set<ValueType>& operator = (const Set<ValueType>& other) {
        Set<ValueType> tmp(other);
        tmp.swap(*this);
        return *this;
    }

    bool empty() const {
        return size_ == 0;
    }

    size_t size() const {
        return size_;
    }

    iterator begin() const {
        return iterator(minimum_, this);
    }

    iterator end() const {
        return iterator(nil_, this);
    }

    iterator find(ValueType value) const {
        auto it = lower_bound(value);
        if (it != end()) {
            ValueType value_lower_bound = *it;
            if (value < value_lower_bound) {
                return end();
            }
            return it;
        }
        return end();
    }

    void erase(ValueType value) {
        if (find(value) == end()) {
            return;
        }
        Node<ValueType>* current = root_;
        while (current != nil_) {
            if (current->value < value) {
                current = current->right;
            } else if (value < current->value) {
                current = current->left;
            } else {
                erase(current);
                break;
            }
        }
    }

    iterator lower_bound(ValueType value) const {
        Node<ValueType>* ans = nil_;
        Node<ValueType>* current = root_;
        while (current != nil_) {
            if (current->value < value) {
                current = current->right;
            } else if (value < current->value) {
                if (ans == nil_ || current->value < ans->value) {
                    ans = current;
                }
                current = current->left;
            } else {
                ans = current;
                break;
            }
        }
        return iterator(ans, this);
    }
};
