/* Valency 1e-4
 * 2012, Max R. P. Grossmann
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstring>
#include <stack>
#include <memory>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include "types.hpp"
#include "chrono.cpp"
#include "functions.hpp"

instr_t* get() {
	instr_t* p = new instr_t();

	if (p == NULL) {
		cerr << "Fatal error: Not enough memory available." << endl;
		exit(122);
	}
	
	return p;
}

instr_t* get(void* np, var_type ntype) {
	instr_t* p = new instr_t(np,ntype);

	if (p == NULL) {
		cerr << "Fatal error: Not enough memory available." << endl;
		exit(122);
	}
	
	return p;
}

inline void gc_handler(shared_ptr<instr_t> ptr) {
	if (find(deleted.begin(),deleted.end(),ptr->p) != deleted.end()) {
		return;
	}
	
	switch (ptr->type) {
		case XNULLT: break;
		case XNUMT: delete (int64_t*)ptr->p; break;
		case XSTRINGT: delete (string*)ptr->p; break;
		case XFLOATT: delete (double*)ptr->p; break;
		case XFUNCT: delete (func_t*)ptr->p; break;
		case XLISTT: {
						for (auto cur: *((vlist*)ptr->p)) {
							gc_handler(cur.first);
							gc_handler(cur.second);
						}
						
						delete (vlist*)ptr->p;
						break;
		             }
	}

	deleted.push_back(ptr->p);

	ptr->type = XNULLT;
	ptr->p = nullptr;
}

instr_t::instr_t() {
	this->p = NULL;
	this->type = XNULLT;
}

instr_t::instr_t(void* p, var_type type) {
	this->p = p;
	this->type = type;
}

shared_ptr<instr_t> instr_t::copy() {
	shared_ptr<instr_t> r(get());
	void* np;

	if (this->type == XNUMT) {
		np = new int64_t;

		memcpy(np,p,sizeof(int64_t));
	}
	else if (this->type == XFLOATT) {
		np = new double;

		memcpy(np,p,sizeof(double));
	}
	else if (this->type == XSTRINGT) {
		np = new string(*((string*)this->p));
	}
	else if (this->type == XLISTT) {
		np = new vlist(*((vlist*)this->p));
	}
	else if (this->type == XFUNCT) {
		np = new func_t;
		((func_t*)np)->user = ((func_t*)this->p)->user;
		((func_t*)np)->b = ((func_t*)this->p)->b;
		((func_t*)np)->built = ((func_t*)this->p)->built;
	}
	else {
		// Will never happen
		np = NULL;
	}

	r->p = np;
	r->type = this->type;

	return r;
}

func_t::func_t() {
	built = false;
	user = "";

	if (!declared_export.empty()) {
		exported.reset(new unordered_map<string,shared_ptr<instr_t>>(declared_export));
		declared_export.clear();
	}
}

func_t::func_t(const string& str) {
	built = false;
	user = str;
	if (user.front() == '{') {
		user.erase(0,1);
	}
	if (user.back() == '}') {
		user.erase(user.end()-1);
	}
	user += "\n\n";

	if (!declared_export.empty()) {
		exported.reset(new unordered_map<string,shared_ptr<instr_t>>(declared_export));
		declared_export.clear();
	}
}

func_t::func_t(builtin f) {
	built = true;
	b = f;
}

void func_t::operator()(vector<shared_ptr<instr_t>>& arg) {
	if (built == true) {
		b(arg);
	}
	else {
		unordered_map<string,shared_ptr<instr_t>> nvar;

		for (auto& v: vars.top()) {
			if ((v.second)->type == 4) {
				nvar.insert(v);
			}
		}

		if (exported) {
			nvar.insert(exported->begin(),exported->end());
		}
		
		vars.push(nvar);

		unsigned i = 0;
		vector<vector<string>> lines;

		for (shared_ptr<instr_t> ca: arg) {
			vars.top()["#"+to_string(i)] = ca;
			i++;
		}

		if (vars.top().find("#n") != vars.top().end()) {
			gc_handler(vars.top()["#n"]);
		}
		
		vars.top()["#n"].reset(get(new int64_t(arg.size()-1), XNUMT));

		istringstream fstr(user);

		vector<shared_ptr<instr_t>> instr;

		parse(fstr,lines);

		for (auto& ci: lines) {
			transform(ci, instr);
			call(instr);

			if (program_break || function_return) {
				program_break = false;
				function_return = false;
				break;
			}
		}

		vars.pop();
	}
}

void halt(int code) {
	if (!interactive) {
		cerr << "[Halt]." << endl;
		exit(code);
	}
	else {
		interactive_error = true;
		cout << "Note: Line ignored." << endl;
	}
}
