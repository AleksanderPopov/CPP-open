#include "svg.h"

namespace svg {

using namespace std::literals;
using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;

std::ostream& operator<<(std::ostream& out, const svg::StrokeLineCap& info) {
    switch (info) {
    case svg::StrokeLineCap::BUTT:   out << "butt"; break;
    case svg::StrokeLineCap::ROUND: out <<  "round"; break;
    case svg::StrokeLineCap::SQUARE: out <<"square" ; break;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const svg::StrokeLineJoin& info) {   
    switch (info) {
    case svg::StrokeLineJoin::ARCS: out <<"arcs" ; break;
    case svg::StrokeLineJoin::BEVEL: out << "bevel"; break;
    case svg::StrokeLineJoin::MITER: out << "miter"; break;
    case svg::StrokeLineJoin::MITER_CLIP: out <<"miter-clip"; break;
    case svg::StrokeLineJoin::ROUND: out <<"round"; break;
    }
    return out;
}
std::ostream& operator<<(std::ostream& out, const svg::Color& color) {
    std::visit(svg::PrintColor(), color);
    return out;
}



void Object::Render(const RenderContext& context) const {
    context.RenderIndent();
    RenderObject(context);
    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    
    RenderAttrs(out);

    out << "/>"sv;
}
//-------------End Cicle-------------------

// ---------------Polyline-----------------

Polyline& Polyline::AddPoint(Point point) {
    point_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    if (point_.empty()) {
        out <<"<polyline points=\"\"/>";
    }
    else
    {
        out << "<polyline points=\""sv;
        for (size_t x = 0; x < point_.size()-1; x++) {
            out << point_[x].x << "," << point_[x].y << " ";
        }
        out << point_[point_.size()-1].x << "," << point_[point_.size()-1].y;
        out << "\" "sv;

        RenderAttrs(out);    

        out << "/>"sv;
    }

}
//----------End Polyline------------------

//---------------Text---------------------

Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}

Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

Text& Text::SetFontSize(uint32_t size) {
    size_ = size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

Text& Text::SetData(std::string data) {
    data_ = data;
    return *this;
}


void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text "sv;

    RenderAttrs(out);

    out << "x=\""sv<<pos_.x<< "\" "sv;
    out << "y=\""sv << pos_.y << "\" "sv;
    out << "dx=\""sv << offset_.x << "\" "sv;
    out << "dy=\""sv << offset_.y << "\" "sv;
    out << "font-size=\""sv <<size_ << "\" "sv;
    if (!font_family_.empty()) {
        out << "font-family=\""sv << font_family_ << "\""sv;           
    }
    if (!font_weight_.empty()) {
        out << " font-weight=\""sv << font_weight_ << "\""sv;             
    }
    out<< ">"sv;
    //Data text parser section
    for (auto cr : data_) {
        if (cr == 34) {
            out << "&quot;"sv;
        }
        else if (cr == 60) {          
            out << "&lt;"sv;
        }
        else if (cr == 62) {
            out << "&gt;"sv;
        }
        else if (cr == 38) {                    
            out << "&amp;"sv;
        }
        else if (cr == 39) {
            out << "&apos;"sv;
        }
        else{
            out << cr;
        }
    }  
    //End parser section

    
    out << "</text>"sv;
}
//-------------End Text-------------------


//------------Document--------------------
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(std::move(obj));
}

void Document::Render(std::ostream& out) const{
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
    RenderContext ctx(std::cout, 2, 2);

    for (auto &str : objects_) {
        str->Render(ctx);
    }

    out << "</svg>"sv;
    
}
//---------End Document------------------

}  