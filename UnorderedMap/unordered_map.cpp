#include <vector>
#include <iostream>
#include <list>

template<typename NodeType, typename Alloc>
using List = std::list<NodeType, Alloc>;

template <typename Key, typename Value, typename Hash = std::hash<Key>,
          typename Equal = std::equal_to<Key>, typename Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap
{
public:
    static constexpr size_t init_cap = 10;
    static constexpr double init_load_factor = 0.75d;

    using NodeType = std::pair<Key, Value>;

    using Iterator = typename List<NodeType, Alloc>::iterator;
    using ConstIterator = typename List<NodeType, Alloc>::const_iterator;
    using ReverseIterator = typename List<NodeType, Alloc>::reverse_iterator;
    using ConstReverseIterator = typename List<NodeType, Alloc>::const_reverse_iterator;

    Iterator begin();
    ConstIterator begin() const;

    Iterator end();
    ConstIterator end() const;

    ConstIterator cbegin() const;
    ConstIterator cend() const;

    ReverseIterator rbegin();
    ConstReverseIterator rbegin() const;

    ReverseIterator rend();
    ConstReverseIterator rend() const;

    ConstReverseIterator crbegin() const;
    ConstReverseIterator crend() const;

    List<NodeType, Alloc> data_holder;
    std::vector<Iterator> data;

    size_t capacity, curr_size;
    double max_lf;

    template <typename U>
    size_t get_hash(U &&key) const;

    UnorderedMap();

    UnorderedMap(const UnorderedMap &other);
    UnorderedMap(UnorderedMap &&other);

    size_t size() const;

    double &max_load_factor();

    void max_load_factor(double new_load_factor);

    double load_factor() const;

    void reserve(size_t needed_cap);

    size_t max_size() const;

    template <typename U>
    ConstIterator find_ins_pos(U &&key) const;

    template <typename U>
    Iterator find_ins_pos(U &&key);

    std::pair<Iterator, bool> insert(NodeType &&ins_pair);
    std::pair<Iterator, bool> insert(const NodeType &ins_pair);

    void erase(ConstIterator to_erase);
    void erase(ConstIterator first, ConstIterator second);

    template <typename InputIt>
    void insert(InputIt first, InputIt second);

    void check_load();
    void rehash();

    UnorderedMap &operator=(const UnorderedMap &other);
    UnorderedMap &operator=(UnorderedMap &&other);

    void swap(UnorderedMap &other);

    template <typename U>
    Iterator find(U &&key);

    template <typename U>
    ConstIterator find(U &&key) const;

    Value &operator[](Key &&key);
    Value &operator[](const Key &key);

    Value &at(Key &&key);
    Value &at(const Key &key);

    const Value &at(Key &&key) const;
    const Value &at(const Key &key) const;

    template <typename... Args>
    std::pair<Iterator, bool> emplace(Args &&...args);
};

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename U>
size_t UnorderedMap<Key, Value, Hash, Equal, Alloc>::get_hash(U &&key) const
{
    return (Hash{}(std::forward<U>(key)) % capacity);
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
size_t UnorderedMap<Key, Value, Hash, Equal, Alloc>::max_size() const
{
    return 357913941;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
double &UnorderedMap<Key, Value, Hash, Equal, Alloc>::max_load_factor()
{
    return max_lf;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::max_load_factor(double new_load_factor)
{
    max_lf = new_load_factor;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::reserve(size_t needed_capacity)
{
    while (capacity * max_lf < needed_capacity)
    {
        rehash();
    }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
double UnorderedMap<Key, Value, Hash, Equal, Alloc>::load_factor() const
{
    return (static_cast<double>(curr_size) / static_cast<double>(capacity));
}
template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
size_t UnorderedMap<Key, Value, Hash, Equal, Alloc>::size() const
{
    return curr_size;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::begin()
{
    return data_holder.begin();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ConstIterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::begin() const
{
    return data_holder.begin();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::end()
{
    return data_holder.end();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ConstIterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::end() const
{
    return data_holder.end();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ConstIterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::cbegin() const
{
    return data_holder.cbegin();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ConstIterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::cend() const
{
    return data_holder.cend();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ReverseIterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::rbegin()
{
    return data_holder.rbegin();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ConstReverseIterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::rbegin() const
{
    return data_holder.rbegin();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ReverseIterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::rend()
{
    return data_holder.rend();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ConstReverseIterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::rend() const
{
    return data_holder.rend();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ConstReverseIterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::crbegin() const
{
    return data_holder.crbegin();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ConstReverseIterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::crend() const
{
    return data_holder.crend();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename... Args>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator, bool> UnorderedMap<Key, Value, Hash, Equal, Alloc>::emplace(Args &&...args)
{

    using Tmp = typename std::allocator_traits<Alloc>::template rebind_alloc<NodeType>;
    using Tr = typename std::allocator_traits<Tmp>;

    Tmp my_alloc(data_holder.get_allocator());

    auto ptr = Tr::allocate(my_alloc, 1);
    Tr::construct(my_alloc, ptr, std::forward<Args>(args)...);

    auto result = insert(std::move(*ptr));

    if (result.second)
    {
        return result;
    }

    Tr::destroy(my_alloc, ptr);
    Tr::deallocate(my_alloc, ptr, 1);

    return result;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
const Value &UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(Key &&key) const
{

    auto elem_pos = find(std::move(key));

    if (elem_pos != end())
    {
        return (*elem_pos).second;
    }

    throw std::out_of_range("No such key exists!");
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
const Value &UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(const Key &key) const
{

    auto elem_pos = find(key);

    if (elem_pos != end())
    {
        return (*elem_pos).second;
    }

    throw std::out_of_range("No such key exists!");
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
Value &UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(Key &&key)
{

    auto elem_pos = find(std::move(key));

    if (elem_pos != end())
    {
        return (*elem_pos).second;
    }

    throw std::out_of_range("No such key exists!");
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
Value &UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(const Key &key)
{

    auto elem_pos = find(key);

    if (elem_pos != end())
    {
        return (*elem_pos).second;
    }

    throw std::out_of_range("No such key exists!");
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
Value &UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator[](Key &&key)
{

    auto elem_pos = find(std::move(key));

    if (elem_pos != end())
    {
        return (*elem_pos).second;
    }

    insert(std::make_pair(key, Value()));
    return (*this)[std::move(key)];
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
Value &UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator[](const Key &key)
{

    auto elem_pos = find(key);

    if (elem_pos != end())
    {
        return (*elem_pos).second;
    }

    insert(std::make_pair(key, Value()));
    return (*this)[key];
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::swap(UnorderedMap &copy)
{
    std::swap(data_holder, copy.data_holder);
    std::swap(data, copy.data);
    std::swap(curr_size, copy.curr_size);
    std::swap(capacity, copy.capacity);
    std::swap(max_lf, copy.max_lf);
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc> &UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(const UnorderedMap &other)
{

    if (this == &other)
    {
        return *this;
    }

    UnorderedMap copy(other);

    swap(copy);

    return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc> &UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(UnorderedMap &&other)
{

    if (this == &other)
    {
        return *this;
    }

    UnorderedMap copy(std::move(other));

    swap(copy);

    return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename InputIt>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(InputIt first, InputIt second)
{

    while (first != second)
    {
        insert(*first);
        ++first;
    }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(ConstIterator to_erase)
{

    size_t erased_hash = get_hash(to_erase->first);

    if (Equal{}(data[erased_hash]->first, to_erase->first))
    {
        auto curr_iter = data[erased_hash];
        ++curr_iter;
        if (curr_iter != end() && get_hash(curr_iter->first) == erased_hash)
        {
            ++data[erased_hash];
        }
        else
        {
            data[erased_hash] = end();
        }
    }

    data_holder.erase(to_erase);
    --curr_size;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(ConstIterator first, ConstIterator second)
{

    ConstIterator copy(first);
    while (first != second)
    {
        ++first;
        erase(copy);
        copy = first;
    }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap() : data_holder(), data(init_cap, data_holder.end()), capacity(init_cap), curr_size(0),
                                                               max_lf(init_load_factor)
{
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(const UnorderedMap &other) : data_holder(), data(other.capacity, data_holder.end()), capacity(other.capacity), curr_size(0),
                                                                                        max_lf(other.max_lf)
{
    auto iter = other.begin();
    auto end_iter = other.end();

    while (iter != end_iter)
    {
        insert(*iter);
        ++iter;
    }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(UnorderedMap &&other) : data_holder(std::move(other.data_holder)), data(std::move(other.data)),
                                                                                   capacity(other.capacity), curr_size(other.curr_size),
                                                                                   max_lf(other.max_lf)
{
    other.max_lf = init_load_factor;
    other.capacity = init_cap;
    other.curr_size = 0;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename U>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ConstIterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::find_ins_pos(U &&key) const
{

    size_t elem_hash = get_hash(key);

    ConstIterator iter = data[elem_hash];
    ConstIterator end_iter = end();

    while (iter != end_iter && (get_hash((*iter).first) == elem_hash))
    {
        if (Equal{}((*iter).first, key))
        {
            return iter;
        }
        ++iter;
    }

    return iter;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename U>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::find_ins_pos(U &&key)
{

    size_t elem_hash = get_hash(key);

    Iterator iter = data[elem_hash];
    Iterator end_iter = end();

    while (iter != end_iter && (get_hash((*iter).first) == elem_hash))
    {
        if (Equal{}((*iter).first, key))
        {
            return iter;
        }
        ++iter;
    }

    return iter;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename U>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::find(U &&key)
{

    Iterator iter = find_ins_pos(key);

    std::cerr.flush();
    if (iter != end() && Equal{}((*iter).first, key))
    {
        return iter;
    }
    else
    {
        return end();
    }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename U>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::ConstIterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::find(U &&key) const
{

    ConstIterator iter = find_ins_pos(key);

    std::cerr.flush();
    if (iter != end() && Equal{}((*iter).first, key))
    {
        return iter;
    }
    else
    {
        return end();
    }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator, bool> UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(NodeType &&ins_pair)
{

    auto ins_pos = find_ins_pos(ins_pair.first);
    bool is_found = ((ins_pos != end()) && Equal{}(ins_pair.first, (*ins_pos).first));

    size_t key_hash = get_hash(ins_pair.first);

    if (!is_found)
    {

        data_holder.insert(ins_pos, std::move(ins_pair));
        ++curr_size;

        if (data[key_hash] == end())
        {
            --data[key_hash];
        }

        check_load();
        return std::make_pair(ins_pos, true);
    }

    return std::make_pair(ins_pos, false);
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::Iterator, bool> UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(const NodeType &ins_pair)
{

    auto ins_pos = find_ins_pos(ins_pair.first);
    bool is_found = ((ins_pos != end()) && Equal{}(ins_pair.first, (*ins_pos).first));

    size_t key_hash = get_hash(ins_pair.first);

    if (!is_found)
    {

        data_holder.insert(ins_pos, ins_pair);
        ++curr_size;

        if (data[key_hash] == end())
        {
            --data[key_hash];
        }

        check_load();
        return std::make_pair(ins_pos, true);
    }

    return std::make_pair(ins_pos, false);
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::check_load()
{
    if (capacity * max_lf < curr_size)
    {
        rehash();
    }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::rehash()
{

    List<NodeType, Alloc> new_data_holder;
    std::vector<Iterator> new_data(capacity << 1, new_data_holder.end());

    std::swap(data, new_data);
    std::swap(data_holder, new_data_holder);

    curr_size = 0;
    capacity <<= 1;

    for (auto iter = new_data_holder.begin(); iter != new_data_holder.end(); ++iter)
    {
        insert(std::forward<NodeType>(*iter));
    }

    check_load();
}


