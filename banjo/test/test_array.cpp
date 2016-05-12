#include "test.hpp"
#include<iostream>

int
main() {
		Context cxt;
		Builder build(cxt);
				
	  Type& z = build.get_int_type();

		Expr& e = build.get_int(2);
				
		Expr_list e1 = {
			&build.get_int(5),
			&build.get_int(3)
		};

		auto a =	build.get_array_type(z, e);		
		auto tuple = build.make_tuple_expr(a,e1);
		
		auto var = build.make_variable_declaration("a1", a,as<Expr>(tuple));
		std::cout << var << "\n";
}