#pragma once

/****
    author: jz
    example:

    Quantity<float, length> len1(10.23f);
    Quantity<float, length> len2(5.0f);
    Quantity<float, mass> mass1(2.3f);
    cout << (len1 + len2).Value() << endl;
    cout << (len1 + mass1).Value() << endl; // compile error!
****/

namespace jz
{
    // dimension analysis by template metaprogramming
    // The International System of Units contains seven base units
    // 1.mass
    // 2.length
    // 3.time
    // 4.electric current
    // 5.thermodynamic temperature
    // 6.luminous intensity
    // 7.amount of substance

    // variadic template since C++11
    template <typename T, T... Args>
    struct DimensionVector
    {
        using Type = DimensionVector<T, Args...>;
    };

    using mass = DimensionVector<int, 1, 0, 0, 0, 0, 0, 0>;
    using length = DimensionVector<int, 0, 1, 0, 0, 0, 0, 0>;
    using time = DimensionVector<int, 0, 0, 1, 0, 0, 0, 0>;
    using electric_current = DimensionVector<int, 0, 0, 0, 1, 0, 0, 0>;
    using thermodynamic_temperature = DimensionVector<int, 0, 0, 0, 0, 1, 0, 0>;
    using luminous_intensity = DimensionVector<int, 0, 0, 0, 0, 0, 1, 0>;
    using amount_of_substance = DimensionVector<int, 0, 0, 0, 0, 0, 0, 1>;

    // define a variable like this: Quantity<double, mass> m(1.11);
    template <typename T, typename Dimension>
    class Quantity
    {
    public:
        explicit Quantity(T value) : value_(value) {}
        T Value() const { return value_; }

    private:
        T value_;
    };

    // addition and subtraction for same dimensions
    template <typename T, typename Dimension>
    Quantity<T, Dimension> operator+(const Quantity<T, Dimension>& lhs,
                                     const Quantity<T, Dimension>& rhs)
    {
        return Quantity<T, Dimension>(lhs.Value() + rhs.Value());
    }

    template <typename T, typename Dimension>
    Quantity<T, Dimension> operator-(const Quantity<T, Dimension>& lhs,
                                     const Quantity<T, Dimension>& rhs)
    {
        return Quantity<T, Dimension>(lhs.Value() - rhs.Value());
    }

    // 1.ValueType is a numerical type
    // 2.multiplication(division) between two dimensions means
    //   addition(subtraction) between every pair of corresponding template parameters
    template <typename ValueType,
              ValueType lhs,
              ValueType rhs>
    struct DimensionPlusImpl
    {
        using Type = DimensionPlusImpl<ValueType, lhs, rhs>;
        const static ValueType value = lhs + rhs;
    };

    struct DimensionPlus
    {
        template <typename ValueType,
                  ValueType lhs,
                  ValueType rhs>
        struct Apply : public DimensionPlusImpl<ValueType, lhs, rhs> {};
    };

    template <typename ValueType,
              ValueType lhs,
              ValueType rhs>
    struct DimensionMinusImpl
    {
        using Type = DimensionMinusImpl<ValueType, lhs, rhs>;
        const static ValueType value = lhs - rhs;
    };

    struct DimensionMinus
    {
        template <typename ValueType,
                  ValueType lhs,
                  ValueType rhs>
        struct Apply : public DimensionMinusImpl<ValueType, lhs, rhs> {};
    };

    // for a given DimensionVector, execute addition/subtraction term by term
    // MetaFunc receive DimensionPlus or DimensionMinus
    template <typename T1,
              typename T2,
              typename MetaFunc>
    struct DimensionTransform {};

    template <template <typename ValueType, ValueType... Args1> typename T1,
              template <typename ValueType, ValueType... Args2> typename T2,
              typename ValueType,
              ValueType... Args1,
              ValueType... Args2,
              typename MetaFunc>
    struct DimensionTransform<T1<ValueType, Args1...>,
                              T2<ValueType, Args2...>,
                              MetaFunc>
    {
        using Type =
            DimensionVector<ValueType,
                            MetaFunc::template Apply<ValueType,
                                                     Args1,
                                                     Args2>::value...>;
    };

    // multiplication and division between dimensions generate new dimension
    // example: velocity = length / time => DimensionVector<int, 0, 1, -1, 0, 0, 0, 0>
    template <typename T,
              typename Dimension1,
              typename Dimension2>
    Quantity<T,
             typename DimensionTransform<Dimension1,
                                         Dimension2,
                                         DimensionPlus>::Type>
    operator*(const Quantity<T, Dimension1>& lhs,
              const Quantity<T, Dimension2>& rhs)
    {
        using Dimension = DimensionTransform<Dimension1,
                                             Dimension2,
                                             DimensionPlus>::Type;
        return Quantity<T, Dimension>(lhs.Value() * rhs.Value());
    }

    template <typename T,
              typename Dimension1,
              typename Dimension2>
    Quantity<T,
             typename DimensionTransform<Dimension1,
                                         Dimension2,
                                         DimensionMinus>::Type>
    operator/(const Quantity<T, Dimension1>& lhs,
              const Quantity<T, Dimension2>& rhs)
    {
        using Dimension = DimensionTransform<Dimension1,
                                             Dimension2,
                                             DimensionMinus>::Type;
        return Quantity<T, Dimension>(lhs.Value() / rhs.Value());
    }
}