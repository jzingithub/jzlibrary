/****
    * author: jz
    * example:

    * NewHandlerSupport:
    * class Widget : public NewHandlerSupport<Widget>
    * {
    * public:
    *     // ...
    * };
    * Widget::SetNewHandler(&OutOfMemory);
    * Widget* pw = new Widget;
    * std::string* ps = new std::string;
    * Widget::SetNewHandler(&OutOfMemory);
    * Widget* pw2 = new Widget;

    * StandardNewDeleteForms:
    * class Widget : public StandardNewDeleteForms
    * {
    * public:
    *     using StandardNewDeleteForms::operator new;
    *     using StandardNewDeleteForms::operator delete;
    *     // ... define your customed operator new/delete
    * }
****/

#pragma once
#include <cstddef>
#include <new>

namespace jz
{
    // use RAII to manage new-handler
    class NewHandlerHolder
    {
    public:
        explicit NewHandlerHolder(std::new_handler handler)
            : handler_(handler) {}

        ~NewHandlerHolder() { std::set_new_handler(handler_); }

        NewHandlerHolder(const NewHandlerHolder&) = delete;
        NewHandlerHolder& operator=(const NewHandlerHolder&) = delete;

    private:
        std::new_handler handler_;
    };

    // a mixin-style base class
    // actually typename T is never used
    template <typename T>
    class NewHandlerSupport
    {
    public:
        static std::new_handler SetNewHandler(std::new_handler handler) noexcept
        {
            auto old_handler = current_handler_;
            current_handler_ = handler;
            return old_handler;
        }

        static void* operator new(std::size_t size) throw(std::bad_alloc)
        {
            NewHandlerHolder holder(std::set_new_handler(current_handler_));
            return ::operator new(size);
        }

    private:
        static std::new_handler current_handler_;
    };

    template <typename T>
    std::new_handler NewHandlerSupport<T>::current_handler_ = nullptr;

    // 1.solve the problem that operator new/delete in derived classes will
    //   hide both global and inherited version of operator new/delete
    // 2.C++ provide three kinds of operator new in global scope:
    //   void* operator new(std::size_t) throw(std::bad_alloc);
    //   void* operator new(std::size_t, void*) noexcept;
    //   void* operator new(std::size_t, const std::nothrow_t&) noexcept;
    class StandardNewDeleteForms
    {
    public:
        static void* operator new(std::size_t size) throw(std::bad_alloc)
        {
            return ::operator new(size);
        }

        static void operator delete(void* p_memory) noexcept
        {
            return ::operator delete(p_memory);
        }

        static void* operator new(std::size_t size, void* ptr) noexcept
        {
            return ::operator new(size, ptr);
        }

        static void operator delete(void* p_memory, void* ptr) noexcept
        {
            return ::operator delete(p_memory, ptr);
        }

        static void* operator new(std::size_t size, const std::nothrow_t& nt) noexcept
        {
            return ::operator new(size, nt);
        }

        static void operator delete(void* p_memory, const std::nothrow_t& nt) noexcept
        {
            return ::operator delete(p_memory);
        }
    }
}