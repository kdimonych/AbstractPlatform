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
    return CheckBit(*iBlockPtr, iRelativeBitIndex);
  }

  inline constexpr TBitRef& operator=(bool aValue) NOEXCEPT
  {
    if (aValue)
    {
      *iBlockPtr = SetBit(*iBlockPtr, iRelativeBitIndex);
    }
    else
    {
      *iBlockPtr = ClearBit(*iBlockPtr, iRelativeBitIndex);
    }
    return *this;
  }

  inline constexpr void Flip() NOEXCEPT
  {
    *this->iBlockPtr = FlipBit(*this->iBlockPtr, iRelativeBitIndex);
  }

  inline constexpr TBitRef& operator^=(bool aValue) NOEXCEPT
  {
    if (aValue)
    {
      *this->iBlockPtr = FlipBit(*this->iBlockPtr, iRelativeBitIndex);
    }
    return *this;
  }

  inline constexpr TBitRef& operator&=(bool aValue) NOEXCEPT
  {
    if (!aValue)
    {
      *this->iBlockPtr = ClearBit(*this->iBlockPtr, iRelativeBitIndex);
    }
    return *this;
  }

  inline constexpr TBitRef& operator|=(bool aValue) NOEXCEPT
  {
    if (aValue)
    {
      *this->iBlockPtr = SetBit(*this->iBlockPtr, iRelativeBitIndex);
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
  using TBlockType    = taBlockType;
  using TBitIndex     = size_t;
  using TBitRef       = TBitRef<TBlockType>;
  using TBufferLayout = TBufferLayout<TBlockType>;

  struct TIterator;
  struct TConstIterator;

  static constexpr Endian kBlockEndian = taBlockEndian;
  static constexpr size_t kBlockSize   = sizeof(TBlockType);
  static constexpr size_t kBitSize     = taBitSize;
  static constexpr size_t kByteSize    = kBitSize / kBitsPerByte;
  static constexpr size_t kBlockBits   = kBlockSize * kBitsPerByte;
  static constexpr size_t kBufferSize  = kBitSize / kBlockBits;

  using TBitIndexMapper = TEndianBitIndexMapper<kBlockSize, taBlockEndian>;

  static_assert(kBitSize <= std::numeric_limits<TBitIndex>::max(),
                "BitBuffer size must be less than or equal to the maximum index");
  static_assert(kBitSize > 0, "BitBuffer size must be greater than zero");
  static_assert(std::is_integral_v<TBlockType>, "TBlockType type must be integral");
  static_assert(kBitSize % kBlockBits == 0,
                "BitBuffer size must be a multiple of block size in bits");

  TBlockType iBuffer[kBufferSize];

  bool Test(size_t aIndex) const NOEXCEPT
  {
    return CheckBit(iBuffer[TBufferLayout::BlockIndex(aIndex)],
                    TBitIndexMapper::MapIndex(TBufferLayout::BlockBitIndex(aIndex)));
  }

  inline constexpr bool operator[](size_t aIndex) const NOEXCEPT
  {
    return Test(aIndex);
  }

  inline constexpr TBitRef operator[](size_t aIndex) NOEXCEPT
  {
    return {&iBuffer[TBufferLayout::BlockIndex(aIndex)],
            TBitIndexMapper::MapIndex(TBufferLayout::BlockBitIndex(aIndex))};
  }

  inline constexpr void SetBit(size_t aIndex) NOEXCEPT
  {
    auto& block = iBuffer[TBufferLayout::BlockIndex(aIndex)];
    block       = SetBit(block, TBitIndexMapper::MapIndex(TBufferLayout::BlockBitIndex(aIndex)));
  }

  inline constexpr void ClearBit(size_t aIndex) NOEXCEPT
  {
    auto& block = iBuffer[TBufferLayout::BlockIndex(aIndex)];
    block       = ClearBit(block, TBitIndexMapper::MapIndex(TBufferLayout::BlockBitIndex(aIndex)));
  }

  inline constexpr void FlipBit(size_t aIndex) NOEXCEPT
  {
    auto& block = iBuffer[TBufferLayout::BlockIndex(aIndex)];
    block       = FlipBit(block, TBitIndexMapper::MapIndex(TBufferLayout::BlockBitIndex(aIndex)));
  }

  /**
   * @brief Set all bits in the buffer to 1.
   *
   */
  inline constexpr void SetAll() NOEXCEPT
  {
    for (size_t i = 0; i < kBufferSize; ++i)
    {
      iBuffer[i] = InvertBits(TBlockType{0});
    }
  }

  /**
   * @brief Clear all bits in the buffer to 0.
   *
   */
  inline constexpr void ClearAll() NOEXCEPT
  {
    for (size_t i = 0; i < kBufferSize; ++i)
    {
      iBuffer[i] = TBlockType{0};
    }
  }

  /**
   * @brief Flip all bits in the buffer.
   *
   */
  inline constexpr void FlipAll() NOEXCEPT
  {
    for (size_t i = 0; i < kBufferSize; ++i)
    {
      iBuffer[i] = InvertBits(iBuffer[i]);
    }
  }

  /**
   * @brief Check if all bits in the buffer are set to 1.
   *
   * @return true if all bits are set, false otherwise.
   */
  inline constexpr bool All() const NOEXCEPT
  {
    for (size_t i = 0; i < kBufferSize; ++i)
    {
      if (iBuffer[i] != InvertBits(TBlockType{0}))
      {
        return false;
      }
    }
    return true;
  }

  /**
   * @brief Check if any bits in the buffer are set to 1.
   *
   * @return true if at least one bit is set, false otherwise.
   */
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

  /**
   * @brief Check if no bits in the buffer are set to 1.
   *
   * @return true if no bits are set, false otherwise.
   */
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

  /**
   * @brief Get the size of the buffer in bits.
   *
   * @return size_t The size of the buffer in bits.
   */
  inline static constexpr size_t Size()
  {
    return kBitSize;
  }

  /**
   * @brief Get an iterator to the beginning of the buffer.
   *
   * @return TIterator An iterator to the beginning of the buffer.
   */
  inline constexpr TIterator begin()
  {
    return TIterator{iBuffer, 0};
  }

  inline constexpr TConstIterator begin() const
  {
    return TConstIterator{iBuffer, size_t{0}};
  }

  inline constexpr TConstIterator cbegin() const
  {
    return TConstIterator{iBuffer, size_t{0}};
  }

  /**
   * @brief Get an iterator to the end of the buffer.
   *
   * @return TIterator An iterator to the end of the buffer.
   */
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

  template <typename taDerivedIterator, typename taItBlockType>
  struct TBaseIterator
  {
    using TBlockType        = taItBlockType;
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

    inline constexpr TDerivedIterator& operator++() NOEXCEPT
    {
      iBlockPtr         = iBufferPtr + TBufferLayout::BlockIndex(++iGlobalBitIndex);
      iRelativeBitIndex = TBitIndexMapper::MapIndex(TBufferLayout::BlockBitIndex(iGlobalBitIndex));
      return DerivedRef();
    }

    inline constexpr TDerivedIterator operator++(int) NOEXCEPT
    {
      TDerivedIterator tmp = DerivedRef();
      ++(*this);
      return tmp;
    }

    inline constexpr TDerivedIterator& operator--() NOEXCEPT
    {
      iBlockPtr         = iBufferPtr + TBufferLayout::BlockIndex(--iGlobalBitIndex);
      iRelativeBitIndex = TBitIndexMapper::MapIndex(TBufferLayout::BlockBitIndex(iGlobalBitIndex));
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

  struct TIterator : public TBaseIterator<TIterator, TBlockType>
  {
    using TBaseIterator     = TBaseIterator<TIterator, TBlockType>;
    using iterator_category = typename TBaseIterator::iterator_category;

    using TBaseIterator::TBaseIterator;

    inline constexpr TBitRef operator*() const NOEXCEPT
    {
      return TBitRef{this->iBlockPtr, this->iRelativeBitIndex};
    }

    /**
     * @brief Set the bit at the current iterator position.
     *
     */
    inline constexpr void Set() NOEXCEPT
    {
      *this->iBlockPtr = SetBit(*this->iBlockPtr, this->iRelativeBitIndex);
    }

    /**
     * @brief Clear the bit at the current iterator position.
     *
     */
    inline constexpr void Clear() NOEXCEPT
    {
      *this->iBlockPtr = ClearBit(*this->iBlockPtr, this->iRelativeBitIndex);
    }

    /**
     * @brief Flip the bit at the current iterator position.
     *
     */
    inline constexpr void Flip() NOEXCEPT
    {
      *this->iBlockPtr = FlipBit(*this->iBlockPtr, this->iRelativeBitIndex);
    }
  };

  struct TConstIterator : public TBaseIterator<TConstIterator, const TBlockType>
  {
    using TBaseIterator     = TBaseIterator<TConstIterator, const TBlockType>;
    using iterator_category = typename TBaseIterator::iterator_category;

    using TBaseIterator::TBaseIterator;

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

    inline constexpr bool operator*() const NOEXCEPT
    {
      return CheckBit(*(this->iBlockPtr), this->iRelativeBitIndex);
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
