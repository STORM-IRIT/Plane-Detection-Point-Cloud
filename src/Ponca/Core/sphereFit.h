/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include "algebraicSphere.h"

namespace Ponca
{

/*!
    \brief Algebraic Sphere fitting procedure on point sets without normals

    Method published in \cite Guennebaud:2007:APSS.

    \inherit Concept::FittingProcedureConcept

    \see AlgebraicSphere
*/
template < class DataPoint, class _WFunctor, typename T = void >
class SphereFit : public AlgebraicSphere<DataPoint, _WFunctor>
{
private:

    typedef AlgebraicSphere<DataPoint, _WFunctor> Base;

public:

    /*! \brief Scalar type inherited from DataPoint*/
    typedef typename Base::Scalar     Scalar;
    /*! \brief Vector type inherited from DataPoint*/
    typedef typename Base::VectorType VectorType;
    /*! \brief Weight Function*/
    typedef _WFunctor                 WFunctor;

 protected:
    typedef Eigen::Matrix<Scalar, DataPoint::Dim+2, 1>      VectorA;
    typedef Eigen::Matrix<Scalar, DataPoint::Dim+2, DataPoint::Dim+2>  MatrixA;

    // computation data
    MatrixA  m_matA;  /*!< \brief Covariance matrix of [1, p, p^2] */
    Scalar   m_sumW;  /*!< \brief Sum of queries weight */
    WFunctor m_w;     /*!< \brief Weight function (must inherits BaseWeightFunc) */

public:

    /*! \brief Default constructor */
    PONCA_MULTIARCH inline SphereFit()
        : Base(){}

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

}; //class SphereFit



#include "sphereFit.hpp"

} //namespace Ponca
