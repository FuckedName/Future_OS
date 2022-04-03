#ifndef _L1_LIBRARY_LANGUAGE_H

#define _L1_LIBRARY_LANGUAGE_H


typedef struct
{
    UINT16 AreaCode;
	UINT16 BitCode;
}GBK_Code;


UINT16 L1_LIBRARY_QueryAreaCodeBitCodeByChineseChar(INT16 ChineseChar, GBK_Code *pCode);

#endif

