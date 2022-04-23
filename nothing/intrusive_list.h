/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_INTRUSIVE_LIST_H_
#define NOTHING_INTRUSIVE_LIST_H_

#include <concepts>
#include <iterator>
#include <ranges>

namespace nothing {

template <class Tag>
class intrusive_list_member {
  public:
    using tag_type = Tag;

    constexpr intrusive_list_member() noexcept : _next{ this }, _prev{ this } {}

    constexpr intrusive_list_member(intrusive_list_member &&other) noexcept
    {
        if (this != std::addressof(other)) {
            _link(*other._prev, *other._next);
            other._reset();
        }
    }

    intrusive_list_member(const intrusive_list_member &) = delete;

    ~intrusive_list_member() noexcept
    {
        _cross();
        _reset();
    }

    constexpr auto &operator=(intrusive_list_member &&other) noexcept
    {
        _cross();
        _link(*other._prev, *other._next);
        return *this;
    }

    auto &operator=(const intrusive_list_member &) = delete;

  private:
    template <class, class>
    friend class intrusive_list;

    intrusive_list_member *_next;
    intrusive_list_member *_prev;

    constexpr bool _alone() const noexcept { return _next == this; }

    constexpr void _cross() noexcept
    {
        _next->_prev = _prev;
        _prev->_next = _next;
    }

    constexpr void _link(intrusive_list_member &prev,
                         intrusive_list_member &next) noexcept
    {
        _prev = std::addressof(prev);
        _next = std::addressof(next);
        prev._next = this;
        next._prev = this;
    }

    constexpr void _reset() noexcept
    {
        _next = this;
        _prev = this;
    }

    friend constexpr bool operator==(const intrusive_list_member &lh,
                                     const intrusive_list_member &rh) noexcept
    {
        return std::addressof(lh) == std::addressof(rh);
    }
};

template <class T, class Tag>
    requires std::derived_from<T, intrusive_list_member<Tag>> &&
        std::same_as<T, std::remove_cv_t<T>>
class intrusive_list {
  public:
    using value_type = T;
    using reference = T &;
    using const_reference = const T &;
    using pointer = T *;
    using const_pointer = const T *;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
    using tag_type = Tag;

    class iterator;
    class const_iterator;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  private:
    using member_type = intrusive_list_member<Tag>;

  public:
    constexpr void push_front(reference value) noexcept
    {
        insert(begin(), value);
    }

    constexpr void push_back(reference value) noexcept { insert(end(), value); }

    constexpr void pop_front() noexcept { erase(*_root._next); }
    constexpr void pop_back() noexcept { erase(*_root._prev); }

    constexpr auto &front() noexcept
    {
        return static_cast<reference>(*_root._next);
    }

    constexpr auto &back() noexcept
    {
        return static_cast<reference>(*_root._prev);
    }

    constexpr auto &front() const noexcept
    {
        return static_cast<const_reference>(*_root._next);
    }

    constexpr auto &back() const noexcept
    {
        return static_cast<const_reference>(*_root._prev);
    }

    constexpr bool empty() const noexcept { return _root._alone(); }

    constexpr iterator insert(const_iterator pos, reference value) noexcept
    {
        member_type *pos_ptr = const_cast<value_type *>(std::addressof(*pos));
        member_type *value_ptr = std::addressof(value);

        if (pos_ptr != value_ptr) {
            value_ptr->_cross();
            value_ptr->_link(*pos_ptr->_prev, *pos_ptr);
        }

        return iterator(value);
    }

    // clang-format off
    template <std::input_iterator I, std::sentinel_for<I> S>
        requires std::convertible_to<std::iter_reference_t<I>, reference>
    constexpr iterator insert(const_iterator pos, I first, S last)
    {
        member_type *prev = const_cast<pointer>(std::addressof(*pos))->_prev;
        member_type *last_inserted = prev;

        for(reference value : std::ranges::subrange{ first, last }) {
            member_type *curr = std::addressof(value);

            if(curr == prev) {
                prev = prev->_prev;
            }

            if(curr != last_inserted) {
                curr->_cross();
                curr->_link(*last_inserted, *last_inserted->_next);
                last_inserted = curr;
            }
        }

        return iterator(static_cast<reference>(*prev->_next));
    }

    template <std::ranges::input_range R>
        requires std::convertible_to<std::ranges::range_reference_t<R>,
                                     reference>
    constexpr iterator insert(const_iterator pos, R &&r)
    {
        return insert(pos, std::ranges::begin(r), std::ranges::end(r));
    }

    // clang-format on

    constexpr iterator erase(const_iterator pos) noexcept
    {
        member_type &pos_member = const_cast<reference>(*pos);
        iterator ret{ static_cast<reference>(*pos_member._next) };

        pos_member._cross();
        pos_member._reset();

        return ret;
    }

    constexpr iterator erase(const_iterator first, const_iterator last)
    {
        member_type *prev = const_cast<reference>(*first).member_type::_prev;
        member_type *next = const_cast<pointer>(std::addressof(*last));

        for (member_type *pos = prev->_next, *pos_next = pos->_next;
             pos != next; pos = pos_next, pos_next = pos->_next) {
            pos->_reset();
        }

        prev->_next = next;
        next->_prev = prev;

        return iterator(static_cast<reference>(*next));
    }

    constexpr auto begin() noexcept
    {
        return iterator(static_cast<reference>(*_root._next));
    }

    constexpr auto begin() const noexcept
    {
        return const_iterator(static_cast<const_reference>(*_root._next));
    }

    constexpr auto end() noexcept
    {
        return iterator(static_cast<reference>(_root));
    }

    constexpr auto end() const noexcept
    {
        return const_iterator(static_cast<const_reference>(_root));
    }

    constexpr auto cbegin() const noexcept { return begin(); }
    constexpr auto cend() const noexcept { return end(); }

    constexpr auto rbegin() noexcept { return reverse_iterator(end()); }
    constexpr auto rbegin() const noexcept { return reverse_iterator(end()); }
    constexpr auto crbegin() const noexcept { return rend(); }

    constexpr auto rend() noexcept { return reverse_iterator(begin()); }
    constexpr auto rend() const noexcept { return reverse_iterator(begin()); }
    constexpr auto crend() const noexcept { return rbegin(); }

    constexpr size_type count() const noexcept
    {
        return std::ranges::distance(*this);
    }

    class iterator {
      public:
        using difference_type = intrusive_list::difference_type;
        using value_type = intrusive_list::value_type;
        using pointer = intrusive_list::pointer;
        using reference = intrusive_list::reference;
        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_concept = std::bidirectional_iterator_tag;

        constexpr iterator() noexcept = default;

        constexpr iterator(value_type &value) noexcept
        {
            _pos = std::addressof(value);
        }

        constexpr auto &operator++() noexcept
        {
            _pos = _pos->_next;
            return *this;
        }

        constexpr auto &operator--() noexcept
        {
            _pos = _pos->_prev;
            return *this;
        }

        constexpr auto operator++(int) noexcept
        {
            iterator ret{ *this };
            _pos = _pos->_next;
            return ret;
        }

        constexpr auto operator--(int) noexcept
        {
            iterator ret{ *this };
            _pos = _pos->_prev;
            return ret;
        }

        constexpr auto &operator*() const noexcept
        {
            return static_cast<reference>(*_pos);
        }

        constexpr auto operator->() const noexcept
        {
            return static_cast<pointer>(_pos);
        }

        friend constexpr bool operator==(const iterator &lh,
                                         const iterator &rh) noexcept = default;

      private:
        member_type *_pos;
    };

    class const_iterator {
      public:
        using difference_type = intrusive_list::difference_type;
        using value_type = intrusive_list::value_type;
        using pointer = intrusive_list::const_pointer;
        using reference = intrusive_list::const_reference;
        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_concept = std::bidirectional_iterator_tag;

        constexpr const_iterator() noexcept = default;

        constexpr const_iterator(iterator pos) noexcept
        {
            _pos = std::addressof(*pos);
        }

        constexpr const_iterator(const value_type &value) noexcept
        {
            _pos = std::addressof(value);
        }

        constexpr auto &operator++() noexcept
        {
            _pos = _pos->_next;
            return *this;
        }

        constexpr auto &operator--() noexcept
        {
            _pos = _pos->_prev;
            return *this;
        }

        constexpr auto operator++(int) noexcept
        {
            const_iterator ret{ *this };
            _pos = _pos->_next;
            return ret;
        }

        constexpr auto operator--(int) noexcept
        {
            const_iterator ret{ *this };
            _pos = _pos->_prev;
            return ret;
        }

        constexpr auto &operator*() const noexcept
        {
            return static_cast<const_reference>(*_pos);
        }

        constexpr auto operator->() const noexcept
        {
            return static_cast<const_pointer>(_pos);
        }

        friend constexpr bool
        operator==(const const_iterator &lh,
                   const const_iterator &rh) noexcept = default;

      private:
        const member_type *_pos;
    };

  private:
    member_type _root;
};

} // namespace nothing

#endif
