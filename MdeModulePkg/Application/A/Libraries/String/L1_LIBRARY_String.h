
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



int L1_STRING_ToUpper( int c  );

int L1_STRING_IsSpace (int c);

int L1_STRING_IsAllNumber (int c);

int L1_STRING_Compare(unsigned char *p1, unsigned char *p2, unsigned int length);

char *L1_STRING_FloatToString(float val, int precision, char *buf);

unsigned long long L1_STRING_Length(char *String);

