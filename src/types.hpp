/* Valency 0.1
 * 2012, Max R. P. Grossmann
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#pragma once

#include <vector>
#include <stack>
#include <unordered_map>

using namespace std;

class instr_t;

class func_t;

typedef function<void(vector<shared_ptr<instr_t>>&)> builtin;
typedef vector<pair<shared_ptr<instr_t>,shared_ptr<instr_t>>> vlist;

stack<unordered_map<string,shared_ptr<instr_t>>> vars;
unordered_map<string,shared_ptr<instr_t>> declared_export;
//~ vector<void*> deleted;

bool interactive = false;
bool interactive_error = false;
bool program_break = false, function_return = false;

enum var_type : char {
	XNUMT = 1,
	XSTRINGT = 2,
	XFLOATT = 3,
	XFUNCT = 4,
	XNULLT = 0,
	XEMPTYT = 7,
	XLISTT = 6
};

class func_t {
	public:
		func_t();
		func_t(const string&);
		func_t(builtin);
		void operator()(vector<shared_ptr<instr_t>>&);

		string user;
		builtin b;
		bool built, inCurrentNamespace;
		unique_ptr<unordered_map<string,shared_ptr<instr_t>>> exported;
};

class instr_t {
	public:
		instr_t();
		instr_t(void*, var_type);
		
		void* p;
		var_type type;

		shared_ptr<instr_t> copy();
};
