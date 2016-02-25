// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast_type.hpp"
#include "ast_expr.hpp"
#include "type.hpp"
#include "constraint.hpp"
#include "context.hpp"
#include "lookup.hpp"
#include "conversion.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// Unary logical expressions

// Build a standard unary logical operator. The operand is contextually
// converted to bool, and the result type is bool.
template<typename Make>
static Expr&
make_standard_logical_expr(Context& cxt, Expr& e, Make make)
{
  Expr& c = contextual_conversion_to_bool(cxt, e);
  Type& t = cxt.get_bool_type();
  return make(t, c);
}


// Build a dependent unary logical expression. See comments on the
// binary overload for details.
template<typename Make>
static Expr&
make_dependent_logical_expr(Context& cxt, Expr& e, Make make)
{
  // Build a dependent expression.
  Type& t = make_fresh_type(cxt);
  Expr& f = make(t, e);

  // Don't do lookup in requirements or unconstrained templates.
  if (cxt.in_requirements())
    return f;
  if (!cxt.current_template_constraints())
    return f;

  // Inside a constrained template, search the constraints to determine
  // if the expression is admissible.
  Expr& con = *cxt.current_template_constraints();
  if (Expr* ret = admit_expression(cxt, con, f))
    return *ret;

  // If no no such expression is admitted by the constraints, then
  // we must be using the standard operator. Dependent conversions
  // may apply.
  return make_standard_logical_expr(cxt, e, make);
}


template<typename Make>
static Expr&
make_regular_logical_expr(Context& cxt, Expr& e, Make make)
{
  return make_standard_logical_expr(cxt, e, make);
}


// Determine the result type of logical expression.
template<typename Make>
static Expr&
make_logical_expr(Context& cxt, Expr& e, Make make)
{
  Type& t = e.type();
  try {
    if (is_dependent_type(t))
      return make_dependent_logical_expr(cxt, e, make);
    else
      return make_regular_logical_expr(cxt, e, make);
  } catch (Translation_error&) {
    // FIXME: Buiild an expression with a poisoned type
    // so that we can continue diagnosting errors.
    throw;
  }
}


// Binary logical expressions

// Build a standard binary logical operators. The operands are
// contextually converted to bool, and the result type is bool.
template<typename Make>
static Expr&
make_standard_logical_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  Expr& c1 = contextual_conversion_to_bool(cxt, e1);
  Expr& c2 = contextual_conversion_to_bool(cxt, e2);
  Type& t = cxt.get_bool_type();
  return make(t, c1, c2);
}


// Build a dependent binary logical expression.
//
// TODO: If either expression has occurred previously, then we should
// use it's result type and not generate a fresh type. Also, should the
// initial typing of the expression be a fresh type or the bottom type?
//
// FIXME: If we're in requirements, it might also be nice to unify the
// types of expressions. However, there isn't currently a mechanism to
// do that.
template<typename Make>
static Expr&
make_dependent_logical_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  // Build a dependent expression.
  Type& t = make_fresh_type(cxt);
  Expr& f = make(t, e1, e2);

  // Don't do dependent when parsing requirements.
  if (cxt.in_requirements())
    return f;
  if (!cxt.current_template_constraints())
    return f;

  // Inside a constrained template, search the constraints to determine
  // if the expression is admissible.
  Expr& con = *cxt.current_template_constraints();
  if (Expr* ret = admit_expression(cxt, con, f))
    return *ret;

  // If no no such expression is admitted by the constraints, then
  // we must be using the standard operator. Dependent conversions
  // may apply.
  return make_standard_logical_expr(cxt, e1, e2, make);
}



// Build a non-dependent logical operator. If either e1 or e2 is
// a user-defined type, then this may find an overloaded operator.
template<typename Make>
static Expr&
make_regular_logical_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  // FIXME: search for overloaded operators.
  return make_standard_logical_expr(cxt, e1, e2, make);
}


// Determine the result type of logical expression.
template<typename Make>
static Expr&
make_logical_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  Type& t1 = e1.type();
  Type& t2 = e2.type();
  try {
    if (is_dependent_type(t1) || is_dependent_type(t2))
      return make_dependent_logical_expr(cxt, e1, e2, make);
    else
      return make_regular_logical_expr(cxt, e1, e2, make);
  } catch (Translation_error&) {
    // FIXME: Buiild an expression with a poisoned type
    // so that we can continue diagnosting errors.
    throw;
  }
}


Expr&
make_logical_and(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_and(t, e1, e2);
  };
  return make_logical_expr(cxt, e1, e2, make);
}


Expr&
make_logical_or(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_or(t, e1, e2);
  };
  return make_logical_expr(cxt, e1, e2, make);
}


Expr&
make_logical_not(Context& cxt, Expr& e)
{
  Builder build(cxt);
  Expr& c = contextual_conversion_to_bool(cxt, e);
  Type& t = c.type();
  return build.make_not(t, c);
}


} // namespace banjo