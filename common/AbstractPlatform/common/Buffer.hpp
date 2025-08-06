#pragma once
#include <AbstractPlatform/common/ArrayHelper.hpp>
#include <AbstractPlatform/common/BufferTraits.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <functional>
#include <initializer_list>

namespace AbstractPlatform {

template <typename taValueType>
struct TRPtr
{
  using TValueType = taValueType;
  const TValueType* const iDataPtr; // Pointer to the data (const).

  inline constexpr const TValueType* const get() const
  {
    return iDataPtr;
  }
};

template <typename taValueType>
struct TRWPtr
{
  using TValueType = taValueType;
  TValueType* const iDataPtr; // Pointer to the data.

  inline constexpr TValueType* const get() const
  {
    return iDataPtr;
  }
};

template <typename taValueType>
struct TBufferDataRef
{
  using TRPtr  = TRPtr<taValueType>;
  using TRWPtr = TRWPtr<taValueType>;

  union
  {
    TRPtr  iDataRPtr;  // Pointer to the data.
    TRWPtr iDataRWPtr; // Pointer to the data (const).
    static_assert(sizeof(TRPtr) == sizeof(TRWPtr), "DataRef size mismatch");
  };

  size_t iSize; // Size of the data.
};

// Forward declarations
template <typename taValueType, size_t taAlignment>
class TRWBuffer;
template <typename taValueType, size_t taSize, size_t taAlignment>
class TStackBuffer;
template <typename taValueType, size_t taAlignment>
class TBufferView;
template <typename taValueType, size_t taAlignment>
class THeapBuffer;

template <typename taValueType, size_t taAlignment = AbstractPlatform::kWordAlignment>
class TRBuffer
{
public:
  using TValueType                   = taValueType;
  using TConstIterator               = const TValueType*;
  static constexpr size_t kAlignment = taAlignment;

  /**
   * @brief Get the RAW buffer pointer.
   *
   * @return constexpr const TValueType*
   */
  inline constexpr const TValueType* GetBuffer() const NOEXCEPT
  {
    return iDataRef.iDataRPtr.get();
  }

  /**
   * @brief Returns the size of the buffer in elements.
   *
   * @return constexpr size_t
   */
  inline constexpr size_t Size() const NOEXCEPT
  {
    return iDataRef.iSize;
  }

  inline constexpr TConstIterator begin() const NOEXCEPT
  {
    return GetBuffer();
  }

  inline constexpr TConstIterator end() const NOEXCEPT
  {
    return GetBuffer() + Size();
  }

  inline constexpr TConstIterator cbegin() const NOEXCEPT
  {
    return GetBuffer();
  }

  inline constexpr TConstIterator cend() const NOEXCEPT
  {
    return GetBuffer() + Size();
  }

  const TValueType& operator[](size_t index) const NOEXCEPT
  {
    assert(index < Size() && "Index out of bounds");
    return GetBuffer()[index];
  }

private:
  friend class TRWBuffer<TValueType, kAlignment>;
  template <typename taTBData, size_t taTBSize, size_t taTBAlignment>
  friend class TStackBuffer;
  friend class TBufferView<TValueType, kAlignment>;
  friend class THeapBuffer<TValueType, kAlignment>;

  inline constexpr TRBuffer(TBufferDataRef<TValueType> aDataRef) NOEXCEPT : iDataRef{aDataRef} { }

  // The pointer is not constant to allow modification from derived classes.
  TBufferDataRef<TValueType> iDataRef; // Pointer to the buffer.
};

template <typename taValueTypeType, size_t taAlignment>
inline constexpr size_t ArrayLength(const TRBuffer<taValueTypeType, taAlignment>& aBuffer) NOEXCEPT
{
  return aBuffer.Size();
}

template <typename taValueTypeType, size_t taAlignment>
inline constexpr size_t
ArraySizeBytes(const TRBuffer<taValueTypeType, taAlignment>& aBuffer) NOEXCEPT
{
  return aBuffer.Size() * sizeof(taValueTypeType);
}

template <typename taValueType, size_t taAlignment>
struct TBufferTraits<TRBuffer<taValueType, taAlignment>>
{
  using TBuffer        = TRBuffer<taValueType, taAlignment>;
  using TValueType     = typename TBuffer::TValueType;
  using TConstIterator = typename TBuffer::TConstIterator;

  inline static constexpr size_t Size(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.Size();
  }

  inline static constexpr TConstIterator begin(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.cbegin();
  }

  inline static constexpr TConstIterator cbegin(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.cbegin();
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

template <typename taValueType, size_t taAlignment = AbstractPlatform::kNoAlignment>
class TRWBuffer : public TRBuffer<taValueType, taAlignment>
{
public:
  using TRBuffer       = TRBuffer<taValueType, taAlignment>;
  using TValueType     = typename TRBuffer::TValueType;
  using TIterator      = TValueType*;
  using TConstIterator = typename TRBuffer::TConstIterator;

  using TRBuffer::begin;
  using TRBuffer::end;
  using TRBuffer::GetBuffer;
  using TRBuffer::kAlignment;
  using TRBuffer::Size;

  inline TValueType* GetBuffer() NOEXCEPT
  {
    return this->iDataRef.iDataRWPtr.get();
  }

  inline TIterator begin() NOEXCEPT
  {
    return this->GetBuffer();
  }

  inline TIterator end() NOEXCEPT
  {
    return this->GetBuffer() + this->Size();
  }

  TValueType& operator[](size_t index) NOEXCEPT
  {
    assert(index < this->Size() && "Index out of bounds");
    return GetBuffer()[index];
  }

private:
  template <typename taTBData, size_t taTBSize, size_t taTBAlignment>
  friend class TStackBuffer;
  friend class TBufferView<TValueType, kAlignment>;
  friend class THeapBuffer<TValueType, kAlignment>;

  using TRBuffer::TRBuffer;
};

template <typename taValueType, size_t taAlignment>
struct TBufferTraits<TRWBuffer<taValueType, taAlignment>>
{
  using TBuffer        = TRWBuffer<taValueType, taAlignment>;
  using TValueType     = typename TBuffer::TValueType;
  using TIterator      = typename TBuffer::TIterator;
  using TConstIterator = typename TBuffer::TConstIterator;

  inline static constexpr size_t Size(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.size();
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

template <typename taValueType, size_t taSize, size_t taAlignment = AbstractPlatform::kNoAlignment>
class TStackBuffer : public TRWBuffer<taValueType, taAlignment>
{
public:
  static_assert(taSize > 0, "Buffer size must be greater than zero");
  using TRWBuffer      = TRWBuffer<taValueType, taAlignment>;
  using TValueType     = typename TRWBuffer::TValueType;
  using TIterator      = typename TRWBuffer::TIterator;
  using TConstIterator = typename TRWBuffer::TConstIterator;

  static constexpr size_t kSize = taSize;
  using TRWBuffer::kAlignment;

  template <typename taInitFunction>
  inline constexpr TStackBuffer(taInitFunction aInitFunction) NOEXCEPT
    : TRWBuffer{TBufferDataRef{this->iAligned.iBuffer, kSize}}
  {
    for (auto& item : this->iAligned.iBuffer)
    {
      item = aInitFunction();
    }
  }

  inline constexpr TStackBuffer(size_t aInitValue = size_t{}) NOEXCEPT
    : TRWBuffer{TBufferDataRef{this->iAligned.iBuffer, kSize}}
  {
    std::memset(this->iAligned.iBuffer, aInitValue, sizeof(this->iAligned.iBuffer));
  }

  TStackBuffer(std::initializer_list<TValueType> aInitList) NOEXCEPT
    : TRWBuffer{TBufferDataRef{this->iAligned.iBuffer, kSize}}
  {
    assert(aInitList.size() == kSize && "Initializer list size must match buffer size");
    // Copy the initializer list into the buffer.
    std::copy(aInitList.begin(), aInitList.end(), this->iAligned.iBuffer);
  }

  // This is on stack buffers, so we do not allow copying or moving.
  TStackBuffer(const TStackBuffer& aOther)            = delete;
  TStackBuffer& operator=(const TStackBuffer& aOther) = delete;
  TStackBuffer(TStackBuffer&&)                        = delete;
  TStackBuffer& operator=(TStackBuffer&&)             = delete;

private:
  template <size_t taTBAlignment>
  struct TAlignmentHelper
  {
    alignas(taTBAlignment) TValueType iBuffer[kSize];
  };

  template <>
  struct TAlignmentHelper<AbstractPlatform::kNoAlignment>
  {
    TValueType iBuffer[kSize];
  };

  // set alignment to the platform word size
  TAlignmentHelper<taAlignment> iAligned;
  using TBufferDataRef = TBufferDataRef<TValueType>;
};

template <typename taValueTypeType, size_t taSize, size_t taAlignment>
inline constexpr size_t ArrayLength(const TStackBuffer<taValueTypeType, taSize, taAlignment>&)
{
  return taSize;
}

template <typename taValueTypeType, size_t taSize, size_t taAlignment>
inline constexpr size_t ArraySizeBytes(const TStackBuffer<taValueTypeType, taSize, taAlignment>&)
{
  return taSize * sizeof(taValueTypeType);
}

template <typename taValueType, size_t taSize, size_t taAlignment>
struct TBufferTraits<TStackBuffer<taValueType, taSize, taAlignment>>
{
  using TBuffer        = TStackBuffer<taValueType, taSize, taAlignment>;
  using TValueType     = typename TBuffer::TValueType;
  using TIterator      = typename TBuffer::TIterator;
  using TConstIterator = typename TBuffer::TConstIterator;

  inline static constexpr size_t Size(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.Size();
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

template <typename taValueType, size_t taAlignment = AbstractPlatform::kNoAlignment>
class TBufferView : public TRWBuffer<taValueType, taAlignment>
{
public:
  using TRWBuffer      = TRWBuffer<taValueType, taAlignment>;
  using TValueType     = typename TRWBuffer::TValueType;
  using TIterator      = typename TRWBuffer::TIterator;
  using TConstIterator = typename TRWBuffer::TConstIterator;
  using TRWBuffer::kAlignment;

  // To prevent double modification of the buffer, we remove the copy constructor and assignment
  // operator.
  TBufferView(TBufferView&&)            = default;
  TBufferView& operator=(TBufferView&&) = default;

  inline constexpr TBufferView(TValueType* const aBufferPtr, size_t aSize)
    : TRWBuffer{TBufferDataRef{aBufferPtr, aSize}}
  {
    // Check if the buffer is aligned to the specified alignment.
    if constexpr (AbstractPlatform::IsAlignmentAware(kAlignment))
    {
      assert(AbstractPlatform::IsAligned(aBufferPtr, kAlignment));
    }
  }

  ~TBufferView() = default;

private:
  using TBufferDataRef = TBufferDataRef<TValueType>;
};

template <typename taValueType, size_t taAlignment>
struct TBufferTraits<TBufferView<taValueType, taAlignment>>
{
  using TBuffer        = TBufferView<taValueType, taAlignment>;
  using TValueType     = typename TBuffer::TValueType;
  using TIterator      = typename TBuffer::TIterator;
  using TConstIterator = typename TBuffer::TConstIterator;

  inline static constexpr size_t Size(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.Size();
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

// Const specialization of TBufferView
template <typename taValueType, size_t taAlignment>
class TBufferView<const taValueType, taAlignment> : public TRBuffer<const taValueType, taAlignment>
{
public:
  using TRBuffer       = TRBuffer<const taValueType, taAlignment>;
  using TValueType     = typename TRBuffer::TValueType;
  using TConstIterator = typename TRBuffer::TConstIterator;
  using TRBuffer::kAlignment;

  inline constexpr TBufferView(const TValueType* const aBufferPtr, size_t aSize)
    : TRBuffer{TBufferDataRef{aBufferPtr, aSize}}
  {
    // Check if the buffer is aligned to the specified alignment.
    if constexpr (AbstractPlatform::IsAlignmentAware(kAlignment))
    {
      assert(AbstractPlatform::IsAligned(aBufferPtr, kAlignment));
    }
  }

  ~TBufferView() = default;

private:
  using TBufferDataRef = TBufferDataRef<TValueType>;
};

template <typename taValueType, size_t taAlignment>
struct TBufferTraits<TBufferView<const taValueType, taAlignment>>
{
  using TBuffer        = TBufferView<const taValueType, taAlignment>;
  using TValueType     = typename TBuffer::TValueType;
  using TConstIterator = typename TBuffer::TConstIterator;

  inline static constexpr size_t Size(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.Size();
  }

  inline static constexpr TConstIterator begin(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.cbegin();
  }

  inline static constexpr TConstIterator cbegin(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.cbegin();
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

template <typename taValueType, size_t taAlignment = AbstractPlatform::kNoAlignment>
class THeapBuffer : public TRWBuffer<taValueType, taAlignment>
{
public:
  using TRWBuffer      = TRWBuffer<taValueType, taAlignment>;
  using TValueType     = typename TRWBuffer::TValueType;
  using TIterator      = typename TRWBuffer::TIterator;
  using TConstIterator = typename TRWBuffer::TConstIterator;
  using TRWBuffer::kAlignment;
  using AlignedDeleter    = std::function<void(TValueType*)>;
  using TAlignedUniquePtr = std::unique_ptr<TValueType[], AlignedDeleter>;

  THeapBuffer(size_t aSize) NOEXCEPT
    : TRWBuffer{TBufferDataRef{
        static_cast<TValueType*>(::operator new[](sizeof(TValueType) * aSize,
                                                  std::align_val_t(kAlignment))),
        aSize}},
      iBuffer{}
  {
    assert(AbstractPlatform::IsAligned(this->GetBuffer(), kAlignment));
    iBuffer = std::move(std::unique_ptr<TValueType[], AlignedDeleter>{
      this->GetBuffer(),
      [](TValueType* ptr) NOEXCEPT { ::operator delete[](ptr, std::align_val_t(kAlignment)); }});
  }

  inline constexpr THeapBuffer(TAlignedUniquePtr aBuffer, size_t aSize) NOEXCEPT
    : TRWBuffer{TBufferDataRef{aBuffer.get(), aSize}},
      iBuffer{std::move(aBuffer)}
  {
    // Check if the buffer is aligned to the specified alignment.
    assert(AbstractPlatform::IsAligned(this->GetBuffer(), kAlignment));
  }

private:
  // set alignment to the platform word size
  TAlignedUniquePtr iBuffer;
  using TBufferDataRef = TBufferDataRef<TValueType>;
};

template <typename taValueType>
class THeapBuffer<taValueType, AbstractPlatform::kNoAlignment>
  : public TRWBuffer<taValueType, AbstractPlatform::kNoAlignment>
{
public:
  using TBuffer    = TRWBuffer<taValueType, AbstractPlatform::kNoAlignment>;
  using TValueType = typename TBuffer::TValueType;

  THeapBuffer(size_t aSize) NOEXCEPT : TBuffer{TBufferDataRef{new TValueType[aSize], aSize}},
                                       iBuffer{std::unique_ptr<TValueType[]>{this->GetBuffer()}}
  {
  }

  inline constexpr THeapBuffer(std::unique_ptr<TValueType[]> aBuffer, size_t aSize) NOEXCEPT
    : TBuffer{TBufferDataRef{aBuffer.get(), aSize}},
      iBuffer{std::move(aBuffer)}
  {
  }

private:
  // set alignment to the platform word size
  std::unique_ptr<TValueType[]> iBuffer;
  using TBufferDataRef = TBufferDataRef<TValueType>;
};

template <typename taValueType, size_t taAlignment>
struct TBufferTraits<THeapBuffer<taValueType, taAlignment>>
{
  using TBuffer        = THeapBuffer<taValueType, taAlignment>;
  using TValueType     = typename TBuffer::TValueType;
  using TIterator      = typename TBuffer::TIterator;
  using TConstIterator = typename TBuffer::TConstIterator;

  inline static constexpr size_t Size(const TBuffer& aBuffer) NOEXCEPT
  {
    return aBuffer.Size();
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
