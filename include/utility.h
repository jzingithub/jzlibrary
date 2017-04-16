// author: jz
// This header has various uses.
#pragma once

namespace jz
{
    // class derived from jz::Uncopyable cannot be copied.
    class Uncopyable
    {
    protected:
        constexpr Uncopyable() = default;
        ~Uncopyable() = default;

    private:
        Uncopyable(const Uncopyable&) = delete;
        Uncopyable& operator=(const Uncopyable&) = delete;
    }
}