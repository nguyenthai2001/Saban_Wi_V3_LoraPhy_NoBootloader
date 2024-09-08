#ifndef _DES_H_
#define _DES_H_

#include <stdio.h>
#include "string.h"
#include "stdint.h"

void SetKey(char acMyKey[8]);
void RunDesEncrypt(char acg_acMyMessage[8], char acHexMssage[16]);
void RunDesDecrypt(char acg_acMyMessage[8], char acHexMessage[16]);
void TestDES( char *MyKey , char *MyData , char *MyHex , char *ExtData );

#endif // _DES_H_