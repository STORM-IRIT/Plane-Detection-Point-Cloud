/*
 Copyright (C) 2018 Nicolas Mellado <nmellado0@gmail.com>

 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/


template < class DataPoint, class _WFunctor, typename T>
void
MeanPlaneFit<DataPoint, _WFunctor, T>::init(const VectorType& _evalPos)
{
    // Setup primitive
    Base::resetPrimitive();
    Base::basisCenter() = _evalPos;

    // Setup fitting internal values
    m_sumP = VectorType::Zero();
    m_sumN = VectorType::Zero();
    m_sumW = Scalar(0.0);
}

template < class DataPoint, class _WFunctor, typename T>
bool
MeanPlaneFit<DataPoint, _WFunctor, T>::addNeighbor(const DataPoint& _nei)
{
    VectorType q = _nei.pos() - Base::basisCenter();
    // compute weight
    Scalar w = m_w.w(q, _nei);

    if (w > Scalar(0.))
    {
      m_sumP += q * w;
      m_sumN += _nei.normal() * w;
      m_sumW += w;

      ++(Base::m_nbNeighbors);
      return true;
    }
    return false;
}


template < class DataPoint, class _WFunctor, typename T>
FIT_RESULT
MeanPlaneFit<DataPoint, _WFunctor, T>::finalize ()
{
    // handle specific configurations
    // With less than 3 neighbors the fitting is undefined
    if(m_sumW == Scalar(0.) || Base::m_nbNeighbors == 0)
    {
      Base::resetPrimitive();
      Base::m_eCurrentState = UNDEFINED;
      return Base::m_eCurrentState;
    }

    Base::setPlane(m_sumN / m_sumW, m_sumP / m_sumW);

    return Base::m_eCurrentState = STABLE;
}

