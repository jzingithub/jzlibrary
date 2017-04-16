/****
    author: jz
    example:

    // class Widget : public NewHandlerSupport<Widget>
    // {
    // public:
    //     // ...
    // };
    
    // void OutOfMemory();
    // Widget::SetNewHandler(&OutOfMemory);
    // Widget* pw = new Widget;
    // std::string* ps = new std::string;

    // Widget::SetNewHandler(&OutOfMemory);
    // Widget* pw2 = new Widget;
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
}
