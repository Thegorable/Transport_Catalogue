#define _USE_MATH_DEFINES 
#include <cmath>
#include <sstream>

#include "svg.h"

namespace svg {

using namespace std;

ostream& operator <<(ostream& out, const Color& color) {
    if (holds_alternative<monostate>(color)) {
        out << NoneColor;
        return out;
    }
    visit([&out] (const auto& value) {out << value; }, color);

    return out;
}

ostream &operator<<(ostream &out, const Rgb &color) {
    out << "rgb(" << unsigned(color.red) << ',' << unsigned(color.green) << ',' << unsigned(color.blue) << ')';
    return out;
}

ostream &operator<<(ostream &out, const Rgba &color) {
    out << "rgba(" << unsigned(color.red) << ',' << unsigned(color.green) << ',' << unsigned(color.blue) << ',' << color.opacity << ')';
    return out;
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

void Object::Render(const RenderProperties& context) const {
    context.RenderIndent();
    RenderObject(context);
    context.out << std::endl;
}

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderProperties& context) const {
    auto& out = context.out;
    out << "<circle";
    WriteAttribute<double>(out, {"cx", center_.x});
    WriteAttribute<double>(out, {"cy", center_.y});
    WriteAttribute<double>(out, {"r", radius_});
    WriteBasicAttrs(out);
    out << "/>"sv;
}

Polyline& Polyline::AddPoint(Point p) {
    points_.push_back(p);
    return *this;
}

void Polyline::RenderObject(const RenderProperties& ctx) const {
    auto& out = ctx.out;
    out << "<polyline";
    stringstream points_value;

    if (!points_.empty()) {
        auto it = points_.begin();
        points_value << it->x << ',' << it->y;
        for (++it; it != points_.end(); ++it) {
            points_value << ' ' << it->x << ',' << it->y;
        }
    }
    WriteAttribute<const string&>(out, {"points", points_value.str()});
    WriteBasicAttrs(out);

    out << "/>";

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

void Text::RenderObject(const RenderProperties& ctx) const {
    auto& out = ctx.out;
    out << "<text";
    WriteBasicAttrs(out);
    WriteAttribute<double>(out, {"x"s, position_.x});
    WriteAttribute<double>(out, {"y"s, position_.y});
    WriteAttribute<double>(out, {"dx"s, offset_.x});
    WriteAttribute<double>(out, {"dy"s, offset_.y});
    WriteAttribute<uint32_t>(out, {"font-size"s, font_size_});
    WriteAttribute<const string&>(out, {"font-family"s, font_family_}, !font_family_.empty());
    WriteAttribute<const string&>(out, {"font-weight"s, font_weight_}, !font_weight_.empty());
    out << '>' << data_ << "</text>";
}

void Document::AddObjectPtr(std::unique_ptr<Object>&& obj_ptr) {
    objects_ptrs_.push_back(move(obj_ptr));
}

void Document::Render(ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
    
    RenderProperties ctx(out, 1, 2);
    for(auto& object_ptr : objects_ptrs_) {
        object_ptr->Render(ctx);
    }

    out << "</svg>"sv;
}

void Document::Render(std::string &str) const {
    str += "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\\n"s +
    "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\\n"s;

    str += "</svg>\\n"s;
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
}

}