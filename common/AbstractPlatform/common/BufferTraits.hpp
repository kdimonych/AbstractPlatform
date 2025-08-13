#pragma once

#include <AbstractPlatform/common/ArrayHelper.hpp>

#include <array>
#include <bitset>
#include <iterator>
#include <type_traits>
#include <vector>

namespace AbstractPlatform {
template <typename taBuffer>
struct TBufferTraits;

// raw array specialization
template <typename taElementType, std::size_t taNm>
struct TBufferTraits<taElementType[taNm]>
{
  using TBuffer        = taElementType[taNm];
  using TValueType     = taElementType;
  using TValueRef      = TValueType&;
  using TValueConstRef = const TValueType&;
  using TIterator      = taElementType*;
  using TConstIterator = const taElementType*;

  inline static constexpr size_t Size(const TBuffer& aBuffer) NOEXCEPT
  {
    return ArrayLength(aBuffer);
  }

  inline static constexpr TValueRef At(TBuffer& aBuffer, size_t aIndex) NOEXCEPT
  {
    return aBuffer[aIndex];
  }

  inline static constexpr TValueConstRef At(const TBuffer& aBuffer, size_t aIndex) NOEXCEPT
  {
    return aBuffer[aIndex];
  }

  inline static constexpr TValueType GetValue(TBuffer& aBuffer, size_t aIndex) NOEXCEPT
  {
    return aBuffer[aIndex];
  }

  inline static constexpr void SetValue(TBuffer& aBuffer, size_t aIndex, TValueType aValue) NOEXCEPT
  {
    aBuffer[aIndex] = aValue;
  }

  inline constexpr void SetAll(TBuffer& aBuffer, TValueType aValue) NOEXCEPT
  {
    for (auto& value : aBuffer)
    {
      value = aValue;
    }
  }

  inline static constexpr TIterator begin(TBuffer& aBuffer) NOEXCEPT
  {
    return std::begin(aBuffer);
  }

  inline static constexpr TConstIterator begin(const TBuffer& aBuffer) NOEXCEPT
  {
    return std::begin(aBuffer);
  }

  inline static constexpr TConstIterator cbegin(const TBuffer& aBuffer) NOEXCEPT
  {
    return std::cbegin(aBuffer);
  }

  inline static constexpr TIterator end(TBuffer& aBuffer) NOEXCEPT
  {
    return std::end(aBuffer);
  }

  inline static constexpr TConstIterator end(const TBuffer& aBuffer) NOEXCEPT
  {
    return std::end(aBuffer);
  }

  inline static constexpr TConstIterator cend(const TBuffer& aBuffer) NOEXCEPT
  {
    return std::cend(aBuffer);
  }
};

// raw matrix specialization
template <typename taElementType, std::size_t taRows, std::size_t taCols>
struct TBufferTraits<taElementType[taRows][taCols]>
{
  using TBuffer        = taElementType[taRows][taCols];
  using TValueType     = taElementType;
  using TValueRef      = TValueType&;
  using TValueConstRef = const TValueType&;
  using TIterator      = taElementType*;
  using TConstIterator = const taElementType*;

  inline static constexpr size_t Size(const TBuffer& aBuffer) NOEXCEPT
  {
    return ArrayLength(aBuffer);
  }

  inline static constexpr TValueRef At(TBuffer& aBuffer, size_t aIndex) NOEXCEPT
  {
    return aBuffer[aIndex];
  }

  inline static constexpr TValueConstRef At(const TBuffer& aBuffer, size_t aIndex) NOEXCEPT
  {
    return aBuffer[aIndex];
  }

  inline static constexpr TValueType GetValue(TBuffer& aBuffer, size_t aIndex) NOEXCEPT
  {
    return aBuffer[aIndex];
  }

  inline static constexpr void SetValue(TBuffer& aBuffer, size_t aIndex, TValueType aValue) NOEXCEPT
  {
    aBuffer[aIndex] = aValue;
  }

  inline constexpr void SetAll(TBuffer& aBuffer, TValueType aValue) NOEXCEPT
  {
    for (auto& value : aBuffer)
    {
      value = aValue;
    }
  }

  inline static constexpr TIterator begin(TBuffer& aBuffer) NOEXCEPT
  {
    return std::begin(*std::begin(aBuffer));
  }

  inline static constexpr TConstIterator begin(const TBuffer& aBuffer) NOEXCEPT
  {
    return std::cbegin(*std::cbegin(aBuffer));
  }

  inline static constexpr TConstIterator cbegin(const TBuffer& aBuffer) NOEXCEPT
  {
    return std::cbegin(*std::cbegin(aBuffer));
  }

  inline static constexpr TIterator end(TBuffer& aBuffer) NOEXCEPT
  {
    return begin(aBuffer) + (taRows * taCols);
  }

  inline static constexpr TConstIterator end(const TBuffer& aBuffer) NOEXCEPT
  {
    return begin(aBuffer) + (taRows * taCols);
  }

  inline static constexpr TConstIterator cend(const TBuffer& aBuffer) NOEXCEPT
  {
    return begin(aBuffer) + (taRows * taCols);
  }
};

// std::array specialization
template <typename taElementType, std::size_t taNm>
struct TBufferTraits<std::array<taElementType, taNm>>
{
  using TBuffer        = std::array<taElementType, taNm>;
  using TValueType     = typename TBuffer::value_type;
  using TValueRef      = TValueType&;
  using TValueConstRef = const TValueType&;
  using TIterator      = typename TBuffer::iterator;
  using TConstIterator = typename TBuffer::const_iterator;

  inline static constexpr size_t Size(const TBuffer& aBuffer) NOEXCEPT
  {
    return ArrayLength(aBuffer);
  }

  inline static constexpr TValueRef At(TBuffer& aBuffer, size_t aIndex) NOEXCEPT
  {
    return aBuffer[aIndex];
  }

  inline static constexpr TValueConstRef At(const TBuffer& aBuffer, size_t aIndex) NOEXCEPT
  {
    return aBuffer[aIndex];
  }

  inline static constexpr TValueType GetValue(TBuffer& aBuffer, size_t aIndex) NOEXCEPT
  {
    return aBuffer[aIndex];
  }

  inline static constexpr void SetValue(TBuffer& aBuffer, size_t aIndex, TValueType aValue) NOEXCEPT
  {
    aBuffer[aIndex] = aValue;
  }

  inline constexpr void SetAll(TBuffer& aBuffer, TValueType aValue) NOEXCEPT
  {
    for (auto& value : aBuffer)
    {
      value = aValue;
    }
  }

  inline static constexpr TIterator begin(TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.begin();
  }

  inline static constexpr TConstIterator begin(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.cbegin();
  }

  inline static constexpr TConstIterator cbegin(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.cbegin();
  }

  inline static constexpr TIterator end(TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.end();
  }

  inline static constexpr TConstIterator end(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.cend();
  }

  inline static constexpr TConstIterator cend(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.cend();
  }
};

// std::vector specialization
template <typename taElementType, typename taAllocator>
struct TBufferTraits<std::vector<taElementType, taAllocator>>
{
  using TBuffer        = std::vector<taElementType, taAllocator>;
  using TValueType     = typename TBuffer::value_type;
  using TValueRef      = TValueType&;
  using TValueConstRef = const TValueType&;
  using TIterator      = typename TBuffer::iterator;
  using TConstIterator = typename TBuffer::const_iterator;

  inline static constexpr size_t Size(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.size();
  }

  inline static constexpr TValueRef At(TBuffer& aBuffer, size_t aIndex) NOEXCEPT
  {
    return aBuffer[aIndex];
  }

  inline static constexpr TValueConstRef At(const TBuffer& aBuffer, size_t aIndex) NOEXCEPT
  {
    return aBuffer[aIndex];
  }

  inline static constexpr TValueType GetValue(TBuffer& aBuffer, size_t aIndex) NOEXCEPT
  {
    return aBuffer[aIndex];
  }

  inline static constexpr void SetValue(TBuffer& aBuffer, size_t aIndex, TValueType aValue) NOEXCEPT
  {
    aBuffer[aIndex] = aValue;
  }

  inline constexpr void SetAll(TBuffer& aBuffer, TValueType aValue) NOEXCEPT
  {
    for (auto& value : aBuffer)
    {
      value = aValue;
    }
  }

  inline static constexpr TIterator begin(TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.begin();
  }

  inline static constexpr TConstIterator begin(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.cbegin();
  }

  inline static constexpr TConstIterator cbegin(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.cbegin();
  }

  inline static constexpr TIterator end(TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.end();
  }

  inline static constexpr TConstIterator end(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.cend();
  }

  inline static constexpr TConstIterator cend(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.cend();
  }
};

} // namespace AbstractPlatform
