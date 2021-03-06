// @copyright 2022, Antoine Basset (CNES)
// This file is part of Litl <github.com/kabasset/Raster>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _LITLRUN_STEPPERPIPELINE_H
#define _LITLRUN_STEPPERPIPELINE_H

#include <chrono>
#include <map>
#include <numeric> // accumulate
#include <tuple>
#include <typeindex>
#include <utility> // enable_if

namespace Litl {

/**
 * @relates StepperPipeline
 * @brief Traits class which gives the cardinality (number of elements) of a type.
 * @details
 * Cardinality of:
 * - `void` is 0;
 * - a tuple is its size;
 * - any other type is 1.
 */
template <typename T>
struct TypeCardinality {
  static constexpr std::size_t value = 1;
};

/**
 * @relates StepperPipeline
 * @brief `void` specialization
 */
template <>
struct TypeCardinality<void> {
  static constexpr std::size_t value = 0;
};

/**
 * @relates StepperPipeline
 * @brief Tuple specialization
 */
template <typename... Ts>
struct TypeCardinality<std::tuple<Ts...>> {
  static constexpr std::size_t value = sizeof...(Ts);
};

/**
 * @relates StepperPipeline
 * @brief Cardinality of a step's prerequisite.
 * @see `TypeCardinality`
 */
template <typename S>
constexpr int prerequisiteCardinality() {
  return TypeCardinality<typename S::Prerequisite>::value;
}

/**
 * @brief A pipeline or directed acyclic graph (DAG) which can be run step-by-step using lazy evaluation.
 * @details
 * The only public method, `get<S>()`, returns the value of step `S`.
 * If not already done, the prerequisites of `S` are first triggered.
 * 
 * This class relies on the CRTP.
 * Child classes should provide a specialization of the following methods for each step `S`:
 * - `void doEvaluate<S>()`, which evaluates `S` assuming upstream tasks were already computed;
 * - `S::Return doGet<S>()`, which returns the computed value of `S`.
 * 
 * A step `S` is a class which contains the following type definitions:
 * - `Return` is the return value type of `get<S>`;
 * - `Prerequisite` is (are) the step(s) which must be run prior to `S`, or `void` if there is no prerequisite;
 *   Multiple prerequisites are describled with tuples.
 */
template <typename TDerived>
class StepperPipeline {

public:
  /**
   * @brief Evaluation of step `S` which has no prerequisite.
   */
  template <typename S>
  std::enable_if_t<prerequisiteCardinality<S>() == 0, typename S::Return> get() {
    return evaluateGet<S>();
  }

  /**
   * @brief Lazy evaluation of step `S` which triggers a single prerequisite evaluation if needed.
   */
  template <typename S>
  std::enable_if_t<prerequisiteCardinality<S>() == 1, typename S::Return> get() {
    get<typename S::Prerequisite>();
    return evaluateGet<S>();
  }

  /**
   * @brief Lazy evaluation of step `S` which triggers multiple prerequisite evaluations if needed.
   */
  template <typename S>
  std::enable_if_t<prerequisiteCardinality<S>() >= 2, typename S::Return> get() {
    getMultiple<typename S::Prerequisite>(std::make_index_sequence<prerequisiteCardinality<S>()> {});
    return evaluateGet<S>();
  }

  /**
   * @brief Check whether some step `S` has already been evaluated.
   */
  template <typename S>
  bool evaluated() const {
    return m_milliseconds.find(key<S>()) != m_milliseconds.end();
  }

  /**
   * @brief Get the elapsed time of step `S`.
   * @return The time in millisecond if the step was evaluated, or -1 otherwise.
   */
  template <typename S>
  double milliseconds() const {
    const auto it = m_milliseconds.find(key<S>());
    if (it != m_milliseconds.end()) {
      return it->second;
    }
    return -1;
  }

  /**
   * @brief Get the total elapsed time.
   */
  double milliseconds() const {
    return std::accumulate(m_milliseconds.begin(), m_milliseconds.end(), 0., [](const auto sum, const auto& e) {
      return sum + e.second;
    });
  }

protected:
  /**
   * @brief Reset to initial step.
   */
  void reset() {
    m_milliseconds.clear();
  }

private:
  /**
   * @brief Call `get()` on each element of a tuple.
   */
  template <typename STuple, std::size_t... Is>
  void getMultiple(std::index_sequence<Is...>) {
    using mockUnpack = int[];
    (void)mockUnpack {0, (get<std::tuple_element_t<Is, STuple>>(), void(), 0)...};
    // TODO could be done in threads!
  }

  /**
   * @brief Access to protected methods of `TDerived`.
   */
  template <typename S>
  struct Accessor : TDerived {

    /**
     * @brief Call `algo.doEvaluate<S>()`.
     */
    static void evaluate(TDerived& algo) {
      auto f = &Accessor::template doEvaluate<S>;
      (algo.*f)();
    }

    /**
     * @brief Call `algo.doGet<S>()`.
     */
    static typename S::Return get(TDerived& algo) {
      auto f = &Accessor::template doGet<S>;
      return (algo.*f)();
    }
  };

  /**
   * @brief Run step `S` if not done and return its output.
   */
  template <typename S>
  typename S::Return evaluateGet() {
    if (not evaluated<S>()) { // FIXME not thread-safe
      const auto start = std::chrono::high_resolution_clock::now();
      Accessor<S>::evaluate(derived());
      const auto stop = std::chrono::high_resolution_clock::now();
      m_milliseconds[key<S>()] = std::chrono::duration<double, std::milli>(stop - start).count();
    }
    return Accessor<S>::get(derived());
  }

  /**
   * @brief Cast as `TDerived`.
   */
  inline TDerived& derived() {
    return static_cast<TDerived&>(*this);
  }

private:
  /**
   * @brief Get the key of a step `S`.
   */
  template <typename S>
  std::type_index key() const {
    return std::type_index(typeid(S));
  }

  /**
   * @brief The set of performed steps and durations.
   */
  std::map<std::type_index, double> m_milliseconds;
};

/**
 * @brief Helper class to declare a pipeline steps.
 * @details
 * Usage:
 * \code
 * struct Step0 : PipelineStep<void, char> {};
 * struct Step1a : PipelineStep<Step0, short> {};
 * struct Step1b : PipelineStep<Step0, int> {};
 * struct Step2 : PipelineStep<std::tuple<Step1a, Step1b>, long>{};
 * \endcode
 */
template <typename TPrerequisite, typename TReturn>
struct PipelineStep {
  /**
   * @brief The step's prerequisite's.
   * @details
   * Can be:
   * - `void` for no prerequisite;
   * - `std::tuple` for multiple prerequisites;
   * - Any other type for simple prerequisite.
   */
  using Prerequisite = TPrerequisite;

  /**
   * @brief The return type of the step.
   * @details
   * This is exactly the return type of the `StepperPipeline::get()`
   * which can for example be a const reference.
   */
  using Return = TReturn;
};

} // namespace Litl

#endif
