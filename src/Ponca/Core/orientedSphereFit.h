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
    \brief Algebraic Sphere fitting procedure on oriented point sets

    Method published in \cite Guennebaud:2007:APSS.

    \inherit Concept::FittingProcedureConcept

    \see AlgebraicSphere
*/
template < class DataPoint, class _WFunctor, typename T = void >
class OrientedSphereFit : public AlgebraicSphere<DataPoint, _WFunctor>
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

    // computation data
    VectorType  m_sumN, /*!< \brief Sum of the normal vectors */
                m_sumP; /*!< \brief Sum of the relative positions */
    Scalar  m_sumDotPN, /*!< \brief Sum of the dot product betwen relative positions and normals */
            m_sumDotPP, /*!< \brief Sum of the squared relative positions */
            m_sumW,     /*!< \brief Sum of queries weight */
            m_nume,     /*!< \brief Numerator of the quadratic parameter (excluding the 0.5 coefficient)   */
            m_deno;     /*!< \brief Denominator of the quadratic parameter (excluding the 0.5 coefficient) */

    WFunctor m_w;      /*!< \brief Weight function (must inherits BaseWeightFunc) */

public:

    /*! \brief Default constructor */
    PONCA_MULTIARCH inline OrientedSphereFit()
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

    /**************************************************************************/
    /* Use results                                                            */
    /**************************************************************************/

    /*!
        \brief Returns the weighted sum of squared residuals of the fit

        Residuals are the norm of the difference between the gradient of the
        fitted sphere and the normal at neighboring points.
    */
    PONCA_MULTIARCH inline Scalar residualMean() const;

}; //class OrientedSphereFit


namespace internal
{

/*!
    \brief Internal generic class performing the Fit derivation
    \inherit Concept::FittingExtensionConcept

    The differentiation can be done automatically in scale and/or space, by
    combining the enum values FitScaleDer and FitSpaceDer in the template
    parameter Type.

    The differenciated values are stored in static arrays. The size of the
    arrays is computed with respect to the derivation type (scale and/or space)
    and the number of the dimension of the ambiant space.
    By convention, the scale derivatives are stored at index 0 when Type
    contains at least FitScaleDer. The size of these arrays can be known using
    derDimension(), and the differentiation type by isScaleDer() and
    isSpaceDer().
*/
template < class DataPoint, class _WFunctor, typename T, int _Type>
class OrientedSphereDer : public T
{
private:
    typedef T Base; /*!< \brief Generic base type */

protected:
    enum
    {
        Check = Base::PROVIDES_ALGEBRAIC_SPHERE, /*!< \brief Needs Algebraic Sphere */
        PROVIDES_ALGEBRAIC_SPHERE_DERIVATIVE,    /*!< \brief Provides Algebraic Sphere derivative*/
        PROVIDES_NORMAL_DERIVATIVE
    };

public:
    typedef typename Base::Scalar     Scalar;     /*!< \brief Inherited scalar type*/
    typedef typename Base::VectorType VectorType; /*!< \brief Inherited vector type*/
    typedef typename Base::WFunctor   WFunctor;   /*!< \brief Weight Function*/

#define GLS_DER_NB_DERIVATIVES(TYPE,DIM) ((TYPE & FitScaleDer) ? 1 : 0 ) + ((TYPE & FitSpaceDer) ? DIM : 0)
#define GLS_DER_STORAGE_ORDER(TYPE)      ((TYPE & FitSpaceDer) ? Eigen::RowMajor : Eigen::ColMajor )

    /*! \brief Static array of scalars with a size adapted to the differentiation type */
    typedef Eigen::Matrix < Scalar,
        DataPoint::Dim,
        GLS_DER_NB_DERIVATIVES(_Type,DataPoint::Dim),
        GLS_DER_STORAGE_ORDER(_Type) > VectorArray;

    /*! \brief Static array of scalars with a size adapted to the differentiation type */
    typedef Eigen::Matrix < Scalar,
                            1,
                            GLS_DER_NB_DERIVATIVES(_Type,DataPoint::Dim)/*,
                            GLS_DER_STORAGE_ORDER(Type)*/ > ScalarArray;

protected:
    // computation data
    VectorArray m_dSumN,     /*!< \brief Sum of the normal vectors with differenciated weights */
                m_dSumP;     /*!< \brief Sum of the relative positions with differenciated weights*/
    ScalarArray m_dSumDotPN, /*!< \brief Sum of the dot product betwen relative positions and normals with differenciated weights */
                m_dSumDotPP, /*!< \brief Sum of the squared relative positions with differenciated weights */
                m_dSumW,     /*!< \brief Sum of queries weight with differenciated weights */
                m_dNume,     /*!< \brief Differenciation of the numerator of the quadratic parameter   */
                m_dDeno;     /*!< \brief Differenciation of the denominator of the quadratic parameter */

public:
    // results
    ScalarArray m_dUc, /*!< \brief Derivative of the hyper-sphere constant term  */
                m_dUq; /*!< \brief Derivative of the hyper-sphere quadratic term */
    VectorArray m_dUl; /*!< \brief Derivative of the hyper-sphere linear term    */

    /************************************************************************/
    /* Initialization                                                       */
    /************************************************************************/
    /*! \see Concept::FittingProcedureConcept::init() */
    PONCA_MULTIARCH void init       (const VectorType &evalPos);

    /************************************************************************/
    /* Processing                                                           */
    /************************************************************************/
    /*! \see Concept::FittingProcedureConcept::addNeighbor() */
    PONCA_MULTIARCH bool addNeighbor(const DataPoint  &nei);
    /*! \see Concept::FittingProcedureConcept::finalize() */
    PONCA_MULTIARCH FIT_RESULT finalize   ();


    /**************************************************************************/
    /* Use results                                                            */
    /**************************************************************************/

    /*! \brief Returns the derivatives of the scalar field at the evaluation point */
    PONCA_MULTIARCH inline ScalarArray dPotential() const;

    /*! \brief Returns the derivatives of the primitive normal */
    PONCA_MULTIARCH inline VectorArray dNormal() const;

    /*! \brief compute  the square of the Pratt norm derivative */
    PONCA_MULTIARCH inline ScalarArray dprattNorm2() const
    {
        return   Scalar(2.) * Base::m_ul.transpose() * m_dUl
            - Scalar(4.) * Base::m_uq * m_dUc
            - Scalar(4.) * Base::m_uc * m_dUq;
    }

    /*! \brief compute the square of the Pratt norm derivative for dimension _d */
    PONCA_MULTIARCH inline Scalar dprattNorm2(unsigned int _d) const
    {
        return   Scalar(2.) * m_dUl.col(_d).dot(Base::m_ul)
            - Scalar(4.) * m_dUc.col(_d)[0]*Base::m_uq
            - Scalar(4.) * m_dUq.col(_d)[0]*Base::m_uc;
    }

    /*! \brief compute the Pratt norm derivative for the dimension _d */
    PONCA_MULTIARCH inline Scalar dprattNorm(unsigned int _d) const
    {
        PONCA_MULTIARCH_STD_MATH(sqrt);
        return sqrt(dprattNorm2(_d));
    }

    /*! \brief compute the Pratt norm derivative */
    PONCA_MULTIARCH inline Scalar dprattNorm() const
    {
        PONCA_MULTIARCH_STD_MATH(sqrt);
        return dprattNorm2().array().sqrt();
    }

    /*! \brief State specified at compilation time to differenciate the fit in scale */
    static constexpr PONCA_MULTIARCH inline bool isScaleDer() {return bool(_Type & FitScaleDer);}
    /*! \brief State specified at compilation time to differenciate the fit in space */
    static constexpr PONCA_MULTIARCH inline bool isSpaceDer() {return bool(_Type & FitSpaceDer);}
    /*! \brief Number of dimensions used for the differentiation */
    static constexpr PONCA_MULTIARCH inline unsigned int derDimension() { return GLS_DER_NB_DERIVATIVES(_Type,DataPoint::Dim);}

    //! Normalize the scalar field by the Pratt norm
    /*!
        \warning Requieres that isNormalized() return false
        \return false when the original sphere has already been normalized.
    */
    PONCA_MULTIARCH inline bool applyPrattNorm();

}; //class OrientedSphereFitDer

}// namespace internal

/*!
    \brief Differentiation in scale of the OrientedSphereFit
    \inherit Concept::FittingExtensionConcept

    Requierement:
    \verbatim PROVIDES_ALGEBRAIC_SPHERE \endverbatim
    Provide:
    \verbatim PROVIDES_ALGEBRAIC_SPHERE_SCALE_DERIVATIVE \endverbatim
*/
template < class DataPoint, class _WFunctor, typename T>
class OrientedSphereScaleDer:public internal::OrientedSphereDer<DataPoint, _WFunctor, T, internal::FitScaleDer>
{
protected:
    /*! \brief Inherited class */
    typedef internal::OrientedSphereDer<DataPoint, _WFunctor, T, internal::FitScaleDer> Base;
    enum
    {
      PROVIDES_ALGEBRAIC_SPHERE_SCALE_DERIVATIVE,
      PROVIDES_NORMAL_SCALE_DERIVATIVE
    };
};


/*!
    \brief Spatial differentiation of the OrientedSphereFit
    \inherit Concept::FittingExtensionConcept

    Requierement:
    \verbatim PROVIDES_ALGEBRAIC_SPHERE \endverbatim
    Provide:
    \verbatim PROVIDES_ALGEBRAIC_SPHERE_SPACE_DERIVATIVE \endverbatim
*/
template < class DataPoint, class _WFunctor, typename T>
class OrientedSphereSpaceDer:public internal::OrientedSphereDer<DataPoint, _WFunctor, T, internal::FitSpaceDer>
{
protected:
    /*! \brief Inherited class */
    typedef internal::OrientedSphereDer<DataPoint, _WFunctor, T, internal::FitSpaceDer> Base;
    enum
    {
      PROVIDES_ALGEBRAIC_SPHERE_SPACE_DERIVATIVE,
      PROVIDES_NORMAL_SPACE_DERIVATIVE
    };
};


/*!
    \brief Differentiation both in scale and space of the OrientedSphereFit
    \inherit Concept::FittingExtensionConcept

    Requierement:
    \verbatim PROVIDES_ALGEBRAIC_SPHERE \endverbatim
    Provide:
    \verbatim PROVIDES_ALGEBRAIC_SPHERE_SCALE_DERIVATIVE
    PROVIDES_ALGEBRAIC_SPHERE_SPACE_DERIVATIVE
    \endverbatim
*/
template < class DataPoint, class _WFunctor, typename T>
class OrientedSphereScaleSpaceDer:public internal::OrientedSphereDer<DataPoint, _WFunctor, T, internal::FitSpaceDer | internal::FitScaleDer>
{
protected:
    /*! \brief Inherited class */
    typedef internal::OrientedSphereDer<DataPoint, _WFunctor, T, internal::FitSpaceDer | internal::FitScaleDer> Base;
    enum
    {
        PROVIDES_ALGEBRAIC_SPHERE_SCALE_DERIVATIVE,
        PROVIDES_ALGEBRAIC_SPHERE_SPACE_DERIVATIVE,
        PROVIDES_NORMAL_SCALE_DERIVATIVE,
        PROVIDES_NORMAL_SPACE_DERIVATIVE
    };
};


#include "orientedSphereFit.hpp"

} //namespace Ponca
