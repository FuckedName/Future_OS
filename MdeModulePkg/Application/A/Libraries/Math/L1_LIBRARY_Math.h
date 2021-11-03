
/*************************************************
    .
    File name:      	A.c
    Author：	        任启红
    ID：		00001
    Date:          	202107
    Description:    	整个模块的主入口函数
    Others:         	无
    History:        	无
	    1.  Date:
		    Author: 
		    ID:
		    Modification:
		    
	    2.  Date:
		    Author: 
		    ID:
		    Modification:
*************************************************/




#pragma once

#define LLONG_MIN  (((INT64) -9223372036854775807LL) - 1)
#define LLONG_MAX   ((INT64)0x7FFFFFFFFFFFFFFFULL)

long long L1_MATH_Multi(long x, long y);

int L1_MATH_DigitToInteger( int c);

long long
L1_MATH_StringToLongLong(const char * nptr, char ** endptr, int base);

long L1_MATH_ABS(long v);

