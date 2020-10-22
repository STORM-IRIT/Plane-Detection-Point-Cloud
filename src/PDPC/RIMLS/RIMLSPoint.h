#pragma once

namespace pdpc {

template<class VectorT>
class RIMLSPoint
{
public:
    enum {Dim         = VectorT::RowsAtCompileTime};
    using VectorType  = VectorT;
    using Scalar      = typename VectorT::Scalar;
    using MatrixType  = Eigen::Matrix<Scalar, Dim, Dim>;
    using HVectorType = Eigen::Matrix<Scalar, Dim+1, 1>;

public:
    inline RIMLSPoint(const VectorType& pos,
                      const VectorType& normal,
                      Scalar gradientDiff,
                      Scalar potentialDiff) :
        m_pos(pos.data()),
        m_normal(normal.data()),
        m_gradientDiff(gradientDiff),
        m_potentialDiff(potentialDiff)
    {
    }

    inline const Eigen::Map<const VectorType>& pos()    const {return m_pos;}
    inline const Eigen::Map<const VectorType>& normal() const {return m_normal;}

    inline Scalar gradientDiff()  const {return m_gradientDiff;}
    inline Scalar potentialDiff() const {return m_potentialDiff;}

protected:
    Eigen::Map<const VectorType> m_pos;
    Eigen::Map<const VectorType> m_normal;
    Scalar m_gradientDiff;
    Scalar m_potentialDiff;
};

} // namespace pdpc
