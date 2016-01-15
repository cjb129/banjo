// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast.hpp"
#include "equivalence.hpp"


namespace beaker
{


// Returns true if `t` is an object type. That is, any type
// except function types and reference types.
//
// This function is not defined for placeholder types and
// dependent types.
//
// TODO: Is void an object type? Currently, yes.
//
// TODO: Does the type category of a cv-T really depend T? This
// will almost certainly return true. What is this (()->void) const&?
//
// FIXME: Can a dependent type be categorized? We should probably
// assert that as a precondition.
//
// FIXME: The value category of a decltype-type can be determined
// if its expression is not type-dependent.
bool
is_value_type(Type const& t)
{
  struct fn
  {
    bool operator()(Void_type const&)        { return true; }
    bool operator()(Boolean_type const&)     { return true; }
    bool operator()(Integer_type const&)     { return true; }
    bool operator()(Float_type const&)       { return true; }
    bool operator()(Auto_type const&)        { lingo_unreachable(); }
    bool operator()(Decltype_type const&)    { lingo_unimplemented(); }
    bool operator()(Declauto_type const&)    { lingo_unreachable(); }
    bool operator()(Function_type const&)    { return false; }
    bool operator()(Qualified_type const& t) { return is_value_type(t.type()); }
    bool operator()(Pointer_type const&)     { return true; }
    bool operator()(Reference_type const&)   { return false; }
    bool operator()(Array_type const&)       { return true; }
    bool operator()(Sequence_type const&)    { return true; }
    bool operator()(Class_type const&)       { return true; }
    bool operator()(Union_type const&)       { return true; }
    bool operator()(Enum_type const&)        { return true; }
    bool operator()(Typename_type const&)    { lingo_unreachable(); }
  };
  return apply(t, fn{});
}


// Returns a qualified id for the declaration.
//
// TODO: Use an allocator.
Name const&
Decl::qualified_id() const
{
  Name* n = first;
  Decl* d = cxt;
  while (d->cxt) {
    n = new Qualified_id(*d, *n);
    d = d->cxt;
  }
  return *n;
}


// Returns the fully qualified id for the declaration.
//
// TODO: Use an allocator.
Name const&
Decl::fully_qualified_id() const
{
  Name* n = first;
  Decl* d = cxt;
  while (d) {
    n = new Qualified_id(*d, *n);
    d = d->cxt;
  }
  return *n;
}


} // namespace beaker
