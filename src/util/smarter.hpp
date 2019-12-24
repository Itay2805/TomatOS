#ifndef LIBSMARTER_SMARTER_HPP
#define LIBSMARTER_SMARTER_HPP

/*
 * Taken from
 * https://github.com/avdgrinten/libsmarter
 *
 * modified to use the kernel's headers
 * instead of the std headers
 */

#include <arch/sync.hpp>
#include "debug.h"

// sometimes I hate C++
// I might dig into how to remove that at some point
namespace std {


    template< class T > struct remove_reference      {typedef T type;};
    template< class T > struct remove_reference<T&>  {typedef T type;};
    template< class T > struct remove_reference<T&&> {typedef T type;};

    template <class T>
    inline T&& forward(typename std::remove_reference<T>::type& t) noexcept
    {
        return static_cast<T&&>(t);
    }

    template <class T>
    inline T&& forward(typename std::remove_reference<T>::type&& t) noexcept
    {
        return static_cast<T&&>(t);
    }

    template <typename T>
    typename remove_reference<T>::type&& move(T&& arg)
    {
        return static_cast<typename remove_reference<T>::type&&>(arg);
    }

    template<typename T> void swap(T& t1, T& t2) {
        T temp = std::move(t1); // or T temp(std::move(t1));
        t1 = std::move(t2);
        t2 = std::move(temp);
    }

    typedef decltype(nullptr) nullptr_t;

    template<class T, class U = T>
    T exchange(T& obj, U&& new_value)
    {
        T old_value = std::move(obj);
        obj = std::forward<U>(new_value);
        return old_value;
    }

}

namespace util {

    struct adopt_rc_t { };
//inline constexpr adopt_rc_t adopt_rc;
    static constexpr adopt_rc_t adopt_rc;

// Each counter has a 'holder'. The holder is either null or
// another counter that controls the lifetime of the counter itself.
    struct counter {
    public:
        counter()
                : _holder{nullptr}, _count(0) { }

        // TODO: Do not take a pointer to a counter but a shared_ptr here.
        counter(adopt_rc_t, counter *holder, unsigned int initial_count)
                : _holder{holder}, _count(initial_count) { }

        counter(const counter &) = delete;

        counter &operator= (const counter &) = delete;

    protected:
        ~counter() {
            ASSERT(!_count);
        }

        virtual void dispose() = 0;

    public:
        void setup(adopt_rc_t, counter *holder, unsigned int initial_count) {
            ASSERT(!_count);
            _holder = holder;
            _count = initial_count;
        }

        counter *holder() {
            return _holder;
        }

        unsigned int check_count() {
            return _count;
        }

        void increment() {
            auto count = arch::sync::interlocked_increment(&_count);
            ASSERT(count);
        }

        bool increment_if_nonzero() {
            auto count = _count;
            do {
                if(!count)
                    return false;
            } while(arch::sync::interlocked_compare_exchange(&_count, count, count + 1));
            return true;
        }

        void decrement() {
            auto count = arch::sync::interlocked_decrement(&_count);
            if(count > 1)
                return;
            ASSERT(count == 1);

            // dispose() is allowed to destruct the counter itself;
            // therefore we load _holder before calling it.
            auto h = _holder;

            dispose();

            // We expect that this recursion is too shallow to be a problem.
            if(h)
                h->decrement();
        }

    private:
        counter *_holder;
        _Atomic unsigned int _count;
    };

    template<typename D, typename A = void>
    struct crtp_counter : counter {
        crtp_counter() = default;

        crtp_counter(adopt_rc_t, counter *holder, unsigned int initial_count)
                : counter{adopt_rc, holder, initial_count} { }

        void dispose() override {
            auto d = static_cast<D *>(this);
            d->dispose(A{});
        }
    };

    struct dispose_memory { };
    struct dispose_object { };

    template<typename T>
    struct box {
        template<typename... Args>
        void construct(Args &&... args) {
            new (&_stor) T{std::forward<Args>(args)...};
        }

        T *get() {
            return __builtin_launder(reinterpret_cast<T *>(&_stor));
        }

        void destruct() {
            get()->T::~T();
        }

    private:
        _Alignas(alignof(T)) char _stor[sizeof(T)];
    };

    struct default_deallocator {
        template<typename X>
        void operator() (X *p) {
            delete p;
        }
    };

    template<typename A>
    struct allocator_deallocator {
        template<typename X>
        void operator() (X *p) {
            A allocator;
            p->~X();
            allocator.deallocate(p, sizeof(X));
        }
    };

    template<typename T, typename Deallocator = default_deallocator>
    struct meta_object
            : private crtp_counter<meta_object<T, Deallocator>, dispose_memory>,
              private crtp_counter<meta_object<T, Deallocator>, dispose_object> {
        friend struct crtp_counter<meta_object, dispose_memory>;
        friend struct crtp_counter<meta_object, dispose_object>;

        template<typename... Args>
        meta_object(unsigned int initial_count, Args &&... args)
                : crtp_counter<meta_object, dispose_memory>{adopt_rc, nullptr, 1},
                  crtp_counter<meta_object, dispose_object>{adopt_rc,
                                                            static_cast<crtp_counter<meta_object, dispose_memory> *>(this),
                                                            initial_count} {
            _bx.construct(std::forward<Args>(args)...);
        }

        virtual ~meta_object() = default;

        T *get() {
            return _bx.get();
        }

        counter *memory_ctr() {
            return static_cast<crtp_counter<meta_object, dispose_memory> *>(this);
        }

        counter *object_ctr() {
            return static_cast<crtp_counter<meta_object, dispose_object> *>(this);
        }

    private:
        // Suppress Clang's warning about hidden virtual functions.
        using crtp_counter<meta_object, dispose_object>::dispose;
        using crtp_counter<meta_object, dispose_memory>::dispose;

        void dispose(dispose_object) {
            _bx.destruct();
        }

        void dispose(dispose_memory) {
            Deallocator d;
            d(this);
        }

        box<T> _bx;
    };

    template<typename T, typename D>
    struct shared_ptr;

    template<typename T, typename D>
    struct ptr_access_crtp {
        T &operator* () const {
            auto d = static_cast<const D *>(this);
            return *d->get();
        }

        T *operator-> () const {
            auto d = static_cast<const D *>(this);
            return d->get();
        }
    };

    template<typename D>
    struct ptr_access_crtp<void, D> {

    };

    template<typename T, typename H = void>
    struct shared_ptr : ptr_access_crtp<T, shared_ptr<T, H>> {
        template<typename T_, typename H_>
        friend struct shared_ptr;

        friend struct ptr_access_crtp<T, shared_ptr>;

        friend void swap(shared_ptr &x, shared_ptr &y) {
            std::swap(x._object, y._object);
            std::swap(x._ctr, y._ctr);
        }

        template<typename L>
        friend shared_ptr handle_cast(shared_ptr<T, L> other) {
            shared_ptr p;
            std::swap(p._object, other._object);
            std::swap(p._ctr, other._ctr);
            return std::move(p);
        }

        shared_ptr()
                : _object{nullptr}, _ctr{nullptr} { }

        shared_ptr(std::nullptr_t)
                : _object{nullptr}, _ctr{nullptr} { }

        shared_ptr(adopt_rc_t, T *object, counter *ctr)
                : _object{object}, _ctr{ctr} { }

        shared_ptr(const shared_ptr &other)
                : _object{other._object}, _ctr{other._ctr} {
            if(_ctr)
                _ctr->increment();
        }

        shared_ptr(shared_ptr &&other)
                : shared_ptr{} {
            swap(*this, other);
        }

        template<typename X>//, typename = std::enable_if_t<std::is_base_of_v<X, T>>>
        shared_ptr(shared_ptr<X, H> other)
                : _object{std::exchange(other._object, nullptr)},
                  _ctr{std::exchange(other._ctr, nullptr)} { }

        // Aliasing constructor.
        template<typename X>
        shared_ptr(const shared_ptr<X, H> &other, T *object)
                : _object{object}, _ctr{other._ctr} {
            if(_ctr)
                _ctr->increment();
        }

        ~shared_ptr() {
            if(_ctr)
                _ctr->decrement();
        }

        shared_ptr &operator= (shared_ptr other) {
            swap(*this, other);
            return *this;
        }

        operator bool () const {
            return _object;
        }

#if __STDC_HOSTED__
        std::pair<T *, counter *> release() {
		return std::make_pair(std::exchange(_object, nullptr),
				std::exchange(_ctr, nullptr));
	}
#else
        void release() {
            _object = nullptr;
            _ctr = nullptr;
        }
#endif

        T *get() const {
            return _object;
        }

        counter *ctr() const {
            return _ctr;
        }

    private:
        T *_object;
        counter *_ctr;
    };

    template<typename X, typename T, typename H>
    shared_ptr<X, H> static_pointer_cast(shared_ptr<T, H> other) {
        auto ptr = shared_ptr<X, H>{adopt_rc, static_cast<X *>(other.get()), other.ctr()};
        other.release();
        return std::move(ptr);
    }

    template<typename T, typename H = void>
    struct borrowed_ptr : ptr_access_crtp<T, borrowed_ptr<T, H>> {
        template<typename T_, typename H_>
        friend struct borrowed_ptr;

        borrowed_ptr()
                : _object{nullptr}, _ctr{nullptr} { }

        borrowed_ptr(std::nullptr_t)
                : _object{nullptr}, _ctr{nullptr} { }

        borrowed_ptr(T *object, counter *ctr)
                : _object{object}, _ctr{ctr} { }

        template<typename X>//, typename = std::enable_if_t<std::is_base_of_v<X, T>>>
        borrowed_ptr(borrowed_ptr<X, H> other)
                : _object{other._object}, _ctr{other._ctr} { }

        // Construction from shared_ptr.
        // TODO: enable_if X * is convertible to T *.
        template<typename X>
        borrowed_ptr(const shared_ptr<X, H> &other)
                : _object{other.get()}, _ctr{other.ctr()} { }

        T *get() const {
            return _object;
        }

        counter *ctr() const {
            return _ctr;
        }

        shared_ptr<T, H> lock() const {
            if(!_ctr)
                return shared_ptr<T, H>{};
            _ctr->increment();
            return shared_ptr<T, H>{adopt_rc, _object, _ctr};
        }

    private:
        T *_object;
        counter *_ctr;
    };

    template<typename X, typename T, typename H>
    borrowed_ptr<X, H> static_pointer_cast(borrowed_ptr<T, H> other) {
        return borrowed_ptr<X, H>{static_cast<X *>(other.get()), other.ctr()};
    }

    template<typename T, typename H = void>
    struct weak_ptr {
        friend void swap(weak_ptr &x, weak_ptr &y) {
            std::swap(x._object, y._object);
            std::swap(x._ctr, y._ctr);
        }

        weak_ptr()
                : _object{nullptr}, _ctr{nullptr} { }

        weak_ptr(const shared_ptr<T, H> &ptr)
                : _object{ptr.get()}, _ctr{ptr.ctr()} {
            ASSERT(_ctr->holder());
            _ctr->holder()->increment();
        }

        template<typename X>
        weak_ptr(const shared_ptr<X, H> &ptr)
                : _object{ptr.get()}, _ctr{ptr.ctr()} {
            ASSERT(_ctr->holder());
            _ctr->holder()->increment();
        }

        weak_ptr(const weak_ptr &other)
                : _object{other.object}, _ctr{other.ctr} {
            if(_ctr) {
                ASSERT(_ctr->holder());
                _ctr->holder()->increment();
            }
        }

        weak_ptr(weak_ptr &&other)
                : weak_ptr{} {
            swap(*this, other);
        }

        ~weak_ptr() {
            if(_ctr) {
                ASSERT(_ctr->holder());
                _ctr->holder()->decrement();
            }
        }

        weak_ptr &operator= (weak_ptr other) {
            swap(*this, other);
            return *this;
        }

        shared_ptr<T, H> lock() const {
            if(!_ctr)
                return shared_ptr<T, H>{};

            if(!_ctr->increment_if_nonzero())
                return shared_ptr<T, H>{};

            return shared_ptr<T, H>{adopt_rc, _object, _ctr};
        }

    private:
        T *_object;
        counter *_ctr;
    };

    template<typename T, typename... Args>
    shared_ptr<T> make_shared(Args &&... args) {
        auto meta = new meta_object<T>{1, std::forward<Args>(args)...};
        return shared_ptr<T>{adopt_rc, meta->get(), meta->object_ctr()};
    }

    template<typename T, typename Allocator, typename... Args>
    shared_ptr<T> allocate_shared(Allocator alloc, Args &&... args) {
        using meta_type = meta_object<T, allocator_deallocator<Allocator>>;
        auto memory = alloc.allocate(sizeof(meta_type));
        auto meta = new (memory) meta_type{1, std::forward<Args>(args)...};
        return shared_ptr<T>{adopt_rc, meta->get(), meta->object_ctr()};
    }

} // namespace util

#endif // LIBSMARTER_SMARTER_HPP