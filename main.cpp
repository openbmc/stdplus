//#include <stdplus/fd/dupable.hpp>
//#include <stdplus/fd/managed.hpp>
#include <stdplus/handle/copyable.hpp>
#include <stdplus/handle/managed.hpp>

#include <iostream>
#include <utility>

template <typename... Ts>
struct sPrinter;

template <typename T, typename... Ts>
struct sPrinter<T, Ts...>
{
    static void opSep(const T& t, const Ts&... ts)
    {
        std::cout << " " << t;
        sPrinter<Ts...>::opSep(ts...);
    }

    static void op(const T& t, const Ts&... ts)
    {
        std::cout << t;
        sPrinter<Ts...>::opSep(ts...);
        std::cout << "\n";
    }
};

template <>
struct sPrinter<>
{
    static void opSep()
    {
    }

    static void op()
    {
        std::cout << "\n";
    }
};

template <typename... Ts>
void printer(Ts&&... ts)
{
    sPrinter<Ts...>::op(ts...);
}

template <typename T, typename... Ts>
T refPrinter(const T& t, Ts&... ts)
{
    std::cout << "Ref ";
    printer(t, ts...);
    return t;
}

template <typename... Ts>
void dropPrinter(Ts&&... ts)
{
    std::cout << "Drop ";
    printer(ts...);
}

template <typename T, typename A = void>
struct Var
{
    template <void (*f)(T&, A&)>
    struct Handle
    {
        A arg;

        template <typename V>
        Handle(V&& arg) : arg(std::forward<V>(arg))
        {
        }

        template <typename V>
        void call(V&& x)
        {
            f(std::forward<V>(x), arg);
        }
    };
};

template <typename T>
struct Var<T, void>
{
    template <void (*f)(T&)>
    struct Handle
    {
        void call(T&& x)
        {
            f(std::forward<T>(x));
        }
    };
};

#include <tuple>

template <typename T, typename... As>
struct VarG
{
    template <void (*f)(T&&, As&...)>
    struct Handle
    {
        std::tuple<As...> as;

        template <typename... Vs>
        Handle(Vs&&... vs) : as(std::forward<Vs>(vs)...)
        {
        }

        template <typename V, std::size_t... Indices>
        void callImpl(V&& x, std::index_sequence<Indices...>)
        {
            f(std::forward<V>(x), std::get<Indices>(as)...);
        }

        template <typename V>
        void call(V&& x)
        {
            callImpl(std::forward<V>(x), std::index_sequence_for<As...>());
        }
    };
};

int main()
{
    Var<int, int>::Handle<printer> v(1);
    printf("Size: %lu\n", sizeof(v));
    v.call(3);
    v.call(4);

    Var<int>::Handle<printer> v2;
    printf("Size: %lu\n", sizeof(v2));
    v2.call(5);
    v2.call(6);

    VarG<int, int, int, int>::Handle<printer> v3(10, 5, 6);
    printf("Size: %lu\n", sizeof(v3));
    v3.call(11);
    v3.call(12);

    VarG<int>::Handle<printer> v4;
    printf("Size: %lu\n", sizeof(v4));
    v4.call(13);
    v4.call(14);

    stdplus::Copyable<int>::Handle<dropPrinter, refPrinter>();
    stdplus::Copyable<int>::Handle<dropPrinter, refPrinter>(10,
                                                            std::false_type());
    stdplus::Copyable<int>::Handle<dropPrinter, refPrinter> c1(1);
    {
        stdplus::Managed<int>::Handle<dropPrinter> m1(2, std::false_type());
        const int j = 3;
        stdplus::Managed<int>::Handle<dropPrinter> m2(j, std::false_type());
    }
    stdplus::Copyable<int>::Handle<dropPrinter, refPrinter> c2(c1);
}
