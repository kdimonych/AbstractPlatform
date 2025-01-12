#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/common/BinaryOperations.hpp>
#include <AbstractPlatform/common/TypeBinaryRepresentation.hpp>

#include <cassert>
#include <cstdint>
#include <type_traits>
#include <tuple>
#include <utility>

namespace AbstractPlatform::Tensor
{
enum class TIterationDirection
{
    Forward,
    Backward
};

template < typename taTag,
           size_t taSize,
           TIterationDirection taIterationDirection = TIterationDirection::Forward >
struct TStaticDimension
{
    using TTag = taTag;
    using TSize = size_t;
    using TPosition = size_t;

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
    static constexpr TSize
    Size( )
    {
        return kSize;
    }

    /**
     * @brief Get the current forward position.
     * @note It always returns a forward-directional position, not a kIterationDirection-like one.
     * To get a raw directional-dependent position, use the property iDirectionalPosition.
     *
     * @return constexpr TPosition
     */
    constexpr TPosition
    GetPosition( ) const
    {
        return kIterationDirection == TIterationDirection::Forward
                   ? iDirectionalPosition
                   : kSize - iDirectionalPosition - 1;
    }

    /**
     * @brief Set the forward-directional position.
     * @note To set the raw directional-dependent position, use the property iDirectionalPosition.
     * @param aForwardPosition  The forward-directional position.
     */
    constexpr void
    SetPosition( TPosition aForwardPosition )
    {
        assert( aForwardPosition <= kSize );
        iDirectionalPosition = kIterationDirection == TIterationDirection::Forward
                                   ? aForwardPosition
                                   : ( kSize - 1 ) - aForwardPosition;
    }

    constexpr bool
    IsBegin( )
    {
        return kIterationDirection == TIterationDirection::Forward
                   ? iDirectionalPosition == 0
                   : iDirectionalPosition + 1 == kSize;
    }

    constexpr bool
    IsLast( )
    {
        return kIterationDirection == TIterationDirection::Forward
                   ? iDirectionalPosition + 1 == kSize
                   : iDirectionalPosition == 0;
    }

    constexpr bool
    IsEnd( )
    {
        return kIterationDirection == TIterationDirection::Forward
                   ? iDirectionalPosition == kSize
                   : iDirectionalPosition == TSize{ } - 1;
    }

    TPosition iDirectionalPosition
        = ( kIterationDirection == TIterationDirection::Forward ? 0 : kSize - 1 );
};

template < typename taStaticDimension >
struct TStaticDimensionRef
{
    using TStaticDimension = taStaticDimension;
    using TTag = typename TStaticDimension::TTag;
    using TSize = typename TStaticDimension::TSize;
    using TPosition = typename TStaticDimension::TPosition;

    TStaticDimensionRef( TStaticDimension& aDimension )
        : iDimension{ aDimension }
    {
    }

    /**
     * @brief Returns dimension's cardinality (total number of iterable elements).
     *
     * @return constexpr size_t  Dimension's cardinality.
     */
    static constexpr TSize
    Size( )
    {
        return TStaticDimension::Size( );
    }

    /**
     * @brief Get the current forward position.
     * @note It always returns a forward-directional position, not a kIterationDirection-like one.
     * To get a raw directional-dependent position, use the property iDirectionalPosition.
     *
     * @return constexpr TPosition
     */
    constexpr inline TPosition
    GetPosition( ) const
    {
        return iDimension.GetPosition( );
    }

    /**
     * @brief Set the forward-directional position.
     * @note To set the raw directional-dependent position, use the property iDirectionalPosition.
     * @param aForwardPosition  The forward-directional position.
     */
    constexpr inline void
    SetPosition( TPosition aForwardPosition )
    {
        iDimension.SetPosition( aForwardPosition );
    }

    constexpr inline bool
    IsBegin( )
    {
        return iDimension.IsBegin( );
    }

    constexpr inline bool
    IsLast( )
    {
        return iDimension.IsLast( );
    }

    constexpr inline bool
    IsEnd( )
    {
        return iDimension.IsEnd( );
    }

    TStaticDimension& iDimension;
};

template < typename taStaticDimension >
static constexpr auto
MakeStaticDimensionRef( taStaticDimension& aDimension )
{
    return TStaticDimensionRef< taStaticDimension >( aDimension );
}

/**
 * @param taDimension The dimension type list, ordered from the first iterable dimension to the last
 * one.
 */
template < typename... taDimension >
struct TStaticDevider
{
    using TDimensionList = std::tuple< taDimension... >;
    using TSize = size_t;
    using TPosition = size_t;

    static constexpr size_t
    DimentsionCount( )
    {
        return kDimentsionCount;
    }

    /**
     * @brief Returns tensor's cardinality (total number of elements).
     *
     * @return constexpr TSize  Tensor's cardinality.
     */
    static constexpr TSize
    Size( )
    {
        return ( ... * std::decay< taDimension >::type::Size( ) );
    }

    template < size_t taDimentsionIndex >
    static constexpr TSize
    SubTensorSize( )
    {
        static_assert( taDimentsionIndex < kDimentsionCount,
                       "The taDimentsionIndex must be less than kDimentsionCount" );

        return SubTensorSizeImpl( std::make_index_sequence< taDimentsionIndex + 1 >{ } );
    }

    static constexpr inline void
    Devide( TPosition aGlobalPosition, TDimensionList& aByDimensionList )
    {
        assert( aGlobalPosition <= Size( ) );
        DevideImpl( aGlobalPosition, aByDimensionList,
                    std::make_index_sequence< kDimentsionCount >{ } );
    }

    static constexpr inline TPosition
    Product( const TDimensionList& aDimensionList )
    {
        return ProductImpl( aDimensionList, std::make_index_sequence< kDimentsionCount >{ } );
    }

private:
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
            return TStaticDevider::SubTensorSize< taIdx - 1 >( );
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
            using TStaticDimension = std::tuple_element_t< taIdx, TDimensionList >;
            return TStaticDimension::Size( );
        };
    };

    template <>
    struct Module< kDimentsionCount - 1 >
    {
        static constexpr size_t
        Value( )
        {
            using TStaticDimension = std::tuple_element_t< kDimentsionCount - 1, TDimensionList >;
            static_assert( TStaticDimension::Size( )
                           < std::numeric_limits< typename TStaticDimension::TSize >::max( ) );
            return TStaticDimension::Size( ) + 1;
        };
    };

    // Dimension_0   = idx / 1 % Dimension_0::kSize;
    // Dimension_1   = idx / Dimension::SubTensorSize<0>() % Dimension_1::kSize
    //                        ...
    // Dimension_n-1 = idx / Dimension::SubTensorSize<n-2>() % Dimension_n-1::kSize
    // Dimension_n   = idx / Dimension::SubTensorSize<n-1>() % (Dimension_n::kSize + 1)
    template < typename taT, taT... taIndexes >
    static constexpr void
    DevideImpl( TPosition aGlobalPosition,
                TDimensionList& aByDimensionList,
                std::integer_sequence< taT, taIndexes... > )
    {
        ( ( std::get< taIndexes >( aByDimensionList )
                .SetPosition( aGlobalPosition / Devider< taIndexes >::Value( )
                              % Module< taIndexes >::Value( ) ) ),
          ... );
    }

    // Dimension_0::idx * 1
    // + Dimension_1::idx * Dimension_0::kSize
    // + Dimension_2::idx * Dimension_1::kSize * Dimension_0::kSize
    // + ...
    // + Dimension_n::idx * Dimension_n-1::kSize * ... * Dimension_0::kSize
    template < typename taT, taT... taIndex >
    static constexpr TPosition
    ProductImpl( const TDimensionList& aDimensionList, std::integer_sequence< taT, taIndex... > )
    {
        return ( ( std::get< taIndex >( aDimensionList ).GetPosition( )
                   * Multiplier< taIndex >::Value( ) )
                 + ... );
    }
};

template < typename... taDimensions >
static constexpr void
DevideByDimensions( size_t aPosition, taDimensions&&... aByDimensions )
{
    using TStaticDevider = TStaticDevider< typename std::decay< taDimensions >::type&&... >;
    return TStaticDevider::Devide( aPosition, std::forward_as_tuple( aByDimensions... ) );
}

template < typename... taDimensions >
static constexpr inline void
DevideByDimensionsTuple( size_t aPosition, std::tuple< taDimensions... >& aDimensions )
{
    using TStaticDevider = TStaticDevider< taDimensions... >;
    return TStaticDevider::Devide( aPosition, aDimensions );
}

template < typename... taDimensions >
static constexpr auto
DimensiosProduct( const taDimensions&... aDimensions )
{
    using TStaticDevider = TStaticDevider< typename std::decay< taDimensions >::type&&... >;
    return TStaticDevider::Product(
        std::forward_as_tuple( std::forward< taDimensions >( aDimensions )... ) );
}

template < typename... taDimensions >
static constexpr inline auto
DimensiosProductTuple( const std::tuple< taDimensions... >& aDimensions )
{
    using TStaticDevider = TStaticDevider< taDimensions... >;
    return TStaticDevider::Product( aDimensions );
}

/**
 * @param taDimension The dimension type list, ordered from the first iterable dimension to the last
 * one.
 */
template < typename... taDimension >
class TStaticTensorIterator
{
public:
    using TSize = size_t;

    constexpr TStaticTensorIterator( ) = default;

    template < typename... taArgs >
    constexpr TStaticTensorIterator( taArgs&&... aArgs )
        : iDimensionList{ std::forward< taArgs >( aArgs )... }
    {
        static_assert(
            ( ... && std::is_same< typename std::decay< taArgs >::type, taDimension >::value ) );
    }

    static constexpr size_t
    DimentsionCount( )
    {
        return kDimentsionCount;
    }

    /**
     * @brief Returns tensor's cardinality (total number of elements).
     *
     * @return constexpr TSize  Tensor's cardinality.
     */
    static constexpr TSize
    Size( )
    {
        return ( ... * taDimension::Size( ) );
    }

    template < size_t taDimentsionIndex >
    static constexpr TSize
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
    SetPosition( size_t aGlobalPosition )
    {
        DevideByDimensionsTuple( aGlobalPosition, iDimensionList );
    }

    size_t
    GetPosition( ) const
    {
        return DimensiosProductTuple( iDimensionList );
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

    TDimensionList iDimensionList;
};

template < typename... taDimension >
static constexpr auto
MakeStaticTensorIterator( taDimension&&... aDimension )
{
    using TStaticTensorIterator
        = TStaticTensorIterator< typename std::decay< taDimension >::type... >;
    return TStaticTensorIterator{ std::forward< taDimension >( aDimension )... };
}

}  // namespace AbstractPlatform::Tensor