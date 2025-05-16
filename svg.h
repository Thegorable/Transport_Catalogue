#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <limits>
#include <type_traits>
#include <sstream>

namespace svg {

using namespace std::literals;

template<class T>
concept ToOstream = requires(std::ostream& out, T value) {
    out << value;
};

template<class T>
concept ToString = requires(T value) {
    std::to_string(value);
};

template<class T>
concept StringConvertible = ToString<T> ||
std::is_convertible<T, std::string>::value ||
std::constructible_from<T, std::string> ||
std::same_as<T, char>;

template <std::floating_point T>
class FloatConverter {
public:
    FloatConverter() = default;

    std::string operator () (T value) {
        std::string result;
        converter_ << value;
        converter_ >> result;
        converter_.clear();
        return result;
    }

private:
    std::stringstream converter_;
};

template <class T>
std::string ToStringGeneric(const T& val) {
    if constexpr (std::is_convertible_v<T, std::string>) {
        return val;
    }
    if constexpr (ToString<T>) {
        return std::to_string(val);
    }
    
    throw std::logic_error("ToStringGeneric was unsuccessful"s);
};

struct Rgb {
    Rgb();
    Rgb(uint8_t r, uint8_t g, uint8_t b);
    Rgb(const Rgb& other);

    Rgb& operator =(const Rgb& other);
    operator std::string() const;

    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct Rgba : public Rgb {
    Rgba();
    Rgba(uint8_t r, uint8_t g, uint8_t b, double o);
    Rgba(const Rgba& other);

    Rgba& operator =(const Rgba& other);
    operator std::string() const;

    double opacity = 1.0;
};

std::ostream& operator <<(std::ostream& out, const Rgb& color);
std::ostream& operator <<(std::ostream& out, const Rgba& color);

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

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
inline const Color NoneColor{"none"};

std::ostream& operator <<(std::ostream& out, const Color& color);

inline bool IsEqualDouble(double l, double r);

struct Point {
    Point();
    Point(double x, double y);
    double x = 0;
    double y = 0;
};

struct RenderContext {
    using OstreamRef = std::reference_wrapper<std::ostream>;
    using StringRef = std::reference_wrapper<std::string>;
    using OutVar = std::variant<OstreamRef, StringRef>;

    RenderContext(const OutVar& out);
    RenderContext(const OutVar& out, int indent_step, int indent = 0);
    RenderContext Indented() const;
    void RenderIndent() const; 

    template <class T>
    RenderContext& operator <<(const T& value)
    requires ToOstream<T> && (StringConvertible<T> || std::is_enum_v<T>) {
        if (std::holds_alternative<OstreamRef>(value_)) {
            get<OstreamRef>(value_).get() << value;
        }
        else if (std::holds_alternative<StringRef>(value_)) {
            get<StringRef>(value_).get() += ToStringGeneric(value);
        }

        return *this;
    };

    std::size_t index() const { return value_.index(); }

private:
    OutVar value_;
    // std::stringstream converter_;
    int indent_step = 0;
    int indent = 0; 
};

template <ToOstream ValueType>
struct AttributeValue {
    AttributeValue(const std::string& attr_name, const ValueType& val) :
    attribute_name_(attr_name), value_(val) {}

    std::string attribute_name_;
    ValueType value_;
};

template <ToOstream ValueType>
void WriteAttribute(RenderContext& ctx, const AttributeValue<ValueType>& attr_val) {
    ctx << ' ' << attr_val.attribute_name_ << "=\"" << attr_val.value_ << '\"';
};

template <ToOstream ValueType>
void WriteAttribute(RenderContext& ctx, const AttributeValue<ValueType>& attr_val, bool is_valid) {
    if (is_valid) {
        WriteAttribute(ctx, attr_val);
    }
}; 

class Object {
public:
    virtual void Render(RenderContext& context) const;
    virtual ~Object() = default;

private:
    virtual void RenderObject(RenderContext& context) const = 0;
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
    void WriteBasicAttrs(RenderContext& out) const {
        WriteAttribute<const Color&>(out, {"fill", fill_color_}, !holds_alternative<std::monostate>(fill_color_));
        WriteAttribute<const Color&>(out, {"stroke", stroke_color_}, !holds_alternative<std::monostate>(stroke_color_));
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
    void RenderObject(RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};


class Polyline final : public Object, public ObjectProperties<Polyline> {
public:
    Polyline() = default;
    Polyline(const Polyline& other) = default;
    Polyline& AddPoint(Point point);

private:
    void RenderObject(RenderContext& ctx) const override;

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
    void RenderObject(RenderContext& ctx) const override;

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
    void Render(std::string& str) const;

private:
    template<class T>
    void RenderObjects(T& out) const 
    requires std::derived_from<T, std::ostream> || std::is_convertible_v<T, std::string> {
        RenderContext props(out, 1, 2);
        props << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"s;
        props << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"s;

        for(auto& object_ptr : objects_ptrs_) {
            object_ptr->Render(props);
        }

        props << "</svg>"s;
    }   

    std::vector<std::unique_ptr<Object>> objects_ptrs_;
};

class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;
    virtual ~Drawable() = default;
};

Polyline CreateStar(Point center, double outer_rad, double inner_rad, int num_rays);

}