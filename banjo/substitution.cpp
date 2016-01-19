// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "substitution.hpp"
#include "builder.hpp"

#include <iostream>


namespace banjo
{

template<typename T>
List<T>
substitute(Context& cxt, List<T>& list, Substitution& sub)
{
  List<T> ret;
  ret.reserve(list.size());
  for (T& x : list)
    ret.push_back(substitute(cxt, x, sub));
  return ret;
}


// -------------------------------------------------------------------------- //
// Substitution into types

Type& substitute_type(Context&, Function_type&, Substitution&);
Type& substitute_type(Context&, Reference_type&, Substitution&);
Type& substitute_type(Context&, Qualified_type&, Substitution&);
Type& substitute_type(Context&, Pointer_type&, Substitution&);
Type& substitute_type(Context&, Array_type&, Substitution&);
Type& substitute_type(Context&, Sequence_type&, Substitution&);
Type& substitute_type(Context&, Typename_type&, Substitution&);


Type&
substitute(Context& cxt, Type& t, Substitution& sub)
{
  struct fn
  {
    Context&      cxt;
    Substitution& sub;
    Type& operator()(Void_type& t)      { return t; }
    Type& operator()(Boolean_type& t)   { return t; }
    Type& operator()(Integer_type& t)   { return t; }
    Type& operator()(Float_type& t)     { return t; }
    Type& operator()(Auto_type& t)      { lingo_unimplemented(); }
    Type& operator()(Decltype_type& t)  { lingo_unimplemented(); }
    Type& operator()(Declauto_type& t)  { lingo_unimplemented(); }
    Type& operator()(Function_type& t)  { return substitute_type(cxt, t, sub); }
    Type& operator()(Reference_type& t) { return substitute_type(cxt, t, sub); }
    Type& operator()(Qualified_type& t) { return substitute_type(cxt, t, sub); }
    Type& operator()(Pointer_type& t)   { return substitute_type(cxt, t, sub); }
    Type& operator()(Array_type& t)     { return substitute_type(cxt, t, sub); }
    Type& operator()(Sequence_type& t)  { return substitute_type(cxt, t, sub); }
    Type& operator()(Class_type& t)     { return t; }
    Type& operator()(Union_type& t)     { return t; }
    Type& operator()(Enum_type& t)      { return t; }
    Type& operator()(Typename_type& t)  { return substitute_type(cxt, t, sub); }
  };
  return apply(t, fn{cxt, sub});
}


Type&
substitute_type(Context& cxt, Function_type& t, Substitution& sub)
{
  Builder build(cxt);
  Type_list ps = substitute(cxt, t.parameter_types(), sub);
  Type& r = substitute(cxt, t.return_type(), sub);
  return build.get_function_type(ps, r);
}


Type&
substitute_type(Context& cxt, Reference_type& t, Substitution& sub)
{
  Builder build(cxt);
  Type& s = substitute(cxt, t.type(), sub);
  return build.get_reference_type(s);
}


Type&
substitute_type(Context& cxt, Qualified_type& t, Substitution& sub)
{
  Builder build(cxt);
  Type& s = substitute(cxt, t.type(), sub);
  return build.get_qualified_type(s, t.qualifier());
}


Type&
substitute_type(Context& cxt, Pointer_type& t, Substitution& sub)
{
  Builder build(cxt);
  Type& s = substitute(cxt, t.type(), sub);
  return build.get_pointer_type(s);
}


Type&
substitute_type(Context& cxt, Array_type& t, Substitution& sub)
{
  lingo_unimplemented();
}


Type&
substitute_type(Context& cxt, Sequence_type& t, Substitution& sub)
{
  Builder build(cxt);
  Type& s = substitute(cxt, t.type(), sub);
  return build.get_sequence_type(s);
}


// Substitute into a typename type. If the type's declaration is
// in the mapping, then reteurn the mapped type. Otherwise,
// return the original type.
Type&
substitute_type(Context& cxt, Typename_type& t, Substitution& sub)
{
  if (Term* x = sub.get(t.declaration()))
    return cast<Type>(*x);
  else
    return t;
}


// -------------------------------------------------------------------------- //
// Substitution into expressions

Expr&
substitute(Context& cxt, Expr& e, Substitution& sub)
{
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Substitution into declarations
//
// Note that substitution into a declaration does not change
// the name of the declaration. Specialization, however, is a
// special form of substitution where we generate a newly named
// declaration.

Decl& substitute_decl(Context&, Variable_decl&, Substitution&);


Decl&
substitute(Context& cxt, Decl& d, Substitution& sub)
{
  struct fn
  {
    Context&      cxt;
    Substitution& sub;
    Decl& operator()(Decl& d)           { lingo_unimplemented(); }
    Decl& operator()(Variable_decl& d)  { return substitute_decl(cxt, d, sub); }
  };
  return apply(d, fn{cxt, sub});
}


// TODO: Substitute into the initializer.
//
// TODO: Should we declare the variable? In general, how should we
// handle elaboration?
Decl&
substitute_decl(Context& cxt, Variable_decl& d, Substitution& sub)
{
  Name& n = d.name();
  Type& t = substitute(cxt, d.type(), sub);

  Builder build(cxt);
  return build.make_variable(n, t);
}


} // namespace banjo
