/* Valency 1e-4
 * 2012, Max R. P. Grossmann
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include <chrono>

using namespace std;

class Chronometry {
	public:
		inline void start();
		inline void stop();
		inline double round();
		inline long double time();

	private:
		chrono::time_point<chrono::high_resolution_clock> a,b;
};

inline void Chronometry::start() {
	a = chrono::high_resolution_clock::now();
}

inline void Chronometry::stop() {
	b = chrono::high_resolution_clock::now();
}

inline double Chronometry::round() {
	return ((chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now()-a).count())/1000000000.0);
}

inline long double Chronometry::time() {
	return ((chrono::duration_cast<chrono::nanoseconds>(b-a).count())/1000000000.0);
}
