/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/


#pragma once

#include "primitive.h" // PrimitiveBase

#include PONCA_MULTIARCH_INCLUDE_STD(cmath)
#include PONCA_MULTIARCH_INCLUDE_STD(limits)

#include <Eigen/Core>

namespace Ponca
{

/*!
    \brief Algebraic Ellipsoid primitive

    This primitive provides:
    \verbatim PROVIDES_ALGEBRAIC_ELLIPSOID \endverbatim

    \see AlgebraicSphere
*/
template < class DataPoint, class _WFunctor, typename T = void  >
class AlgebraicEllipsoid : public PrimitiveBase<DataPoint, _WFunctor>
{
private:

    typedef PrimitiveBase<DataPoint, _WFunctor> Base;

protected:

    enum
    {
        PROVIDES_ALGEBRAIC_ELLIPSOID /*!< \brief Provides Algebraic Ellipsoid */
    };

public:

    /*! \brief Scalar type inherited from DataPoint */
    typedef typename DataPoint::Scalar     Scalar;
    /*! \brief Vector type inherited from DataPoint */
    typedef typename DataPoint::VectorType VectorType;
    /*! \brief Matrix type inherited from DataPoint */
    typedef typename DataPoint::MatrixType MatrixType;
    /*! \brief Weight Function */
    typedef _WFunctor                      WFunctor;

private:

    //! \brief Evaluation position (needed for centered basis)
    VectorType m_p;

protected:

    //! \brief Is the implicit scalar field normalized using Pratt
    bool m_isNormalized;

// results
public:

    Scalar m_uc;       /*!< \brief Constant parameter of the Algebraic hyper-ellipsoid */
    VectorType m_ul;   /*!< \brief Linear parameter of the Algebraic hyper-ellipsoid  */
    MatrixType m_uq;   /*!< \brief Quadratic parameter of the Algebraic hyper-ellipsoid  */

public:

    /*! \brief Default constructor */
    PONCA_MULTIARCH inline AlgebraicEllipsoid()
        : Base()
    {
        m_p = VectorType::Zero();
        resetPrimitive();
    }

    /*! \brief Explicit conversion to AlgebraicEllipsoid, to access methods potentially hidden by inheritage */
    PONCA_MULTIARCH inline
    AlgebraicEllipsoid<DataPoint, WFunctor, T>& algebraicEllipsoid()
    { return * static_cast<AlgebraicEllipsoid<DataPoint, WFunctor, T>*>(this); }

    /*! \brief Set the scalar field values to 0 and reset the isNormalized() status

        \warning Set m_ul to Zero(), which leads to nans in OrientedEllipsoid::normal()
        \FIXME Set and use Base::m_state to handle invalid configuration
    */
    PONCA_MULTIARCH inline void resetPrimitive()
    {
        Base::resetPrimitive();

        m_uc = Scalar(0.0);
        m_ul = VectorType::Zero();
        m_uq = MatrixType::Zero();

        m_isNormalized = false;
    }

    PONCA_MULTIARCH inline bool operator==(const AlgebraicEllipsoid<DataPoint, WFunctor, T>& other) const{
        return m_uc == other.m_uc &&
               m_uq == other.m_uq &&
               m_ul == other.m_ul;
    }

    /*! \brief Comparison operator, convenience function */
    PONCA_MULTIARCH inline bool operator!=(const AlgebraicEllipsoid<DataPoint, WFunctor, T>& other) const{
        return ! ((*this) == other);
    }

    /*! \brief Reading access to the basis center (evaluation position) */
    PONCA_MULTIARCH inline const VectorType& basisCenter () const { return m_p; }
    /*! \brief Writing access to the (evaluation position) */
    PONCA_MULTIARCH inline       VectorType& basisCenter ()       { return m_p; }

//    /*! \brief Express the scalar field relatively to a new basis */
//    PONCA_MULTIARCH inline void changeBasis(const VectorType& newbasis)
//    {
//        VectorType diff = m_p- newbasis;
//        m_uc = m_uc - m_ul.dot(diff) + m_uq * diff.dot(diff);
//        m_ul = m_ul - Scalar(2.)*m_uq*diff;
//        //m_uq is not changed
//        m_p = newbasis;
//        m_isNormalized = false;
//        applyPrattNorm();
//    }

//    /*! \brief compute the Pratt norm of the implicit scalar field. */
//    PONCA_MULTIARCH inline Scalar prattNorm() const
//    {
//        PONCA_MULTIARCH_STD_MATH(sqrt);
//        return sqrt(prattNorm2());
//    }

//    /*! \brief compute the squared Pratt norm of the implicit scalar field. */
//    PONCA_MULTIARCH inline Scalar prattNorm2() const
//    {
//        return m_ul.squaredNorm() - Scalar(4.) * m_uc * m_uq;
//    }

//    /*!
//        \brief Normalize the scalar field by the Pratt norm
//        \return false when the normalization fails (ellipsoid  is already normalized)
//    */
//    PONCA_MULTIARCH inline bool applyPrattNorm()
//    {
//        if (! m_isNormalized)
//        {
//            Scalar pn = prattNorm();
//            m_uc /= pn;
//            m_ul *= Scalar(1.)/pn;
//            m_uq /= pn;

//            m_isNormalized = true;
//        }
//        return true;
//    }

//    /*!
//        \brief return the estimated radius of the ellipsoid
//        \warning return inf if the fitted surface is planar
//    */
//    PONCA_MULTIARCH inline Scalar radius() const
//    {
//        if(isPlane())
//        {
//            //return infinity (non-sense value)
//#ifdef __CUDACC__
//            Scalar inf = 0.;
//            return Scalar(1.)/inf;
//#else
//            return std::numeric_limits<Scalar>::infinity();
//#endif
//        }

//        PONCA_MULTIARCH_STD_MATH(sqrt);
//        Scalar b = Scalar(1.)/m_uq;
//        return Scalar(sqrt( ((Scalar(-0.5)*b)*m_ul).squaredNorm() - m_uc*b ));
//    }

    /*!
        \brief return the estimated center of the ellipsoid
        \warning return Vector inf if the fitted surface is planar or cylindric
    */
    PONCA_MULTIARCH inline VectorType center() const
    {
        //TODO manage when uq is not invertible (should check uq determinant)
//        if(isPlane())
//        {
//            //return infinity (non-sense value)
//            Scalar inf = 0.;
//            return VectorType::Constant(Scalar(1.)/inf);
//        }

        return - Scalar(.5) * m_uq.inverse() * m_ul + basisCenter();
    }

    /*!
        \brief return the matrix form of the ellipsoid
    */
    PONCA_MULTIARCH inline MatrixType matrix() const
    {
        //TODO manage when uq is not invertible (should check uq determinant)

        return m_uq / (m_uc - Scalar(0.25) * m_ul.transpose() * m_uq.inverse() * m_ul);
    }

    //! \brief State indicating when the ellipsoid  has been normalized
    PONCA_MULTIARCH inline bool isNormalized() const { return m_isNormalized; }

    //! \brief Value of the scalar field at the location \f$ \mathbf{q} \f$
    PONCA_MULTIARCH inline Scalar potential (const VectorType& _q) const;

    /*! \brief Value of the scalar field at the evaluation point */
    PONCA_MULTIARCH inline Scalar potential() const { return m_uc; }

    //! \brief Project a point on the ellipsoid
    PONCA_MULTIARCH inline VectorType project (const VectorType& _q) const;

    //! \brief Approximation of the scalar field gradient at \f$ \mathbf{q} (not normalized) \f$
    PONCA_MULTIARCH inline VectorType primitiveGradient (const VectorType& _q) const;

    /*! \brief Approximation of the scalar field gradient at the evaluation point */
    PONCA_MULTIARCH inline VectorType primitiveGradient () const { return m_ul/*.normalized()*/; }

//    /*!
//        \brief Used to know if the fitting result to a plane
//        \return true if finalize() have been called and the fitting result to a plane
//    */
//    PONCA_MULTIARCH inline bool isPlane() const
//    {
//        PONCA_MULTIARCH_STD_MATH(abs);
//        Scalar epsilon = Eigen::NumTraits<Scalar>::dummy_precision();
//        bool bPlanar   = Eigen::internal::isMuchSmallerThan(abs(m_uq), Scalar(1.), epsilon);
//        bool bReady    = Base::isReady();

//        return bReady && bPlanar;
//    }

}; //class AlgebraicEllipsoid

#include "algebraicEllipsoid.hpp"

}
