#pragma once
#include <AbstractPlatform/common/ArrayHelper.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <functional>
#include <initializer_list>

namespace AbstractPlatform {

template <typename taData>
struct TRPtr
{
  using TData = taData;
  const TData* const iDataPtr; // Pointer to the data (const).

  inline constexpr const TData* const get() const
  {
    return iDataPtr;
  }
};

template <typename taData>
struct TRWPtr
{
  using TData = taData;
  TData* const iDataPtr; // Pointer to the data.

  inline constexpr TData* const get() const
  {
    return iDataPtr;
  }
};

template <typename taData>
struct TBufferDataRef
{
  using TRPtr  = TRPtr<taData>;
  using TRWPtr = TRWPtr<taData>;

  union
  {
    TRPtr  iDataRPtr;  // Pointer to the data.
    TRWPtr iDataRWPtr; // Pointer to the data (const).
    static_assert(sizeof(TRPtr) == sizeof(TRWPtr), "DataRef size mismatch");
  };

  size_t iSize; // Size of the data.
};

// Forward declarations
template <typename taData, size_t taAlignment>
class TRWBuffer;
template <typename taData, size_t taSize, size_t taAlignment>
class TStackBuffer;
template <typename taData, size_t taAlignment>
class TBufferView;
template <typename taData, size_t taAlignment>
class THeapBuffer;

template <typename taData, size_t taAlignment = AbstractPlatform::kWordAlignment>
class TRBuffer
{
public:
  using TData                        = taData;
  static constexpr size_t kAlignment = taAlignment;

  /**
   * @brief Get the RAW buffer pointer.
   *
   * @return constexpr const TData*
   */
  inline constexpr const TData* GetBuffer() const NOEXCEPT
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

  inline constexpr const TData* begin() const NOEXCEPT
  {
    return GetBuffer();
  }

  inline constexpr const TData* end() const NOEXCEPT
  {
    return GetBuffer() + Size();
  }

  inline constexpr const TData* cbegin() const NOEXCEPT
  {
    return GetBuffer();
  }

  inline constexpr const TData* cend() const NOEXCEPT
  {
    return GetBuffer() + Size();
  }

  const TData& operator[](size_t index) const NOEXCEPT
  {
    assert(index < Size() && "Index out of bounds");
    return GetBuffer()[index];
  }

private:
  friend class TRWBuffer<TData, kAlignment>;
  template <typename taTBData, size_t taTBSize, size_t taTBAlignment>
  friend class TStackBuffer;
  friend class TBufferView<TData, kAlignment>;
  friend class THeapBuffer<TData, kAlignment>;

  inline constexpr TRBuffer(TBufferDataRef<TData> aDataRef) NOEXCEPT : iDataRef{aDataRef} { }

  // The pointer is not constant to allow modification from derived classes.
  TBufferDataRef<TData> iDataRef; // Pointer to the buffer.
};

template <typename taDataType, size_t taAlignment>
inline constexpr size_t ArrayLength(const TRBuffer<taDataType, taAlignment>& aBuffer) NOEXCEPT
{
  return aBuffer.Size();
}

template <typename taDataType, size_t taAlignment>
inline constexpr size_t ArraySizeBytes(const TRBuffer<taDataType, taAlignment>& aBuffer) NOEXCEPT
{
  return aBuffer.Size() * sizeof(taDataType);
}

template <typename taData, size_t taAlignment = AbstractPlatform::kNoAlignment>
class TRWBuffer : public TRBuffer<taData, taAlignment>
{
public:
  using TData = typename TRBuffer<taData, taAlignment>::TData;
  using TRBuffer<taData, taAlignment>::kAlignment;
  using TRBuffer<taData, taAlignment>::GetBuffer;
  using TRBuffer<taData, taAlignment>::Size;

  inline constexpr TData* GetBuffer() NOEXCEPT
  {
    return this->iDataRef.iDataRWPtr.get();
  }

  inline constexpr TData* begin() NOEXCEPT
  {
    return this->GetBuffer();
  }

  inline constexpr TData* end() NOEXCEPT
  {
    return this->GetBuffer() + this->Size();
  }

  TData& operator[](size_t index) NOEXCEPT
  {
    assert(index < this->Size() && "Index out of bounds");
    return GetBuffer()[index];
  }

private:
  template <typename taTBData, size_t taTBSize, size_t taTBAlignment>
  friend class TStackBuffer;
  friend class TBufferView<TData, kAlignment>;
  friend class THeapBuffer<TData, kAlignment>;

  using TRBuffer<TData, kAlignment>::TRBuffer;
};

template <typename taData, size_t taSize, size_t taAlignment = AbstractPlatform::kNoAlignment>
class TStackBuffer : public TRWBuffer<taData, taAlignment>
{
public:
  static_assert(taSize > 0, "Buffer size must be greater than zero");
  using TBuffer = TRWBuffer<taData, taAlignment>;
  using TData   = typename TBuffer::TData;
  using TBuffer::kAlignment;
  static constexpr size_t kSize = taSize;

  template <typename taInitFunction>
  inline constexpr TStackBuffer(taInitFunction aInitFunction) NOEXCEPT
    : TBuffer{TBufferDataRef{this->iAligned.iBuffer, kSize}}
  {
    for (auto& item : this->iAligned.iBuffer)
    {
      item = aInitFunction();
    }
  }

  inline constexpr TStackBuffer(size_t aInitValue = size_t{}) NOEXCEPT
    : TBuffer{TBufferDataRef{this->iAligned.iBuffer, kSize}}
  {
    std::memset(this->iAligned.iBuffer, aInitValue, sizeof(this->iAligned.iBuffer));
  }

  TStackBuffer(std::initializer_list<TData> aInitList) NOEXCEPT
    : TBuffer{TBufferDataRef{this->iAligned.iBuffer, kSize}}
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
    alignas(taTBAlignment) TData iBuffer[kSize];
  };

  template <>
  struct TAlignmentHelper<AbstractPlatform::kNoAlignment>
  {
    TData iBuffer[kSize];
  };

  // set alignment to the platform word size
  TAlignmentHelper<taAlignment> iAligned;
  using TBufferDataRef = TBufferDataRef<TData>;
};

template <typename taDataType, size_t taSize, size_t taAlignment>
inline constexpr size_t ArrayLength(const TStackBuffer<taDataType, taSize, taAlignment>&)
{
  return taSize;
}

template <typename taDataType, size_t taSize, size_t taAlignment>
inline constexpr size_t ArraySizeBytes(const TStackBuffer<taDataType, taSize, taAlignment>&)
{
  return taSize * sizeof(taDataType);
}

template <typename taData, size_t taAlignment = AbstractPlatform::kNoAlignment>
class TBufferView : public TRWBuffer<taData, taAlignment>
{
public:
  using TBuffer = TRWBuffer<taData, taAlignment>;
  using TData   = typename TBuffer::TData;
  using TBuffer::kAlignment;

  // To prevent double modification of the buffer, we remove the copy constructor and assignment
  // operator.
  TBufferView(TBufferView&&)            = default;
  TBufferView& operator=(TBufferView&&) = default;

  inline constexpr TBufferView(TData* const aBufferPtr, size_t aSize)
    : TBuffer{TBufferDataRef{aBufferPtr, aSize}}
  {
    // Check if the buffer is aligned to the specified alignment.
    if constexpr (AbstractPlatform::IsAlignmentAware(kAlignment))
    {
      assert(AbstractPlatform::IsALigned(aBufferPtr, kAlignment));
    }
  }

  ~TBufferView() = default;

private:
  using TBufferDataRef = TBufferDataRef<TData>;
};

// Const specialization of TBufferView
template <typename taData, size_t taAlignment>
class TBufferView<const taData, taAlignment> : public TRBuffer<const taData, taAlignment>
{
public:
  using TBuffer = TRBuffer<const taData, taAlignment>;
  using TData   = typename TBuffer::TData;
  using TBuffer::kAlignment;

  inline constexpr TBufferView(const TData* const aBufferPtr, size_t aSize)
    : TBuffer{TBufferDataRef{aBufferPtr, aSize}}
  {
    // Check if the buffer is aligned to the specified alignment.
    if constexpr (AbstractPlatform::IsAlignmentAware(kAlignment))
    {
      assert(AbstractPlatform::IsALigned(aBufferPtr, kAlignment));
    }
  }

  ~TBufferView() = default;

private:
  using TBufferDataRef = TBufferDataRef<TData>;
};

template <typename taData, size_t taAlignment = AbstractPlatform::kNoAlignment>
class THeapBuffer : public TRWBuffer<taData, taAlignment>
{
public:
  using TBuffer = TRWBuffer<taData, taAlignment>;
  using TData   = typename TBuffer::TData;
  using TBuffer::kAlignment;
  using AlignedDeleter    = std::function<void(TData*)>;
  using TAlignedUniquePtr = std::unique_ptr<TData[], AlignedDeleter>;

  THeapBuffer(size_t aSize) NOEXCEPT
    : TBuffer{TBufferDataRef{
        static_cast<TData*>(::operator new[](sizeof(TData) * aSize, std::align_val_t(kAlignment))),
        aSize}},
      iBuffer{}
  {
    assert(AbstractPlatform::IsALigned(this->GetBuffer(), kAlignment));
    iBuffer = std::move(std::unique_ptr<TData[], AlignedDeleter>{
      this->GetBuffer(),
      [](TData* ptr) NOEXCEPT { ::operator delete[](ptr, std::align_val_t(kAlignment)); }});
  }

  inline constexpr THeapBuffer(TAlignedUniquePtr aBuffer, size_t aSize) NOEXCEPT
    : TBuffer{TBufferDataRef{aBuffer.get(), aSize}},
      iBuffer{std::move(aBuffer)}
  {
    // Check if the buffer is aligned to the specified alignment.
    assert(AbstractPlatform::IsALigned(this->GetBuffer(), kAlignment));
  }

private:
  // set alignment to the platform word size
  TAlignedUniquePtr iBuffer;
  using TBufferDataRef = TBufferDataRef<TData>;
};

template <typename taData>
class THeapBuffer<taData, AbstractPlatform::kNoAlignment>
  : public TRWBuffer<taData, AbstractPlatform::kNoAlignment>
{
public:
  using TBuffer = TRWBuffer<taData, AbstractPlatform::kNoAlignment>;
  using TData   = typename TBuffer::TData;

  THeapBuffer(size_t aSize) NOEXCEPT : TBuffer{TBufferDataRef{new TData[aSize], aSize}},
                                       iBuffer{std::unique_ptr<TData[]>{this->GetBuffer()}}
  {
  }

  inline constexpr THeapBuffer(std::unique_ptr<TData[]> aBuffer, size_t aSize) NOEXCEPT
    : TBuffer{TBufferDataRef{aBuffer.get(), aSize}},
      iBuffer{std::move(aBuffer)}
  {
  }

private:
  // set alignment to the platform word size
  std::unique_ptr<TData[]> iBuffer;
  using TBufferDataRef = TBufferDataRef<TData>;
};

} // namespace AbstractPlatform
