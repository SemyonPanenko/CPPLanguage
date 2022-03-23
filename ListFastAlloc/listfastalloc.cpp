#include <vector>

const size_t global_chunk_size = 32;
const size_t global_pool_size = (1 << 22);

template <size_t chunkSize>
class FixedAllocator
{
    static FixedAllocator *instance;

    struct Unit
    {
        char free_memory[chunkSize];
    };

    static const size_t pool_size = global_pool_size;

    std::vector<Unit *> all_pools;
    std::vector<void *> holes;

    size_t shift = 0;
    int tot_el = 0;

    void create_pool();
    void recreate();

public:
    FixedAllocator();

    static FixedAllocator *get_instance();

    void *allocate();
    void deallocate(void *);

    ~FixedAllocator();
};

template <typename T>
class FastAllocator
{
public:
    using value_type = T;
    using pointer = T *;
    using propagate_on_container_copy_assignment = std::false_type;

    static const size_t tp_size = sizeof(value_type);
    FixedAllocator<global_chunk_size> *const chunk_allock;

    FastAllocator();

    template <typename U>
    FastAllocator(const FastAllocator<U> &other);

    pointer allocate(size_t amount);
    void deallocate(T *ptr, size_t amount);

    void do_nothing() const
    {
        // testing system causes error on unused variable
        return;
    }

    FastAllocator &operator=(const FastAllocator &other)
    {
        // testing system causes error on unused variable
        other.do_nothing();
        return *this;
    }
};

template <typename T, typename Alloc = std::allocator<T>>
class List
{
    using AllocTraits = std::allocator_traits<Alloc>;

    struct BaseNode
    {
        BaseNode *next, *prev;
        BaseNode(BaseNode *next_node, BaseNode *prev_node) : next(next_node), prev(prev_node){};
        virtual ~BaseNode()
        {
            next = nullptr;
            prev = nullptr;
        };
    };

    struct Node : public BaseNode
    {
        T contained_data;
        Node(const T &to_put, BaseNode *next_node, BaseNode *prev_node) : BaseNode(next_node, prev_node), contained_data(to_put){};
        Node(const T &val) : BaseNode(nullptr, nullptr), contained_data(val){};
        Node(BaseNode *next_node, BaseNode *prev_node) : BaseNode(next_node, prev_node), contained_data(T()){};
    };

    using AllocType = typename AllocTraits::template rebind_alloc<Node>;
    using Traits = std::allocator_traits<AllocType>;

    size_t total_nodes;
    AllocType allocator;

    BaseNode *end_node;
    BaseNode *rend_node;

public:
    explicit List(const Alloc &alloc = Alloc());

    List(size_t count, const T &value, const Alloc &alloc = Alloc());
    List(size_t count, const Alloc &alloc = Alloc());
    List(const List &other);

    typename AllocTraits::template rebind_alloc<Node> get_allocator() const
    {
        return allocator;
    }

    void push_back(const T &value);
    void push_back();

    void push_front(const T &value);
    void push_front();

    void pop_back();
    void pop_front();

    size_t size() const
    {
        return total_nodes;
    }

    ~List();

    List &operator=(const List &other);

    template <bool IsConst>
    struct common_iterator
    {
        using difference_type = std::ptrdiff_t;
        using reference = std::conditional_t<IsConst, const T &, T &>;
        using pointer = std::conditional_t<IsConst, const T *, T *>;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::conditional_t<IsConst, const T, T>;

        BaseNode *ptr;

        common_iterator(BaseNode *init_value = nullptr) : ptr(init_value) {}

        common_iterator(const common_iterator<IsConst> &other_iter) : ptr(other_iter.ptr)
        {
        }

        const reference operator*() const
        {

            return (dynamic_cast<Node *>(ptr))->contained_data;
        }

        reference operator*()
        {
            return (dynamic_cast<Node *>(ptr))->contained_data;
        }

        reference operator->()
        {
            return &ptr->contained_data;
        }

        common_iterator &operator--()
        {
            ptr = ptr->prev;
            return *this;
        }

        common_iterator operator--(int)
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        common_iterator &operator++()
        {
            ptr = ptr->next;
            return *this;
        }

        common_iterator operator++(int)
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        bool operator==(const common_iterator &other)
        {
            return (ptr == other.ptr);
        }

        bool operator!=(const common_iterator &other)
        {
            return !(*this == other);
        }

        operator common_iterator<true>() const
        {
            return common_iterator<true>(ptr);
        }
    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin()
    {
        return iterator(rend_node->next);
    }

    const_iterator begin() const
    {
        return cbegin();
    }

    const_iterator end() const
    {
        return cend();
    }

    iterator end()
    {
        return iterator(end_node);
    }

    const_iterator cbegin() const
    {
        return const_iterator(rend_node->next);
    }

    const_iterator cend() const
    {
        return const_iterator(end_node);
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end_node);
    }

    const_reverse_iterator rbegin() const
    {
        return crbegin();
    }

    const_reverse_iterator rend() const
    {
        return crend();
    }

    reverse_iterator rend()
    {
        return reverse_iterator(rend_node->next);
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(end_node);
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(rend_node->next);
    }

    void erase(const_iterator iter);
    void insert(const_iterator iter, const T &value);
};

template <typename T, typename Alloc>
List<T, Alloc>::~List()
{
    while (total_nodes > 0)
    {
        pop_back();
    }
    Traits::destroy(allocator, end_node);
    Traits::destroy(allocator, rend_node);

    Traits::deallocate(allocator, reinterpret_cast<Node *>(end_node), 1);
    Traits::deallocate(allocator, reinterpret_cast<Node *>(rend_node), 1);
}

template <typename T, typename Alloc>
List<T, Alloc>::List(const Alloc &alloc) : total_nodes(0),
                                           allocator(alloc), end_node(nullptr), rend_node(nullptr)
{

    end_node = Traits::allocate(allocator, 1);
    rend_node = Traits::allocate(allocator, 1);

    Traits::construct(allocator, end_node, rend_node, rend_node);
    Traits::construct(allocator, rend_node, end_node, end_node);
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t count, const T &value, const Alloc &alloc) : List(alloc)
{

    if (count == 0)
    {
        return;
    }
    while (count--)
    {
        push_back(value);
    }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t count, const Alloc &alloc) : List(alloc)
{
    if (count == 0)
    {
        return;
    }
    while (count--)
    {
        push_back();
    }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(const List &other) : List(std::allocator_traits<Alloc>::select_on_container_copy_construction(other.get_allocator()))
{
    total_nodes = other.size();

    if (total_nodes == 0)
    {
        return;
    }

    auto iter = other.begin();

    Node *first_elem = Traits::allocate(allocator, 1);
    Traits::construct(allocator, first_elem, *iter, nullptr, nullptr);
    ++iter;

    first_elem->prev = rend_node;
    rend_node->next = first_elem;

    Node *last_node = first_elem;

    for (int i = 0; i < ((int)total_nodes - 1); i++)
    {
        Node *new_node = Traits::allocate(allocator, 1);
        Traits::construct(allocator, new_node, *iter);
        last_node->next = new_node;
        new_node->prev = last_node;
        last_node = new_node;
        ++iter;
    }

    last_node->next = end_node;
    end_node->prev = last_node;
}

template <typename T, typename Alloc>
List<T, Alloc> &List<T, Alloc>::operator=(const List &other)
{
    while (total_nodes > 0)
    {
        pop_back();
    }

    if (Traits::propagate_on_container_copy_assignment::value)
    {
        allocator = other.get_allocator();
    }

    if (other.total_nodes == 0)
    {
        return *this;
    }

    auto iter = other.begin();
    int tnodes = other.size();
    Node *first_elem = Traits::allocate(allocator, 1);
    Traits::construct(allocator, first_elem, *iter);
    ++iter;

    first_elem->prev = rend_node;
    rend_node->next = first_elem;

    Node *last_node = first_elem;

    for (int i = 0; i < ((int)tnodes - 1); i++)
    {
        Node *new_node = Traits::allocate(allocator, 1);
        Traits::construct(allocator, new_node, *iter);
        last_node->next = new_node;
        new_node->prev = last_node;
        last_node = new_node;
        ++iter;
    }

    last_node->next = end_node;
    end_node->prev = last_node;

    total_nodes = tnodes;
    return *this;
}

template <typename T, typename Alloc>
void List<T, Alloc>::erase(const_iterator iter)
{
    if ((total_nodes == 0) || (iter.ptr == end_node) || (iter.ptr == rend_node))
    {
        return;
    }

    iter.ptr->prev->next = iter.ptr->next;
    iter.ptr->next->prev = iter.ptr->prev;

    Traits::destroy(allocator, iter.ptr);
    Traits::deallocate(allocator, dynamic_cast<Node *>(iter.ptr), 1);
    iter.ptr = nullptr;

    --total_nodes;
}

template <typename T, typename Alloc>
void List<T, Alloc>::insert(const_iterator iter, const T &value)
{
    if ((iter.ptr == rend_node))
    {
        return;
    }

    Node *new_node = Traits::allocate(allocator, 1);
    Traits::construct(allocator, new_node, value);

    new_node->next = iter.ptr;
    new_node->prev = iter.ptr->prev;

    iter.ptr->prev->next = new_node;
    iter.ptr->prev = new_node;

    ++total_nodes;
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_front()
{
    if (total_nodes == 0)
    {
        return;
    }

    Node *old_elem = dynamic_cast<Node *>(rend_node->next);

    rend_node->next->next->prev = rend_node;
    rend_node->next = rend_node->next->next;

    Traits::destroy(allocator, old_elem);
    Traits::deallocate(allocator, old_elem, 1);

    --total_nodes;
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_back()
{
    if (total_nodes == 0)
    {
        return;
    }

    Node *old_elem = dynamic_cast<Node *>(end_node->prev);

    end_node->prev->prev->next = end_node;
    end_node->prev = end_node->prev->prev;

    Traits::destroy(allocator, old_elem);
    Traits::deallocate(allocator, old_elem, 1);

    --total_nodes;
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_back(const T &value)
{
    Node *new_elem = Traits::allocate(allocator, 1);
    Traits::construct(allocator, new_elem, value);

    new_elem->next = end_node;
    new_elem->prev = end_node->prev;

    end_node->prev->next = new_elem;
    end_node->prev = new_elem;

    ++total_nodes;
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_back()
{
    Node *new_elem = Traits::allocate(allocator, 1);
    Traits::construct(allocator, new_elem, end_node, end_node->prev);

    end_node->prev->next = new_elem;
    end_node->prev = new_elem;

    ++total_nodes;
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_front()
{
    Node *new_elem = Traits::allocate(allocator, 1);
    Traits::construct(allocator, new_elem, rend_node->next, rend_node);

    rend_node->next->prev = new_elem;
    rend_node->next = new_elem;

    ++total_nodes;
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_front(const T &value)
{
    Node *new_elem = Traits::allocate(allocator, 1);
    Traits::construct(allocator, new_elem, value);

    new_elem->next = rend_node->next;
    new_elem->prev = rend_node;

    rend_node->next->prev = new_elem;
    rend_node->next = new_elem;

    ++total_nodes;
}

template <size_t chunkSize>
FixedAllocator<chunkSize> *FixedAllocator<chunkSize>::instance = FixedAllocator<chunkSize>::get_instance();

template <size_t chunkSize>
void FixedAllocator<chunkSize>::create_pool()
{
    Unit *new_pool = new Unit[pool_size];
    all_pools.push_back(new_pool);
}

template <size_t chunkSize>
FixedAllocator<chunkSize> *FixedAllocator<chunkSize>::get_instance()
{
    if (!instance)
    {
        instance = new FixedAllocator();
    }
    return instance;
}

template <size_t chunkSize>
FixedAllocator<chunkSize>::FixedAllocator()
{
    shift = pool_size;
}

template <size_t chunkSize>
FixedAllocator<chunkSize>::~FixedAllocator()

{
    for (size_t i = 0; i < all_pools.size(); i++)
    {
        delete[] all_pools[i];
    }
}

template <size_t chunkSize>
void *FixedAllocator<chunkSize>::allocate()
{
    ++tot_el;
    if (!holes.empty())
    {
        void *to_r = holes.back();
        holes.pop_back();
        if (to_r == nullptr)
        {
            return allocate();
        }
        return to_r;
    }
    if (shift == pool_size)
    {
        create_pool();
        shift = 0;
    }

    Unit *tmp = &all_pools.back()[shift++];
    return static_cast<void *>(tmp);
}

template <size_t chunkSize>
void FixedAllocator<chunkSize>::deallocate(void *ptr)
{
    --tot_el;
    holes.push_back(ptr);
    if (tot_el == 0)
    {
        recreate();
    }
}

template <size_t chunkSize>
void FixedAllocator<chunkSize>::recreate()

{
    for (size_t i = 0; i < all_pools.size(); i++)
    {
        delete[] all_pools[i];
    }
    std::vector<void *>().swap(holes);
    std::vector<Unit *>().swap(all_pools);
    shift = pool_size;
}

template <typename T>
typename FastAllocator<T>::pointer FastAllocator<T>::allocate(size_t amount)
{
    pointer to_r = nullptr;
    if (amount * tp_size <= global_chunk_size)
    {
        to_r = static_cast<pointer>(chunk_allock->allocate());
    }
    else
    {
        to_r = static_cast<pointer>(::operator new(amount *tp_size));
    }
    return to_r;
}

template <typename T>
void FastAllocator<T>::deallocate(T *ptr, size_t amount)
{
    if (tp_size * amount <= global_chunk_size)
    {
        chunk_allock->deallocate(ptr);
        return;
    }
    else
    {
        ::operator delete(ptr);
        return;
    }
}

template <typename T>
template <typename U>
FastAllocator<T>::FastAllocator(const FastAllocator<U> &other) : chunk_allock(other.chunk_alloc)
{
}

template <typename T>
FastAllocator<T>::FastAllocator() : chunk_allock(FixedAllocator<global_chunk_size>::get_instance())
{
}

template <typename T>
bool operator==(const FastAllocator<T> &left, const FastAllocator<T> &right)
{
    return true;
}

template <typename T>
bool operator!=(const FastAllocator<T> &left, const FastAllocator<T> &right)
{
    return false;
}
