// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "constraint.hpp"
#include "ast.hpp"
#include "builder.hpp"
#include "substitution.hpp"
#include "normalization.hpp"
#include "hash.hpp"
#include "equivalence.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// Expand the concept by substituting the template arguments
// throughthe concept's definition and normalizing the result.
//
// TODO: Memoize the expansions.
Cons&
expand(Context& cxt, Concept_cons& c)
{
  Concept_decl& d = c.declaration();
  Decl_list& tparms = d.parameters();
  Term_list& targs = c.arguments();
  Expr& def = d.definition();

  // NOTE: Template arguments must have been checked (in kind?)
  // prior to the formation of the constraint. It's should be
  // a semantic requirement of the original check expression.
  Substitution sub(tparms, targs);

  Expr& e = substitute(cxt, def, sub);
  return normalize(cxt, e);
}


Cons const&
expand(Context& cxt, Concept_cons const& c)
{
  return expand(cxt, const_cast<Concept_cons&>(c));
}


} // namespace banjo
