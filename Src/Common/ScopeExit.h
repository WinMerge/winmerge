/**
 * @file  ScopeExit.h
 *
 * @brief std::scope_exit-like implementation (C++20 Library Fundamentals TS v3)
 *
 * This file provides a scope exit mechanism similar to std::scope_exit from
 * the C++20 Library Fundamentals TS. It executes a function when leaving scope.
 *
 * Usage example:
 *   void foo() {
 *     FILE* file = fopen("test.txt", "r");
 *     auto guard = make_scope_exit([file] { if (file) fclose(file); });
 *     
 *     // ... use file ...
 *     // file will be automatically closed when leaving scope
 *   }
 *
 * With release():
 *   void bar() {
 *     auto guard = make_scope_exit([]{ cleanup(); });
 *     if (success) {
 *       guard.release(); // Don't execute cleanup
 *     }
 *   }
 */
#pragma once

#include <type_traits>
#include <utility>

/**
 * @brief RAII wrapper that executes a function when going out of scope
 * 
 * This class template is similar to std::experimental::scope_exit.
 * The exit function is stored directly (not through std::function) for efficiency.
 *
 * @tparam EF The type of the exit function
 */
template<typename EF>
class scope_exit
{
public:
    /**
     * @brief Constructs a scope_exit from an exit function
     * @param f Exit function to be executed on scope exit
     */
    template<typename EFP,
             typename = typename std::enable_if<
                 std::is_constructible<EF, EFP>::value &&
                 !std::is_same<typename std::remove_cv<typename std::remove_reference<EFP>::type>::type, scope_exit>::value
             >::type>
    explicit scope_exit(EFP&& f) noexcept(std::is_nothrow_constructible<EF, EFP>::value || std::is_nothrow_constructible<EF, EFP&>::value)
        : m_exit_function(std::forward<EFP>(f))
        , m_execute_on_destruction(true)
    {
    }

    /**
     * @brief Move constructor
     */
    scope_exit(scope_exit&& other) noexcept(std::is_nothrow_move_constructible<EF>::value || std::is_nothrow_copy_constructible<EF>::value)
        : m_exit_function(std::forward<EF>(other.m_exit_function))
        , m_execute_on_destruction(other.m_execute_on_destruction)
    {
        other.release();
    }

    /**
     * @brief Destructor - executes the exit function if active
     */
    ~scope_exit() noexcept
    {
        if (m_execute_on_destruction)
        {
            m_exit_function();
        }
    }

    /**
     * @brief Disables the execution of the exit function
     */
    void release() noexcept
    {
        m_execute_on_destruction = false;
    }

    // Non-copyable and non-move-assignable
    scope_exit(const scope_exit&) = delete;
    scope_exit& operator=(const scope_exit&) = delete;
    scope_exit& operator=(scope_exit&&) = delete;

private:
    EF m_exit_function;
    bool m_execute_on_destruction;
};

/**
 * @brief Creates a scope_exit object, deducing the template argument type
 * 
 * This is the recommended way to create scope_exit objects.
 *
 * @tparam EF The type of the exit function (deduced)
 * @param f Exit function to be executed on scope exit
 * @return scope_exit<EF> object
 */
template<typename EF>
scope_exit<typename std::remove_reference<EF>::type> make_scope_exit(EF&& f) noexcept(noexcept(scope_exit<typename std::remove_reference<EF>::type>(std::forward<EF>(f))))
{
    return scope_exit<typename std::remove_reference<EF>::type>(std::forward<EF>(f));
}

