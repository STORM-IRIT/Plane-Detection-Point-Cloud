/*
 Copyright (C) 2014 Nicolas Mellado <nmellado0@gmail.com>

 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include "primitive.h" // PrimitiveBase
#include <Eigen/Geometry>

namespace Ponca
{

/*!
    \brief Implicit hyperplane defined by an homogeneous vector \f$\mathbf{p}\f$.

    In n-dimensionnal space, the plane is defined as
    the \f$0\f$-isosurface of the scalar field

    \f$ s_\mathbf{u}(\mathbf{x}) =
    \left[ \mathbf{x}^T \; 1 \;\right]^T \cdot \mathbf{p} \f$.

    This class inherits Eigen::Hyperplane.

    This primitive requires the definition of n-dimensionnal vectors
    (VectorType) in Concept::PointConcept.

    This primitive provides:
    \verbatim PROVIDES_PLANE \endverbatim

*/
template < class DataPoint, class _WFunctor, typename T = void  >
class CompactPlane : public PrimitiveBase<DataPoint, _WFunctor>,
                     public Eigen::Hyperplane<typename DataPoint::Scalar, DataPoint::Dim >
{
private:

    using Base      = PrimitiveBase<DataPoint, _WFunctor>;

public:
    /// \brief Specialization of Eigen::Hyperplane inherited by Ponca::CompactPlane
    using EigenBase = Eigen::Hyperplane<typename DataPoint::Scalar, DataPoint::Dim >;


protected:

    enum
    {
        PROVIDES_PLANE /*!< \brief Provides a Plane primitive */
    };

public:

    /*! \brief Scalar type inherited from DataPoint */
    typedef typename DataPoint::Scalar      Scalar;
    /*! \brief Vector type inherited from DataPoint */
    typedef typename DataPoint::VectorType  VectorType;
    /*! \brief Matrix type inherited from DataPoint */
    typedef typename DataPoint::MatrixType  MatrixType;
    /*! \brief Weight Function */
    typedef _WFunctor                       WFunctor;

private:

    //! \brief Evaluation position (needed for centered basis)
    VectorType m_p;


public:

    /*! \brief Default constructor */
    PONCA_MULTIARCH inline CompactPlane()
        : Base(), EigenBase()
    {
        m_p = VectorType::Zero();
        resetPrimitive();
    }

    /*! \brief Explicit conversion to CompactPlane, to access methods potentially hidden by inheritage */
    PONCA_MULTIARCH inline
    CompactPlane<DataPoint, WFunctor, T>& compactPlane()
    { return * static_cast<CompactPlane<DataPoint, WFunctor, T>*>(this); }

    /*! \brief Set the scalar field values to 0
         status */
    PONCA_MULTIARCH inline void resetPrimitive()
    {
        Base::resetPrimitive();
        EigenBase* cc = static_cast<EigenBase*>(this);
        *cc = EigenBase();
    }

    PONCA_MULTIARCH inline bool operator==(const CompactPlane<DataPoint, WFunctor, T>& other) const{
        return EigenBase::isApprox(other);
    }

    /*! \brief Comparison operator, convenience function */
    PONCA_MULTIARCH inline bool operator!=(const CompactPlane<DataPoint, WFunctor, T>& other) const{
        return ! ((*this) == other);
    }

    /*! \brief Reading access to the basis center (evaluation position) */
    PONCA_MULTIARCH inline const VectorType& basisCenter () const { return m_p; }
    /*! \brief Writing access to the (evaluation position) */
    PONCA_MULTIARCH inline       VectorType& basisCenter ()       { return m_p; }

    /* \brief Init the plane from a direction and a position
       \param _dir Orientation of the plane, does not need to be normalized
       \param _pos Position of the plane
    */
    PONCA_MULTIARCH inline void setPlane (const VectorType& _dir,
                                    const VectorType& _pos)
    {
        EigenBase* cc = static_cast<EigenBase*>(this);
        *cc = EigenBase(_dir.normalized(), _pos);
    }

    /*! \brief Value of the scalar field at the evaluation point */
    PONCA_MULTIARCH inline Scalar potential ( ) const
    {
        return EigenBase::signedDistance(VectorType::Zero());
    }

    //! \brief Value of the scalar field at the location \f$ \mathbf{q} \f$
    PONCA_MULTIARCH inline Scalar potential (const VectorType& _q) const
    {
        // The potential is the distance from the point to the plane
        return EigenBase::signedDistance(_q - m_p);
    }

    //! \brief Project a point on the plane
    PONCA_MULTIARCH inline VectorType project (const VectorType& _q) const
    {
        // Project on the normal vector and add the offset value
        return EigenBase::projection(_q - m_p) + m_p;
    }

    //! \brief Scalar field gradient direction at the evaluation point
    PONCA_MULTIARCH inline VectorType primitiveGradient () const
    {
        // Uniform gradient defined only by the orientation of the plane
        return EigenBase::normal();
    }

    //! \brief Scalar field gradient direction at \f$ \mathbf{q}\f$
    PONCA_MULTIARCH inline VectorType primitiveGradient (const VectorType&) const
    {
        // Uniform gradient defined only by the orientation of the plane
        return EigenBase::normal();
    }


}; //class Plane

}
