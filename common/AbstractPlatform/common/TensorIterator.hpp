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
    static_assert( taSize > 0, "taSize == 0" );

    using TTag = taTag;
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
        iPosition = kIterationDirection == TIterationDirection::Forward
                        ? iForwardIndex
                        : ( kSize - 1 ) - iForwardIndex;
    }

    size_t iPosition = ( kIterationDirection == TIterationDirection::Forward ? 0 : kSize - 1 );
};

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
        return SubTensorSizeImpl< taDimentsionIndex >(
            std::make_index_sequence< kDimentsionCount - taDimentsionIndex >{ } );
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

    void
    SetGlobalPosition( size_t aGlobalIndex )
    {
        SetGlobalPositionImpl( aGlobalIndex, std::make_index_sequence< kDimentsionCount >{ } );
    }

private:
    using TBlockList = std::tuple< taDimension... >;
    static constexpr size_t kDimentsionCount = std::tuple_size_v< TBlockList >;

    template < size_t taStartFrom, typename taT, taT... taIndexes >
    static constexpr size_t
    SubTensorSizeImpl( std::integer_sequence< taT, taIndexes... > )
    {
        return ( ... * std::tuple_element_t< taStartFrom + taIndexes, TBlockList >::kSize );
    }

    template < size_t taIdx >
    struct Devider
    {
        static constexpr size_t
        Value( )
        {
            return TTensorIterator::SubTensorSize< taIdx + 1 >( );
        };
    };

    template <>
    struct Devider< TTensorIterator::DimentsionCount( ) - 1 >
    {
        static constexpr size_t
        Value( )
        {
            return 1;
        };
    };

    // Block_n   = idx / 1 % Block_n::kSize;
    // Block_n-1 = idx / Block_n::TensorSize() % Block_n-1::kSize
    //                        ...
    // Block_1   = idx / Block_2::TensorSize() % Block_1::kSize
    // Block_0   = idx / Block_1::TensorSize() % Block_0::kSize
    template < typename taT, taT... taIndexes >
    constexpr void
    SetGlobalPositionImpl( size_t aGlobalIndex, std::integer_sequence< taT, taIndexes... > )
    {
        ( ( std::get< taIndexes >( iDimensionList )
                .SetForwardPosition( aGlobalIndex / Devider< taIndexes >::Value( )
                                     % std::tuple_element_t< taIndexes, TBlockList >::kSize ) ),
          ... );
    }

    TBlockList iDimensionList;
};

}  // namespace AbstractPlatform