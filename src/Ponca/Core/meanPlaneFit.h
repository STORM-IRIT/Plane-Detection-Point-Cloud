/*
 Copyright (C) 2018 Nicolas Mellado <nmellado0@gmail.com>

 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include "plane.h"

namespace Ponca
{

/*!
    \brief Plane fitting procedure computing the mean position and orientation
    from oriented points

    \inherit Concept::FittingProcedureConcept

    \see CompactPlane
    \todo Add derivatives
*/
template < class DataPoint, class _WFunctor, typename T >
class MeanPlaneFit : public CompactPlane<DataPoint, _WFunctor>
{
private:
    typedef CompactPlane<DataPoint, _WFunctor> Base;

protected:
    enum
    {
        Check = Base::PROVIDES_PLANE
    };

public:

    /*! \brief Scalar type inherited from DataPoint*/
    typedef typename Base::Scalar     Scalar;
    /*! \brief Vector type inherited from DataPoint*/
    typedef typename Base::VectorType VectorType;
    /*! \brief Vector type inherited from DataPoint*/
    typedef typename Base::MatrixType MatrixType;
    /*! \brief Weight Function*/
    typedef _WFunctor                 WFunctor;

 protected:

    // computation data
    Scalar      m_sumW;    /*!< \brief Sum of queries weight.*/
    VectorType  m_sumN,    /*!< \brief Sum of the normal vectors */
                m_sumP;    /*!< \brief Sum of the relative positions */

    WFunctor m_w;     /*!< \brief Weight function (must inherits BaseWeightFunc) */

public:

    /*! \brief Default constructor */
    PONCA_MULTIARCH inline MeanPlaneFit() : Base() {}

    /**************************************************************************/
    /* Initialization                                                         */
    /**************************************************************************/
    /*! \copydoc Concept::FittingProcedureConcept::setWeightFunc() */
    PONCA_MULTIARCH inline void setWeightFunc (const WFunctor& _w) { m_w  = _w; }

    /*! \copydoc Concept::FittingProcedureConcept::init() */
    PONCA_MULTIARCH inline void init (const VectorType& _evalPos);

    /**************************************************************************/
    /* Processing                                                             */
    /**************************************************************************/
    /*! \copydoc Concept::FittingProcedureConcept::addNeighbor() */
    PONCA_MULTIARCH inline bool addNeighbor(const DataPoint &_nei);

    /*! \copydoc Concept::FittingProcedureConcept::finalize() */
    PONCA_MULTIARCH inline FIT_RESULT finalize();
}; //class MeanPlaneFit


#include "meanPlaneFit.hpp"

} //namespace Ponca
