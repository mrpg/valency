/* Valency 0.2
 * 2012, Max R. P. Grossmann
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include "util.cpp"
#include "generic.cpp"
#include "builtin_arithmetic.cpp"
#include "builtin.cpp"
#include "interactive.cpp"

using namespace std;

bool emptyveccond(const vector<string>& c) {
	if (c.size() == 0) {
		return true;
	}
	else if (c.size() == 1) {
		if (c.at(0).length() == 0) {
			return true;
		}
	}

	return false;
}

bool iswaste(char c) {
	if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
		return true;
	}
	else {
		return false;
	}
}

char escape(char c) {
	if (c == '"') {
		return c;
	}
	else if (c == 't') {
		return '\t';
	}
	else if (c == 'n') {
		return '\n';
	}
	else if (c == 'r') {
		return '\r';
	}
	else {
		return c;
	}
}

template <class Stream>
void parse(Stream& stream, vector<vector<string>>& lines) {
	vector<string> cl;
	string ci;
	char ch;

	while (!stream.eof()) {
		while (iswaste(stream.peek())) {
			stream.get(ch);
		}

		while (!stream.eof()) {
			ch = stream.get();

			if (ch == ' ') continue;
			if (ch == '\n') break;
			
			if (ch == '"') {
				ci = ch;

				while ((ch = stream.get()) != '"' && ch != EOF) {
					ci += ch;

					if (ch == '\\') {
						ci.erase(ci.end()-1);
						ci += escape(stream.get());
					}
				}

				ci += '"';

				cl.push_back(ci);
				ci = "";
			}
			else if (ch == '{') {
				int lvl = 0;
				bool in_string = false;
				ci = ch;
				ci += ' ';

				while (!((ch = stream.get()) == '}' && lvl == 0 && !in_string) && ch != EOF) {
					ci += ch;

					if (ch == '"' && !in_string) {
						in_string = true;
					}
					else if (ch == '"' && in_string) {
						in_string = false;
					}

					if (ch == '{' && !in_string) {
						lvl++;
					}
					else if (ch == '}' && !in_string) {
						lvl--;
					}
				}

				ci += ch;
				cl.push_back(ci);
				ci = "";
			}
			else if (ch == '(') {
				int lvl = 0;
				bool in_string = false;
				ci = ch;

				while (!((ch = stream.get()) == ')' && lvl == 0 && !in_string) && ch != EOF) {
					ci += ch;

					if (ch == '"' && !in_string) {
						in_string = true;
					}
					else if (ch == '"' && in_string) {
						in_string = false;
					}

					if (ch == '(' && !in_string) {
						lvl++;
					}
					else if (ch == ')' && !in_string) {
						lvl--;
					}
				}

				ci += ch;
				cl.push_back(ci);
				ci = "";
			}
			else {
				ci = ch;
				while ((ch = stream.get()) != ' ' && ch != '\n') {
					if (ch != EOF) ci += ch;
				}
				cl.push_back(ci);
				ci = "";

				if (ch == '\n') break;
			}
		}

		lines.push_back(cl);
		cl.clear();
	}

	auto it = lines.begin();

	while ((it = find_if (lines.begin(),lines.end(),emptyveccond)) != lines.end()) {
		lines.erase(it);
	}

	string chl = "\x0009\x000A\x000B\x000C\x000D\x0020\x0085";
	chl.push_back(EOF);
	sort(chl.begin(),chl.end());

	for (vector<string>& outer: lines) {
		for (string& ctok: outer) {
			trimhere(ctok,chl);
		}
	}
}

inline bool isnum(const string& str) {
	if (str.length() == 0 || str.find('.') || str.find('e') || str.find('E')) {
		return false;
	}
	
	try {
		stoll(str);
	}
	catch (...) {
		return false;
	}

	return true;
}

bool isfloat(const string& str) {
	if (str.length() == 0) {
		return false;
	}
	
	try {
		stod(str);
	}
	catch (...) {
		return false;
	}

	return true;
}

inline bool checkcond(shared_ptr<instr_t> a) {
	if (a->type == XNUMT) {
		if (*((int64_t*)a->p) != 0) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (a->type == XFLOATT) {
		if (*((double*)a->p) != 0) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (a->type == XSTRINGT) {
		return !(((string*)a->p)->empty());
	}
	else if (a->type == XFUNCT || a->type == XLISTT) {
		vector<shared_ptr<instr_t>> stack;
		shared_ptr<instr_t> p(new instr_t());

		if (a->type == XFUNCT) {
			((func_t*)a->p)->inCurrentNamespace = true;
		}

		stack.push_back(a);
		stack.push_back(p);

		call(stack);

		return checkcond(p);
	}
	else {
		return (a->type != XNULLT);
	}
}

shared_ptr<instr_t> getinstr(const string& str) {
	if (str.empty()) {
		return NULL;
	}

	shared_ptr<instr_t> r(NULL);

	if (str[0] == '{') {
		r.reset(get(new func_t(str),XFUNCT));

		((string*)r->p)->erase(0,1);
		((string*)r->p)->erase(((string*)r->p)->end()-1);
	}
	else if (str[0] == '(') {		
		string vnm = str;
		vector<vector<string>> line;
		vector<shared_ptr<instr_t>> instr;
		vnm.erase(0,1);
		vnm.erase(vnm.end()-1);
		istringstream fstr(vnm+"\n");

		parse(fstr, line);

		if (!line.empty()) {
			r.reset(get(NULL,XNULLT));
			
			transform(line[0], instr);
			instr.push_back(r);
			call(instr);
		}
	}
	else if (str[0] == '"') {
		r.reset(get(new string(str), XSTRINGT));

		((string*)r->p)->erase(0,1);
		((string*)r->p)->erase(((string*)r->p)->end()-1);
	}
	else if (str[0] == '&') {
		string vnm = str;
		vnm.erase(0,1);

		if (vars.top().find(vnm) == vars.top().end()) {
			vars.top()[vnm].reset(get(NULL,XNULLT));
		}
		
		r = vars.top()[vnm];
	}
	else if (str[0] == '?') {
		r.reset(get());

		r->type = XNUMT;
		string vnm = str;
		vnm.erase(0,1);

		if (vars.top().find(vnm) != vars.top().end()) {
			r->p = new int64_t((checkcond(vars.top()[vnm])) ? 1 : 0);
		}
		else {
			r->p = new int64_t(0);
		}
	}
	else if (isnum(str)) {
		r.reset(get(new int64_t(stoll(str)),XNUMT));
	}
	else if (vars.top().find(str) != vars.top().end()) {
		r = vars.top()[str]->copy();
	}
	else if (isfloat(str)) {
		r.reset(get(new double(stod(str)),XFLOATT));
	}
	else if (str == "..") {
		r.reset(get(NULL,XEMPTYT));
	}
	else {
		cerr << "Fatal error: `" << str << "' is unknown." << endl;
		halt(2);
	}

	return r;
}

void transform(const vector<string>& lines, vector<shared_ptr<instr_t>>& c) {
	c.clear();
	
	size_t i = 0;

	for (auto& ci: lines) {
		if (i == 0) {
			auto pos = vars.top().find(ci);
			
			if (pos == vars.top().end() && (ci.length() == 0 || (ci.length() >= 1 && ci[0] == '#') || (ci.length() >= 2 && ci[0] == '-' && ci[1] == '-'))) {
				break;
			}
			
			if (pos != vars.top().end() && pos->second->type == XFUNCT) {
				c.push_back(pos->second);
			}
			else if (pos != vars.top().end() && pos->second->type == XLISTT && ((vlist*)(pos->second->p))->size() > 1 && ((vlist*)(pos->second->p))->at(0).second->type == XFUNCT) {
				c.push_back(pos->second);
			}
			else if (ci.length() > 2 && ((ci.front() == '(' && ci.back() == ')') || (ci.front() == '{' && ci.back() == '}'))) {
				c.push_back(getinstr(ci));
			}
			else {
				cerr << "Fatal error: `" << ci << "' is not callable." << endl;
				halt(3);
			}
		}
		else {
			c.push_back(getinstr(ci));
		}

		i++;
	}
}

void call(vector<shared_ptr<instr_t>>& ci) {
	if (ci.size() == 0) {
		return;
	}
	
	if (ci[0]->type == XFUNCT) {
		(*(func_t*)(ci[0]->p))(ci);
	}
	else if (ci[0]->type == XLISTT) {
		vector<shared_ptr<instr_t>> nci;
		uint64_t ix = 1;

		for (auto& cur: *((vlist*)ci[0]->p)) {
			if (cur.second->type != XEMPTYT) {
				nci.push_back(cur.second);
			}
			else {
				if (ci.size() > ix-1) {
					nci.push_back(ci[ix]);
				}
				else {
					nci.push_back(shared_ptr<instr_t>(get(NULL,XNULLT)));
				}

				ix++;
			}
		}

		for (auto& cur: ci) {
			if (ix == 0) {
				nci.push_back(cur);
			}
			else {
				ix--;
			}
		}

		(*(func_t*)(nci[0]->p))(nci);
	}
	else {
		cerr << "Fatal error: This is not callable." << endl;
		exit(122);
	}
}

void register_builtin() {
	#define alias(name,orig) vars.top()[#name].reset(get(new func_t(builtin_##orig),XFUNCT));
	#define reg(name) vars.top()[#name].reset(get(new func_t(builtin_##name),XFUNCT));
	
	reg(add); alias(+,add);
	reg(and); alias(&&,and);
	reg(cos);
	reg(curry);
	reg(do);
	reg(delete);
	reg(div); alias(/,div);
	reg(exit);
	reg(export);
	reg(export_clear);
	reg(factorial); alias(!,factorial);
	reg(find);
	reg(for_each);
	reg(for_each_key);
	reg(for_each_value);
	reg(gamma);
	reg(gt); alias(>,gt);
	reg(gte); alias(>=,gte);
	reg(if);
	reg(is); alias(==,is);
	reg(length);
	reg(list);
	reg(list_add);
	reg(log); alias(ln,log);
	reg(lt); alias(<,lt);
	reg(lte); alias(<=,lte);
	reg(makelist);
	reg(mod); alias(%,mod);
	reg(move); alias(->,move);
	reg(mul); alias(*,mul);
	reg(not);
	reg(or); alias(||,or);
	reg(pow); alias(^,pow);
	reg(print);
	reg(push);
	reg(range);
	reg(readlines);
	reg(readstring);
	reg(remove_keys);
	reg(replace);
	reg(set);
	reg(sin);
	reg(split);
	reg(sub); alias(-,sub);
	reg(sum);
	reg(tofloat);
	reg(tofunc);
	reg(tonum);
	reg(tostring);
	reg(tovar);
	reg(type); alias(:,type);
	reg(while);
	reg(write);
}

int main(int argc, char** argv) {
	Chronometry t;

	vector<vector<string>> lines;
	vector<shared_ptr<instr_t>> instr;

	vars.push(unordered_map<string,shared_ptr<instr_t>>());

	cout.precision(16);
	cerr.precision(16);

	if (argc == 2) {
		if (string(argv[1]) != "-i") {
			ifstream fp(argv[1]);

			if (!fp.good()) {
				cerr << "Fatal error: File `" << argv[1] << "' cannot be read." << endl;
				halt(156);
			}

			stringstream buffer;

			copy(istreambuf_iterator<char>(fp),istreambuf_iterator<char>(),ostreambuf_iterator<char>(buffer));

			fp.close();

			t.start();
			
			parse(buffer,lines);
		}
		else {
			interactive = true;

			interactive_mode();
	
			return 0;
		}
	}
	else {
		t.start();
		
		parse(cin,lines);
	}

	register_builtin();

	for (auto& ci: lines) {
		transform(ci, instr);
		call(instr);
	}

	t.stop();

	cerr << "t = " << t.time() << " s" << endl;

	//~ while (!vars.empty()) {
		//~ for (auto& cur: vars.top()) {
			//~ gc_handler(cur.second);
		//~ }
		//~ 
		//~ vars.pop();
	//~ }
}
