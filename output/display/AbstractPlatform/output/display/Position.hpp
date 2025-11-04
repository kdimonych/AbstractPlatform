#pragma once

namespace AbstractPlatform {

struct TPosition
{
  using TIndex = int;
  TIndex iX    = 0;
  TIndex iY    = 0;
};

inline static constexpr bool operator==(const TPosition& lhs, const TPosition& rhs)
{
  return lhs.iX == rhs.iX && lhs.iY == rhs.iY;
}

inline static constexpr bool operator!=(const TPosition& lhs, const TPosition& rhs)
{
  return lhs.iX != rhs.iX || lhs.iY != rhs.iY;
}

} // namespace AbstractPlatform
