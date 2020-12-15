#pragma once

#include <PDPC/Common/Defines.h>

#include <map>
#include <vector>
#include <algorithm>
#include <random>

#include <Eigen/Core>

namespace pdpc {
namespace color {

//!
//! \brief The Colors class is a factoy class to create colors
//!
template<typename ColorT>
class Colors
{
protected:
    using Color  = ColorT;
    using Scalar = typename ColorT::Scalar;

public:
    static Color Black(Scalar a=1.)  {return Color(0.0, 0.0, 0.0, a);}
    static Color Gray10(Scalar a=1.) {return Color(0.1, 0.1, 0.1, a);}
    static Color Gray20(Scalar a=1.) {return Color(0.2, 0.2, 0.2, a);}
    static Color Gray30(Scalar a=1.) {return Color(0.3, 0.3, 0.3, a);}
    static Color Gray40(Scalar a=1.) {return Color(0.4, 0.4, 0.4, a);}
    static Color Gray50(Scalar a=1.) {return Color(0.5, 0.5, 0.5, a);}
    static Color Gray60(Scalar a=1.) {return Color(0.6, 0.6, 0.6, a);}
    static Color Gray70(Scalar a=1.) {return Color(0.7, 0.7, 0.7, a);}
    static Color Gray80(Scalar a=1.) {return Color(0.8, 0.8, 0.8, a);}
    static Color Gray90(Scalar a=1.) {return Color(0.9, 0.9, 0.9, a);}
    static Color White(Scalar a=1.)  {return Color(1.0, 1.0, 1.0, a);}

    static Color Gray(Scalar g=.5, Scalar a=1.) {return Color(g, g, g, a);}

    static Color Red(Scalar a=1.)   {return Color(1.0, 0.0, 0.0, a);}
    static Color Green(Scalar a=1.) {return Color(0.0, 1.0, 0.0, a);}
    static Color Blue(Scalar a=1.)  {return Color(0.0, 0.0, 1.0, a);}

    static Color Cyan(Scalar a=1.)    {return Color(0.0, 1.0, 1.0, a);}
    static Color Magenta(Scalar a=1.) {return Color(1.0, 0.0, 1.0, a);}
    static Color Yellow(Scalar a=1.)  {return Color(1.0, 1.0, 0.0, a);}

    static Color Orange(Scalar a=1.)     {return Color(1.0, 0.5, 0.0, a);}
    static Color Pink(Scalar a=1.)       {return Color(1.0, 0.0, 0.5, a);}
    static Color Chartreuse(Scalar a=1.) {return Color(0.5, 1.0, 0.0, a);}
    static Color Purple(Scalar a=1.)     {return Color(0.5, 0.0, 1.0, a);}
    static Color Spring(Scalar a=1.)     {return Color(0.0, 1.0, 0.5, a);}
    static Color Sky(Scalar a=1.)        {return Color(0.0, 0.5, 1.0, a);}

    static Color Gold(Scalar a=1.) {return Color(1.0, 0.84, 0.0, a);}

    static Color Random(Scalar a=1.)
    {
        Color tmp = Color::Random();
        tmp[3] = a;
        return tmp;
    }

    static Color Glitch(Scalar a=1.)
    {
        // std::random_device throw std::runtime_error in multi-threaded program
        // std::rand() may not be thread-safe but avoid this exception
//        std::random_device rd{};
//        std::mt19937 gen{rd()};
//        std::bernoulli_distribution d(.5);
//        return Color(d(gen),d(gen),d(gen),a);
        return Color(std::rand()%2, std::rand()%2, std::rand()%2, a);
    }

    template<class Vector3T>
    static Color Normal(const Vector3T& n, Scalar a=1.)
    {
        Vector3T tmp = 0.5*(Vector3T::Ones()+n);
        return ColorT(tmp[0], tmp[1], tmp[2], a);
    }

    static Color Gray(const Color& rgb)
    {
        Scalar g = 0.2116 * rgb[0] +
                   0.7152 * rgb[1] +
                   0.0722 * rgb[2];
        return Gray(g,rgb[3]);
    }

    static Color Interpolate(const Color& c1, Scalar w1,
                             const Color& c2, Scalar w2)
    {
        return (w1*c1 + w2*c2)/(w1+w2);
    }

    static Color Interpolate(const Color& c1, Scalar w1,
                             const Color& c2, Scalar w2,
                             const Color& c3, Scalar w3)
    {
        return (w1*c1 + w2*c2 + w3*c3)/(w1+w2+w3);
    }

    static Color Interpolate(const Color& c1, const Color& c2, Scalar t)
    {
        return (Scalar(1)-t)*c1 + t*c2;
    }

    static std::string Hexa(const Color& c)
    {
        const int r = std::round(255 * c[0]);
        const int g = std::round(255 * c[1]);
        const int b = std::round(255 * c[2]);
        std::stringstream str;
        str << "#" << std::hex << r << g << b;
        return str.str();
    }

    static std::string HexaAlpha(const Color& c)
    {
        const int r = std::round(255 * c[0]);
        const int g = std::round(255 * c[1]);
        const int b = std::round(255 * c[2]);
        const int a = std::round(255 * c[3]);
        std::stringstream str;
        str << "#" << std::hex << r << g << b << a;
        return str.str();
    }

    // RGB and HSV in [0,1]
    static Color RGB2HSV(const Color& rgb);
    static Color HSV2RGB(const Color& hsv);

}; // class Colors

// =============================================================================

template<typename ColorT>
class Colormap
{
    // Types -------------------------------------------------------------------
public:
    using Color  = ColorT;
    using Scalar = typename Color::Scalar;

    // Colormap ----------------------------------------------------------------
public:
    inline Colormap(int size = 64);

    inline Colormap(const std::initializer_list<Color>& colors,
                    const std::initializer_list<Scalar>& params,
                    int size = 64);

    inline Colormap(const std::initializer_list<Color>& colors);

    // Modifiers ---------------------------------------------------------------
public:
    template<class ColorIt, class ScalarIt>
    inline void set(ColorIt cfirst, ColorIt clast,
                    ScalarIt sfirst, ScalarIt slast);

    template<class ColorIt>
    inline void set(ColorIt cfirst, ColorIt clast);

    inline void shuffle();

    // Accessors ---------------------------------------------------------------
public:
    inline int size() const;

    inline const Color& at(int idx) const;
    inline const Color& operator[](int idx) const;

    inline Color eval(Scalar t) const;
    inline Color eval(Scalar t, Scalar tmax) const;
    inline Color eval(Scalar t, Scalar tmin, Scalar tmax) const;

    inline Color operator() (Scalar t) const;
    inline Color operator() (Scalar t, Scalar tmax) const;
    inline Color operator() (Scalar t, Scalar tmin, Scalar tmax) const;

    // Static ------------------------------------------------------------------
public:
    static inline Colormap<Color> Jet(Scalar a = Scalar(1));
    static inline Colormap<Color> Hot(Scalar a = Scalar(1));
    static inline Colormap<Color> RGB(Scalar a = Scalar(1));

    static inline Colormap<Color> Bright24(Scalar a = Scalar(1));
    static inline Colormap<Color> Tab10(Scalar a = Scalar(1));
    static inline Colormap<Color> Tab20(Scalar a = Scalar(1));

    static inline Colormap<Color> Flash(Scalar a = Scalar(1));
    static inline Colormap<Color> Pastel(Scalar a = Scalar(1));

    // Data --------------------------------------------------------------------
public:
    std::vector<Color> m_colors;
};

// =============================================================================

template<typename ColorT>
class BiColormap
{
    // Types -------------------------------------------------------------------
public:
    using Color  = ColorT;
    using Scalar = typename Color::Scalar;

    // BiColormap --------------------------------------------------------------
public:
    inline BiColormap(const Colormap<Color>& inf, const Colormap<Color>& sup);

    // Accessors ---------------------------------------------------------------
public:
    inline Color eval(Scalar t) const;
    inline Color eval(Scalar t, Scalar tabs) const;
    inline Color eval(Scalar t, Scalar tiso, Scalar tabs) const;

    inline Color operator()(Scalar t) const;
    inline Color operator()(Scalar t, Scalar tabs) const;
    inline Color operator()(Scalar t, Scalar tiso, Scalar tabs) const;

    // Static ------------------------------------------------------------------
public:
    static inline BiColormap<Color> Jet(Scalar a = Scalar(1));

    // Data --------------------------------------------------------------------
protected:
    Colormap<Color> m_inf;
    Colormap<Color> m_sup;
};

// =============================================================================

template<typename ColorT>
class NormalColormap
{
    // Types -------------------------------------------------------------------
public:
    using Color     = ColorT;
    using Scalar    = typename Color::Scalar;
    enum  Direction : int
    {
        UP =     0,
        DOWN, // 1
        LEFT, // 2
        RIGHT,// 3
        FRONT,// 4
        BACK, // 5
        N
    };

    // NormalColormap ----------------------------------------------------------
public:
    inline NormalColormap(const std::array<Color,N>& colors);

    // Evaluation --------------------------------------------------------------
public:
    template<class Vector3T>
    inline Color eval(const Vector3T& n) const;

    template<class Vector3T>
    inline Color operator() (const Vector3T& n) const;

    // Internal ----------------------------------------------------------------
public:
    static inline Color SphereInterpolate(const Color& cx, Scalar x,
                                          const Color& cy, Scalar y,
                                          const Color& cz, Scalar z);

    // Creation ----------------------------------------------------------------
public:
    static inline NormalColormap<Color> RGB(Scalar a=1.);
    static inline NormalColormap<Color> CYM(Scalar a=1.);
    static inline NormalColormap<Color> RGBCYM(Scalar a=1.);

public:
    std::array<Color,N> m_colors;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Colors ----------------------------------------------------------------------

template<typename ColorT>
typename Colors<ColorT>::Color Colors<ColorT>::RGB2HSV(const Color& rgb)
{
    Color hsv;
    auto& H = hsv[0];
    auto& S = hsv[1];
    auto& V = hsv[2];
    const auto R = rgb[0];
    const auto G = rgb[1];
    const auto B = rgb[2];
    const auto A = rgb[3];

    int idx_max = 0;
    if(rgb[1] > rgb[idx_max]) idx_max = 1;
    if(rgb[2] > rgb[idx_max]) idx_max = 2;
    int idx_min = 0;
    if(rgb[1] < rgb[idx_min]) idx_min = 1;
    if(rgb[2] < rgb[idx_min]) idx_min = 2;


         if(rgb[idx_min] == rgb[idx_max]) H = 0;
    else if(idx_max == 0) H = 60.0 * (0.0 + (G-B)/(rgb[idx_max]-rgb[idx_min]));
    else if(idx_max == 1) H = 60.0 * (2.0 + (B-R)/(rgb[idx_max]-rgb[idx_min]));
    else                  H = 60.0 * (4.0 + (R-G)/(rgb[idx_max]-rgb[idx_min]));

    if(H < 0) H += 360;

    if(rgb[idx_max] == 0) S = 0;
    else                  S = (rgb[idx_max]-rgb[idx_min])/rgb[idx_max];

    V = rgb[idx_max];
    H /= 360.0;
    hsv[3] = A;

    return hsv;
}

template<typename ColorT>
typename Colors<ColorT>::Color Colors<ColorT>::HSV2RGB(const Color& hsv)
{
    Color rgb;
    const auto H = hsv[0] * 360.0;
    const auto S = hsv[1];
    const auto V = hsv[2];
    const auto A = hsv[3];

    const auto C  = V * S;
    const auto H2 = H/60.0;
    const auto X  = C * (1 - std::abs(fmod(H2, 2.0) -1));

    if     (0 <= H2 && H2 <= 1) rgb = Color(C,X,0,A);
    else if(1 <  H2 && H2 <= 2) rgb = Color(X,C,0,A);
    else if(2 <  H2 && H2 <= 3) rgb = Color(0,C,X,A);
    else if(3 <  H2 && H2 <= 4) rgb = Color(0,X,C,A);
    else if(4 <  H2 && H2 <= 5) rgb = Color(X,0,C,A);
    else if(5 <  H2 && H2 <= 6) rgb = Color(C,0,X,A);
    else                        rgb = Color(0,0,0,A);

    const auto m = V - C;

    rgb[0] += m;
    rgb[1] += m;
    rgb[2] += m;

    return rgb;
}

// Colormap --------------------------------------------------------------------

template<typename C>
Colormap<C>::Colormap(int size) :
    m_colors(size)
{
    for(Color& c : m_colors)
        c = Scalar(0.5)*(Color::Random()+Color::Ones());
}

template<typename C>
Colormap<C>::Colormap(const std::initializer_list<Color>& colors,
                      const std::initializer_list<Scalar>& params,
                      int size) :
    m_colors(size)
{
    this->set(colors.begin(), colors.end(),
              params.begin(), params.end());
}

template<typename C>
Colormap<C>::Colormap(const std::initializer_list<Color>& colors) :
    m_colors(colors)
{
}

// Modifiers -------------------------------------------------------------------

template<typename C>
template<class ColorIt, class ScalarIt>
void Colormap<C>::set(ColorIt cfirst, ColorIt clast,
                      ScalarIt sfirst, ScalarIt slast)
{
    Scalar smin = *sfirst;
    Scalar smax = *(slast-1);

    m_colors.front() = *cfirst;
    m_colors.back()  = *(clast-1);

    ColorIt  cit = cfirst;
    ScalarIt sit = sfirst;
    for(auto it=m_colors.begin()+1; it<m_colors.end()-1; ++it)
    {
        int i    = std::distance(m_colors.begin(),it);
        Scalar t = Scalar(i)/(this->size()-1);  // in (0,1)
        Scalar s = (Scalar(1)-t)*smin + t*smax; // in (smin,smax)

        while( !(*sit<=s && s<=*(sit+1)) )
        {
            ++cit;
            ++sit;
        }
        Scalar u = s-*sit;
        *it = Interpolate(*cit, *(cit+1), u);
    }
}

template<typename C>
template<class ColorIt>
void Colormap<C>::set(ColorIt cfirst, ColorIt clast)
{
    std::vector<Scalar> s(std::distance(cfirst,clast));
    std::iota(s.begin(), s.end(), 0);
    this->set(cfirst, clast, s.begin(), s.end());
}

template<typename C>
void Colormap<C>::shuffle()
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_colors.begin(), m_colors.end(), g);
}

// Accessors -------------------------------------------------------------------

template<typename C>
int Colormap<C>::size() const
{
    return m_colors.size();
}

template<typename C>
const typename Colormap<C>::Color& Colormap<C>::at(int idx) const
{
    return m_colors[idx % this->size()];
}

template<typename C>
const typename Colormap<C>::Color& Colormap<C>::operator[](int idx) const
{
    return this->at(idx);
}

template<typename C>
typename Colormap<C>::Color Colormap<C>::eval(Scalar t) const
{
    if(std::isnan(t))
    {
        return Colors<C>::Glitch();
    }
    else if(t <= Scalar(0))
    {
        return m_colors.front();
    }
    else if(Scalar(1) <= t)
    {
        return m_colors.back();
    }
    else
    {
        t = t*(this->size()-1); // in (0,N-1)
        int i = std::floor(t);  // in (0,N-1)
        return Colors<C>::Interpolate(m_colors[i], m_colors[i+1], t-i);
    }
}

template<typename C>
typename Colormap<C>::Color Colormap<C>::eval(Scalar t, Scalar tmax) const
{
    if(std::isnan(t))
    {
        return Colors<C>::Glitch();
    }
    else if(t <= Scalar(0))
    {
        return m_colors.front();
    }
    else if(tmax <= t)
    {
        return m_colors.back();
    }
    else
    {
        t = t/tmax;              // in (0,1)
        t = t*(this->size()-1);  // in (0,N-1)
        int i = std::floor(t);   // in (0,N-1)
        return Colors<C>::Interpolate(m_colors[i], m_colors[i+1], t-i);
    }
}

template<typename C>
typename Colormap<C>::Color Colormap<C>::eval(Scalar t, Scalar tmin, Scalar tmax) const
{
    if(std::isnan(t))
    {
        return Colors<C>::Glitch();
    }
    else if(t <= tmin)
    {
        return m_colors.front();
    }
    else if(tmax <= t)
    {
        return m_colors.back();
    }
    else
    {
        t = (t-tmin)/(tmax-tmin);  // in (0,1)
        t = t*(this->size()-1);    // in (0,N-1)
        int i = std::floor(t);     // in (0,N-1)
        return Colors<C>::Interpolate(m_colors[i], m_colors[i+1], t-i);
    }
}

template<typename C>
typename Colormap<C>::Color Colormap<C>::operator() (Scalar t) const
{
    return this->eval(t);
}

template<typename C>
typename Colormap<C>::Color Colormap<C>::operator() (Scalar t, Scalar tmax) const
{
    return this->eval(t, tmax);
}

template<typename C>
typename Colormap<C>::Color Colormap<C>::operator() (Scalar t, Scalar tmin, Scalar tmax) const
{
    return this->eval(t, tmin, tmax);
}

// Static ----------------------------------------------------------------------

template<typename C>
Colormap<C> Colormap<C>::Jet(Scalar a)
{
    return Colormap<C>({Color(0.0, 0.0, 0.5, a),
                        Color(0.0, 0.0, 1.0, a),
                        Color(0.0, 0.5, 1.0, a),
                        Color(0.0, 1.0, 1.0, a),
                        Color(0.5, 1.0, 0.5, a),
                        Color(1.0, 1.0, 0.0, a),
                        Color(1.0, 0.5, 0.0, a),
                        Color(1.0, 0.0, 0.0, a),
                        Color(0.5, 0.0, 0.0, a)});
}

template<typename C>
Colormap<C> Colormap<C>::Hot(Scalar a)
{
    return Colormap<C>({Color(0.00, 0.00, 0.00, a),
                        Color(0.54, 0.00, 0.00, a),
                        Color(1.00, 0.08, 0.00, a),
                        Color(1.00, 0.62, 0.00, a),
                        Color(1.00, 1.00, 0.25, a),
                        Color(1.00, 1.00, 1.00, a)});
}

template<typename C>
Colormap<C> Colormap<C>::RGB(Scalar a)
{
    return Colormap<C>({Color(0.0, 0.0, 1.0, a),
                        Color(0.0, 1.0, 1.0, a),
                        Color(0.0, 1.0, 0.0, a),
                        Color(1.0, 1.0, 0.0, a),
                        Color(1.0, 0.0, 0.0, a)});
}

template<typename C>
Colormap<C> Colormap<C>::Bright24(Scalar a)
{
    Scalar c(0.6);
    Colormap<C> map = Colormap<C>({Colors<C>::Red(a),
                                   Colors<C>::Green(a),
                                   Colors<C>::Blue(a),
                                   Colors<C>::Cyan(a),
                                   Colors<C>::Magenta(a),
                                   Colors<C>::Yellow(a),
                                   Colors<C>::Orange(a),
                                   Colors<C>::Pink(a),
                                   Colors<C>::Chartreuse(a),
                                   Colors<C>::Purple(a),
                                   Colors<C>::Spring(a),
                                   Colors<C>::Sky(a),
                                   Colors<C>::Red(a),
                                   Colors<C>::Green(a),
                                   Colors<C>::Blue(a),
                                   Colors<C>::Cyan(a),
                                   Colors<C>::Magenta(a),
                                   Colors<C>::Yellow(a),
                                   Colors<C>::Orange(a),
                                   Colors<C>::Pink(a),
                                   Colors<C>::Chartreuse(a),
                                   Colors<C>::Purple(a),
                                   Colors<C>::Spring(a),
                                   Colors<C>::Sky(a)});
    for(int i=12; i<24; ++i)
        map.m_colors[i].template head<3>() *= c;
    return map;
}

template<typename C>
Colormap<C> Colormap<C>::Tab10(Scalar a)
{
    return Colormap<C>({Color(0.121569, 0.466667, 0.705882, a),
                        Color(1.000000, 0.498039, 0.054902, a),
                        Color(0.172549, 0.627451, 0.172549, a),
                        Color(0.839216, 0.152941, 0.156863, a),
                        Color(0.580392, 0.403922, 0.741176, a),
                        Color(0.549020, 0.337255, 0.294118, a),
                        Color(0.890196, 0.466667, 0.760784, a),
                        Color(0.498039, 0.498039, 0.498039, a),
                        Color(0.737255, 0.741176, 0.133333, a),
                        Color(0.090196, 0.745098, 0.811765, a)});
}

template<typename C>
Colormap<C> Colormap<C>::Tab20(Scalar a)
{
    return Colormap<C>({Color(0.121569, 0.466667, 0.705882, a),
                        Color(1.000000, 0.498039, 0.054902, a),
                        Color(0.172549, 0.627451, 0.172549, a),
                        Color(0.839216, 0.152941, 0.156863, a),
                        Color(0.580392, 0.403922, 0.741176, a),
                        Color(0.549020, 0.337255, 0.294118, a),
                        Color(0.890196, 0.466667, 0.760784, a),
//                        Color(0.498039, 0.498039, 0.498039, a), mixed up with default geometry color!
                        Color(0.737255, 0.741176, 0.133333, a),
                        Color(0.090196, 0.745098, 0.811765, a),
                        Color(0.682353, 0.780392, 0.909804, a),
                        Color(1.000000, 0.733333, 0.470588, a),
                        Color(0.596078, 0.874510, 0.541176, a),
                        Color(1.000000, 0.596078, 0.588235, a),
                        Color(0.772549, 0.690196, 0.835294, a),
                        Color(0.768627, 0.611765, 0.580392, a),
                        Color(0.968627, 0.713725, 0.823529, a),
//                        Color(0.780392, 0.780392, 0.780392, a), mixed up with default geometry color!
                        Color(0.858824, 0.858824, 0.552941, a),
                        Color(0.619608, 0.854902, 0.898039, a)});
}

template<typename C>
Colormap<C> Colormap<C>::Flash(Scalar a)
{
    return Colormap<C>({Color(0.121569, 0.466667, 0.705882, a),
                        Color(1.000000, 0.498039, 0.054902, a),
                        Color(0.172549, 0.627451, 0.172549, a),
                        Color(0.839216, 0.152941, 0.156863, a),
                        Color(0.580392, 0.403922, 0.741176, a),
                        Color(0.549020, 0.337255, 0.294118, a),
                        Color(0.890196, 0.466667, 0.760784, a),
//                        Color(0.498039, 0.498039, 0.498039, a), mixed up with default geometry color!
                        Color(0.737255, 0.741176, 0.133333, a),
                        Color(0.090196, 0.745098, 0.811765, a)});
}

template<typename C>
Colormap<C> Colormap<C>::Pastel(Scalar a)
{
    return Colormap<C>({Color(0.682353, 0.780392, 0.909804, a),
                        Color(1.000000, 0.733333, 0.470588, a),
                        Color(0.596078, 0.874510, 0.541176, a),
                        Color(1.000000, 0.596078, 0.588235, a),
                        Color(0.772549, 0.690196, 0.835294, a),
                        Color(0.768627, 0.611765, 0.580392, a),
                        Color(0.968627, 0.713725, 0.823529, a),
//                        Color(0.780392, 0.780392, 0.780392, a), mixed up with default geometry color!
                        Color(0.858824, 0.858824, 0.552941, a),
                        Color(0.619608, 0.854902, 0.898039, a)});
}

// =============================================================================

// BiColormap ------------------------------------------------------------------


template<typename C>
BiColormap<C>::BiColormap(const Colormap<C>& inf, const Colormap<C>& sup) :
    m_inf(inf),
    m_sup(sup)
{
}

// Accessors -------------------------------------------------------------------

template<typename C>
typename BiColormap<C>::Color BiColormap<C>::eval(Scalar t) const
{
    if(t <= Scalar(0))
    {
        return m_inf(-t);
    }
    else
    {
        return m_sup(+t);
    }
}

template<typename C>
typename BiColormap<C>::Color BiColormap<C>::eval(Scalar t, Scalar tabs) const
{
    if(t <= Scalar(0))
    {
        return m_inf(-t, tabs);
    }
    else
    {
        return m_sup(+t, tabs);
    }
}

template<typename C>
typename BiColormap<C>::Color BiColormap<C>::eval(Scalar t, Scalar tiso, Scalar tabs) const
{
    Scalar u    = t-tiso;
    Scalar umax = tabs-tiso;

    if(u <= Scalar(0))
    {
        return m_inf(-u, umax);
    }
    else
    {
        return m_sup(+u, umax);
    }
}

template<typename C>
typename BiColormap<C>::Color BiColormap<C>::operator()(Scalar t) const
{
    return this->eval(t);
}

template<typename C>
typename BiColormap<C>::Color BiColormap<C>::operator()(Scalar t, Scalar tabs) const
{
    return this->eval(t, tabs);
}

template<typename C>
typename BiColormap<C>::Color BiColormap<C>::operator()(Scalar t, Scalar tiso, Scalar tabs) const
{
    return this->eval(t, tiso, tabs);
}

// Static ----------------------------------------------------------------------

template<typename C>
BiColormap<C> BiColormap<C>::Jet(Scalar a)
{
    return BiColormap<C>(
                Colormap<C>({Color(1.0, 1.0, 1.0, a),
                             Color(0.0, 1.0, 1.0, a),
                             Color(0.0, 0.5, 1.0, a),
                             Color(0.0, 0.0, 1.0, a),
                             Color(0.0, 0.0, 0.5, a)}),
                Colormap<C>({Color(1.0, 1.0, 1.0, a),
                             Color(1.0, 1.0, 0.0, a),
                             Color(1.0, 0.5, 0.0, a),
                             Color(1.0, 0.0, 0.0, a),
                             Color(0.5, 0.0, 0.0, a)})
            );
}

// =============================================================================

// NormalColormap --------------------------------------------------------------

template<typename C>
NormalColormap<C>::NormalColormap(const std::array<Color,N>& colors) :
    m_colors(colors)
{
}

// Evaluation ------------------------------------------------------------------

template<typename C>
template<class Vector3T>
typename NormalColormap<C>::Color NormalColormap<C>::eval(const Vector3T& n) const
{
    int ncase = (n[2]>0) + 2*(n[1]>0) + 4*(n[0]>0);
    switch(ncase)
    {
    case 0: return SphereInterpolate(m_colors[LEFT ],-n[0], m_colors[DOWN],-n[1], m_colors[BACK ], -n[2]);
    case 1: return SphereInterpolate(m_colors[LEFT ],-n[0], m_colors[DOWN],-n[1], m_colors[FRONT], +n[2]);
    case 2: return SphereInterpolate(m_colors[LEFT ],-n[0], m_colors[UP  ],+n[1], m_colors[BACK ], -n[2]);
    case 3: return SphereInterpolate(m_colors[LEFT ],-n[0], m_colors[UP  ],+n[1], m_colors[FRONT], +n[2]);
    case 4: return SphereInterpolate(m_colors[RIGHT],+n[0], m_colors[DOWN],-n[1], m_colors[BACK ], -n[2]);
    case 5: return SphereInterpolate(m_colors[RIGHT],+n[0], m_colors[DOWN],-n[1], m_colors[FRONT], +n[2]);
    case 6: return SphereInterpolate(m_colors[RIGHT],+n[0], m_colors[UP  ],+n[1], m_colors[BACK ], -n[2]);
    case 7: return SphereInterpolate(m_colors[RIGHT],+n[0], m_colors[UP  ],+n[1], m_colors[FRONT], +n[2]);
    default: return Colors<Color>::Glitch();
    }
}

template<typename C>
template<class Vector3T>
typename NormalColormap<C>::Color NormalColormap<C>::operator() (const Vector3T& n) const
{
    return this->eval(n);
}

// Internal --------------------------------------------------------------------

template<typename C>
typename NormalColormap<C>::Color NormalColormap<C>::SphereInterpolate(const Color& cx, Scalar x,
                                                                       const Color& cy, Scalar y,
                                                                       const Color& cz, Scalar z)
{
    if(x<=Scalar(0)) x = Scalar(0);
    if(y<=Scalar(0)) y = Scalar(0);
    if(z<=Scalar(0)) z = Scalar(0);
    if(x>=Scalar(1)) return cx;
    if(y>=Scalar(1)) return cy;
    if(z>=Scalar(1)) return cz;

    return Colors<C>::Interpolate(cx, 0.5*M_PI-std::acos(x),
                                  cy, 0.5*M_PI-std::acos(y),
                                  cz, 0.5*M_PI-std::acos(z));
}

// Creation --------------------------------------------------------------------

template<typename C>
NormalColormap<C> NormalColormap<C>::RGB(Scalar a)
{
    return NormalColormap<C>(
    {
        Colors<C>::Red(a),   // UP
        Colors<C>::Red(a),   // DOWN
        Colors<C>::Green(a), // LEFT
        Colors<C>::Green(a), // RIGHT
        Colors<C>::Blue(a),  // FRONT
        Colors<C>::Blue(a)   // BACK
    });
}

template<typename C>
NormalColormap<C> NormalColormap<C>::CYM(Scalar a)
{
    return NormalColormap<C>(
    {
        Colors<C>::Cyan(a),    // UP
        Colors<C>::Cyan(a),    // DOWN
        Colors<C>::Yellow(a),  // LEFT
        Colors<C>::Yellow(a),  // RIGHT
        Colors<C>::Magenta(a), // FRONT
        Colors<C>::Magenta(a)  // BACK
    });
}

template<typename C>
NormalColormap<C> NormalColormap<C>::RGBCYM(Scalar a)
{
    return NormalColormap<C>(
    {
        Colors<C>::Red(a),    // UP
        Colors<C>::Yellow(a), // DOWN
        Colors<C>::Green(a),  // LEFT
        Colors<C>::Cyan(a),   // RIGHT
        Colors<C>::Blue(a),   // FRONT
        Colors<C>::Magenta(a) // BACK
    });
}

} // namespace color

// default alias
using Color          = Eigen::Matrix<Scalar,4,1>;
using Colors         = color::Colors<Color>;
using Colormap       = color::Colormap<Color>;
using BiColormap     = color::BiColormap<Color>;
using NormalColormap = color::NormalColormap<Color>;
using ColorArray     = std::vector<Color, Eigen::aligned_allocator<Color>>;

} // namespace pdpc
