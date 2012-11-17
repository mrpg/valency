/* Valency 1e-4
 * 2012, Max R. P. Grossmann
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

void interactive_mode() {
	cout << "Welcome, this is Valency 1e-4 starting in interactive mode [pre-alpha]." << endl;
	cout << "This program is free software. It comes without any warranty, to" << endl;
	cout << "the extent permitted by applicable law. You can redistribute it" << endl;
	cout << "and/or modify it under the terms of the Do What The Fuck You Want" << endl;
	cout << "To Public License, Version 2, as published by Sam Hocevar." << endl;
	cout << endl << "(c) Max R. P. Grossmann, 2012." << endl;
    cout << endl;

    string cmd;
    vector<vector<string>> line;
    vector<shared_ptr<instr_t>> instr;
    double t = 0;
    Chronometry ti;

    while (!cin.eof()) {
		cmd.clear();
		line.clear();
		instr.clear();
		cout << "> ";

		getline(cin,cmd);

		if (cmd.back() == '\\') {
			cmd.erase(cmd.end()-1);
			string cur;
			bool rep = true;

			while (rep) {
				cout << "  ";
				getline(cin,cur);
				if (cur.back() == '\\') {
					rep = true;
					cur.erase(cur.end()-1);
				}
				else {
					rep = false;
				}
				cmd += "\n"+cur;
			}
		}

		ti.start();

		istringstream cmds(cmd+"\n");

		parse(cmds,line);

		register_builtin();

		for (auto& ci: line) {
			transform(ci, instr);
			
			if (!interactive_error) call(instr);
			else interactive_error = false;
		}

		ti.stop();

		t += ti.time();
	}

	cout << endl << "Execution time: " << t << " s" << endl;
}
