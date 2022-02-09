#ifndef NOTHING_MEMORY_H_
#define NOTHING_MEMORY_H_

namespace nothing {
namespace internal {
namespace memory {

template <class Allocator>
constexpr typename std::allocator_traits<Allocator>::pointer
allocate(Allocator &allocator,
         typename std::allocator_traits<Allocator>::size_type n)
{
    return std::allocator_traits<Allocator>::allocate(allocator, n);
}

template <class Allocator>
constexpr typename std::allocator_traits<Allocator>::pointer
allocate(Allocator &allocator,
         typename std::allocator_traits<Allocator>::size_type n,
         typename std::allocator_traits<Allocator>::const_void_pointer hint)
{
    return std::allocator_traits<Allocator>::allocate(allocator, n, hint);
}

template <class Allocator>
constexpr void
deallocate(Allocator &allocator,
           typename std::allocator_traits<Allocator>::pointer ptr,
           typename std::allocator_traits<Allocator>::size_type n)
{
    std::allocator_traits<Allocator>::deallocate(allocator, ptr, n);
}

template <class Allocator, class T, class... Args>
constexpr void construct(Allocator &allocator, T *ptr, Args &&...args)
{
    std::allocator_traits<Allocator>::construct(allocator, ptr,
                                                std::forward<Args>(args)...);
}

template <class Allocator, class T>
constexpr void destroy(Allocator &allocator, T *ptr)
{
    std::allocator_traits<Allocator>::destroy(allocator, ptr);
}

template <class Allocator, class T, class... Args>
constexpr void construct_n(Allocator &allocator, std::size_t n, T *ptr,
                           Args &&...args)
{
    for (; n > 0; n--, ptr++)
        construct(allocator, ptr, std::forward<Args>(args)...);
}

template <class Allocator, class T>
constexpr void destroy_n(Allocator &allocator, T *ptr, std::size_t n)
{
    for (; n > 0; n--, ptr++)
        destroy(allocator, ptr);
}

template <class Allocator, class T>
constexpr void move_construct_n(Allocator &allocator, std::size_t n, T *src,
                                T *dest)
{
    for (; n > 0; n--, dest++, src++)
        construct(allocator, dest, std::move(*src));
}

template <class Allocator, class T, class... Args>
constexpr typename std::allocator_traits<Allocator>::pointer
reallocate(Allocator &allocator, std::size_t size, std::size_t newsize, T *src,
           Args &&...args)
{
    typename std::allocator_traits<Allocator>::pointer dest =
        allocate(allocator, newsize);

    if(newsize > size) {
        move_construct_n(allocator, size, src, dest);
        construct_n(allocator, newsize - size, dest + size, std::forward<Args>(args)...);
    } else {
        move_construct_n(allocator, newsize, src, dest);
    }

    destroy_n(allocator, src, size);
    deallocate(allocator, src, size);

    return dest;
}

} // namespace memory
} // namespace internal
} // namespace nothing

#endif
