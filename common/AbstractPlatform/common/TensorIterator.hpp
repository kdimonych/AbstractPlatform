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
class TTensorIterator
{
public:
    static constexpr size_t
    DimentsionCount( )
    {
        return kDimentsionCount;
    }

    static constexpr size_t
    TensorSize( )
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
    constexpr size_t
    DimensionPosition( ) const
    {
        static_assert( taDimentsionIndex < kDimentsionCount,
                       "The taDimentsionIndex must be less than kDimentsionCount" );
        return std::get< taDimentsionIndex >( iDimensionList ).iPosition;
    }

    template < typename taDimentsion >
    constexpr size_t
    DimensionPosition( ) const
    {
        return std::get< taDimentsion >( iDimensionList ).iPosition;
    }

    template < size_t taDimentsionIndex >
    constexpr const auto&
    Dimension( ) const
    {
        static_assert( taDimentsionIndex < kDimentsionCount,
                       "The taDimentsionIndex must be less than kDimentsionCount" );
        return std::get< taDimentsionIndex >( iDimensionList ).iPosition;
    }

    template < typename taDimentsion >
    constexpr const auto&
    Dimension( ) const
    {
        return std::get< taDimentsion >( iDimensionList );
    }

    void
    SetGlobalPosition( size_t aGlobalIndex )
    {
        assert( aGlobalIndex <= TensorSize( ) );
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
        return ( ... * std::tuple_element_t< taIndexes, TDimensionList >::kSize );
    }

    template < size_t taIdx >
    struct Devider
    {
        static_assert( taIdx < kDimentsionCount, "The taIdx has to be less than kDimentsionCount" );
        static constexpr size_t
        Value( )
        {
            return TTensorIterator::SubTensorSize< taIdx - 1 >( );
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
            return std::tuple_element_t< taIdx - 1, TDimensionList >::kSize
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
            static_assert( TDimension::kSize
                           < std::numeric_limits< typename TDimension::TSize >::max( ) );
            return TDimension::kSize + 1;
        };
    };

    // Block_0   = idx / 1 % Block_0::kSize;
    // Block_1   = idx / Block::SubTensorSize<0>() % Block_1::kSize
    //                        ...
    // Block_n-1 = idx / Block::SubTensorSize<n-2>() % Block_n-1::kSize
    // Block_n   = idx / Block::SubTensorSize<n-1>() % (Block_n::kSize + 1)
    template < typename taT, taT... taIndexes >
    constexpr void
    SetGlobalPositionImpl( size_t aGlobalIndex, std::integer_sequence< taT, taIndexes... > )
    {
        ( ( std::get< taIndexes >( iDimensionList )
                .SetForwardPosition( aGlobalIndex / Devider< taIndexes >::Value( )
                                     % Module< taIndexes >::Value( ) ) ),
          ... );
    }

    // Block_0::idx * 1
    // + Block_1::idx * Block_0::kSize
    // + Block_2::idx * Block_1::kSize * Block_0::kSize
    // + ...
    // + Block_n::idx * Block_n-1::kSize * ... * Block_0::kSize
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