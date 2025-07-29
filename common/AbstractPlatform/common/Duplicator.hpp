#pragma once
#include <AbstractPlatform/platform/Platform.hpp>

#include <array>
#include <cassert>
#include <cstring>

namespace AbstractPlatform {
static constexpr size_t kBulkCopyThreshold =
  kPlatformWordSize * 4; // Threshold for bulk copy optimization in bytes

struct CpuMemCopy;

template <typename taType>
struct MemoryDuplicator;

template <>
struct MemoryDuplicator<CpuMemCopy>
{
  template <typename taObject>
  inline void CopyMemory(const taObject* aSrcObject,
                         taObject*       aDstObject,
                         size_t          aSize = sizeof(taObject)) const NOEXCEPT
  {
    if (std::memcpy(aDstObject, aSrcObject, aSize) == nullptr)
    {
      assert(false && "Memory copy failed");
      std::abort(); // Deliberately trigger abort
    }
  }

  /**
   * @brief Fill the given memory object with a specific value.
   *
   * @tparam taObject object type to fill
   * @tparam taValue value type to fill with
   * @param aSrcObject pointer to the object
   * @param aValue value to fill with
   */
  template <typename taObject, typename taValue>
  inline void FillWith(taObject*      aSrcObject,
                       const taValue& aValue,
                       size_t         aSize = sizeof(taObject)) const NOEXCEPT
  {
    static_assert(sizeof(taObject) % sizeof(taValue) == 0,
                  "taObject size must be a multiple of taValue size");
    taValue* valuePtr = reinterpret_cast<taValue*>(aSrcObject);
    for (size_t i = 0; i < aSize / sizeof(taValue); ++i)
    {
      std::memcpy(&valuePtr[i], &aValue, sizeof(taValue)); // Set each element to the value
    }
  }
};

template <typename taObject, typename Enable = void>
struct Duplicator
{
  /**
   * @brief Clone the given object.
   *
   * @tparam T The type of the object to clone.
   * @param aObject The object to clone.
   * @return T* Pointer to the cloned object.
   */
  template <typename taMemoryDuplicatorType>
  inline static constexpr taObject
  Clone(MemoryDuplicator<taMemoryDuplicatorType>& /*aMemoryDuplicator*/,
        const taObject& aObjectFrom,
        taObject&       aObjectTo) NOEXCEPT
  {
    static_assert(!std::is_pod<taObject>::value, "taObject must be non-POD");
    aObjectTo = aObjectFrom; // Default implementation for most types
  }
};

// Specialization for POD types
template <typename taObject>
struct Duplicator<taObject, std::enable_if_t<std::is_pod<taObject>::value>>
{
  /**
   * @brief Clone the given POD object with a memory duplicator.
   *
   * @tparam T The type of the object to clone.
   * @param aObject The object to clone.
   * @return T* Pointer to the cloned object.
   */
  template <typename taMemoryDuplicatorType>
  inline static void Clone(MemoryDuplicator<taMemoryDuplicatorType>& aMemoryDuplicator,
                           const taObject&                           aObjectFrom,
                           taObject&                                 aObjectTo) NOEXCEPT
  {
    // Allocate memory for the object in stack
    if constexpr (sizeof(taObject) > kBulkCopyThreshold)
    {
      aMemoryDuplicator.CopyMemory(&aObjectFrom, &aObjectTo, sizeof(taObject));
    }
    else
    {
      // No sense to use bulk copy for small objects
      aObjectTo = aObjectFrom; // Copy the object directly
    }
  }
};

// Specialization for POD types
template <size_t taSize, typename taObject>
struct Duplicator<taObject[taSize], std::enable_if_t<std::is_pod<taObject>::value>>
{
  /**
   * @brief Clone the given POD object with a memory duplicator.
   *
   * @tparam T The type of the object to clone.
   * @param aObject The object to clone.
   * @return T* Pointer to the cloned object.
   */
  template <typename taMemoryDuplicatorType>
  inline static constexpr void Clone(MemoryDuplicator<taMemoryDuplicatorType>& aMemoryDuplicator,
                                     const taObject (&aObjectFrom)[taSize],
                                     taObject (&aObjectTo)[taSize]) NOEXCEPT
  {
    if constexpr ((sizeof(taObject) * taSize) > kBulkCopyThreshold)
    {
      aMemoryDuplicator.CopyMemory(&aObjectFrom, &aObjectTo, sizeof(taObject) * taSize);
    }
    else
    {
      // No sense to use bulk copy for small arrays, copy each element individually
      for (size_t i = 0; i < taSize; ++i)
      {
        aObjectTo[i] = aObjectFrom[i]; // Copy each element individually
      }
    }
  }
};

// Specialization for POD types
template <size_t taN, size_t taM, typename taObject>
struct Duplicator<taObject[taN][taM], std::enable_if_t<std::is_pod<taObject>::value>>
{
  /**
   * @brief Clone the given POD object with a memory duplicator.
   *
   * @tparam T The type of the object to clone.
   * @param aObject The object to clone.
   * @return T* Pointer to the cloned object.
   */
  template <typename taMemoryDuplicatorType>
  inline static constexpr void Clone(MemoryDuplicator<taMemoryDuplicatorType>& aMemoryDuplicator,
                                     const taObject (&aObjectFrom)[taN][taM],
                                     taObject (&aObjectTo)[taN][taM]) NOEXCEPT
  {
    if constexpr ((sizeof(taObject) * taN * taM) > kBulkCopyThreshold)
    {
      aMemoryDuplicator.CopyMemory(&aObjectFrom, &aObjectTo, sizeof(taObject) * taN * taM);
    }
    else
    {
      // No sense to use bulk copy for small arrays, copy each element individually
      for (size_t i = 0; i < taN; ++i)
      {
        for (size_t j = 0; j < taM; ++j)
        {
          aObjectTo[i][j] = aObjectFrom[i][j]; // Copy each element individually
        }
      }
    }
  }
};

template <typename taTypeFrom, typename taTypeTo, typename taMemoryDuplicatorType>
inline static constexpr void Clone(MemoryDuplicator<taMemoryDuplicatorType>& aMemoryDuplicator,
                                   const taTypeFrom&                         aObjectFrom,
                                   taTypeTo&                                 aObjectTo) NOEXCEPT
{
  Duplicator<taTypeFrom>::Clone(aMemoryDuplicator, aObjectFrom, aObjectTo);
}

} // namespace AbstractPlatform
