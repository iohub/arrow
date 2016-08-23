// Copyright © 2014-2016 Ryan Leckey, All Rights Reserved.

// Distributed under the MIT License
// See accompanying file LICENSE

#ifndef ARROW_AST_FUNCTION_H
#define ARROW_AST_FUNCTION_H

#include <string>

#include "arrow/ast/nodes/statement.hpp"
#include "arrow/ast/nodes/type.hpp"
#include "arrow/ast/nodes/expression.hpp"

namespace arrow {
namespace ast {

struct Parameter : Node {
  Parameter(
    Span span, std::string name, ptr<Type> type,
    ptr<Expression> default_value
  )
  : Node(span), name(name), type(type), default_value(default_value) {
  }

  virtual ~Parameter() noexcept;

  /// Name of the parameter.
  // TODO: Pattern
  std::string name;

  /// Type annotation of the parameter (required).
  ptr<Type> type;

  /// Default value for the parameter (optional).
  ptr<Expression> default_value;
};

struct Function : Statement {
  Function(Span span, std::string name, ptr<Type> result_type)
  : Statement(span), name(name), result_type(result_type), parameters() {
  }

  virtual ~Function() noexcept;

  /// Name of the function.
  std::string name;

  /// Result type annotation of the function (required).
  ptr<Type> result_type;

  /// Parameters.
  std::vector<ptr<Parameter>> parameters;

  /// Sequence of statements in the function.
  std::vector<ptr<Statement>> statements;
};

}  // namespace ast
}  // namespace arrow

#endif  // ARROW_AST_FUNCTION_H
