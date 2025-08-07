#pragma once
#include <AbstractPlatform/common/ArrayHelper.hpp>
#include <AbstractPlatform/common/BinaryOperations.hpp>
#include <AbstractPlatform/common/BitOperations.hpp>
#include <AbstractPlatform/common/BufferTraits.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <cassert>
#include <cstddef>
#include <tuple>
#include <type_traits>

namespace AbstractPlatform {

template <typename taBlockType>
struct TBufferLayout
{
  using TBlockType                   = taBlockType;
  static constexpr size_t kBlockSize = sizeof(TBlockType);
  static constexpr size_t kBlockBits = kBlockSize * kBitsPerByte;

  static_assert(std::is_integral_v<TBlockType>, "TBlockType must be an integral type");
  static_assert(sizeof(TBlockType) > 0, "TBlockType must be a non-empty type");

  /**
   * @brief Gets the a bit shift within the block for the global bit index.
   * It assumes that blocks are contiguous in memory and aligned from least significant bit (LSB) to
   * most significant bit (MSB).
   *  The Big Endian layout is as follows:
   *  aGlobalBitIndex -|
   *                   V
   * Bits:   | 0...15|16...31|32...       | GlobalBitSize
   * Bytes:  | 0 | 1 | 2 | 3 | 4 |...     | kBlockSize * sizeof(TBlockType) - 1 |
   * Blocks: |   0   |   2   |     ...    | kBlockSize - 1 |
   *
   *  The Little Endian layout is as follows:
   *  aGlobalBitIndex  -----|
   *                        V
   * Bits:   |15...0 |31...16|47...       | GlobalBitSize
   * Bytes:  | 1 | 0 | 3 | 2 | 5 |...     | kBlockSize * sizeof(TBlockType) - 1 |
   * Blocks: |   0   |   2   |     ...    | kBlockSize - 1 |
   * @param aGlobalBitIndex The global bit index to calculate the shift for.
   * @return constexpr size_t The bit shift within the block for the global bit index.
   */
  inline static constexpr size_t BlockBitIndex(size_t aGlobalBitIndex)
  {
    return aGlobalBitIndex % kBlockBits;
  }

  inline static constexpr size_t Inc(size_t& aInBlockBitIndex) NOEXCEPT
  {
    ++aInBlockBitIndex %= kBlockBits;
    // Move to next block if needed
    return (aInBlockBitIndex == 0) ? 1 : 0;
  }

  inline static constexpr size_t Dec(size_t& aInBlockBitIndex) NOEXCEPT
  {
    --aInBlockBitIndex %= kBlockBits;
    // Move to previous block if needed
    return (aInBlockBitIndex == kBlockBits - 1) ? 1 : 0;
  }

  inline static std::ptrdiff_t advance(size_t& aInBlockBitIndex, std::ptrdiff_t aBitCount) NOEXCEPT
  {
    aBitCount += static_cast<std::ptrdiff_t>(aInBlockBitIndex);
    const auto correction = (aBitCount % static_cast<std::ptrdiff_t>(kBlockBits)) < 0 ? 1 : 0;
    aInBlockBitIndex      = static_cast<size_t>(aBitCount) % kBlockBits;
    return aBitCount / static_cast<std::ptrdiff_t>(kBlockBits) - correction;
  }

  inline static constexpr size_t BlockIndex(size_t aGlobalBitIndex)
  {
    return aGlobalBitIndex / kBlockBits;
  }
};

template <typename taBlockType>
struct TBitRef
{
  using TBlockType = taBlockType;
  TBlockType* const iBlockPtr;
  const size_t      iRelativeBitIndex;

  inline constexpr operator bool() const NOEXCEPT
  {
    return (*iBlockPtr >> iRelativeBitIndex) & TBlockType{1};
  }

  inline constexpr TBitRef& operator=(bool aValue) NOEXCEPT
  {
    if (aValue)
    {
      *iBlockPtr |= (TBlockType{1} << iRelativeBitIndex);
    }
    else
    {
      *iBlockPtr &= ~(TBlockType{1} << iRelativeBitIndex);
    }
    return *this;
  }

  inline constexpr void Flip() NOEXCEPT
  {
    *this->iBlockPtr ^= (TBlockType{1} << this->iRelativeBitIndex);
  }

  inline constexpr TBitRef& operator^=(bool aValue) NOEXCEPT
  {
    if (aValue)
    {
      *this->iBlockPtr ^= (TBlockType{1} << this->iRelativeBitIndex);
    }
    return *this;
  }

  inline constexpr TBitRef& operator&=(bool aValue) NOEXCEPT
  {
    if (!aValue)
    {
      *this->iBlockPtr &= ~(TBlockType{1} << this->iRelativeBitIndex);
    }
    return *this;
  }

  inline constexpr TBitRef& operator|=(bool aValue) NOEXCEPT
  {
    if (aValue)
    {
      *this->iBlockPtr |= (TBlockType{1} << this->iRelativeBitIndex);
    }
    return *this;
  }
};

template <typename taBlockType>
struct TBitRef<const taBlockType>
{
  using TBlockType = taBlockType;
  const TBlockType* const iBlockPtr;
  const size_t            iRelativeBitIndex;

  inline constexpr operator bool() const NOEXCEPT
  {
    return (*iBlockPtr >> iRelativeBitIndex) & TBlockType{1};
  }
};

template <size_t taBitSize,
          typename taBlockType = std::uint8_t,
          Endian taBlockEndian = Endian::Native>
struct TBitBuffer
{
  using TBlockType                     = taBlockType;
  using TBitIndex                      = size_t;
  static constexpr Endian kBlockEndian = taBlockEndian;
  using TConstBitRef                   = TBitRef<const TBlockType>;
  using TBitRef                        = TBitRef<TBlockType>;

  using TBufferLayout = TBufferLayout<TBlockType>;

  struct TIterator;
  struct TConstIterator;

  static constexpr size_t kBlockSize  = sizeof(TBlockType);
  static constexpr size_t kBitSize    = taBitSize;
  static constexpr size_t kByteSize   = kBitSize / kBitsPerByte;
  static constexpr size_t kBlockBits  = kBlockSize * kBitsPerByte;
  static constexpr size_t kBufferSize = kBitSize / kBlockBits;

  using TBitIndexMapper = TEndianBitIndexMapper<kBlockSize, taBlockEndian>;

  static_assert(kBitSize <= std::numeric_limits<TBitIndex>::max(),
                "BitBuffer size must be less than or equal to the maximum index");
  static_assert(kBitSize > 0, "BitBuffer size must be greater than zero");
  static_assert(std::is_integral_v<TBlockType>, "TBlockType type must be integral");
  static_assert(kBitSize % kBlockBits == 0,
                "BitBuffer size must be a multiple of block size in bits");

  TBlockType iBuffer[kBufferSize];

  inline constexpr bool operator[](size_t aIndex) const NOEXCEPT
  {
    return iBuffer[TBufferLayout::BlockIndex(aIndex)]
             >> TBitIndexMapper::MapIndex(TBufferLayout::BlockBitIndex(aIndex))
           & TBlockType{1};
  }

  inline constexpr TBitRef operator[](size_t aIndex) NOEXCEPT
  {
    return {&iBuffer[TBufferLayout::BlockIndex(aIndex)],
            TBitIndexMapper::MapIndex(TBufferLayout::BlockBitIndex(aIndex))};
  }

  inline constexpr void Set() NOEXCEPT
  {
    for (size_t i = 0; i < kBufferSize; ++i)
    {
      iBuffer[i] = ~TBlockType{0};
    }
  }

  inline constexpr void Reset() NOEXCEPT
  {
    for (size_t i = 0; i < kBufferSize; ++i)
    {
      iBuffer[i] = TBlockType{0};
    }
  }

  inline constexpr void Flip() NOEXCEPT
  {
    for (size_t i = 0; i < kBufferSize; ++i)
    {
      iBuffer[i] = ~iBuffer[i];
    }
  }

  inline constexpr bool All() const NOEXCEPT
  {
    for (size_t i = 0; i < kBufferSize; ++i)
    {
      if (iBuffer[i] != ~TBlockType{0})
      {
        return false;
      }
    }
    return true;
  }

  inline constexpr bool Any() const NOEXCEPT
  {
    for (size_t i = 0; i < kBufferSize; ++i)
    {
      if (iBuffer[i] != TBlockType{0})
      {
        return true;
      }
    }
    return false;
  }

  inline constexpr bool None() const NOEXCEPT
  {
    for (size_t i = 0; i < kBufferSize; ++i)
    {
      if (iBuffer[i] != TBlockType{0})
      {
        return false;
      }
    }
    return true;
  }

  inline constexpr TIterator begin()
  {
    return TIterator{iBuffer, 0};
  }

  inline constexpr TConstIterator begin() const
  {
    return TConstIterator{iBuffer, 0};
  }

  inline constexpr TConstIterator cbegin() const
  {
    return TConstIterator{iBuffer, 0};
  }

  inline constexpr TIterator end()
  {
    return TIterator{iBuffer, kBitSize};
  }

  inline constexpr TConstIterator end() const
  {
    return TConstIterator{iBuffer, kBitSize};
  }

  inline constexpr TConstIterator cend() const
  {
    return TConstIterator{iBuffer, kBitSize};
  }

  template <typename taDerivedIterator>
  struct TBaseIterator
  {
    using iterator_category = std::random_access_iterator_tag;
    using TDerivedIterator  = taDerivedIterator;

    inline constexpr TDerivedIterator& DerivedRef() NOEXCEPT
    {
      return static_cast<TDerivedIterator&>(*this);
    }

    inline constexpr const TDerivedIterator& DerivedRef() const NOEXCEPT
    {
      return static_cast<const TDerivedIterator&>(*this);
    }

    inline constexpr TBaseIterator(TBlockType* aBufferPtr, TBitIndex aGlobalBitIndex) NOEXCEPT
      : iBufferPtr{aBufferPtr},
        iBlockPtr{aBufferPtr + TBufferLayout::BlockIndex(aGlobalBitIndex)},
        iGlobalBitIndex{aGlobalBitIndex},
        iRelativeBitIndex{TBitIndexMapper::MapIndex(TBufferLayout::BlockBitIndex(aGlobalBitIndex))}
    {
      assert(iBufferPtr != nullptr);
    }

    inline constexpr bool Get() const NOEXCEPT
    {
      return (*iBlockPtr >> iRelativeBitIndex) & TBlockType{1};
    }

    inline constexpr TConstBitRef operator*() const NOEXCEPT
    {
      return TConstBitRef{iBlockPtr, iRelativeBitIndex};
    }

    inline constexpr TDerivedIterator& operator++() NOEXCEPT
    {
      iBlockPtr = iBufferPtr + TBufferLayout::BlockIndex(++iGlobalBitIndex);
      return *this;
    }

    inline constexpr TDerivedIterator operator++(int) NOEXCEPT
    {
      TDerivedIterator tmp = DerivedRef();
      ++(*this);
      return tmp;
    }

    inline constexpr TDerivedIterator& operator--() NOEXCEPT
    {
      iBlockPtr = iBufferPtr + TBufferLayout::BlockIndex(--iGlobalBitIndex);
      return DerivedRef();
    }

    inline constexpr TDerivedIterator& operator+=(std::ptrdiff_t aOffset) const NOEXCEPT
    {
      iGlobalBitIndex += aOffset;
      iBlockPtr         = iBufferPtr + TBufferLayout::BlockIndex(iGlobalBitIndex);
      iRelativeBitIndex = TBitIndexMapper::MapIndex(TBufferLayout::BlockBitIndex(iGlobalBitIndex));
      return DerivedRef();
    }

    inline constexpr TDerivedIterator& operator-=(std::ptrdiff_t aOffset) const NOEXCEPT
    {
      iGlobalBitIndex -= aOffset;
      iBlockPtr         = iBufferPtr + TBufferLayout::BlockIndex(iGlobalBitIndex);
      iRelativeBitIndex = TBitIndexMapper::MapIndex(TBufferLayout::BlockBitIndex(iGlobalBitIndex));
      return DerivedRef();
    }

    inline constexpr TDerivedIterator operator--(int) NOEXCEPT
    {
      TDerivedIterator tmp = DerivedRef();
      --(*this);
      return tmp;
    }

    bool operator==(const TBaseIterator& aOther) const NOEXCEPT
    {
      return std::tie(iGlobalBitIndex, iBlockPtr, iRelativeBitIndex)
             == std::tie(aOther.iGlobalBitIndex, aOther.iBlockPtr, aOther.iRelativeBitIndex);
    }

    bool operator!=(const TBaseIterator& aOther) const NOEXCEPT
    {
      return std::tie(iGlobalBitIndex, iBlockPtr, iRelativeBitIndex)
             != std::tie(aOther.iGlobalBitIndex, aOther.iBlockPtr, aOther.iRelativeBitIndex);
    }

    TBlockType* iBufferPtr;
    // Cached pointer to the current block
    TBlockType* iBlockPtr;

    TBitIndex iGlobalBitIndex;
    // Cached in-block bit index
    TBitIndex iRelativeBitIndex;
  };

  struct TIterator : public TBaseIterator<TIterator>
  {
    using TBaseIterator     = TBaseIterator<TIterator>;
    using iterator_category = typename TBaseIterator::random_access_iterator_tag;

    using TBaseIterator::TBaseIterator;
    using TBaseIterator::operator*;

    inline constexpr void Set(bool aValue) NOEXCEPT
    {
      if (aValue)
      {
        *this->iBlockPtr |= (TBlockType{1} << this->iRelativeBitIndex);
      }
      else
      {
        *this->iBlockPtr &= ~(TBlockType{1} << this->iRelativeBitIndex);
      }
    }

    inline constexpr TBitRef operator*() NOEXCEPT
    {
      return TBitRef{this->iBlockPtr, this->iRelativeBitIndex};
    }

    inline constexpr void Flip() NOEXCEPT
    {
      *this->iBlockPtr ^= (TBlockType{1} << this->iRelativeBitIndex);
    }
  };

  struct TConstIterator : public TBaseIterator<TIterator>
  {
    using TBaseIterator     = TBaseIterator<TIterator>;
    using iterator_category = typename TBaseIterator::random_access_iterator_tag;

    using TBaseIterator::TBaseIterator;
    using TBaseIterator::operator*;

    inline constexpr TConstIterator(const TConstIterator&)            = default;
    inline constexpr TConstIterator(TConstIterator&&)                 = default;
    inline constexpr TConstIterator& operator=(const TConstIterator&) = default;
    inline constexpr TConstIterator& operator=(TConstIterator&&)      = default;

    inline constexpr TConstIterator(const TIterator& aOther)
      : TBaseIterator(aOther) { };
    inline constexpr TConstIterator(TIterator&& aOther)
      : TBaseIterator(std::move(aOther)) { };

    inline constexpr TConstIterator& operator=(const TIterator& aOther)
    {
      *this = TConstIterator(aOther);
      return *this;
    }

    inline constexpr TConstIterator& operator=(TIterator&& aOther)
    {
      *this = TConstIterator(std::move(aOther));
      return *this;
    }
  };
};

template <size_t kBitSize, typename taBlockType, Endian taBlockEndian>
inline constexpr size_t ArrayLength(const TBitBuffer<kBitSize, taBlockType, taBlockEndian>&)
{
  return TBitBuffer<kBitSize, taBlockType, taBlockEndian>::kBitSize;
}

template <size_t kBitSize, typename taBlockType, Endian taBlockEndian>
inline constexpr size_t ArraySizeBytes(const TBitBuffer<kBitSize, taBlockType, taBlockEndian>&)
{
  return TBitBuffer<kBitSize, taBlockType, taBlockEndian>::kByteSize;
}

template <size_t kBitSize, typename taBlockType, Endian taBlockEndian>
inline constexpr size_t BitSize(const TBitBuffer<kBitSize, taBlockType, taBlockEndian>& aArray)
{
  return TBitBuffer<kBitSize, taBlockType, taBlockEndian>::kBitSize;
}

} // namespace AbstractPlatform
