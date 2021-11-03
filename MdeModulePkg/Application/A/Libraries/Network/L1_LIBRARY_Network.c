#include <L1_LIBRARY_Network.h>


// 小端模式
// byte转int  
unsigned long long L1_NETWORK_8BytesToUINT64(unsigned char *bytes)
{
    unsigned long long s = bytes[0];
    s += ((unsigned long long)bytes[1]) << 8;
    s += ((unsigned long long)bytes[2]) << 16;
    s += ((unsigned long long)bytes[3]) << 24;
    s += ((unsigned long long)bytes[4]) << 32;
    s += ((unsigned long long)bytes[5]) << 40;
    s += ((unsigned long long)bytes[6]) << 48;
    s += ((unsigned long long)bytes[7]) << 56;
    //printf("%llu\n",  s );

    return s;
}

unsigned long long L1_NETWORK_6BytesToUINT64(unsigned char *bytes)
{
    unsigned long long s = bytes[0];
    s += ((unsigned long long)bytes[1]) << 8;
    s += ((unsigned long long)bytes[2]) << 16;
    s += ((unsigned long long)bytes[3]) << 24;
    s += ((unsigned long long)bytes[4]) << 32;
    s += ((unsigned long long)bytes[5]) << 40;
    //printf("%llu\n",  s );

    return s;
}

unsigned long L1_NETWORK_4BytesToUINT32(unsigned char *bytes)
{
    unsigned long Result = bytes[0] & 0xFF;
    Result |= (bytes[1] << 8 & 0xFF00);
    Result |= ((bytes[2] << 16) & 0xFF0000);
    Result |= ((bytes[3] << 24) & 0xFF000000);
    return Result;
}

unsigned long L1_NETWORK_3BytesToUINT32(unsigned char *bytes)
{
    //INFO_SELF("%x %x %x\n", bytes[0], bytes[1], bytes[2]);
    unsigned long Result = bytes[0] & 0xFF;
    Result |= (bytes[1] << 8 & 0xFF00);
    Result |= ((bytes[2] << 16) & 0xFF0000);
    return Result;
}

unsigned int L1_NETWORK_2BytesToUINT16(unsigned char *bytes)
{
    unsigned int Result = bytes[0] & 0xFF;
    Result |= (bytes[1] << 8 & 0xFF00);
    return Result;
}

