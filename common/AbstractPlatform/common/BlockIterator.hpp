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
enum TBlockIteratorDirection
{
    BlockForwardIterator,
    BlockBackwardIterator
};

template < typename taTag,
           size_t taSize,
           TBlockIteratorDirection taDirection = TBlockIteratorDirection::BlockForwardIterator >
struct TBlock
{
    static_assert( taSize > 0, "taSize == 0" );

    using TTag = taTag;
    static constexpr auto kSize = taSize;
    static constexpr auto kDirection = taDirection;

    constexpr size_t
    GetForwardIndex( ) const
    {
        return kDirection == TBlockIteratorDirection::BlockForwardIterator ? iIndex
                                                                           : kSize - iIndex - 1;
    }

    constexpr void
    SetForwardIndex( size_t iForwardIndex )
    {
        iIndex = kDirection == TBlockIteratorDirection::BlockForwardIterator
                     ? iForwardIndex
                     : ( kSize - 1 ) - iForwardIndex;
    }

    size_t iIndex = ( kDirection == TBlockIteratorDirection::BlockForwardIterator ? 0 : kSize - 1 );
};

template < typename... taBlocks >
struct TBlockIterator
{
    static constexpr size_t
    BlocksCount( )
    {
        return kBlocks;
    }

    static constexpr size_t
    FullElementsCount( )
    {
        return ( ... * taBlocks::kSize );
    }

    template < size_t taBlockIndex >
    static constexpr size_t
    BlockElementsCount( )
    {
        static_assert( taBlockIndex < kBlocks, "taBlockIndex >=  kBlocks" );
        return BlockCapacity< taBlockIndex >(
            std::make_index_sequence< kBlocks - taBlockIndex >{ } );
    }

    template < size_t taBlockIndex >
    constexpr size_t
    BlockIndex( ) const
    {
        static_assert( taBlockIndex < kBlocks, "taBlockIndex >=  kBlocks" );
        return std::get< taBlockIndex >( iBlockList ).iIndex;
    }

    void
    SetGlobalIndex( size_t aGlobalIndex )
    {
        SetGlobalIndexImpl( aGlobalIndex, std::make_index_sequence< kBlocks >{ } );
    }

private:
    using TBlockList = std::tuple< taBlocks... >;
    static constexpr size_t kBlocks = std::tuple_size_v< TBlockList >;

    template < size_t taStartFrom, typename taT, taT... taIndexes >
    static constexpr size_t
    BlockCapacity( std::integer_sequence< taT, taIndexes... > )
    {
        return ( ... * std::tuple_element_t< taStartFrom + taIndexes, TBlockList >::kSize );
    }

    template < size_t taIdx >
    struct Devider
    {
        static constexpr size_t
        Value( )
        {
            return TBlockIterator::BlockElementsCount< taIdx + 1 >( );
        };
    };

    template <>
    struct Devider< TBlockIterator::BlocksCount( ) - 1 >
    {
        static constexpr size_t
        Value( )
        {
            return 1;
        };
    };

    // Block_n = idx / 1 % Block_n::kSize;
    // Block_n-1 = idx /Block_n::FullElementsCount() % Block_n-1::kSize
    // Block_1 = idx /Block_2::FullElementsCount() % Block_1::kSize
    // Block_0 = idx /Block_1::FullElementsCount() % Block_0::kSize
    template < typename taT, taT... taIndexes >
    constexpr void
    SetGlobalIndexImpl( size_t aGlobalIndex, std::integer_sequence< taT, taIndexes... > )
    {
        ( ( std::get< taIndexes >( iBlockList )
                .SetForwardIndex( aGlobalIndex / Devider< taIndexes >::Value( )
                                  % std::tuple_element_t< taIndexes, TBlockList >::kSize ) ),
          ... );
    }

    TBlockList iBlockList;
};

}  // namespace AbstractPlatform