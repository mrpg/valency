/* Valency 0.1
 * 2012, Max R. P. Grossmann
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
 
inline bool in(const string* s, char c) {
	for (auto& cur: *s) {
		if (c == cur) {
			return true;
		}
	}
	
	return false;
}

void builtin_set(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		//~ gc_handler(arg[1]);
		arg[1]->p = arg[2]->p;
		arg[1]->type = arg[2]->type;
	}
	else {
		cerr << "Fatal, Aborting: `set' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void print(shared_ptr<instr_t> carg) {
	switch (carg->type) {
		case XFLOATT: cout << (*((double*)carg->p)); break;
		case XNUMT: cout << (*((int64_t*)carg->p)); break;
		case XFUNCT: cout << "[function]"; break;
		case XLISTT:{
						cout << "list[";
						size_t i=0, m=((vlist*)carg->p)->size()-1;
						for (auto& cur: *((vlist*)carg->p)) {
							print(cur.first);
							cout << " -> ";
							print(cur.second);
							if (i != m) cout << ", ";
							i++;
						}
						cout << "]";
						
						break;
					}
		case XNULLT: cout << "[null]"; break;
		case XSTRINGT: cout << (*((string*)carg->p)); break;
		default: cout << "[untyped]"; break;
	}
}

void builtin_print(vector<shared_ptr<instr_t>>& arg) {
	size_t i = 0;

	for (auto& carg: arg) {
		if (i != 0) {
			print(carg);
		}
		i++;
	}

	cout << endl;
}

void builtin_write(vector<shared_ptr<instr_t>>& arg) {
	size_t i = 0;

	for (auto& carg: arg) {
		if (i != 0) {
			print(carg);
		}
		i++;
	}
}

void builtin_exit(vector<shared_ptr<instr_t>>& arg) {
	exit(0);
}

void builtin_while (vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		if (arg[2]->type == XFUNCT) {
			istringstream fstr(((func_t*)arg[2]->p)->user);
			vector<vector<string>> lines;
			vector<shared_ptr<instr_t>> instr;

			parse(fstr, lines);

			while (checkcond(arg[1]) && !program_break && !function_return) {
				instr.clear();
				
				for (auto& ci: lines) {
					transform(ci, instr);
					call(instr);
				}
			}

			program_break = false;
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `while'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `while' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_for_each (vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 5) {
		if (arg[1]->type == XLISTT && arg[2]->type == XSTRINGT && arg[3]->type == XSTRINGT && arg[4]->type == XFUNCT) {
			istringstream fstr(((func_t*)arg[4]->p)->user);
			vector<vector<string>> lines;
			vector<shared_ptr<instr_t>> instr;

			parse(fstr, lines);

			for (auto& cur: *((vlist*)arg[1]->p)) {
				if (program_break || function_return) break;

				vars.top()[*((string*)arg[2]->p)] = cur.first;
				vars.top()[*((string*)arg[3]->p)] = cur.second;
				
				instr.clear();
				
				for (auto& ci: lines) {
					transform(ci, instr);
					call(instr);
				}
			}

			program_break = false;
		}
		else if (arg[1]->type == XLISTT && arg[4]->type == XFUNCT) {
			istringstream fstr(((func_t*)arg[4]->p)->user);
			vector<vector<string>> lines;
			vector<shared_ptr<instr_t>> instr;

			parse(fstr, lines);

			for (auto& cur: *((vlist*)arg[1]->p)) {
				if (program_break || function_return) break;

				arg[2]->p = cur.first->p;
				arg[2]->type = cur.first->type;
				arg[3]->p = cur.second->p;
				arg[3]->type = cur.second->type;
				
				instr.clear();
				
				for (auto& ci: lines) {
					transform(ci, instr);
					call(instr);
				}
			}

			program_break = false;
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `for_each'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `for_each' needs exactly 4 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_if (vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		if (arg[2]->type == XFUNCT) {
			if (checkcond(arg[1])) {
				istringstream fstr(((func_t*)arg[2]->p)->user);
				vector<vector<string>> lines;
				vector<shared_ptr<instr_t>> instr;

				parse(fstr, lines);
				
				for (auto& ci: lines) {
					transform(ci, instr);
					call(instr);
				}
			}
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `if'." << endl;
			halt(28);
		}
	}
	else if (arg.size() == 4) {
		if (arg[2]->type == XFUNCT && arg[3]->type == XFUNCT) {
			if (checkcond(arg[1])) {
				istringstream fstr(((func_t*)arg[2]->p)->user);
				vector<vector<string>> lines;
				vector<shared_ptr<instr_t>> instr;

				parse(fstr, lines);
				
				for (auto& ci: lines) {
					transform(ci, instr);
					call(instr);
				}
			}
			else {
				istringstream fstr(((func_t*)arg[3]->p)->user);
				vector<vector<string>> lines;
				vector<shared_ptr<instr_t>> instr;

				parse(fstr, lines);
				
				for (auto& ci: lines) {
					transform(ci, instr);
					call(instr);
				}
			}
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `if'." << endl;
			halt(28);
		}
	}
	else if (arg.size() == 5) {
		// if [state] [true] [false] [&out]
		// #0   #1     #2     #3       #4

		if (checkcond(arg[1])) {
			arg[4]->p = arg[2]->p;
			arg[4]->type = arg[2]->type;
		}
		else {
			arg[4]->p = arg[3]->p;
			arg[4]->type = arg[3]->type;
		}
	}
	else {
		cerr << "Fatal, Aborting: `if' needs 2 or 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_gt(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		//~ gc_handler(arg[3]);
		arg[3]->p = new int64_t;
		arg[3]->type = XNUMT;

		*((int64_t*)arg[3]->p) = 0;
		
		if (arg[1]->type == XFLOATT && arg[2]->type == XFLOATT) {
			*((int64_t*)arg[3]->p) = ((*((double*)arg[1]->p)) > (*((double*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XFLOATT && arg[2]->type == XNUMT) {
			*((int64_t*)arg[3]->p) = ((*((double*)arg[1]->p)) > (*((int64_t*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XFLOATT) {
			*((int64_t*)arg[3]->p) = ((*((int64_t*)arg[1]->p)) > (*((double*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XNUMT) {
			*((int64_t*)arg[3]->p) = ((*((int64_t*)arg[1]->p)) > (*((int64_t*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XSTRINGT && arg[2]->type == XSTRINGT) {
			*((int64_t*)arg[3]->p) = ((*((string*)arg[1]->p)) > (*((string*)arg[2]->p))) ? 1 : 0;
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `gt'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `gt' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_gte(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		//~ gc_handler(arg[3]);
		arg[3]->p = new int64_t;
		arg[3]->type = XNUMT;

		*((int64_t*)arg[3]->p) = 0;
		
		if (arg[1]->type == XFLOATT && arg[2]->type == XFLOATT) {
			*((int64_t*)arg[3]->p) = ((*((double*)arg[1]->p)) >= (*((double*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XFLOATT && arg[2]->type == XNUMT) {
			*((int64_t*)arg[3]->p) = ((*((double*)arg[1]->p)) >= (*((int64_t*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XFLOATT) {
			*((int64_t*)arg[3]->p) = ((*((int64_t*)arg[1]->p)) >= (*((double*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XNUMT) {
			*((int64_t*)arg[3]->p) = ((*((int64_t*)arg[1]->p)) >= (*((int64_t*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XSTRINGT && arg[2]->type == XSTRINGT) {
			*((int64_t*)arg[3]->p) = ((*((string*)arg[1]->p)) >= (*((string*)arg[2]->p))) ? 1 : 0;
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `gte'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `gte' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_lt(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		//~ gc_handler(arg[3]);
		arg[3]->p = new int64_t;
		arg[3]->type = XNUMT;

		*((int64_t*)arg[3]->p) = 0;
		
		if (arg[1]->type == XFLOATT && arg[2]->type == XFLOATT) {
			*((int64_t*)arg[3]->p) = ((*((double*)arg[1]->p)) < (*((double*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XFLOATT && arg[2]->type == XNUMT) {
			*((int64_t*)arg[3]->p) = ((*((double*)arg[1]->p)) < (*((int64_t*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XFLOATT) {
			*((int64_t*)arg[3]->p) = ((*((int64_t*)arg[1]->p)) < (*((double*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XNUMT) {
			*((int64_t*)arg[3]->p) = ((*((int64_t*)arg[1]->p)) < (*((int64_t*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XSTRINGT && arg[2]->type == XSTRINGT) {
			*((int64_t*)arg[3]->p) = ((*((string*)arg[1]->p)) < (*((string*)arg[2]->p))) ? 1 : 0;
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `lt'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `lt' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_lte(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		//~ gc_handler(arg[3]);
		arg[3]->p = new int64_t;
		arg[3]->type = XNUMT;

		*((int64_t*)arg[3]->p) = 0;
		
		if (arg[1]->type == XFLOATT && arg[2]->type == XFLOATT) {
			*((int64_t*)arg[3]->p) = ((*((double*)arg[1]->p)) <= (*((double*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XFLOATT && arg[2]->type == XNUMT) {
			*((int64_t*)arg[3]->p) = ((*((double*)arg[1]->p)) <= (*((int64_t*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XFLOATT) {
			*((int64_t*)arg[3]->p) = ((*((int64_t*)arg[1]->p)) <= (*((double*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XNUMT) {
			*((int64_t*)arg[3]->p) = ((*((int64_t*)arg[1]->p)) <= (*((int64_t*)arg[2]->p))) ? 1 : 0;
		}
		else if (arg[1]->type == XSTRINGT && arg[2]->type == XSTRINGT) {
			*((int64_t*)arg[3]->p) = ((*((string*)arg[1]->p)) <= (*((string*)arg[2]->p))) ? 1 : 0;
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `lte'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `lte' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

bool is(shared_ptr<instr_t> a, shared_ptr<instr_t> b) {
	if (a->type == XFLOATT && b->type == XFLOATT) {
		return ((*((double*)a->p)) == (*((double*)b->p)));
	}
	else if (a->type == XFLOATT && b->type == XNUMT) {
		return ((*((double*)a->p)) == (*((int64_t*)b->p)));
	}
	else if (a->type == XNUMT && b->type == XFLOATT) {
		return ((*((int64_t*)a->p)) == (*((double*)b->p)));
	}
	else if (a->type == XNUMT && b->type == XNUMT) {
		return ((*((int64_t*)a->p)) == (*((int64_t*)b->p)));
	}
	else if (a->type == XSTRINGT && b->type == XSTRINGT) {
		return ((*((string*)a->p)) == (*((string*)b->p)));
	}
	else if (a->type == XLISTT && b->type == XLISTT) {
		if ((((vlist*)a->p))->size() == (((vlist*)b->p))->size()) {
			for (auto& cur: *((vlist*)a->p)) {
				if (find_if((((vlist*)b->p))->begin(),(((vlist*)b->p))->end(),[&cur](pair<shared_ptr<instr_t>,shared_ptr<instr_t>>& x){return (is(x.first,cur.first) && is(x.second,cur.second));}) == (((vlist*)b->p))->end()) {
					return false;
				}
			}
			
			return true;
		}
		else {
			return false;
		}
	}
	else if (a->type == XNULLT && b->type == XNULLT) {
		return true;
	}
	else {
		return false;
	}
}

void builtin_is(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		//~ gc_handler(arg[3]);
		arg[3]->p = new int64_t;
		arg[3]->type = XNUMT;

		*((int64_t*)arg[3]->p) = (is(arg[1],arg[2]) == true) ? 1 : 0;
	}
	else {
		cerr << "Fatal, Aborting: `is' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_not(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		//~ gc_handler(arg[2]);
		arg[2]->p = new int64_t;
		arg[2]->type = XNUMT;

		*((int64_t*)arg[2]->p) = 0;
		
		*((int64_t*)arg[2]->p) = (!checkcond(arg[1])) ? 1 : 0;
	}
	else {
		cerr << "Fatal, Aborting: `not' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_pow(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		//~ gc_handler(arg[3]);
		arg[3]->p = new double;
		arg[3]->type = XFLOATT;

		
		if (arg[1]->type == XFLOATT && arg[2]->type == XFLOATT) {
			*((double*)arg[3]->p) = pow((*((double*)arg[1]->p)),(*((double*)arg[2]->p)));
		}
		else if (arg[1]->type == XFLOATT && arg[2]->type == XNUMT) {
			*((double*)arg[3]->p) = pow((*((double*)arg[1]->p)),(*((int64_t*)arg[2]->p)));
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XFLOATT) {
			*((double*)arg[3]->p) = pow((*((int64_t*)arg[1]->p)),(*((double*)arg[2]->p)));
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XNUMT) {
			*((double*)arg[3]->p) = pow((*((int64_t*)arg[1]->p)),(*((int64_t*)arg[2]->p)));
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `pow'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `pow' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_log(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		//~ gc_handler(arg[2]);
		arg[2]->p = new double;
		arg[2]->type = XFLOATT;

		
		if (arg[1]->type == XFLOATT && arg[2]->type == XFLOATT) {
			*((double*)arg[2]->p) = log((*((double*)arg[1]->p)));
		}
		else if (arg[1]->type == XFLOATT && arg[2]->type == XNUMT) {
			*((double*)arg[2]->p) = log((*((double*)arg[1]->p)));
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XFLOATT) {
			*((double*)arg[2]->p) = log((*((int64_t*)arg[1]->p)));
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XNUMT) {
			*((double*)arg[2]->p) = log((*((int64_t*)arg[1]->p)));
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `log'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `log' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_sin(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		//~ gc_handler(arg[2]);
		arg[2]->p = new double;
		arg[2]->type = XFLOATT;

		
		if (arg[1]->type == XFLOATT && arg[2]->type == XFLOATT) {
			*((double*)arg[2]->p) = sin((*((double*)arg[1]->p)));
		}
		else if (arg[1]->type == XFLOATT && arg[2]->type == XNUMT) {
			*((double*)arg[2]->p) = sin((*((double*)arg[1]->p)));
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XFLOATT) {
			*((double*)arg[2]->p) = sin((*((int64_t*)arg[1]->p)));
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XNUMT) {
			*((double*)arg[2]->p) = sin((*((int64_t*)arg[1]->p)));
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `sin'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `sin' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_cos(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		//~ gc_handler(arg[2]);
		arg[2]->p = new double;
		arg[2]->type = XFLOATT;

		
		if (arg[1]->type == XFLOATT && arg[2]->type == XFLOATT) {
			*((double*)arg[2]->p) = cos((*((double*)arg[1]->p)));
		}
		else if (arg[1]->type == XFLOATT && arg[2]->type == XNUMT) {
			*((double*)arg[2]->p) = cos((*((double*)arg[1]->p)));
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XFLOATT) {
			*((double*)arg[2]->p) = cos((*((int64_t*)arg[1]->p)));
		}
		else if (arg[1]->type == XNUMT && arg[2]->type == XNUMT) {
			*((double*)arg[2]->p) = cos((*((int64_t*)arg[1]->p)));
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `cos'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `cos' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_tostring(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		//~ gc_handler(arg[2]);
		arg[2]->p = new string;
		arg[2]->type = XSTRINGT;

		
		if (arg[1]->type == XFLOATT) {
			*((string*)arg[2]->p) = to_string(*((double*)arg[1]->p));
		}
		else if (arg[1]->type == XNUMT) {
			*((string*)arg[2]->p) = to_string(*((int64_t*)arg[1]->p));
		}
		else if (arg[1]->type == XFUNCT) {
			*((string*)arg[2]->p) = ((func_t*)arg[1]->p)->user;
		}
		else if (arg[1]->type == XSTRINGT) {
			//~ gc_handler(arg[2]);
			arg[2]->p = arg[1]->p;
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `tostring'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `tostring' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_tonum(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		//~ gc_handler(arg[2]);
		arg[2]->type = XNUMT;
		
		if (arg[1]->type == XFLOATT) {
			arg[2]->p = new int64_t((int64_t)(*((double*)arg[1]->p)));
		}
		else if (arg[1]->type == XSTRINGT) {
			arg[2]->p = new int64_t(stoll(*((string*)arg[1]->p)));
		}
		else if (arg[1]->type == XNUMT) {			
			arg[2]->p = arg[1]->p;
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `tonum'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `tonum' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_tofloat(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		//~ gc_handler(arg[2]);
		arg[2]->type = XFLOATT;
		
		if (arg[1]->type == XNUMT) {
			arg[2]->p = new double((double)(*((int64_t*)arg[1]->p)));
		}
		else if (arg[1]->type == XSTRINGT) {
			arg[2]->p = new double(stod(*((string*)arg[1]->p)));
		}
		else if (arg[1]->type == XFLOATT) {
			arg[2]->p = arg[1]->p;
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `tofloat'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `tofloat' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_tofunc(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		arg[2]->type = XFUNCT;

		if (arg[1]->type == XSTRINGT) {
			//~ gc_handler(arg[2]);
			
			arg[2]->p = new func_t((*((string*)arg[1]->p)));
		}
		else if (arg[1]->type == XFUNCT) {
			arg[2]->p = arg[1]->p;
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `tofunc'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `tofunc' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void recursive_replace(string& a, const string& b, const string& c) {
	if (b.empty()) {
		return;
	}
	
	size_t p;

	while ((p = a.find(b)) != a.npos) {
		a.replace(p,b.length(),c);
	}
}

inline void replace(string& a, const string& b, const string& c) {
	if (b.empty()) {
		return;
	}
	
	size_t p = 0;
	
	while ((p = a.find(b, p)) != string::npos) {
		a.replace(p,b.length(),c);
		p += c.length();
	}
}

void builtin_replace(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 5) {
		if (arg[1]->type == XSTRINGT && arg[2]->type == XSTRINGT && arg[3]->type == XSTRINGT) {
			//~ gc_handler(arg[4]);
			
			arg[4]->p = new string(*((string*)arg[1]->p));
			arg[4]->type = XSTRINGT;

			replace(*((string*)arg[4]->p),*((string*)arg[2]->p),*((string*)arg[3]->p));
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `replace'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `replace' needs exactly 4 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_tovar(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		if (arg[1]->type == XSTRINGT) {
			if (vars.top().find(*((string*)arg[1]->p)) == vars.top().end()) {
				vars.top()[*((string*)arg[1]->p)].reset(get(NULL,XNULLT));
			}

			//~ gc_handler(arg[2]);

			memcpy(arg[2].get(),vars.top()[*((string*)arg[1]->p)].get(),sizeof(instr_t));
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `tovar'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `tovar' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_makelist(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 2) {
		//~ gc_handler(arg[1]);
		arg[1]->p = new vlist;
		arg[1]->type = XLISTT;
	}
	else {
		cerr << "Fatal, Aborting: `makelist' needs exactly 1 argument (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_list_add(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		if (arg[1]->type == XLISTT) {
			for (auto& cur: *((vlist*)arg[1]->p)) {
				if (is(cur.first,arg[2])) {

					(cur.second)->p = arg[3]->p;
					(cur.second)->type = arg[3]->type;
					
					return;
				}
			}
			
			((vlist*)arg[1]->p)->push_back(pair<shared_ptr<instr_t>,shared_ptr<instr_t>>(arg[2],arg[3]));
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `list_add'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `list_add' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_find(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		if (arg[1]->type == XLISTT) {
			for (auto& cur: *((vlist*)arg[1]->p)) {
				if (is(cur.first,arg[2])) {

					//~ gc_handler(arg[3]);
					arg[3]->p = (cur.second)->p;
					arg[3]->type = (cur.second)->type;
					
					return;
				}
			}
		}
		else if (arg[1]->type == XSTRINGT && arg[2]->type == XNUMT) {
			if ((int64_t)(((string*)arg[1]->p)->length()) > *((int64_t*)arg[2]->p)) {
				//~ gc_handler(arg[3]);
				arg[3]->p = new string;
				arg[3]->type = XSTRINGT;
				((string*)arg[3]->p)->push_back(((string*)arg[1]->p)->at(*((int64_t*)arg[2]->p)));

				return;
			}
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `find'." << endl;
			halt(28);
		}

		if (arg[3]->type != XNULLT) {
			//~ gc_handler(arg[3]);
			arg[3]->p = NULL;
			arg[3]->type = XNULLT;
		}
	}
	else {
		cerr << "Fatal, Aborting: `find' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_length(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		if (arg[1]->type == XLISTT) {
			//~ gc_handler(arg[2]);
			arg[2]->p = new int64_t(((vlist*)arg[1]->p)->size());
			arg[2]->type = XNUMT;
		}
		else if (arg[1]->type == XSTRINGT) {
			//~ gc_handler(arg[2]);
			arg[2]->p = new int64_t(((string*)arg[1]->p)->size());
			arg[2]->type = XNUMT;
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `length'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `length' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_readstring(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 2) {
		//~ gc_handler(arg[1]);
		arg[1]->p = new string;
		arg[1]->type = XSTRINGT;

		cin >> *((string*)arg[1]->p);
	}
	else if (arg.size() == 3) {
		//~ gc_handler(arg[1]);
		arg[1]->p = new string;
		arg[1]->type = XSTRINGT;

		//~ gc_handler(arg[2]);
		arg[2]->p = new int64_t((cin >> *((string*)arg[1]->p)) ? 1 : 0);
		arg[2]->type = XNUMT;
	}
	else {
		cerr << "Fatal, Aborting: `readstring' needs 1 or 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_export(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		if (arg[1]->type == XSTRINGT) {
			declared_export[*((string*)arg[1]->p)] = arg[2];
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `export'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `export' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_export_clear(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 1) {
		declared_export.clear();
	}
	else {
		cerr << "Fatal, Aborting: `export_clear' needs exactly 0 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_type(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		//~ gc_handler(arg[2]);

		arg[2]->p = new string;
		arg[2]->type = XSTRINGT;

		switch (arg[1]->type) {
			case XFLOATT: *((string*)arg[2]->p) = "float"; break;
			case XNUMT: *((string*)arg[2]->p) = "num"; break;
			case XFUNCT: *((string*)arg[2]->p) = "function"; break;
			case XLISTT: *((string*)arg[2]->p) = "list"; break;
			case XNULLT: *((string*)arg[2]->p) = "null"; break;
			case XSTRINGT: *((string*)arg[2]->p) = "string"; break;
			default: *((string*)arg[2]->p) = "undefined"; break;
		}
	}
	else {
		cerr << "Fatal, Aborting: `type' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_push(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() >= 3) {
		if (arg[1]->type == XLISTT) {
			size_t i = 0;
			
			for (auto& cur: arg) {
				if (i > 1) {
					shared_ptr<instr_t> index_instr(get(new int64_t(((vlist*)arg[1]->p)->size()),XNUMT));
					((vlist*)arg[1]->p)->push_back(pair<shared_ptr<instr_t>,shared_ptr<instr_t>>(index_instr,cur));
				}
				i++;
			}
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `push'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `push' needs at least 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_or(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() >= 4) {
		//~ gc_handler(arg[arg.size()-1]);
		arg[arg.size()-1]->p = new int64_t;
		arg[arg.size()-1]->type = XNUMT;

		bool state = checkcond(arg[1]);
		size_t i = 0;

		for (auto& cur: arg) {
			if (state) break;

			if (i > 1 && i < arg.size()-1) {
				state = (state || checkcond(cur));
			}

			i++;
		}

		*((int64_t*)arg[arg.size()-1]->p) = (state) ? 1 : 0;
	}
	else {
		cerr << "Fatal, Aborting: `or' needs at least 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_and(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() >= 4) {
		//~ gc_handler(arg[arg.size()-1]);
		arg[arg.size()-1]->p = new int64_t;
		arg[arg.size()-1]->type = XNUMT;

		bool state = checkcond(arg[1]);
		size_t i = 0;

		for (auto& cur: arg) {
			if (!state) break;

			if (i > 1 && i < arg.size()-1) {
				state = (state && checkcond(cur));
			}

			i++;
		}

		*((int64_t*)arg[arg.size()-1]->p) = (state) ? 1 : 0;
	}
	else {
		cerr << "Fatal, Aborting: `and' needs at least 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_delete(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		if (arg[1]->type == XLISTT) {
			auto it = find_if(((vlist*)arg[1]->p)->begin(),((vlist*)arg[1]->p)->end(),[&arg](const pair<shared_ptr<instr_t>,shared_ptr<instr_t>>& x){return is(x.first,arg[2]);});
			
			if (it != ((vlist*)arg[1]->p)->end()) ((vlist*)arg[1]->p)->erase(it);
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `delete'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `delete' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_remove_keys(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 2) {
		if (arg[1]->type == XLISTT) {
			size_t i = 0;
			
			for (auto& cur: *((vlist*)arg[1]->p)) {
				cur.first.reset(get(new int64_t(i),XNUMT));
				i++;
			}
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `remove_keys'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `remove_keys' needs exactly 1 argument (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_curry(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() > 3) {
		if (arg[1]->type == XFUNCT) {
			//~ gc_handler(arg[arg.size()-1]);
			int64_t i = 0, ix = arg.size()-1;
			arg[ix]->p = new vlist;
			arg[ix]->type = XLISTT;

			for (auto& carg: arg) {
				if (i != 0 && i != ix) {
					((vlist*)(arg[ix]->p))->push_back(pair<shared_ptr<instr_t>,shared_ptr<instr_t>>(shared_ptr<instr_t>(get(new int64_t(i-1),XNUMT)),carg));
				}
				
				i++;
			}
		}
		else if (arg[1]->type == XLISTT) {
			//~ gc_handler(arg[arg.size()-1]);
			int64_t i = 2, ix = arg.size()-1;
			arg[ix]->p = new vlist(*((vlist*)arg[1]->p));
			arg[ix]->type = XLISTT;

			for (auto& csubst: *((vlist*)(arg[ix]->p))) {
				if (csubst.second->type == XEMPTYT && i < ix) {
					csubst.second = arg[i];
					i++;
				}
			}
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `curry'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `curry' needs at least 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_split(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		if (arg[1]->type == XSTRINGT && arg[2]->type == XSTRINGT) {
			arg[3]->p = new vlist;
			arg[3]->type = XLISTT;
			string cur;
			int64_t i = 0;
			
			if (((string*)arg[2]->p)->length() == 1) {
				for (auto& ch: *((string*)arg[1]->p)) {
					if (ch == ((string*)arg[2]->p)->at(0)) {
						((vlist*)arg[3]->p)->push_back(pair<shared_ptr<instr_t>,shared_ptr<instr_t>>(shared_ptr<instr_t>(get(new int64_t(i),XNUMT)),shared_ptr<instr_t>(get(new string(cur),XSTRINGT))));
						cur.clear();
						i++;
					}
					else {
						cur += ch;
					}
				}
				
				((vlist*)arg[3]->p)->push_back(pair<shared_ptr<instr_t>,shared_ptr<instr_t>>(shared_ptr<instr_t>(get(new int64_t(i),XNUMT)),shared_ptr<instr_t>(get(new string(cur),XSTRINGT))));
			}
			else {
				for (auto& ch: *((string*)arg[1]->p)) {
					if (in((string*)arg[2]->p,ch)) {
						((vlist*)arg[3]->p)->push_back(pair<shared_ptr<instr_t>,shared_ptr<instr_t>>(shared_ptr<instr_t>(get(new int64_t(i),XNUMT)),shared_ptr<instr_t>(get(new string(cur),XSTRINGT))));
						cur.clear();
						i++;
					}
					else {
						cur += ch;
					}
				}
				
				((vlist*)arg[3]->p)->push_back(pair<shared_ptr<instr_t>,shared_ptr<instr_t>>(shared_ptr<instr_t>(get(new int64_t(i),XNUMT)),shared_ptr<instr_t>(get(new string(cur),XSTRINGT))));
			}
		}
		else if (arg[1]->type == XSTRINGT && isnumeric(arg[2]->type)) {
			int64_t sp = getint(arg[2]);
			
			if (sp <= 0) {
				cerr << "Fatal, Aborting: split's second argument must be >= 1." << endl;
				halt(32);
			}
			else {
				arg[3]->p = new vlist;
				arg[3]->type = XLISTT;
				string cur;
				int64_t i = 0, n = 0;
			
				for (auto& ch: *((string*)arg[1]->p)) {
					n++;
					
					cur += ch;
					
					if (n%sp == 0) {
						((vlist*)arg[3]->p)->push_back(pair<shared_ptr<instr_t>,shared_ptr<instr_t>>(shared_ptr<instr_t>(get(new int64_t(i),XNUMT)),shared_ptr<instr_t>(get(new string(cur),XSTRINGT))));
						cur.clear();
						i++;
					}
				}
				
				if ((((string*)arg[1]->p)->length())%sp != 0) {				
					((vlist*)arg[3]->p)->push_back(pair<shared_ptr<instr_t>,shared_ptr<instr_t>>(shared_ptr<instr_t>(get(new int64_t(i),XNUMT)),shared_ptr<instr_t>(get(new string(cur),XSTRINGT))));
				}
			}
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `split'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `split' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_readlines(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 2 || arg.size() == 3) {
		if (arg.size() == 3) {
			if (arg[1]->type == XSTRINGT) {
				ifstream in(*((string*)(arg[1]->p)));

				if (!in.good()) {
					cerr << "Fatal, Aborting: `readlines' is unable to read the file '" << (*((string*)(arg[1]->p))) << "'." << endl;
					halt(36);
				}
				
				arg[2]->p = new vlist;
				arg[2]->type = XLISTT;
				string tmp;
				int64_t i = 0;

				while (getline(in,tmp)) {
					((vlist*)arg[2]->p)->push_back(pair<instr_t*,instr_t*>(get(new int64_t(i),XNUMT),get(new string(tmp),XSTRINGT)));
					i++;
				}
			}
			else {
				cerr << "Fatal, Aborting: Wrong types for `readlines'." << endl;
				halt(28);
			}
		}
		else {
			arg[1]->p = new vlist;
			arg[1]->type = XLISTT;
			string tmp;
			int64_t i = 0;

			while (getline(cin,tmp)) {
				((vlist*)arg[1]->p)->push_back(pair<instr_t*,instr_t*>(get(new int64_t(i),XNUMT),get(new string(tmp),XSTRINGT)));
				i++;
			}
		}
	}
	else {
		cerr << "Fatal, Aborting: `readlines' needs 1 or 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_range(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4 || arg.size() == 5) {
		if (isnumeric(arg[1]->type) && isnumeric(arg[2]->type) && (arg.size() == 4 || isnumeric(arg[3]->type))) {
			double a = getdouble(arg[1]), b = getdouble(arg[2]), step = (arg.size() == 5) ? getdouble(arg[3]) : 1.0;
			int64_t i = 0;

			if (step == 0 || (b-a)/step < 0) {
				cerr << "Fatal, Aborting: Invalid arguments for `range'." << endl;
				halt(28);
			}
			
			arg[arg.size()-1]->p = new vlist;
			arg[arg.size()-1]->type = XLISTT;
			auto dest = (vlist*)(arg[arg.size()-1]->p);

			if (b >= a) {
				for (; a <= b; a += step) {
					dest->push_back(pair<instr_t*,instr_t*>(get(new int64_t(i),XNUMT),get(new double(a),XFLOATT)));
					i++;
				}
			}
			else {
				for (; a >= b; a += step) {
					dest->push_back(pair<instr_t*,instr_t*>(get(new int64_t(i),XNUMT),get(new double(a),XFLOATT)));
					i++;
				}
			}
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `range'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `range' needs 3 or 4 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_sum(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		// sum list &res
		if (arg[1]->type == XLISTT) {
			double sum = 0;

			for (auto& cur: *((vlist*)(arg[1]->p))) {
				if (isnumeric(cur.second->type)) {
					sum += getdouble(cur.second);
				}
				else {
					cerr << "Fatal, Aborting: Wrong types in list, must be numeric." << endl;
					halt(24);
				}
			}

			arg[2]->type = XFLOATT;
			arg[2]->p = new double(sum);
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `sum'." << endl;
			halt(28);
		}
	}
	else if (arg.size() == 4) {
		// sum f range &res
		if (arg[2]->type == XLISTT) {
			if (arg[1]->type == XFUNCT || arg[1]->type == XLISTT) {
				double sum = 0;
				vector<shared_ptr<instr_t>> stack;
				shared_ptr<instr_t> p(new instr_t());

				for (auto& cur: *((vlist*)(arg[2]->p))) {
					stack.push_back(arg[1]);
					stack.push_back(cur.second);
					stack.push_back(p);

					call(stack);

					if (isnumeric(p->type)) {
						sum += getdouble(p);
					}
					else {
						cerr << "Fatal, Aborting: Wrong types; callable function did not return numeric value." << endl;
						halt(22);
					}

					stack.clear();
				}

				arg[3]->type = XFLOATT;
				arg[3]->p = new double(sum);
			}
			else {
				cerr << "Fatal, Aborting: Wrong types for `sum', must be a function." << endl;
				halt(28);
			}
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `sum'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `sum' needs 2 or 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_move(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		if (arg[1]->type == XSTRINGT && arg[2]->type == XSTRINGT) {
			if (vars.top().find(*((string*)(arg[1]->p))) != vars.top().end()) {
				vars.top()[*((string*)(arg[2]->p))] = vars.top()[*((string*)(arg[1]->p))];
				vars.top().erase(*((string*)(arg[1]->p)));
			}
			else {
				cerr << "Fatal, Aborting: move: Argument 1 must already exist." << endl;
				halt(28);
			}
		}
		else {
			cerr << "Fatal, Aborting: Wrong types for `move'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "Fatal, Aborting: `move' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}
