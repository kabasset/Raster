// Copyright (C) 2022, Antoine Basset
// This file is part of Cnes.Raster <github.com/kabasset/Raster>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _RASTER_CONTIGUOUSCONTAINER_H
#define _RASTER_CONTIGUOUSCONTAINER_H

#include <algorithm> // equal
#include <ostream>

namespace Cnes {

/**
 * @ingroup data_concepts
 * @requirements{ContiguousContainer}
 * @brief Standard contiguous container requirements
 * @details
 * A <a href="https://en.cppreference.com/w/cpp/named_req/ContiguousContainer">contiguous container</a>
 * is a standard container whose elements are stored contiguously in memory.
 */

/**
 * @ingroup data_concepts
 * @requirements{SizedData}
 * @brief Requirements for a `DataContainerHolder`.
 * @details
 * A contiguous data holder is some class which stores or points to
 * some data contiguous in memory as a public or protected member `TContainer m_container`,
 * and implements the following methods:
 * - Constructor from a size and pointer;
 * - `std::size_t size() const`;
 * - `inline const T* data() const`.
 * @par_example
 * Here is a minimal `SizedData`-compliant class which wraps a `std::vector`:
 * \code
 * template<typename T>
 * struct MyData {
 * 
 *   MyData(std::size_t s, T* d) : m_container(s) {
 *     if (d) {
 *       std::copy_n(d, s, m_container.begin());
 *     }
 *   }
 * 
 *   std::size_t size() const {
 *     return m_container.size();
 *   }
 * 
 *   inline const T* data() {
 *     return m_container.data();
 *   }
 * 
 *   std::vector<T> m_container;
 * };
 * \endcode
 */

/**
 * @ingroup data_concepts
 * @brief Base class for a FITS data container.
 * 
 * @tparam T The value type
 * @tparam TDerived The child class which implements required methods
 * 
 * @details
 * This class provides the necessary types and methods
 * to meet the standard `ContiguousContainer` requirements.
 * This is a CRTP implementation, which means it takes as template parameter
 * the derived class to be empowered.
 * 
 * The derived class must satisfy the `SizedData` requirements.
 * 
 * @satisfies{ContiguousContainer}
 */
template <typename T, typename TDerived>
struct ContiguousContainerMixin {

  /**
   * @brief The value type.
   */
  using value_type = T;

  /**
   * @brief The value reference.
   */
  using reference = T&;

  /**
   * @brief The constant value reference.
   */
  using const_reference = const T&;

  /**
   * @brief The value iterator.
   */
  using iterator = T*;

  /**
   * @brief The constant value iterator.
   */
  using const_iterator = const T*;

  /**
   * @brief The iterator difference type.
   */
  using difference_type = std::ptrdiff_t;

  /**
   * @brief The underlying container size type.
   */
  using size_type = std::size_t;

  /// @group_properties

  /**
   * @brief Check whether the container is empty.
   * @details
   * Empty corresponds to `begin() == end()`.
   */
  bool emtpy() const {
    return begin() == end();
  }

  /// @group_elements

  /**
   * @brief Access the element with given index.
   */
  inline const T& operator[](size_type index) const {
    return *(static_cast<const TDerived&>(*this).data() + index);
  }

  /**
   * @copybrief operator[]()
   */
  inline T& operator[](size_type index) {
    return const_cast<T&>(const_cast<const ContiguousContainerMixin&>(*this)[index]);
  }

  /// @group_iterators

  /**
   * @brief Iterator to the first element.
   */
  const_iterator begin() const {
    return static_cast<const TDerived&>(*this).data();
  }

  /**
   * @copybrief begin()const
   */
  iterator begin() {
    return const_cast<iterator>(const_cast<const ContiguousContainerMixin&>(*this).begin());
  }

  /**
   * @copybrief begin()const
   */
  const_iterator cbegin() {
    return const_cast<const ContiguousContainerMixin&>(*this).begin();
  }

  /**
   * @brief Iterator to one past the last element.
   */
  const_iterator end() const {
    return begin() + static_cast<const TDerived&>(*this).size();
  }

  /**
   * @copybrief end()const
   */
  iterator end() {
    return const_cast<iterator>(const_cast<const ContiguousContainerMixin&>(*this).end());
  }

  /**
   * @copybrief end()const
   */
  const_iterator cend() {
    return const_cast<const ContiguousContainerMixin&>(*this).end();
  }

  /// @group_operations

  /**
   * @brief Check equality.
   */
  virtual bool operator==(const TDerived& rhs) const {
    return (static_cast<const TDerived&>(*this).size() == rhs.size() && std::equal(begin(), end(), rhs.begin()));
  }

  /**
   * @brief Check inequality.
   */
  bool operator!=(const TDerived& rhs) const {
    return not(*this == rhs);
  }

  /// @}
};

/**
 * @relates ContiguousContainerMixin
 * @brief Insert a `ContiguousContainerMixin` into an output stream.
 */
template <typename T, typename TDerived>
std::ostream& operator<<(std::ostream& os, const ContiguousContainerMixin<T, TDerived>& container) {
  os << "[";
  if (not container.emtpy()) {
    const auto size = static_cast<const TDerived&>(container).size();
    std::size_t i = 0;
    os << container[i];
    if (size > 7) {
      for (++i; i < 3; ++i) {
        os << ", " << container[i];
      }
      i = size - 3;
      os << " ... " << container[i];
    }
    for (++i; i < size; ++i) {
      os << ", " << container[i];
    }
  }
  os << "]";
  return os;
}

} // namespace Cnes

#endif