// Copyright (c) 2014-2015 Ryan Leckey, All Rights Reserved.

// Distributed under the MIT License
// See accompanying file LICENSE

#include "arrow/pass/build.hpp"

using arrow::pass::Build;

auto Build::handle_expression_statement(ptr<ast::ExpressionStatement> x) -> ptr<ir::Value> {
  // Build the expression
  auto ref = run(x->expression);
  if (!ref) return nullptr;

  return ref;
}
