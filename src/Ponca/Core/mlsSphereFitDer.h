/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

namespace Ponca
{

/*!
 * \brief Extension performing derivation of the mls surface
 * \inherit Concept::FittingExtensionConcept
 *
 * The differentiation is determined by a previous basket elements that must
 * provides first order derivatives of the algebraic sphere parameters.
 *
 */
template < class DataPoint, class _WFunctor, typename T>
class MlsSphereFitDer : public T
{
private:
    typedef T Base;

protected:
    enum
    {
        Check = Base::PROVIDES_ALGEBRAIC_SPHERE_DERIVATIVE,
        PROVIDES_NORMAL_SPACE_DERIVATIVE
    };

    enum
    {
        Dim     = DataPoint::Dim,       //!< Dimension of the ambient space
        DerDim  = Base::derDimension()  //!< Number of dimensions used for the differentiation
    };

public:
    typedef typename Base::Scalar     Scalar;     /*!< \brief Inherited scalar type*/
    typedef typename Base::VectorType VectorType; /*!< \brief Inherited vector type*/

    typedef typename Base::VectorArray VectorArray; /*!< \brief Inherited vector array type */
    typedef typename Base::ScalarArray ScalarArray; /*!< \brief Inherited scalar array type */

    /*!
        \brief Static squared matrix of scalars with a size adapted to the
        differentiation type.

        The size is adapted to the differentiation type (scale and/or space)
        only. Such Matrix is typically used to represent Hessian matrix of
        multivariate single-valued functions such as \f$ u_c \f$ and \f$ u_q \f$
        of an AlgebraicSphere obtained from a fitting procedure.
     */
    typedef Eigen::Matrix< Scalar, DerDim, DerDim > Matrix;

    /*!
        \brief Static matrix of scalars with a size adapted to the
        differentiation type and the dimension of the ambient space.

        The size is adapted to the differentiation type (scale and/or space) and
        the dimension of the ambient space. Such Matrix is typically used to
        represent Hessian matrices of multivariate multi-valued functions such
        as \f$ u_l \f$ of an AlgebraicSphere obtained from a fitting procedure.

        A MatrixArray is an array of Matrix (that mimics a 3D matrix) where the
        number of Matrix is equal to the dimension of the ambient space.
        The i-th Matrix can be accesed by the following block operation:
        \code
            MatrixArray a;
            int i;
            //...
            Matrix m_i = a.template block<DerDim, DerDim>(0, i*DerDim);
        \endcode
     */
    typedef Eigen::Matrix< Scalar, DerDim, Dim*DerDim > MatrixArray;

protected:
    // computation data
    Matrix m_d2SumDotPN,  /*!< \brief Sum of the dot product betwen relative positions and normals with twice differenciated weights */
           m_d2SumDotPP,  /*!< \brief Sum of the squared relative positions with twice differenciated weights */
           m_d2SumW;      /*!< \brief Sum of queries weight with twice differenciated weights */

    MatrixArray m_d2SumP, /*!< \brief Sum of relative positions with twice differenciated weights */
                m_d2SumN; /*!< \brief Sum of normal vectors with twice differenciated weights */

public:
    // results
    Matrix m_d2Uc,      /*!< \brief Second derivative of the hyper-sphere constant term  */
           m_d2Uq;      /*!< \brief Second derivative of the hyper-sphere quadratic term */
    MatrixArray m_d2Ul; /*!< \brief Second derivative of the hyper-sphere linear term    */

public:
    /************************************************************************/
    /* Initialization                                                       */
    /************************************************************************/
    /*! \see Concept::FittingProcedureConcept::init() */
    PONCA_MULTIARCH void init(const VectorType &evalPos);

    /************************************************************************/
    /* Processing                                                           */
    /************************************************************************/
    /*! \see Concept::FittingProcedureConcept::addNeighbor() */
    PONCA_MULTIARCH bool addNeighbor(const DataPoint &nei);

    /*! \see Concept::FittingProcedureConcept::finalize() */
    PONCA_MULTIARCH FIT_RESULT finalize();

    /**************************************************************************/
    /* Use results                                                            */
    /**************************************************************************/

    /*! \brief Returns the derivatives of the scalar field at the evaluation point */
    PONCA_MULTIARCH inline ScalarArray dPotential() const;

    /*! \brief Value of the normal of the primitive at the evaluation point */
    PONCA_MULTIARCH inline VectorType normal() const;

    /*! \brief Returns the second derivatives of the scalar field at the evaluation point */
    PONCA_MULTIARCH inline VectorArray dNormal() const;

    /*! \brief Returns the second derivatives of the scalar field at the evaluation point */
    PONCA_MULTIARCH inline Matrix hessian() const;

}; //class MlsSphereFitDer

#include "mlsSphereFitDer.hpp"

} //namespace Ponca
