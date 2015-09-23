
namespace nv {

// generic tweak
template < typename T >
struct Tweak
{
    std::string label;
    T value;
    T min;
    T max;
    T step;
    
    Tweak( const char *_label, const T& _value, const T& _step )
        : label(_label), value(_value), min(T()), max(T()), step(_step) {}
    
    Tweak( const char *_label, const T& _value, const T& _step, const T& _min, const T& _max )
        : label(_label), value(_value), min(_min), max(_max), step(_step) {}
    
    ~Tweak( ) {}
    
    void doTweak( )
    {
        m_widgets.doLabel(nv::Rect(), label.c_str());
        //~ if(m_widgets.doButton
    }
    
};


namespace tweak {
typedef Tweak<bool> bscalar;
typedef Tweak<int> scalar;
typedef Tweak<unsigned int> uscalar;
typedef Tweak<float> fscalar;

typedef Tweak<gk::glsl::vec2> vec2;    
typedef Tweak<gk::glsl::vec3> vec3;    
typedef Tweak<gk::glsl::vec4> vec4;    
}       // namespace

}       // namespace
