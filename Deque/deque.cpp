#include <vector>
#include <stdexcept>
#include <iostream>
#include <type_traits>

template <typename ValType>
class Deque
{
    ValType **rows = nullptr;
    size_t sz = 0;
    size_t row_cap = 0;
    size_t base_row_ind = 0;
    size_t zero_ind = 0;
    static const size_t row_length = 32;

public:
    // constructors
    Deque();
    Deque(int cap, const ValType &elem = ValType());
    Deque(const Deque<ValType> &other);
    Deque &operator=(const Deque<ValType> &other);

    // memory allocation
    void reserve_rows(size_t amount);

    // indexation
    size_t size() const;
    const ValType &operator[](size_t index) const;
    ValType &operator[](size_t index);
    const ValType &at(size_t index) const;
    ValType &at(size_t index);

    // element insertion/deletion
    void push_back(const ValType &value);
    void pop_back();

    void push_front(const ValType &value);
    void pop_front();

    // iterators
    template <bool IsConst>
    class common_iterator;

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    const_iterator cbegin() const;
    const_iterator cend() const;

    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;

    reverse_iterator rend();
    const_reverse_iterator rend() const;

    // insertion and deletion
    void insert(iterator ins_iter, const ValType &ins_value);
    void erase(iterator ers_iter);
};

// iterator initialization
template <typename ValType>
typename Deque<ValType>::const_reverse_iterator Deque<ValType>::crbegin() const
{
    return const_reverse_iterator((int)sz - 1, *this);
};

template <typename ValType>
typename Deque<ValType>::const_reverse_iterator Deque<ValType>::crend() const
{
    return const_reverse_iterator(-1, *this);
};

template <typename ValType>
typename Deque<ValType>::reverse_iterator Deque<ValType>::rbegin()
{
    return reverse_iterator((int)sz - 1, *this);
};

template <typename ValType>
typename Deque<ValType>::const_reverse_iterator Deque<ValType>::rbegin() const
{
    return crbegin();
};

template <typename ValType>
typename Deque<ValType>::reverse_iterator Deque<ValType>::rend()
{
    return reverse_iterator(-1, *this);
};

template <typename ValType>
typename Deque<ValType>::const_reverse_iterator Deque<ValType>::rend() const
{
    return crend();
};

template <typename ValType>
typename Deque<ValType>::const_iterator Deque<ValType>::cbegin() const
{
    return const_iterator(0, *this);
};

template <typename ValType>
typename Deque<ValType>::const_iterator Deque<ValType>::cend() const
{
    return const_iterator(sz, *this);
};

template <typename ValType>
typename Deque<ValType>::iterator Deque<ValType>::begin()
{
    return iterator(0, *this);
};

template <typename ValType>
typename Deque<ValType>::const_iterator Deque<ValType>::begin() const
{
    return cbegin();
};

template <typename ValType>
typename Deque<ValType>::iterator Deque<ValType>::end()
{
    return iterator(sz, *this);
};

template <typename ValType>
typename Deque<ValType>::const_iterator Deque<ValType>::end() const
{
    return cend();
};

// constructors

template <typename ValType>
Deque<ValType>::Deque()
{
    reserve_rows(1);
}

template <typename ValType>
Deque<ValType>::Deque(int cap, const ValType &value)
{
    reserve_rows(1);
    for (int i = 0; i < cap; i++)
    {
        push_back(value);
    }
}

template <typename ValType>
Deque<ValType>::Deque(const Deque<ValType> &other)
{
    reserve_rows(other.row_cap);
    auto other_beg_iter = other.begin();
    // can throw exception
    int elem_cop = 0;
    try
    {
        while (other_beg_iter != other.end())
        {
            push_back(*other_beg_iter);
            ++other_beg_iter;
            ++elem_cop;
        }
    }
    catch (...)
    {
        while (elem_cop--)
        {
            pop_back();
        }
        throw;
    }
}

// I am not sure
template <typename ValType>
Deque<ValType> &Deque<ValType>::operator=(const Deque<ValType> &other)
{
    int elem_copied = 0;
    size_t init_sz = size();
    try
    {
        auto other_iter = other.begin();
        while (other_iter != other.end())
        {
            push_back(*other_iter);
            ++other_iter;
            ++elem_copied;
        }
        for (size_t i = 0; i < init_sz; i++)
        {
            pop_front();
        }
        return (*this);
    }
    catch (...)
    {
        while (elem_copied--)
        {
            pop_back();
        }
        return (*this);
        throw;
    }
}

// self-info
template <typename ValType>
size_t Deque<ValType>::size() const
{
    return sz;
}

// indexation
template <typename ValType>
const ValType &Deque<ValType>::operator[](size_t index_need) const
{
    size_t index = index_need + zero_ind;
    return rows[base_row_ind + (index / row_length)][(index % row_length)];
}

template <typename ValType>
ValType &Deque<ValType>::operator[](size_t index_need)
{
    size_t index = index_need + zero_ind;
    return rows[base_row_ind + (index / row_length)][(index % row_length)];
}

template <typename ValType>
const ValType &Deque<ValType>::at(size_t index_need) const
{
    size_t index = index_need + zero_ind;
    if (index_need < sz)
    {
        return (rows[base_row_ind + (index / row_length)])[(index % row_length)];
    }
    else
    {
        throw std::out_of_range("Index out of range!");
    }
}

template <typename ValType>
ValType &Deque<ValType>::at(size_t index_need)
{
    size_t index = index_need + zero_ind;
    if (index_need < sz)
    {
        return (rows[base_row_ind + (index / row_length)])[(index % row_length)];
    }
    else
    {
        throw std::out_of_range("Index out of range!");
    }
    return rows[base_row_ind][zero_ind];
}

// memory allocation
template <typename ValType>
void Deque<ValType>::reserve_rows(size_t amount)
{
    if (row_cap > amount)
    {
        return;
    }

    ValType **new_rows = new ValType *[amount];

    size_t row_am = ((zero_ind + sz + row_length - 1) / row_length);
    size_t new_base_ind = (amount - row_am) / 2;

    for (size_t i = 0; i < new_base_ind; i++)
    {
        new_rows[i] = reinterpret_cast<ValType *>(new int8_t[sizeof(ValType) * row_length]);
    }

    for (size_t i = 0; i < row_am; i++)
    {
        new_rows[new_base_ind + i] = rows[base_row_ind + i];
    }

    for (size_t i = new_base_ind + row_am; i < amount; i++)
    {
        new_rows[i] = reinterpret_cast<ValType *>(new int8_t[sizeof(ValType) * row_length]);
    }

    delete[] rows;
    rows = new_rows;

    row_cap = amount;
    base_row_ind = new_base_ind;
}

// element insertion/deletion
template <typename ValType>
void Deque<ValType>::push_back(const ValType &value)
{
    size_t ins_row = (zero_ind + sz) / row_length;
    size_t ins_ind = (zero_ind + sz) % row_length;

    if (base_row_ind + ins_row == row_cap)
    {
        reserve_rows(2 * row_cap);
    }

    try
    {
        new (rows[base_row_ind + ins_row] + ins_ind) ValType(value);
        ++sz;
    }
    catch (...)
    {
        std::cerr << "caught someth! \n";
    }
}

template <typename ValType>
void Deque<ValType>::pop_back()
{
    size_t ins_row = (zero_ind + sz) / row_length;
    size_t ins_ind = (zero_ind + sz) % row_length;

    (rows[base_row_ind + ins_row] + ins_ind)->~ValType();
    --sz;
}

template <typename ValType>
void Deque<ValType>::push_front(const ValType &value)
{

    if (zero_ind == 0)
    {
        if (base_row_ind == 0)
        {
            reserve_rows(2 * row_cap + 1);
        }

        --base_row_ind;
        zero_ind = row_length - 1;
    }
    else
    {
        --zero_ind;
    }

    new (rows[base_row_ind] + zero_ind) ValType(value);
    ++sz;
}

template <typename ValType>
void Deque<ValType>::pop_front()
{
    if (zero_ind == row_length - 1)
    {
        if (base_row_ind == row_cap - 1)
        {
            reserve_rows(2 * row_cap);
        }
    }

    (rows[base_row_ind] + zero_ind)->~ValType();
    --sz;

    ++zero_ind;
    if (zero_ind == row_length)
    {
        zero_ind = 0;
        ++base_row_ind;
    }
}

// iter insert and erase
template <typename ValType>
void Deque<ValType>::erase(Deque<ValType>::iterator iter)
{
    Deque<ValType> res_copy;

    try
    {
        auto beg_iter = begin();
        for (; beg_iter != iter; ++beg_iter)
        {
            res_copy.push_back(*beg_iter);
        }
        ++beg_iter;
        for (; beg_iter != end(); ++beg_iter)
        {
            res_copy.push_back(*beg_iter);
        }
        *this = res_copy;
    }
    catch (...)
    {
        throw;
    }
}

template <typename ValType>
void Deque<ValType>::insert(Deque<ValType>::iterator iter, const ValType &value)
{
    Deque<ValType> res_copy;

    try
    {
        for (auto beg_iter = begin(); beg_iter != iter; ++beg_iter)
        {
            res_copy.push_back(*beg_iter);
        }
        res_copy.push_back(value);
        Deque<ValType>::iterator end_iter(res_copy.size() - 1, *this);
        for (; end_iter != end(); ++end_iter)
        {
            res_copy.push_back(*end_iter);
        }
        *this = res_copy;
    }
    catch (...)
    {
        throw;
    }
}

// iterator
template <typename ValType>
template <bool IsConst>
class Deque<ValType>::common_iterator
{
    size_t row_ind;
    size_t in_row_ind;
    std::conditional_t<IsConst, const ValType *, ValType *> ptr;
    ValType **outer_arr;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = ValType;
    using pointer = ValType *;
    using reference = ValType &;
    using difference_type = std::ptrdiff_t;

    common_iterator(int index, const Deque<ValType> &in_deq);

    std::conditional_t<IsConst, const ValType &, ValType &> operator*();
    std::conditional_t<IsConst, const ValType *, ValType *> operator->();

    Deque<ValType>::common_iterator<IsConst> &operator++();
    Deque<ValType>::common_iterator<IsConst> &operator--();
    Deque<ValType>::common_iterator<IsConst> &operator+=(size_t incr);
    Deque<ValType>::common_iterator<IsConst> &operator-=(size_t decr);

    // iter comparison
    int operator-(const common_iterator<IsConst> &other);
    bool operator<(const common_iterator<IsConst> &other);
    bool operator>(const common_iterator<IsConst> &other);
    bool operator<=(const common_iterator<IsConst> &other);
    bool operator>=(const common_iterator<IsConst> &other);
    bool operator==(const common_iterator<IsConst> &other);
    bool operator!=(const common_iterator<IsConst> &other);

    // big arithmetic
    Deque<ValType>::common_iterator<IsConst> operator+(int summand)
    {
        std::cerr << "calling + iter " << summand << " ";
        Deque<ValType>::common_iterator<IsConst> copy(*this);
        copy += summand;
        return copy;
    }

    Deque<ValType>::common_iterator<IsConst> operator-(int summand)
    {
        std::cerr << "calling - iter " << summand << " ";
        Deque<ValType>::common_iterator<IsConst> copy(*this);
        copy -= summand;
        return copy;
    }
};

template <typename ValType>
template <bool IsConst>
Deque<ValType>::common_iterator<IsConst>::common_iterator(int index, const Deque<ValType> &in_deq)
{
    in_row_ind = (in_deq.zero_ind + index) % row_length;
    row_ind = in_deq.base_row_ind + ((in_deq.zero_ind + index) / row_length);
    outer_arr = in_deq.rows;
    ptr = outer_arr[row_ind] + in_row_ind;
}

template <typename ValType>
template <bool IsConst>
Deque<ValType>::common_iterator<IsConst> &Deque<ValType>::common_iterator<IsConst>::operator+=(size_t incr)
{
    size_t new_row_ind = ((in_row_ind + incr) / row_length) + row_ind;
    size_t new_in_row_ind = (in_row_ind + incr) % row_length;

    row_ind = new_row_ind;
    in_row_ind = new_in_row_ind;

    ptr = outer_arr[row_ind] + in_row_ind;

    return *this;
}

template <typename ValType>
template <bool IsConst>
Deque<ValType>::common_iterator<IsConst> &Deque<ValType>::common_iterator<IsConst>::operator-=(size_t decr)
{
    size_t go_back_rows = decr / row_length;
    size_t go_back_ind = decr % row_length;

    if (go_back_rows <= row_ind)
    {
        row_ind -= go_back_rows;
        if (go_back_ind <= in_row_ind)
        {
            in_row_ind -= go_back_ind;
            ptr = outer_arr[row_ind] + in_row_ind;
        }
        else
        {
            if (row_ind > 0)
            {
                --row_ind;
                in_row_ind = in_row_ind + row_length - go_back_ind;
                ptr = outer_arr[row_ind] + in_row_ind;
            }
        }
    }

    return (*this);
}

template <typename ValType>
template <bool IsConst>
Deque<ValType>::common_iterator<IsConst> &Deque<ValType>::common_iterator<IsConst>::operator++()
{
    return (*this += 1);
}

template <typename ValType>
template <bool IsConst>
Deque<ValType>::common_iterator<IsConst> &Deque<ValType>::common_iterator<IsConst>::operator--()
{
    return (*this -= 1);
}

template <typename ValType>
template <bool IsConst>
std::conditional_t<IsConst, const ValType &, ValType &> Deque<ValType>::common_iterator<IsConst>::operator*()
{
    return *ptr;
}

template <typename ValType>
template <bool IsConst>
std::conditional_t<IsConst, const ValType *, ValType *> Deque<ValType>::common_iterator<IsConst>::operator->()
{
    return ptr;
}

// iter diff
template <typename ValType>
template <bool IsConst>
int Deque<ValType>::common_iterator<IsConst>::operator-(const Deque<ValType>::common_iterator<IsConst> &other)
{
    return ((row_ind - other.row_ind) * row_length + (in_row_ind - other.in_row_ind));
}

template <typename ValType>
template <bool IsConst>
bool Deque<ValType>::common_iterator<IsConst>::operator<(const Deque<ValType>::common_iterator<IsConst> &other)
{
    return (*this - other) < 0;
}

template <typename ValType>
template <bool IsConst>
bool Deque<ValType>::common_iterator<IsConst>::operator>(const Deque<ValType>::common_iterator<IsConst> &other)
{
    return other > (*this);
}

template <typename ValType>
template <bool IsConst>
bool Deque<ValType>::common_iterator<IsConst>::operator<=(const Deque<ValType>::common_iterator<IsConst> &other)
{
    return !(*this > other);
}

template <typename ValType>
template <bool IsConst>
bool Deque<ValType>::common_iterator<IsConst>::operator>=(const Deque<ValType>::common_iterator<IsConst> &other)
{
    return !(*this < other);
}

template <typename ValType>
template <bool IsConst>
bool Deque<ValType>::common_iterator<IsConst>::operator==(const Deque<ValType>::common_iterator<IsConst> &other)
{
    return (*this - other) == 0;
}

template <typename ValType>
template <bool IsConst>
bool Deque<ValType>::common_iterator<IsConst>::operator!=(const Deque<ValType>::common_iterator<IsConst> &other)
{
    return !(*this == other);
}