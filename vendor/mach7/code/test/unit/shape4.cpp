//
//  Mach7: Pattern Matching Library for C++
//
//  Copyright 2011-2013, Texas A&M University.
//  Copyright 2014 Yuriy Solodkyy.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//
//      * Neither the names of Mach7 project nor the names of its contributors
//        may be used to endorse or promote products derived from this software
//        without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

///
/// \file
///
/// This file is a part of Mach7 library test suite.
///
/// \author Yuriy Solodkyy <yuriy.solodkyy@gmail.com>
///
/// \see https://parasol.tamu.edu/mach7/
/// \see https://github.com/solodon4/Mach7
/// \see https://github.com/solodon4/SELL
///

#include <iostream>
#include <utility>
#include <mach7/match.hpp>                 // Support for Match statement

typedef std::pair<double,double> loc;
struct cloc { double first; double second; };

struct Shape;
struct Circle;
struct Square;
struct Triangle;

struct ShapeVisitor
{
	virtual void visit(const Circle&)   {}
	virtual void visit(const Square&)   {}
	virtual void visit(const Triangle&) {}
};

// An Algebraic Data Type implemented through inheritance
struct Shape
{
    enum ShapeKind {SK_Circle,SK_Square,SK_Triangle};
    ShapeKind kind;
    Shape(ShapeKind k) : kind(k) {}
    virtual void accept(ShapeVisitor& v) = 0;
};

struct Circle : Shape
{
    Circle(const loc& c, const double& r) : Shape(SK_Circle), center(c), radius(r) {}

    void accept(ShapeVisitor& v) { v.visit(*this); }

    const loc& get_center() const { return center; }

    loc  center;
    double radius;
};

struct Square : Shape
{
    Square(const loc& c, const double& s) : Shape(SK_Square), upper_left(c), side(s) {}

    void accept(ShapeVisitor& v) { v.visit(*this); }

    loc  upper_left;
    double side;
};

struct Triangle : Shape
{
    Triangle(const loc& a, const loc& b, const loc& c) : Shape(SK_Triangle), first(a), second(b), third(c) {}

    void accept(ShapeVisitor& v) { v.visit(*this); }

    loc first;
    loc second;
    loc third;
};

struct ADTShape
{
	enum shape_kind {circle, square, triangle} kind;

#ifndef POD_ONLY
	ADTShape(const cloc& c, double r) : kind(circle), center(c), radius(r) {}
	ADTShape(double s, const cloc& c) : kind(square), upper_left(c), size(s) {}
	ADTShape(const cloc& x, const cloc& y, const cloc& z) : kind(triangle), first(x), second(y), third(z) {}
	virtual ~ADTShape() {}
#endif

#ifndef _MSC_VER
	union
	{
		struct { cloc center;     double radius; }; // as_circle;
		struct { cloc upper_left; double size; }; //   as_square;
		struct { cloc first, second, third; }; //    as_triangle;
	};
#else
    // MSVC doesn't seem to allow anonymous structs inside union so we just dump
    // here same members name directly to make the rest just compile at least.
	cloc center;     double radius; // as_circle;
	cloc upper_left; double size;   // as_square;
	cloc first, second, third;    // as_triangle;
#endif
};

namespace mch ///< Mach7 library namespace
{
template <> struct bindings<Shape>    { KS(Shape::kind); };

template <> struct bindings<Circle>   { KV(Shape,Shape::SK_Circle);  Members(Circle::get_center,Circle::radius); };
template <> struct bindings<Square>   { KV(Shape,Shape::SK_Square);  Members(Square::upper_left,Square::side);   };
template <> struct bindings<Triangle> { KV(Shape,Shape::SK_Triangle);Members(Triangle::first,   Triangle::second, Triangle::third); };

template <> struct bindings<ADTShape> { KS(ADTShape::kind); };

template <> struct bindings<ADTShape,ADTShape::circle>   { KV(ADTShape,ADTShape::circle);  Members(ADTShape::center,     ADTShape::radius); };
template <> struct bindings<ADTShape,ADTShape::square>   { KV(ADTShape,ADTShape::square);  Members(ADTShape::upper_left, ADTShape::size); };
template <> struct bindings<ADTShape,ADTShape::triangle> { KV(ADTShape,ADTShape::triangle);Members(ADTShape::first,      ADTShape::second, ADTShape::third); };
} // of namespace mch

int main()
{
#ifdef POD_ONLY
    ADTShape ac = {ADTShape::circle,   {{{1,1}, 7}}};
    ADTShape as = {ADTShape::square,   {{{1,1}, 2}}};
    ADTShape at = {ADTShape::triangle, {{{1,1}, 1}}};
#else
    cloc l00 = {0,0};
    cloc l11 = {1,1};
    cloc l10 = {1,0};
    ADTShape ac(l11, 7);
    ADTShape as(2, l11);
    ADTShape at(l11, l10, l00);
#endif

    ADTShape* adtshapes[] = {&ac,&as,&at};

    Shape* c = new Circle(loc(1,1),7);
    Shape* s = new Square(loc(1,1),2);
    Shape* t = new Triangle(loc(1,1),loc(1,0),loc(0,0));

    Shape* shapes[] = {c,s,t};

    double m = 0.0;

    for (size_t i = 0; i < 3; ++i)
    {
        MatchK(shapes[i])
        {
        CaseK(Circle,_,x) std::cout << "Circle"   << std::endl; m += x;       XTL_UNUSED(_); break;
        CaseK(Square,_,x) std::cout << "Square"   << std::endl; m += x;       XTL_UNUSED(_); break;
        CaseK(Triangle,l) std::cout << "Triangle" << std::endl; m += l.first; break;
      //CaseK(Triangle,l) std::cout << "Triangle" << std::endl; m += l.first; break; // NOTE: Not possible to have another kind match
        }
        EndMatchK
    }

    std::cout << m << std::endl;

    m = 0.0;

    for (size_t i = 0; i < 3; ++i)
    {
        MatchP(shapes[i])
        {
        CaseP(Circle)   std::cout << "Circle"   << std::endl; m += matched->radius;      break;
        CaseP(Square)   std::cout << "Square"   << std::endl; m += matched->side;        break;
        CaseP(Triangle) std::cout << "Triangle" << std::endl; m += matched->first.first; break;
        CaseP(Triangle) std::cout << "Triangle" << std::endl; m += matched->first.first; break; // NOTE: Possible to have another type case match
        }
        EndMatchP
    }

    std::cout << m << std::endl;

    m = 0.0;

    for (size_t i = 0; i < 3; ++i)
    {
        MatchP(shapes[i])
        {
        CaseP(Circle,_,x)    std::cout << "Circle"   << std::endl; m += x;       XTL_UNUSED(_); break;
        CaseP(Square,_,x)    std::cout << "Square"   << std::endl; m += x;       XTL_UNUSED(_); break;
        CaseP(Triangle,l)    std::cout << "Triangle" << std::endl; m += l.first; break;
        CaseP(Triangle,l)    std::cout << "Triangle" << std::endl; m += l.first; break; // NOTE: Possible to have another regular match
        }
        EndMatchP
    }

    std::cout << m << std::endl;

    m = 0.0;

    for (size_t i = 0; i < 3; ++i)
    {
        MatchU(adtshapes[i])
        {
        CaseU(ADTShape::circle,_,x)  std::cout << "ADTCircle"   << std::endl; m += x;       XTL_UNUSED(_); break;
        CaseU(ADTShape::square,_,x)  std::cout << "ADTSquare"   << std::endl; m += x;       XTL_UNUSED(_); break;
        CaseU(ADTShape::triangle,cl) std::cout << "ADTTriangle" << std::endl; m += cl.first;break;
      //CaseU(ADTShape::triangle,cl) std::cout << "ADTTriangle" << std::endl; m += cl.first;break; // NOTE: Not possible to have another union match
        }
        EndMatchU
    }

    std::cout << m << std::endl;

}
