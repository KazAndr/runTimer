%module runTimer
%{
#define SWIG_FILE_WITH_INIT
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cmath>
#include <bitset>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iomanip>
#include <stdlib.h>
#include "runTimer.h"
%}

%include "std_vector.i"
%include "std_string.i"
%include "typemaps.i"

namespace std{
	%template(IntVector) vector<int>;
	%template(DoubleVector) vector<double>;
	%template(LongDoubleVector) vector<long double>;
	%template(FloatVector) vector<float>;
	%template(StringVector) vector<string>;
	%template(ConstCharVector) vector<const char*>;
}




%include "runTimer.h"
%include "structRunTimer.h"

