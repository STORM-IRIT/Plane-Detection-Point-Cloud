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
    \brief Algebraic Sphere primitive

    Method published in \cite Guennebaud:2007:APSS

    An algebraic hyper-sphere is defined as the \f$0\f$-isosurface of the scalar field

    \f$ s_\mathbf{u}(\mathbf{x}) = \left[ 1 \; \mathbf{x}^T \; \mathbf{x}^T\mathbf{x}\right]^T \cdot \mathbf{u} \f$

    with \f$ \mathbf{u} \left[ u_c \; \mathbf{u_l} \; u_q\right]^T \f$ is the
    vector of the constant, linear and quadratic parameters.

    \note If internally the scalar fields are stored in a local frame defined
    by the evaluation position, the public methods involving a query (such as
    project, potential, gradient) have to be defined in global
    coordinates (e.g. you don't need to convert your query in the current locale
    frame).

    This primitive provides:
    \verbatim PROVIDES_ALGEBRAIC_SPHERE \endverbatim

    \todo Deal with planar case (_uq == 0) and what about _ul == 0 ?
*/
template < class DataPoint, class _WFunctor, typename T = void  >
class AlgebraicSphere : public PrimitiveBase<DataPoint, _WFunctor>
{
private:

    typedef PrimitiveBase<DataPoint, _WFunctor> Base;

protected:

    enum
    {
        PROVIDES_ALGEBRAIC_SPHERE /*!< \brief Provides Algebraic Sphere */
    };

public:

    /*! \brief Scalar type inherited from DataPoint */
    typedef typename DataPoint::Scalar     Scalar;
    /*! \brief Vector type inherited from DataPoint */
    typedef typename DataPoint::VectorType VectorType;
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

    Scalar m_uc,       /*!< \brief Constant parameter of the Algebraic hyper-sphere */
            m_uq;       /*!< \brief Quadratic parameter of the Algebraic hyper-sphere */
    VectorType m_ul;   /*!< \brief Linear parameter of the Algebraic hyper-sphere */

public:

    /*! \brief Default constructor */
    PONCA_MULTIARCH inline AlgebraicSphere()
        : Base()
    {
        m_p = VectorType::Zero();
        resetPrimitive();
    }

    /*! \brief Explicit conversion to AlgebraicSphere, to access methods potentially hidden by inheritage */
    PONCA_MULTIARCH inline
    AlgebraicSphere<DataPoint, WFunctor, T>& algebraicSphere()
    { return * static_cast<AlgebraicSphere<DataPoint, WFunctor, T>*>(this); }

    /*! \brief Set the scalar field values to 0 and reset the isNormalized() status

        \warning Set m_ul to Zero(), which leads to nans in OrientedSphere::normal()
        \FIXME Set and use Base::m_state to handle invalid configuration
    */
    PONCA_MULTIARCH inline void resetPrimitive()
    {
        Base::resetPrimitive();

        m_uc = Scalar(0.0);
        m_ul = VectorType::Zero();
        m_uq = Scalar(0.0);

        m_isNormalized = false;
    }

    PONCA_MULTIARCH inline bool operator==(const AlgebraicSphere<DataPoint, WFunctor, T>& other) const{
        return m_uc == other.m_uc &&
               m_uq == other.m_uq &&
               m_ul == other.m_ul;
    }

    /*! \brief Comparison operator, convenience function */
    PONCA_MULTIARCH inline bool operator!=(const AlgebraicSphere<DataPoint, WFunctor, T>& other) const{
        return ! ((*this) == other);
    }

    /*! \brief Reading access to the basis center (evaluation position) */
    PONCA_MULTIARCH inline const VectorType& basisCenter () const { return m_p; }
    /*! \brief Writing access to the (evaluation position) */
    PONCA_MULTIARCH inline       VectorType& basisCenter ()       { return m_p; }

    /*! \brief Express the scalar field relatively to a new basis */
    PONCA_MULTIARCH inline void changeBasis(const VectorType& newbasis)
    {
        VectorType diff = m_p- newbasis;
        m_uc = m_uc - m_ul.dot(diff) + m_uq * diff.dot(diff);
        m_ul = m_ul - Scalar(2.)*m_uq*diff;
        //m_uq is not changed
        m_p = newbasis;
        m_isNormalized = false;
        applyPrattNorm();
    }

    /*! \brief compute the Pratt norm of the implicit scalar field. */
    PONCA_MULTIARCH inline Scalar prattNorm() const
    {
        PONCA_MULTIARCH_STD_MATH(sqrt);
        return sqrt(prattNorm2());
    }

    /*! \brief compute the squared Pratt norm of the implicit scalar field. */
    PONCA_MULTIARCH inline Scalar prattNorm2() const
    {
        return m_ul.squaredNorm() - Scalar(4.) * m_uc * m_uq;
    }

    /*!
        \brief Normalize the scalar field by the Pratt norm
        \return false when the normalization fails (sphere is already normalized)
    */
    PONCA_MULTIARCH inline bool applyPrattNorm()
    {
        if (! m_isNormalized)
        {
            Scalar pn = prattNorm();
            m_uc /= pn;
            m_ul *= Scalar(1.)/pn;
            m_uq /= pn;

            m_isNormalized = true;
        }
        return true;
    }

    /*!
        \brief return the estimated radius of the sphere
        \warning return inf if the fitted surface is planar
    */
    PONCA_MULTIARCH inline Scalar radius() const
    {
        if(isPlane())
        {
            //return infinity (non-sense value)
#ifdef __CUDACC__
            Scalar inf = 0.;
            return Scalar(1.)/inf;
#else
            return std::numeric_limits<Scalar>::infinity();
#endif
        }

        PONCA_MULTIARCH_STD_MATH(sqrt);
        Scalar b = Scalar(1.)/m_uq;
        return Scalar(sqrt( ((Scalar(-0.5)*b)*m_ul).squaredNorm() - m_uc*b ));
    }

    /*!
        \brief return the estimated center of the sphere
        \warning return Vector inf if the fitted surface is planar
    */
    PONCA_MULTIARCH inline VectorType center() const
    {
        if(isPlane())
        {
            //return infinity (non-sense value)
            Scalar inf = 0.;
            return VectorType::Constant(Scalar(1.)/inf);
        }

        Scalar b = Scalar(1.)/m_uq;
        return (Scalar(-0.5)*b)*m_ul + basisCenter();
    }

    //! \brief State indicating when the sphere has been normalized
    PONCA_MULTIARCH inline bool isNormalized() const { return m_isNormalized; }

    //! \brief Value of the scalar field at the location \f$ \mathbf{q} \f$
    PONCA_MULTIARCH inline Scalar potential (const VectorType& _q) const;

    /*! \brief Value of the scalar field at the evaluation point */
    PONCA_MULTIARCH inline Scalar potential() const { return m_uc; }

    //! \brief Project a point on the sphere
    PONCA_MULTIARCH inline VectorType project (const VectorType& _q) const;

    //! \brief Approximation of the scalar field gradient at \f$ \mathbf{q} (not normalized) \f$
    PONCA_MULTIARCH inline VectorType primitiveGradient (const VectorType& _q) const;

    /*! \brief Approximation of the scalar field gradient at the evaluation point */
    PONCA_MULTIARCH inline VectorType primitiveGradient () const { return m_ul.normalized(); }

    /*!
        \brief Used to know if the fitting result to a plane
        \return true if finalize() have been called and the fitting result to a plane
    */
    PONCA_MULTIARCH inline bool isPlane() const
    {
        PONCA_MULTIARCH_STD_MATH(abs);
        Scalar epsilon = Eigen::NumTraits<Scalar>::dummy_precision();
        bool bPlanar   = Eigen::internal::isMuchSmallerThan(abs(m_uq), Scalar(1.), epsilon);
        bool bReady    = Base::isReady();

        return bReady && bPlanar;
    }

}; //class AlgebraicSphere

#include "algebraicSphere.hpp"

}
