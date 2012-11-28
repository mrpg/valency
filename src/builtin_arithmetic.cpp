/* Valency 1e-3
 * 2012, Max R. P. Grossmann
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

inline bool all_of_type(vector<shared_ptr<instr_t>>& arg, var_type type) {
	size_t i = 0, m = arg.size()-1;

	for (auto& cur: arg) {
		if (i != 0 && i != m) {
			if (cur->type != type) {
				return false;
			}
		}
		i++;
	}

	return true;
}

inline double getdouble(shared_ptr<instr_t>& x) {
	if (x->type == XFLOATT) {
		return *((double*)x->p);
	}
	else if (x->type == XNUMT) {
		return (double)(*((int64_t*)x->p));
	}
	else {
		return 0.0;
	}
}

inline double getdouble(shared_ptr<instr_t>& x, double onfail) {
	if (x->type == XFLOATT) {
		return *((double*)x->p);
	}
	else if (x->type == XNUMT) {
		return (double)(*((int64_t*)x->p));
	}
	else {
		return onfail;
	}
}

inline int64_t getint(shared_ptr<instr_t>& x) {
	if (x->type == XFLOATT) {
		return (int64_t)(*((double*)x->p));
	}
	else if (x->type == XNUMT) {
		return (*((int64_t*)x->p));
	}
	else {
		return 0;
	}
}

inline var_type biggest_range(vector<shared_ptr<instr_t>>& arg) {
	size_t i = 0, m = arg.size()-1;
	var_type r = XNULLT;

	for (auto& cur: arg) {
		if (i != 0 && i != m) {
			if (cur->type == XFLOATT) {
				r = XFLOATT;
			}
			else if (cur->type == XSTRINGT) {
				return XSTRINGT;
			}
			else if (cur->type == XNUMT && r != XFLOATT) {
				r = XNUMT;
			}
		}
		i++;
	}

	return r;
}

void builtin_mul(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() >= 4) {
		size_t m = arg.size()-1;
		gc_handler(arg[m]);
		arg[m]->type = biggest_range(arg);
		size_t i = 0;

		if (arg[m]->type == XFLOATT) {
			arg[m]->p = new double(1);
			double* p = (double*)arg[m]->p;
			
			for (auto& cur: arg) {
				if (i != m && i != 0) {
					*p *= getdouble(cur);
				}
				i++;
			}
		}
		else if (arg[m]->type == XNUMT) {
			arg[m]->p = new int64_t(1);
			int64_t* p = (int64_t*)arg[m]->p;
			
			for (auto& cur: arg) {
				if (i != m && i != 0) {
					*p *= getint(cur);
				}
				i++;
			}
		}
		else {
			cerr << "Wrong types for `mul'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`mul' needs at least 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_add(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() >= 4) {
		size_t m = arg.size()-1;
		gc_handler(arg[m]);
		arg[m]->type = biggest_range(arg);
		size_t i = 0;

		if (arg[m]->type == XFLOATT) {
			arg[m]->p = new double(0);
			double* p = (double*)arg[m]->p;
			
			for (auto& cur: arg) {
				if (i != m && i != 0) {
					*p += getdouble(cur);
				}
				i++;
			}
		}
		else if (arg[m]->type == XNUMT) {
			arg[m]->p = new int64_t(0);
			int64_t* p = (int64_t*)arg[m]->p;
			
			for (auto& cur: arg) {
				if (i != m && i != 0) {
					*p += getint(cur);
				}
				i++;
			}
		}
		else if (arg[m]->type == XSTRINGT && all_of_type(arg,XSTRINGT)) {
			arg[m]->p = new string;
			string* p = (string*)arg[m]->p;
			
			for (auto& cur: arg) {
				if (i != m && i != 0) {
					*p += *((string*)cur->p);
				}
				i++;
			}
		}
		else {
			cerr << "Wrong types for `add'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`add' needs at least 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_sub(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() >= 4) {
		size_t m = arg.size()-1;
		gc_handler(arg[m]);
		arg[m]->type = biggest_range(arg);
		size_t i = 0;

		if (arg[m]->type == XFLOATT) {
			arg[m]->p = new double(getdouble(arg[1]));
			double* p = (double*)arg[m]->p;
			
			for (auto& cur: arg) {
				if (i != m && i != 0 && i != 1) {
					*p -= getdouble(cur);
				}
				i++;
			}
		}
		else if (arg[m]->type == XNUMT) {
			arg[m]->p = new int64_t(getint(arg[1]));
			int64_t* p = (int64_t*)arg[m]->p;
			
			for (auto& cur: arg) {
				if (i != m && i != 0 && i != 1) {
					*p -= getint(cur);
				}
				i++;
			}
		}
		else {
			cerr << "Wrong types for `sub'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`sub' needs at least 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_div(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() >= 4) {
		size_t m = arg.size()-1;
		gc_handler(arg[m]);
		arg[m]->type = XFLOATT;
		size_t i = 0;

		arg[m]->p = new double(getdouble(arg[1]));
		double* p = (double*)arg[m]->p;
		
		for (auto& cur: arg) {
			if (i != m && i != 0 && i != 1) {
				*p /= getdouble(cur,1);
			}
			i++;
		}
	}
	else {
		cerr << "`div' needs at least 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}

void builtin_mod(vector<shared_ptr<instr_t>>& arg) {
	if (arg.size() >= 4) {
		size_t m = arg.size()-1;
		gc_handler(arg[m]);
		arg[m]->type = biggest_range(arg);
		size_t i = 0;

		if (arg[m]->type == XFLOATT) {
			arg[m]->p = new double(getdouble(arg[1]));
			double* p = (double*)arg[m]->p;
			
			for (auto& cur: arg) {
				if (i != m && i != 0 && i != 1) {
					*p = fmod(*p,getdouble(cur));
				}
				i++;
			}
		}
		else if (arg[m]->type == XNUMT) {
			arg[m]->p = new int64_t(getint(arg[1]));
			int64_t* p = (int64_t*)arg[m]->p;
			
			for (auto& cur: arg) {
				if (i != m && i != 0 && i != 1) {
					*p %= getint(cur);
				}
				i++;
			}
		}
		else {
			cerr << "Wrong types for `mod'." << endl;
			halt(28);
		}
	}
	else {
		cerr << "`mod' needs at least 3 arguments (" << arg.size()-1 << " given)." << endl;
		halt(34);
	}
}
