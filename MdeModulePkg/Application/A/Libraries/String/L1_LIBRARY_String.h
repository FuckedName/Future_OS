#pragma once



int L1_STRING_ToUpper( int c  );

int L1_STRING_IsSpace (int c);

int L1_STRING_IsAllNumber (int c);

int L1_STRING_Compare(unsigned char *p1, unsigned char *p2, unsigned int length);

char *L1_STRING_FloatToString(float val, int precision, char *buf);

unsigned long long L1_STRING_Length(char *String);

