#define _USE_MATH_DEFINES 
#include <cmath>
#include <sstream>

#include "svg.h"

namespace svg {

using namespace std;

FloatConverter<double> float_converter;

ostream& operator <<(ostream& out, const Color& color) {
    if (holds_alternative<monostate>(color)) {
        out << NoneColor;
        return out;
    }
    visit([&out] (const auto& value) {out << value; }, color);

    return out;
}

template <>
std::string ToStringGeneric(const double& val) {
    return float_converter(val);
}

ostream &operator<<(ostream &out, const Rgb &color) {
    out << "rgb(" << unsigned(color.red) << ',' << unsigned(color.green) << ',' << unsigned(color.blue) << ')';
    return out;
}

ostream &operator<<(ostream &out, const Rgba &color) {
    out << "rgba(" << unsigned(color.red) << ',' << unsigned(color.green) << ',' << unsigned(color.blue) << ',' << color.opacity << ')';
    return out;
}

bool IsEqualDouble(double l, double r) {
    return std::abs(l - r) < std::numeric_limits<double>::epsilon();
}

ostream &operator<<(ostream &o, StrokeLineCap in) {
    switch (in)
    {
    case StrokeLineCap::BUTT:
        o << "butt";
        break;

    case StrokeLineCap::ROUND:
        o << "round";
        break;

    case StrokeLineCap::SQUARE:
        o << "square";
        break;

    case StrokeLineCap::NONE:
        o << "none";
        break;
    }
    return o;
}

ostream& operator <<(ostream& o, StrokeLineJoin in) {
    switch (in)
    {
    case StrokeLineJoin::ARCS:
        o << "arcs"s;
        break;
    case StrokeLineJoin::BEVEL:
        o << "bevel"s;
        break;
    case StrokeLineJoin::MITER:
        o << "miter"s;
        break;
    case StrokeLineJoin::MITER_CLIP:
        o << "miter-clip"s;
        break;
    case StrokeLineJoin::ROUND:
        o << "round"s;
        break;
    case StrokeLineJoin::NONE:
        o << "none";
        break;
    }
    return o;
}

void Object::Render(RenderContext& context) const {
    context.RenderIndent();
    RenderObject(context);
    context << '\n';
}

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(RenderContext& context) const {
    context << "<circle";
    WriteAttribute<double>(context, {"cx", center_.x});
    WriteAttribute<double>(context, {"cy", center_.y});
    WriteAttribute<double>(context, {"r", radius_});
    WriteBasicAttrs(context);
    context << "/>"s;
}

Polyline& Polyline::AddPoint(Point p) {
    points_.push_back(p);
    return *this;
}

void Polyline::RenderObject(RenderContext& ctx) const {
    ctx << "<polyline";
    stringstream points_value;

    if (!points_.empty()) {
        auto it = points_.begin();
        points_value << it->x << ',' << it->y;
        for (++it; it != points_.end(); ++it) {
            points_value << ' ' << it->x << ',' << it->y;
        }
    }
    WriteAttribute<const string&>(ctx, {"points", points_value.str()});
    WriteBasicAttrs(ctx);

    ctx << "/>";

}

Text& Text::SetPosition(Point p) {
    position_ = p;
    return *this;
}

Text& Text::SetOffset(Point p) {
    offset_ = p;
    return *this;
}

Text& Text::SetFontSize(uint32_t size) {
    font_size_ = size;
    return *this;
}

Text& Text::SetFontFamily(const string& font) {
    font_family_ = font;
    return *this;
}

Text& Text::SetFontWeight(const string& weight) {
    font_weight_ = weight;
    return *this;
}

Text& Text::SetData(const string& data) {
    data_ = data;
    return *this;
}

void Text::RenderObject(RenderContext& ctx) const {
    ctx << "<text";
    WriteBasicAttrs(ctx);
    WriteAttribute<double>(ctx, {"x"s, position_.x});
    WriteAttribute<double>(ctx, {"y"s, position_.y});
    WriteAttribute<double>(ctx, {"dx"s, offset_.x});
    WriteAttribute<double>(ctx, {"dy"s, offset_.y});
    WriteAttribute<uint32_t>(ctx, {"font-size"s, font_size_});
    WriteAttribute<const string&>(ctx, {"font-family"s, font_family_}, !font_family_.empty());
    WriteAttribute<const string&>(ctx, {"font-weight"s, font_weight_}, !font_weight_.empty());
    ctx << '>' << data_ << "</text>";
}

void Document::AddObjectPtr(std::unique_ptr<Object>&& obj_ptr) {
    objects_ptrs_.push_back(move(obj_ptr));
}

void Document::Render(ostream& out) const {
    RenderObjects<ostream>(out);
}

void Document::Render(string& out) const {
    RenderObjects<string>(out);
}

Polyline CreateStar(Point center, double outer_rad, double inner_rad, int num_rays) {
    using namespace svg;
    Polyline polyline;
    for (int i = 0; i <= num_rays; ++i) {
        double angle = 2 * M_PI * (i % num_rays) / num_rays;
        polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
        if (i == num_rays) {
            break;
        }
        angle += M_PI / num_rays;
        polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
    }
    return polyline;
};

Rgb::Rgb() = default;
Rgb::Rgb(uint8_t r, uint8_t g, uint8_t b) :
red(r), green(g), blue(b) {}
Rgb::Rgb(const Rgb& other) : 
red(other.red), green(other.green), blue(other.blue) {}

Rgb& Rgb::operator =(const Rgb& other) {
    red = other.red;
    green = other.green;
    blue = other.blue;
    return *this;
}

Rgb::operator std::string() const {
    return "rgb("s + ToStringGeneric(red) + ',' + ToStringGeneric(green) + 
    ',' + ToStringGeneric(blue) + ')';
}

Rgba::Rgba() = default;
Rgba::Rgba(uint8_t r, uint8_t g, uint8_t b, double o) :
Rgba::Rgb(r, g, b), opacity(o) {}
Rgba::Rgba(const Rgba& other) : 
Rgba::Rgb(other.red, other.green, other.blue), opacity(other.opacity) {}

Rgba& Rgba::operator =(const Rgba& other) {
    red = other.red;
    green = other.green;
    blue = other.blue;
    opacity = other.opacity;
    return *this;
}

Rgba::operator std::string() const {
    return "rgba("s + ToStringGeneric(red) + ',' + ToStringGeneric(green) + 
    ',' + ToStringGeneric(blue) + ',' + 
    ToStringGeneric(opacity) + ')';
}

Point::Point() = default;
Point::Point(double x, double y)
    : x(x)
    , y(y) {
}

RenderContext::RenderContext(const OutVar& out)
    : value_(out) {
}

RenderContext::RenderContext(const OutVar& out, int indent_step, int indent)
    : value_(out)
    , indent_step(indent_step)
    , indent(indent) {
}

RenderContext RenderContext::Indented() const {
    return {value_, indent_step, indent + indent_step};
}

void RenderContext::RenderIndent() const {
    if (std::holds_alternative<OstreamRef>(value_)) {
        auto& stream = get<OstreamRef>(value_).get();
        for (int i = 0; i < indent; ++i) {
            stream.put(' ');
        }
    }
    else if (std::holds_alternative<StringRef>(value_)) {
        get<StringRef>(value_).get() += std::string(indent, ' ');
    }
}

template <>
RenderContext& RenderContext::operator <<(const double& value)  {
    if (std::holds_alternative<OstreamRef>(value_)) {
        get<OstreamRef>(value_).get() << value;
    }
    else if (std::holds_alternative<StringRef>(value_)) {
        get<StringRef>(value_).get() += float_converter(value);
    }

    return *this;
}

template<>
std::string ToStringGeneric<Rgb>(const Rgb& val) { return val; }
template<>
std::string ToStringGeneric<Rgba>(const Rgba& val) { return val; }

struct ColorVisitor {
    std::string operator ()(std::monostate) {
        return "none"s;
    }

    std::string operator ()(const Rgb &color) {
        return color;
    }

    std::string operator ()(const Rgba &color) {
        return color;
    }

    std::string operator ()(const std::string &color) {
        return color;
    }
};

template <>
std::string ToStringGeneric(const Color &color) {
    return std::visit(ColorVisitor{}, color);
}

template <>
std::string ToStringGeneric(const char& sym) {
    return std::string(1, sym);
}

template <>
std::string ToStringGeneric(const StrokeLineCap &val) {
    switch (val)
    {
    case StrokeLineCap::BUTT:
        return "butt";
        break;

    case StrokeLineCap::ROUND:
        return "round";
        break;

    case StrokeLineCap::SQUARE:
        return "square";
        break;

    case StrokeLineCap::NONE:
        return "none";
        break;
    }
    
    throw std::logic_error("The StrokeLineCap conversion to string was unsuccessful"s);
    return {};
}

template <>
std::string ToStringGeneric(const StrokeLineJoin &val) {
    switch (val)
    {
    case StrokeLineJoin::ARCS:
        return "arcs"s;
        break;
    case StrokeLineJoin::BEVEL:
        return "bevel"s;
        break;
    case StrokeLineJoin::MITER:
        return "miter"s;
        break;
    case StrokeLineJoin::MITER_CLIP:
        return "miter-clip"s;
        break;
    case StrokeLineJoin::ROUND:
        return "round"s;
        break;
    case StrokeLineJoin::NONE:
        return "none";
        break;
    }
    
    throw std::logic_error("The StrokeLineJoin conversion to string was unsuccessful"s);
    return {};
}

}