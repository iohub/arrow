// Copyright (c) 2014-2015 Ryan Leckey, All Rights Reserved.

// Distributed under the MIT License
// See accompanying file LICENSE

#include "arrow/ir.hpp"
#include "arrow/generator.hpp"

using arrow::ir::Constant;

LLVMValueRef Constant::handle(GContext& ctx) noexcept {
  return transmute(value, type)->value_of(ctx);
}
