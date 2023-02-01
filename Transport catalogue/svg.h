#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

using namespace std::literals;

namespace svg {
struct Rgb {
        Rgb() = default;
        Rgb(uint8_t red, uint8_t green, uint8_t blue)
            : red(red), green(green), blue(blue)
        {
        }
        uint8_t red=0;
        uint8_t green=0;
        uint8_t blue=0;
};

struct Rgba {
    Rgba() = default;
    Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity)
        : red(red), green(green), blue(blue),opacity(opacity)
    {
    }
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
};

 using Color = std::variant<std::monostate, std::string, svg::Rgb , svg::Rgba>;
 inline const Color NoneColor{ "none" };

 struct PrintColor
 {
     void operator()(std::monostate) const {
         std::cout << "none" ;
     }
     void operator()(std::string str) const {
         std::cout << str ;
     }
     void operator()(svg::Rgb rgb) const {
         std::cout <<"rgb("<< static_cast<int>(rgb.red) <<"," << static_cast<int>(rgb.green) <<"," << static_cast<int>(rgb.blue) << ")";     //static_cast<uint16_t>
     }
     void operator()(svg::Rgba rgba) const {
         std::cout << "rgba(" << static_cast<int>(rgba.red)<<","<< static_cast<int>(rgba.green)<<","<< static_cast<int>(rgba.blue)<<","<<rgba.opacity << ")";
     }
 };

 std::ostream& operator<<(std::ostream& out, const svg::Color& color);


 enum class StrokeLineCap {
     BUTT,
     ROUND,
     SQUARE,
 };

 enum class StrokeLineJoin {
     ARCS,
     BEVEL,
     MITER,
     MITER_CLIP,
     ROUND,
 };

struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};



struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
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

class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

std::ostream& operator<<(std::ostream& out, const svg::StrokeLineCap& info);
std::ostream& operator<<(std::ostream& out, const svg::StrokeLineJoin& info);

template <typename Owner>
class PathProps {
public:

    Owner& SetFillColor(Color fill_color) {
        fill_color_ = fill_color;
        return AsOwner();
    }

    Owner& SetStrokeColor(Color stroke_color) {
        stroke_color_ = stroke_color;
        return AsOwner();
    }

    Owner& SetStrokeWidth(double width) {
        width_ = width;
        return AsOwner();
    }

    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_ = line_cap;
        return AsOwner();
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_ = line_join;
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_) {
            out << "fill=\""sv << *fill_color_ << "\" "sv;
        }
        if (stroke_color_) {
            out << "stroke=\""sv << *stroke_color_ << "\" "sv;
        }
        if (width_) {
            out << "stroke-width=\""sv << *width_ << "\" "sv;
        }
        if (line_cap_) {
            out << "stroke-linecap=\""sv << *line_cap_ << "\" "sv;
        }
        if (line_join_) {
            out << "stroke-linejoin=\""sv << *line_join_ << "\" "sv;
        }
    }

private:
    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;

};


class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

class Polyline : public Object, public PathProps<Polyline> {
public:
    Polyline& AddPoint(Point point);

private:
    void RenderObject(const RenderContext& context) const override;
    std::vector<Point> point_;
};

class Text : public Object, public PathProps<Text> {
public:
    Text& SetPosition(Point pos);
    Text& SetOffset(Point offset);
    Text& SetFontSize(uint32_t size);
    Text& SetFontFamily(std::string font_family);
    Text& SetFontWeight(std::string font_weight);
    Text& SetData(std::string data);

private:   
    void RenderObject(const RenderContext& context) const override;
      
    Point pos_{ 0.0, 0.0 };
    Point offset_{ 0.0, 0.0 };
    uint32_t size_=1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_{};
};

class ObjectContainer {
public:

    template <typename T>
    void Add(T obj) {
        AddPtr(std::make_unique<T>(std::move(obj)));
    }

    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
protected:
    ~ObjectContainer() = default;
};

class Drawable {
public:
    virtual void Draw(svg::ObjectContainer& container) const = 0;

    virtual ~Drawable() = default;
};

class Document : public ObjectContainer {
public:

    void AddPtr(std::unique_ptr<Object>&& obj) override;
    void Render(std::ostream& out) const;

private:
    std::vector<std::unique_ptr<Object>> objects_;
};

}  

