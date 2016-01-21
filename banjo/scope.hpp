// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_SCOPE_HPP
#define BANJO_SCOPE_HPP

#include "prelude.hpp"
#include "overload.hpp"
#include "context.hpp"

#include <lingo/environment.hpp>


namespace banjo
{


struct Decl;
struct Namespace_decl;
struct Function_decl;
struct Class_decl;
struct Object_decl;


// -------------------------------------------------------------------------- //
// Scope definitions
//
// TODO: Do I need derived kinds of scopes?


// A scope defines a maximal lexical region of text where an 
// entity  may be referred to without qualification. A scope can 
// be (but is not always) associated with a declaration.
struct Scope : Environment<Symbol const*, Overload_set>
{
  using Base = Environment<Symbol const*, Overload_set>;

  Scope()
    : parent(nullptr), decl(nullptr)
  { }

  Scope(Scope& p)
    : parent(&p), decl(nullptr)
  { }

  Scope(Scope& p, Decl& d)
    : parent(&p), decl(&d)
  { }

  virtual ~Scope() { }

  // Returns the enclosing scope, if any. Only the global
  // namespace does not have an enclosing scope.
  Scope const* enclosing_scope() const { return parent; }
  Scope*       enclosing_scope()       { return parent; }

  // Returns the declaration associated with the scope.
  // Not all scopes are associated with a declaration
  // (e.g., block scope).
  Decl const* context() const { return decl; }
  Decl*       context()       { return decl; }

  // Return the binding for the given symbol, or nullptr
  // if no such binding exists.
  Binding const* lookup(Symbol const& s) const { return Base::lookup(&s); }
  Binding*       lookup(Symbol const& s)       { return Base::lookup(&s); }

  Scope* parent;
  Decl*  decl;
};


// Represents a namespace scope.
struct Namespace_scope : Scope
{
  // Returns the namespace declaration associated with the scope.
  Namespace_decl const& declaration() const;
  Namespace_decl&       declaration();
};



// Represents function scope.
struct Function_scope : Scope
{
  // Returns the function declaration associated with the scope.
  Function_decl const& declaration() const;
  Function_decl&       declaration();
};


// Represents a class scope.
struct Class_scope : Scope
{
  // Returns the function declaration associated with the scope.
  Class_decl const& declaration() const;
  Class_decl&       declaration();
};


// Represents the scope of a variables initializer. This is
// primarily used to support lookup of identifiers when the
// variable is declared with a qualified-id (i.e., a static
// member of a class or a namespace variable defined outside
// of the namespace.
struct Initializer_scope : Scope
{
  // Returns the function declaration associated with the scope.
  Object_decl const& declaration() const;
  Object_decl&       declaration();
};


// Represents a block scope.
struct Block_scope : Scope
{
};


// TODO: Define other kinds of scope.


// Returns true if s is a scope for a namespace.
inline bool
is_namespace_scope(Scope const& s)
{
  return is<Namespace_scope>(&s);
}


// Returns true if s is a scope for a function definition.
inline bool
is_function_scope(Scope const& s)
{
  return is<Function_scope>(&s);
}


} // namespace banjo


#endif
