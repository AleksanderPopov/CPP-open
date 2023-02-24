#define _USE_MATH_DEFINES
#include "svg.h"

#include <cmath>
#include <cassert>


using namespace std::literals;

namespace shapes {
    class Star : public svg::Drawable {
    public:
        Star(svg::Point center, double outer_radius, double inner_radius, int num_rays)
            : centre_(center), outer_radius_(outer_radius), inner_radius_(inner_radius), num_rays_(num_rays)
        {
        }

        void Draw(svg::ObjectContainer& container) const override {
            svg::Polyline polyline;
            for (int i = 0; i <= num_rays_; ++i) {
                double angle = 2 * M_PI * (i % num_rays_) / num_rays_;
                polyline.AddPoint({ centre_.x + outer_radius_ * sin(angle), centre_.y - outer_radius_ * cos(angle) });
                if (i == num_rays_) {
                    break;
                }
                angle += M_PI / num_rays_;
                polyline.AddPoint({ centre_.x + inner_radius_ * sin(angle), centre_.y - inner_radius_ * cos(angle) });
            }
            container.Add(polyline .SetFillColor("red").SetStrokeColor("black"));
        }

    private:
        svg::Point centre_;
        double outer_radius_;
        double inner_radius_;
        int num_rays_;

    };
    class Snowman : public svg::Drawable {
    public:
        Snowman(svg::Point head_center, double head_radius) :
            head_center_(head_center), head_radius_(head_radius)
        {

        }
        void Draw(svg::ObjectContainer& container) const override {
            container.Add(svg::Circle().SetCenter({ head_center_.x, (head_center_.y + 5 * head_radius_) }).SetRadius(head_radius_ * 2).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
            container.Add(svg::Circle().SetCenter({ head_center_.x, (head_center_.y + 2 * head_radius_) }).SetRadius(head_radius_ * 1.5).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
            container.Add(svg::Circle().SetCenter({ head_center_.x, head_center_.y }).SetRadius(head_radius_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        }

    private:
        svg::Point head_center_;
        double head_radius_;

    };

    class Triangle : public svg::Drawable {
    public:
        Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
            : p1_(p1)
            , p2_(p2)
            , p3_(p3) {
        }

        void Draw(svg::ObjectContainer& container) const override {
            container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
        }

    private:
        svg::Point p1_, p2_, p3_;
    };
}

template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target) {
    for (auto it = begin; it != end; ++it) {
        (*it)->Draw(target);
    }
}

template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target) {
    using namespace std;
    DrawPicture(begin(container), end(container), target);
}

int main() {

    using namespace svg;
    using namespace shapes;
    using namespace std;

    //// Задаёт цвет в виде трех компонент в таком порядке: red, green, blue
    //svg::Rgb rgb{ 255, 0, 100 };
    //assert(rgb.red == 255);
    //assert(rgb.green == 0);
    //assert(rgb.blue == 100);

    //// Задаёт цвет по умолчанию: red=0, green=0, blue=0
    //svg::Rgb color;
    //assert(color.red == 0 && color.green == 0 && color.blue == 0);

    //// Задаёт цвет в виде четырёх компонент: red, green, blue, opacity
    //svg::Rgba rgba{ 100, 20, 50, 0.3 };
    //assert(rgba.red == 100);
    //assert(rgba.green == 20);
    //assert(rgba.blue == 50);
    //assert(rgba.opacity == 0.3);

    //// Чёрный непрозрачный цвет: red=0, green=0, blue=0, alpha=1.0
    //svg::Rgba color2;
    //assert(color2.red == 0 && color2.green == 0 && color2.blue == 0 && color2.opacity == 1.0);


    vector<unique_ptr<svg::Drawable>> picture;

    picture.emplace_back(make_unique<Triangle>(svg::Point{ 100, 20 }, svg::Point{ 120, 50 }, svg::Point{ 80, 40 }));
    picture.emplace_back(make_unique<Star>(Point{ 50.0, 20.0 }, 10.0, 4.0, 5));
    picture.emplace_back(make_unique<Snowman>(Point{ 30, 20 }, 10.0));

    svg::Document doc;
    DrawPicture(picture, doc);

    const Text base_text =  //
    Text()
    .SetFontFamily("Verdana"s)
    .SetFontSize(12)
    .SetPosition({ 10, 100 })
    .SetData("Happy New Year!"s);
doc.Add(Text{ base_text }
    .SetStrokeColor("yellow"s)
    .SetFillColor("yellow"s)
    .SetStrokeLineJoin(StrokeLineJoin::ROUND)
    .SetStrokeLineCap(StrokeLineCap::ROUND)
    .SetStrokeWidth(3));
doc.Add(Text{ base_text }.SetFillColor("red"s));

    // Выводим полученный документ в stdout
    //cout << endl << endl;
    doc.Render(cout);

    //Color none_color;
    //cout << none_color << endl; // none

    //Color purple{ "purple"s };
    //cout << purple << endl; // purple

    //Color rgb = Rgb{ 100, 200, 255 };
    //cout << rgb << endl; // rgb(100,200,255)

    //Color rgba = Rgba{ 100, 200, 255, 0.5 };
    //cout << rgba << endl; // rgba(100,200,255,0.5)

    //   Circle c;
    //c.SetRadius(53.5).SetCenter({50.0, 100.0});
    //c.SetFillColor(rgba);
    //c.SetStrokeColor(purple);
    //
    //Document doc;
    //doc.Add(std::move(c));
    //doc.Render(cout);

}
