#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/common/BinaryOperations.hpp>
#include <AbstractPlatform/common/TypeBinaryRepresentation.hpp>

#include <cstdint>
#include <type_traits>
#include <tuple>
#include <utility>

namespace AbstractPlatform
{
enum class TIterationDirection
{
    Forward,
    Backward
};

template < typename taTag,
           size_t taSize,
           TIterationDirection taIterationDirection = TIterationDirection::Forward >
struct TDimension
{
    using TTag = taTag;
    using TSize = size_t;
    static_assert( taSize > 0, "taSize has to be > 0" );
    static_assert( taSize < std::numeric_limits< TSize >::max( ),
                   "taSize has to be < std::numeric_limits< TSize >::max( )" );

    static constexpr auto kSize = taSize;
    static constexpr auto kIterationDirection = taIterationDirection;

    /**
     * @brief Returns dimension's cardinality (total number of iterable elements).
     *
     * @return constexpr size_t  Dimension's cardinality.
     */
    static constexpr size_t
    Size( )
    {
        return kSize;
    }

    constexpr size_t
    GetForwardPosition( ) const
    {
        return kIterationDirection == TIterationDirection::Forward ? iPosition
                                                                   : kSize - iPosition - 1;
    }

    constexpr void
    SetForwardPosition( size_t iForwardIndex )
    {
        assert( iForwardIndex <= kSize );
        iPosition = kIterationDirection == TIterationDirection::Forward
                        ? iForwardIndex
                        : ( kSize - 1 ) - iForwardIndex;
    }

    constexpr bool
    IsBegin( )
    {
        return kIterationDirection == TIterationDirection::Forward ? iPosition == 0
                                                                   : iPosition + 1 == kSize;
    }

    constexpr bool
    IsLast( )
    {
        return kIterationDirection == TIterationDirection::Forward ? iPosition + 1 == kSize
                                                                   : iPosition == 0;
    }

    constexpr bool
    IsEnd( )
    {
        return kIterationDirection == TIterationDirection::Forward ? iPosition == kSize
                                                                   : iPosition == TSize{ } - 1;
    }

    size_t iPosition = ( kIterationDirection == TIterationDirection::Forward ? 0 : kSize - 1 );
};

/**
 * @param taDimension The dimension type list, ordered from the first iterable dimension to the last
 * one.
 */
template < typename... taDimension >
class TStaticTensorIterator
{
public:
    static constexpr size_t
    DimentsionCount( )
    {
        return kDimentsionCount;
    }

    /**
     * @brief Returns tensor's cardinality (total number of elements).
     *
     * @return constexpr size_t  Tensor's cardinality.
     */
    static constexpr size_t
    Size( )
    {
        return ( ... * taDimension::kSize );
    }

    template < size_t taDimentsionIndex >
    static constexpr size_t
    SubTensorSize( )
    {
        static_assert( taDimentsionIndex < kDimentsionCount,
                       "The taDimentsionIndex must be less than kDimentsionCount" );

        return SubTensorSizeImpl( std::make_index_sequence< taDimentsionIndex + 1 >{ } );
    }

    template < size_t taDimentsionIndex >
    constexpr const auto&
    Dimension( ) const
    {
        static_assert( taDimentsionIndex < kDimentsionCount,
                       "The taDimentsionIndex must be less than kDimentsionCount" );
        return std::get< taDimentsionIndex >( iDimensionList );
    }

    template < size_t taDimentsionIndex >
    constexpr auto&
    Dimension( )
    {
        static_assert( taDimentsionIndex < kDimentsionCount,
                       "The taDimentsionIndex must be less than kDimentsionCount" );
        return std::get< taDimentsionIndex >( iDimensionList );
    }

    template < typename taDimentsion >
    constexpr const auto&
    Dimension( ) const
    {
        return std::get< taDimentsion >( iDimensionList );
    }

    template < typename taDimentsion >
    constexpr auto&
    Dimension( )
    {
        return std::get< taDimentsion >( iDimensionList );
    }

    void
    SetGlobalPosition( size_t aGlobalIndex )
    {
        assert( aGlobalIndex <= Size( ) );
        SetGlobalPositionImpl( aGlobalIndex, std::make_index_sequence< kDimentsionCount >{ } );
    }

    size_t
    GetGlobalPosition( )
    {
        return GetGlobalPositionImpl( std::make_index_sequence< kDimentsionCount >{ } );
    }

private:
    using TDimensionList = std::tuple< taDimension... >;
    static constexpr size_t kDimentsionCount = std::tuple_size_v< TDimensionList >;

    template < typename taT, taT... taIndexes >
    static constexpr size_t
    SubTensorSizeImpl( std::integer_sequence< taT, taIndexes... > )
    {
        return ( ... * std::tuple_element_t< taIndexes, TDimensionList >::Size( ) );
    }

    template < size_t taIdx >
    struct Devider
    {
        static_assert( taIdx < kDimentsionCount, "The taIdx has to be less than kDimentsionCount" );
        static constexpr size_t
        Value( )
        {
            return TStaticTensorIterator::SubTensorSize< taIdx - 1 >( );
        };
    };

    template <>
    struct Devider< 0 >
    {
        static constexpr size_t
        Value( )
        {
            return 1;
        };
    };

    template < size_t taIdx >
    struct Multiplier
    {
        static_assert( taIdx < kDimentsionCount, "The taIdx has to be less than kDimentsionCount" );
        static constexpr size_t
        Value( )
        {
            return std::tuple_element_t< taIdx - 1, TDimensionList >::Size( )
                   * Multiplier< taIdx - 1 >::Value( );
        };
    };

    template <>
    struct Multiplier< 0 >
    {
        static constexpr size_t
        Value( )
        {
            return 1;
        };
    };

    template < size_t taIdx >
    struct Module
    {
        static_assert( taIdx < kDimentsionCount, "The taIdx has to be less than kDimentsionCount" );
        static constexpr size_t
        Value( )
        {
            using TDimension = std::tuple_element_t< taIdx, TDimensionList >;
            return TDimension::kSize;
        };
    };

    template <>
    struct Module< kDimentsionCount - 1 >
    {
        static constexpr size_t
        Value( )
        {
            using TDimension = std::tuple_element_t< kDimentsionCount - 1, TDimensionList >;
            static_assert( TDimension::Size( )
                           < std::numeric_limits< typename TDimension::TSize >::max( ) );
            return TDimension::Size( ) + 1;
        };
    };

    // Dimension_0   = idx / 1 % Dimension_0::kSize;
    // Dimension_1   = idx / Dimension::SubTensorSize<0>() % Dimension_1::kSize
    //                        ...
    // Dimension_n-1 = idx / Dimension::SubTensorSize<n-2>() % Dimension_n-1::kSize
    // Dimension_n   = idx / Dimension::SubTensorSize<n-1>() % (Dimension_n::kSize + 1)
    template < typename taT, taT... taIndexes >
    constexpr void
    SetGlobalPositionImpl( size_t aGlobalIndex, std::integer_sequence< taT, taIndexes... > )
    {
        ( ( std::get< taIndexes >( iDimensionList )
                .SetForwardPosition( aGlobalIndex / Devider< taIndexes >::Value( )
                                     % Module< taIndexes >::Value( ) ) ),
          ... );
    }

    // Dimension_0::idx * 1
    // + Dimension_1::idx * Dimension_0::kSize
    // + Dimension_2::idx * Dimension_1::kSize * Dimension_0::kSize
    // + ...
    // + Dimension_n::idx * Dimension_n-1::kSize * ... * Dimension_0::kSize
    template < typename taT, taT... taIndex >
    constexpr size_t
    GetGlobalPositionImpl( std::integer_sequence< taT, taIndex... > )
    {
        return ( ( std::get< taIndex >( iDimensionList ).GetForwardPosition( )
                   * Multiplier< taIndex >::Value( ) )
                 + ... );
    }

    TDimensionList iDimensionList;
};

}  // namespace AbstractPlatform