// Copyright © 2014-2016 Ryan Leckey, All Rights Reserved.

// Distributed under the MIT License
// See accompanying file LICENSE

#include "mach7.hpp"
#include "arrow/ast/print.hpp"

using arrow::ast::Print;

void Print::print_tuple(ptr<Tuple> n) {
  handle("Tuple", n, [&, this] {
    _w.Key("elements");
    _w.StartArray();

    for (auto& param : n->elements) {
      print(param);
    }

    _w.EndArray();
  });
}
