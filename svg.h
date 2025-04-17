#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <limits>

inline bool IsEqualDouble(double l, double r) {
    return std::abs(l - r) < std::numeric_limits<double>::epsilon();
}

namespace svg {

struct Rgb {
    Rgb() = default;
    Rgb(uint8_t r, uint8_t g, uint8_t b) :
    red(r), green(g), blue(b) {}
    Rgb(const Rgb& other) : 
    red(other.red), green(other.green), blue(other.blue) {}

    Rgb& operator =(const Rgb& other) {
        red = other.red;
        green = other.green;
        blue = other.blue;
        return *this;
    }

    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct Rgba : public Rgb {
    Rgba() = default;
    Rgba(uint8_t r, uint8_t g, uint8_t b, double o) :
    Rgb(r, g, b), opacity(o) {}
    Rgba(const Rgba& other) : 
    Rgb(other.red, other.green, other.blue), opacity(other.opacity) {}

    Rgba& operator =(const Rgba& other) {
        red = other.red;
        green = other.green;
        blue = other.blue;
        opacity = other.opacity;
        return *this;
    }

    double opacity = 1.0;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

std::ostream& operator <<(std::ostream& out, const Color& color);
std::ostream& operator <<(std::ostream& out, const Rgb& color);
std::ostream& operator <<(std::ostream& out, const Rgba& color);

inline const Color NoneColor{"none"};

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
    NONE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
    NONE,
};

std::ostream& operator <<(std::ostream& o, StrokeLineCap in);
std::ostream& operator <<(std::ostream& o, StrokeLineJoin in);

struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

struct RenderProperties {
    RenderProperties(std::ostream& out)
        : out(out) {
    }

    RenderProperties(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderProperties Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0; 
};

template <typename ValueType>
struct AttributeValue {
    AttributeValue(const std::string& attr_name, const ValueType& val) :
    attribute_name_(attr_name), value_(val) {}

    std::string attribute_name_;
    ValueType value_;
};

template <typename ValueType>
void WriteAttribute(std::ostream& out, const AttributeValue<ValueType>& attr_val, bool is_valid) {
    if (is_valid) {
        out << ' ' << attr_val.attribute_name_ << "=\"" << attr_val.value_ << '\"';
    }
} 

template <typename ValueType>
void WriteAttribute(std::ostream& out, const AttributeValue<ValueType>& attr_val) {
    out << ' ' << attr_val.attribute_name_ << "=\"" << attr_val.value_ << '\"';
}

class Object {
public:
    virtual void Render(const RenderProperties& context) const;
    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderProperties& context) const = 0;
};

template<typename Owner>
class ObjectProperties {
public:
    Owner& SetFillColor(const Color& color) {fill_color_ = color; return AsOwner();}
    Owner& SetStrokeColor(const Color& color) {stroke_color_ = color; return AsOwner();}
    Owner& SetStrokeWidth(double width) {stroke_width_ = width; return AsOwner();}
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {line_cap_ = line_cap; return AsOwner();}
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {line_join_ = line_join; return AsOwner();}
protected:
    void WriteBasicAttrs(std::ostream& out) const {
        WriteAttribute<const Color&>(out, {"fill", fill_color_}, !std::holds_alternative<std::monostate>(fill_color_));
        WriteAttribute<const Color&>(out, {"stroke", stroke_color_}, !std::holds_alternative<std::monostate>(stroke_color_));
        WriteAttribute<double>(out, {"stroke-width", stroke_width_}, !IsEqualDouble(stroke_width_, 0.0));
        WriteAttribute<StrokeLineCap> (out, {"stroke-linecap", line_cap_}, line_cap_ != StrokeLineCap::NONE);
        WriteAttribute<StrokeLineJoin> (out, {"stroke-linejoin", line_join_}, line_join_ != StrokeLineJoin::NONE);
    }

    Color fill_color_;
    Color stroke_color_;
    double stroke_width_ = 0.0;
    StrokeLineCap line_cap_ = StrokeLineCap::NONE;
    StrokeLineJoin line_join_ = StrokeLineJoin::NONE;

private:
    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }
};


class Circle final : public Object, public ObjectProperties<Circle> {
public:
    Circle() = default;
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderProperties& context) const override;

    Point center_;
    double radius_ = 1.0;
};


class Polyline final : public Object, public ObjectProperties<Polyline> {
public:
    Polyline() = default;
    Polyline& AddPoint(Point point);

private:
    void RenderObject(const RenderProperties& ctx) const override;

    std::vector<Point> points_;
};


class Text final : public Object, public ObjectProperties<Text> {
public:
    Text() = default;
    Text& SetPosition(Point pos);
    Text& SetOffset(Point offset);
    Text& SetFontSize(uint32_t size);
    Text& SetFontFamily(const std::string& font_family);
    Text& SetFontWeight(const std::string& font_weight);
    Text& SetData(const std::string& data);

private:
    void RenderObject(const RenderProperties& ctx) const override;

    Point position_;
    Point offset_;
    uint32_t font_size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;
};

class ObjectContainer {
public:
    template<typename T>
    void AddObject(T object) {
        AddObjectPtr(std::make_unique<T>(std::move(object)));
    }
    virtual void AddObjectPtr(std::unique_ptr<Object>&& object_ptr) = 0;

    virtual ~ObjectContainer() = default;
};

class Document : public ObjectContainer {
public:
    void AddObjectPtr(std::unique_ptr<Object>&& obj_ptr) override;

    void Render(std::ostream& out) const;

private:
    std::vector<std::unique_ptr<Object>> objects_ptrs_;
};

class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;
    virtual ~Drawable() = default;
};

Polyline CreateStar(Point center, double outer_rad, double inner_rad, int num_rays);

}