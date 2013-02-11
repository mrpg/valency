/* Valency 0.1
 * 2012, Max R. P. Grossmann
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

void builtin_set(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		gc_handler(arg[1]);
		arg[1]->p = arg[2]->p;
		arg[1]->type = arg[2]->type;
	}
	else {
		cerr << "`set' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
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
			cerr << "Wrong types for `while'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`while' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
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
			cerr << "Wrong types for `for_each'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`for_each' needs exactly 4 arguments (" << arg.size()-1 << " given)." << endl;
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
			cerr << "Wrong types for `if'." << endl;
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
			cerr << "Wrong types for `if'." << endl;
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
		cerr << "`if' needs 2 or 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_gt(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		gc_handler(arg[3]);
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
			cerr << "Wrong types for `gt'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`gt' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_gte(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		gc_handler(arg[3]);
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
			cerr << "Wrong types for `gte'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`gte' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_lt(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		gc_handler(arg[3]);
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
			cerr << "Wrong types for `lt'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`lt' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_lte(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		gc_handler(arg[3]);
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
			cerr << "Wrong types for `lte'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`lte' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
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
		gc_handler(arg[3]);
		arg[3]->p = new int64_t;
		arg[3]->type = XNUMT;

		*((int64_t*)arg[3]->p) = (is(arg[1],arg[2]) == true) ? 1 : 0;
	}
	else {
		cerr << "`is' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_not(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		gc_handler(arg[2]);
		arg[2]->p = new int64_t;
		arg[2]->type = XNUMT;

		*((int64_t*)arg[2]->p) = 0;
		
		*((int64_t*)arg[2]->p) = (!checkcond(arg[1])) ? 1 : 0;
	}
	else {
		cerr << "`not' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_pow(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		gc_handler(arg[3]);
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
			cerr << "Wrong types for `pow'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`pow' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_log(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		gc_handler(arg[2]);
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
			cerr << "Wrong types for `log'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`log' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_sin(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		gc_handler(arg[2]);
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
			cerr << "Wrong types for `sin'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`sin' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_cos(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		gc_handler(arg[2]);
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
			cerr << "Wrong types for `cos'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`cos' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_tostring(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		gc_handler(arg[2]);
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
			gc_handler(arg[2]);
			arg[2]->p = arg[1]->p;
		}
		else {
			cerr << "Wrong types for `tostring'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`tostring' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_tonum(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		gc_handler(arg[2]);
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
			cerr << "Wrong types for `tonum'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`tonum' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_tofloat(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		gc_handler(arg[2]);
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
			cerr << "Wrong types for `tofloat'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`tofloat' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_tofunc(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		arg[2]->type = XFUNCT;

		if (arg[1]->type == XSTRINGT) {
			gc_handler(arg[2]);
			
			arg[2]->p = new func_t((*((string*)arg[1]->p)));
		}
		else if (arg[1]->type == XFUNCT) {
			arg[2]->p = arg[1]->p;
		}
		else {
			cerr << "Wrong types for `tofunc'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`tofunc' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
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
			gc_handler(arg[4]);
			
			arg[4]->p = new string(*((string*)arg[1]->p));
			arg[4]->type = XSTRINGT;

			replace(*((string*)arg[4]->p),*((string*)arg[2]->p),*((string*)arg[3]->p));
		}
		else {
			cerr << "Wrong types for `replace'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`replace' needs exactly 4 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_tovar(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		if (arg[1]->type == XSTRINGT) {
			if (vars.top().find(*((string*)arg[1]->p)) == vars.top().end()) {
				vars.top()[*((string*)arg[1]->p)].reset(get(NULL,XNULLT));
			}

			gc_handler(arg[2]);

			memcpy(arg[2].get(),vars.top()[*((string*)arg[1]->p)].get(),sizeof(instr_t));
		}
		else {
			cerr << "Wrong types for `tovar'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`tovar' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_makelist(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 2) {
		gc_handler(arg[1]);
		arg[1]->p = new vlist;
		arg[1]->type = XLISTT;
	}
	else {
		cerr << "`makelist' needs exactly 1 argument (" << arg.size()-1 << " given)." << endl;
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
			cerr << "Wrong types for `list_add'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`list_add' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_find(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 4) {
		if (arg[1]->type == XLISTT) {
			for (auto& cur: *((vlist*)arg[1]->p)) {
				if (is(cur.first,arg[2])) {

					gc_handler(arg[3]);
					arg[3]->p = (cur.second)->p;
					arg[3]->type = (cur.second)->type;
					
					return;
				}
			}
		}
		else if (arg[1]->type == XSTRINGT && arg[2]->type == XNUMT) {
			if ((int64_t)(((string*)arg[1]->p)->length()) > *((int64_t*)arg[2]->p)) {
				gc_handler(arg[3]);
				arg[3]->p = new string;
				arg[3]->type = XSTRINGT;
				((string*)arg[3]->p)->push_back(((string*)arg[1]->p)->at(*((int64_t*)arg[2]->p)));

				return;
			}
		}
		else {
			cerr << "Wrong types for `find'." << endl;
			halt(28);
		}

		if (arg[3]->type != XNULLT) {
			gc_handler(arg[3]);
			arg[3]->p = NULL;
			arg[3]->type = XNULLT;
		}
	}
	else {
		cerr << "`find' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_length(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		if (arg[1]->type == XLISTT) {
			gc_handler(arg[2]);
			arg[2]->p = new int64_t(((vlist*)arg[1]->p)->size());
			arg[2]->type = XNUMT;
		}
		else if (arg[1]->type == XSTRINGT) {
			gc_handler(arg[2]);
			arg[2]->p = new int64_t(((string*)arg[1]->p)->size());
			arg[2]->type = XNUMT;
		}
		else {
			cerr << "Wrong types for `length'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`length' needs exactly 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_readstring(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 2) {
		gc_handler(arg[1]);
		arg[1]->p = new string;
		arg[1]->type = XSTRINGT;

		cin >> *((string*)arg[1]->p);
	}
	else if (arg.size() == 3) {
		gc_handler(arg[1]);
		arg[1]->p = new string;
		arg[1]->type = XSTRINGT;

		gc_handler(arg[2]);
		arg[2]->p = new int64_t((cin >> *((string*)arg[1]->p)) ? 1 : 0);
		arg[2]->type = XNUMT;
	}
	else {
		cerr << "`readstring' needs 1 or 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_export(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		if (arg[1]->type == XSTRINGT) {
			declared_export[*((string*)arg[1]->p)] = arg[2];
		}
		else {
			cerr << "Wrong types for `export'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`export' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_export_clear(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 1) {
		declared_export.clear();
	}
	else {
		cerr << "`export_clear' needs exactly 0 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_type(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() == 3) {
		gc_handler(arg[2]);

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
		cerr << "`type' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
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
			cerr << "Wrong types for `push'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`push' needs at least 2 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_or(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() >= 4) {
		gc_handler(arg[arg.size()-1]);
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
		cerr << "`or' needs at least 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_and(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() >= 4) {
		gc_handler(arg[arg.size()-1]);
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
		cerr << "`and' needs at least 3 arguments (" << arg.size()-1 << " given)." << endl;
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
			cerr << "Wrong types for `delete'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`delete' needs exactly 2 arguments (" << arg.size()-1 << " given)." << endl;
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
			cerr << "Wrong types for `remove_keys'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`remove_keys' needs exactly 1 argument (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_curry(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() > 3) {
		if (arg[1]->type == XFUNCT) {
			gc_handler(arg[arg.size()-1]);
			uint64_t i = 0, ix = arg.size()-1;
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
			gc_handler(arg[arg.size()-1]);
			uint64_t i = 2, ix = arg.size()-1;
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
			cerr << "Wrong types for `curry'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`curry' needs at least 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}
