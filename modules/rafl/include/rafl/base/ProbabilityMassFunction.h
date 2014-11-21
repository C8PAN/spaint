/**
 * rafl: ProbabilityMassFunction.h
 */

#ifndef H_RAFL_PROBABILITYMASSFUNCTION
#define H_RAFL_PROBABILITYMASSFUNCTION

#include <cmath>

#include <tvgutil/LimitedMap.h>
#include "Histogram.h"

namespace rafl {

/**
 * \brief An instance of an instantiation of this class template represents a probability mass function (PMF).
 */
template <typename Label>
class ProbabilityMassFunction
{
  //#################### PRIVATE VARIABLES ####################
private:
  /** The masses for the various labels. */
  std::map<Label,float> m_masses;

  //#################### CONSTRUCTORS ####################
public:
  /**
   * \brief Constructs a probability mass function (PMF) as a normalised version of the specified histogram.
   *
   * \param histogram The histogram from which to construct a PMF.
   */
  explicit ProbabilityMassFunction(const Histogram<Label>& histogram)
  {
    const float SMALL_EPSILON = 1e-9f;

    // Determine the masses for the labels in the histogram by dividing the number of instances in each bin by the histogram count.
    const std::map<Label,size_t>& bins = histogram.get_bins();
    size_t count = histogram.get_count();
    for(typename std::map<Label,size_t>::const_iterator it = bins.begin(), iend = bins.end(); it != iend; ++it)
    {
      float mass = static_cast<float>(it->second) / count;

      // Our implementation is dependent on the masses never becoming too small. If this assumption turns out not to be ok,
      // we may need to change the implementation.
      assert(mass >= SMALL_EPSILON);

      m_masses.insert(std::make_pair(it->first, mass));
    }
  }

  //#################### PUBLIC MEMBER FUNCTIONS ####################
public:
  /**
   * \brief Calculates the entropy of the PMF using the definition H(X) = -sum_{i} P(x_i) log2(P(x_i)).
   * 
   * \return The entropy of the PMF. When outcomes are equally likely, the entropy will be high; when the outcome is predictable, the entropy wil be low.
   */
  float calculate_entropy() const
  {
    float entropy = 0.0f;
    for(typename std::map<Label,float>::const_iterator it = m_masses.begin(), iend = m_masses.end(); it != iend; ++it)
    {
      float mass = it->second;
      if(mass > 0)
      {
        // Note: If P(x_i) = 0, the value of the corresponding sum 0*log2(0) is taken to be 0, since lim{p->0+} p*log2(p) = 0 (see Wikipedia!).
        entropy += mass * log2(mass);
      }
    }
    return -entropy;
  }

  /**
   * \brief Gets the masses for the various labels.
   *
   * \return The masses for the various labels.
   */
  const std::map<Label,float>& get_masses() const
  {
    return m_masses;
  }
};

//#################### STREAM OPERATORS ####################

/**
 * \brief Outputs a probability mass function (PMF) to the specified stream.
 *
 * \param os  The stream to which to output the PMF.
 * \param rhs The PMF to output.
 * \return    The stream.
 */
template <typename Label>
std::ostream& operator<<(std::ostream& os, const ProbabilityMassFunction<Label>& rhs)
{
  const size_t ELEMENT_DISPLAY_LIMIT = 3;
  os << tvgutil::make_limited_map(rhs.get_masses(), ELEMENT_DISPLAY_LIMIT);
  return os;
}

}

#endif
