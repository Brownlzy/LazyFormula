﻿#pragma once
#include <string>
using namespace std;

/*编码
DataByte
	[in]输入的数据长度,以字节为单位
*/
/*解码
DataByte
	[in]输入的数据长度,以字节为单位
OutByte
	[out]输出的数据长度,以字节为单位,请不要通过返回值计算
	输出数据的长度
*/
string Encode(const unsigned char* Data, int DataByte);
string Decode(const char* Data, int DataByte, int& OutByte);
