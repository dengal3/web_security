#include <iostream>
#include <string>
#include <vector>
#include "stdio.h"
using namespace std;

#define shift(x, n) (((x) << (n)) | ((x) >> (32-(n))))//右移的时候，高位一定要补零，而不是补充符号位

unsigned int A = 0x67452301, B = 0xefcdab89, C = 0x98badcfe, D = 0x10325476;
unsigned int a, b, c, d;  // temp in functions
unsigned int group_num;

//常量ti unsigned int(abs(sin(i+1))*(2pow32))
const unsigned int t[]={
        0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,
        0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,0x698098d8,
        0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,
        0xa679438e,0x49b40821,0xf61e2562,0xc040b340,0x265e5a51,
        0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
        0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,
        0xfcefa3f8,0x676f02d9,0x8d2a4c8a,0xfffa3942,0x8771f681,
        0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,
        0xbebfbc70,0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,
        0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,0xf4292244,
        0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,
        0xffeff47d,0x85845dd1,0x6fa87e4f,0xfe2ce6e0,0xa3014314,
        0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391};
//向左位移数
const unsigned int s[]={7,12,17,22,7,12,17,22,7,12,17,22,7,
        12,17,22,5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
        4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,6,10,
        15,21,6,10,15,21,6,10,15,21,6,10,15,21};
		
const unsigned int sub[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12,
	5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2,
	0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9};

unsigned int* add(string str) {
		unsigned int num = (str.length()*8+64)/512+1;   // 一共有多少组
		unsigned int *group = new unsigned int[ num* 16 ];    // 每组有16*4*8 = 512位, 16-小组总数，4-unsigned int(放4个char),  8-位
		unsigned int str_len = str.length();
		group_num = num;
		
		for (int i = 0; i < num*16; i++) {
				group[i] = 0;
		}
		
		for (int i = 0; i < str_len; i++) {
				group[i >> 2] |= str[i] << (i%4)*8;		// 假设abcd,将以dcba存入
		}
		group[str_len>>2] |= 0x80 << (str_len%4)*8;	//补一个1  补0
		// 最后两个int留给字符串的长度的位数
		// 这里的bit位长度大小将假设小于等于unsigned int的表示上限
		group[num*16-2] |= str_len*8;
		return group;
} 

unsigned int F(unsigned int x, unsigned int y, unsigned int z) {
		return ((x)&(y))|((~x)&(z));
}

unsigned int G(unsigned int x, unsigned int y, unsigned int z) {
		return ((x)&(z))|((y)&(~z));
}

unsigned int H(unsigned int x, unsigned int y, unsigned int z) {
		return (x)^(y)^(z);
}

unsigned int I(unsigned int x, unsigned int y, unsigned int z) {
		return (y)^((x)|(~z));
}

unsigned int FF(unsigned int *_a, unsigned int *_b, unsigned int *_c, unsigned int *_d, unsigned int Mj, const unsigned int si, const unsigned int ti) {
		*_a = *_b + shift((*_a + F(*_b, *_c, *_d) + Mj + ti), si);
}

unsigned int GG(unsigned int *_a, unsigned int *_b, unsigned int *_c, unsigned int *_d, unsigned int Mj, const unsigned int si, const unsigned int ti) {
		*_a = *_b + shift((*_a + G(*_b, *_c, *_d) + Mj + ti), si);
}

unsigned int HH(unsigned int *_a, unsigned int *_b, unsigned int *_c, unsigned int *_d, unsigned int Mj, const unsigned int si, const unsigned int ti) {
		*_a = *_b + shift((*_a + H(*_b, *_c, *_d) + Mj + ti), si);
}

unsigned int II(unsigned int *_a, const unsigned int *_b, unsigned int *_c, unsigned int *_d, unsigned int Mj, const unsigned int si, const unsigned int ti) {
		*_a = *_b + shift((*_a + I(*_b, *_c, *_d) + Mj + ti), si);
}

void loop(unsigned int *group, int sub_loop_num) {
		vector<unsigned int*> q;
		q.push_back(&a);
		q.push_back(&b);
		q.push_back(&c);
		q.push_back(&d);
		switch (sub_loop_num) {
				case 0:
					for (int i = 0; i < 16; i++) {
						int sub_num = sub_loop_num*16 + i;
						//cout << hex << *q[0] << " " << *q[1] << " " << *q[2] << " " << *q[3] << endl;
						//cout << hex << *(group+sub[sub_num]) << '\t';
						//cout << hex << *(group+sub[sub_num]) << '\t';
						FF(q[0], q[1], q[2], q[3], *(group+sub[sub_num]), s[sub_num], t[sub_num]);
						q.insert(q.begin(), q.back());
						q.pop_back();
					}
					break;
				case 1:
					for (int i = 0; i < 16; i++) {
						int sub_num = sub_loop_num*16 + i;
						GG(q[0], q[1], q[2], q[3], *(group+sub[sub_num]), s[sub_num], t[sub_num]);
						q.insert(q.begin(), q.back());
						q.pop_back();
					}
					break;
				case 2:
					for (int i = 0; i < 16; i++) {
						int sub_num = sub_loop_num*16 + i;
						HH(q[0], q[1], q[2], q[3], *(group+sub[sub_num]), s[sub_num], t[sub_num]);
						q.insert(q.begin(), q.back());
						q.pop_back();
					}
					break;
				case 3:
					for (int i = 0; i < 16; i++) {
						int sub_num = sub_loop_num*16 + i;
						II(q[0], q[1], q[2], q[3], *(group+sub[sub_num]), s[sub_num], t[sub_num]);
						q.insert(q.begin(), q.back());
						q.pop_back();
					}
					break;
				default:
					break;
		}
		 /*
		 
		 cout << hex << a << '\t' << b << '\t' << c << '\t' << d << endl;
		 
		 FF(a ,b ,c ,d ,group[0] ,7 ,0xd76aa478 );
		 cout << hex << a << '\t' << b << '\t' << c << '\t' << d << endl;
		FF(d ,a ,b ,c ,group[1] ,12 ,0xe8c7b756 );
		FF(c ,d ,a ,b ,group[2] ,17 ,0x242070db );
		FF(b ,c ,d ,a ,group[3] ,22 ,0xc1bdceee );
		FF(a ,b ,c ,d ,group[4] ,7 ,0xf57c0faf );
		FF(d ,a ,b ,c ,group[5] ,12 ,0x4787c62a );
		FF(c ,d ,a ,b ,group[6] ,17 ,0xa8304613 );
		FF(b ,c ,d ,a ,group[7] ,22 ,0xfd469501);
		FF(a ,b ,c ,d ,group[8] ,7 ,0x698098d8 );
		FF(d ,a ,b ,c ,group[9] ,12 ,0x8b44f7af );
		FF(c ,d ,a ,b ,group[10] ,17 ,0xffff5bb1 );
		FF(b ,c ,d ,a ,group[11] ,22 ,0x895cd7be );
		FF(a ,b ,c ,d ,group[12] ,7 ,0x6b901122 );
		FF(d ,a ,b ,c ,group[13] ,12 ,0xfd987193 );
		FF(c ,d ,a ,b ,group[14] ,17 ,0xa679438e );
		FF(b ,c ,d ,a ,group[15] ,22 ,0x49b40821 );
		 //cout << hex << a << '\t' << b << '\t' << c << '\t' << d << endl;
		 
		 GG(a ,b ,c ,d ,group[1] ,5 ,0xf61e2562 );
		GG(d ,a ,b ,c ,group[6] ,9 ,0xc040b340 );
		GG(c ,d ,a ,b ,group[11] ,14 ,0x265e5a51 );
		GG(b ,c ,d ,a ,group[0] ,20 ,0xe9b6c7aa );
		GG(a ,b ,c ,d ,group[5] ,5 ,0xd62f105d );
		GG(d ,a ,b ,c ,group[10] ,9 ,0x02441453 );
		GG(c ,d ,a ,b ,group[15] ,14 ,0xd8a1e681 );
		GG(b ,c ,d ,a ,group[4] ,20 ,0xe7d3fbc8 );
		GG(a ,b ,c ,d ,group[9] ,5 ,0x21e1cde6 );
		GG(d ,a ,b ,c ,group[14] ,9 ,0xc33707d6 );
		GG(c ,d ,a ,b ,group[3] ,14 ,0xf4d50d87 );
		GG(b ,c ,d ,a ,group[8] ,20 ,0x455a14ed );
		GG(a ,b ,c ,d ,group[13] ,5 ,0xa9e3e905 );
		GG(d ,a ,b ,c ,group[2] ,9 ,0xfcefa3f8 );
		GG(c ,d ,a ,b ,group[7] ,14 ,0x676f02d9 );
		GG(b ,c ,d ,a ,group[12] ,20 ,0x8d2a4c8a );
		//cout << hex << a << '\t' << b << '\t' << c << '\t' << d << endl;
		
		HH(a ,b ,c ,d ,group[5] ,4 ,0xfffa3942 );
		HH(d ,a ,b ,c ,group[8] ,11 ,0x8771f681 );
		HH(c ,d ,a ,b ,group[11] ,16 ,0x6d9d6122 );
		HH(b ,c ,d ,a ,group[14] ,23 ,0xfde5380c );
		HH(a ,b ,c ,d ,group[1] ,4 ,0xa4beea44 );
		HH(d ,a ,b ,c ,group[4] ,11 ,0x4bdecfa9 );
		HH(c ,d ,a ,b ,group[7] ,16 ,0xf6bb4b60 );
		HH(b ,c ,d ,a ,group[10] ,23 ,0xbebfbc70 );
		HH(a ,b ,c ,d ,group[13] ,4 ,0x289b7ec6 );
		HH(d ,a ,b ,c ,group[0] ,11 ,0xeaa127fa );
		HH(c ,d ,a ,b ,group[3] ,16 ,0xd4ef3085 );
		HH(b ,c ,d ,a ,group[6] ,23 ,0x04881d05 );
		HH(a ,b ,c ,d ,group[9] ,4 ,0xd9d4d039 );
		HH(d ,a ,b ,c ,group[12] ,11 ,0xe6db99e5 );
		HH(c ,d ,a ,b ,group[15] ,16 ,0x1fa27cf8 );
		HH(b ,c ,d ,a ,group[2] ,23 ,0xc4ac5665 );
		//cout << hex << a << '\t' << b << '\t' << c << '\t' << d << endl;
		
		II(a ,b ,c ,d ,group[0] ,6 ,0xf4292244 );
		II(d ,a ,b ,c ,group[7] ,10 ,0x432aff97 );
		II(c ,d ,a ,b ,group[14] ,15 ,0xab9423a7 );
		II(b ,c ,d ,a ,group[5] ,21 ,0xfc93a039 );
		II(a ,b ,c ,d ,group[12] ,6 ,0x655b59c3 );
		II(d ,a ,b ,c ,group[3] ,10 ,0x8f0ccc92 );
		II(c ,d ,a ,b ,group[10] ,15 ,0xffeff47d );
		II(b ,c ,d ,a ,group[1] ,21 ,0x85845dd1 );
		II(a ,b ,c ,d ,group[8] ,6 ,0x6fa87e4f );
		II(d ,a ,b ,c ,group[15] ,10 ,0xfe2ce6e0 );
		II(c ,d ,a ,b ,group[6] ,15 ,0xa3014314 );
		II(b ,c ,d ,a ,group[13] ,21 ,0x4e0811a1 );
		II(a ,b ,c ,d ,group[4] ,6 ,0xf7537e82 );
		II(d ,a ,b ,c ,group[11] ,10 ,0xbd3af235 );
		II(c ,d ,a ,b ,group[2] ,15 ,0x2ad7d2bb );
		II(b ,c ,d ,a ,group[9] ,21 ,0xeb86d391 );
		//cout << hex << a << '\t' << b << '\t' << c << '\t' << d << endl;
		 * */
}

string getStr(unsigned int a_result) {
		string a_str = "";
		char temp[256];
		sprintf(temp, "%x", a_result & 0x000000FF);
		a_str += string(temp);
		sprintf(temp, "%x", a_result & 0x0000FF00);
		a_str += string(temp);
		sprintf(temp, "%x", a_result & 0x00FF0000);
		a_str += string(temp);
		sprintf(temp, "%x", a_result & 0xFF000000);
		a_str += string(temp);
		return a_str;
}
string changeHex(unsigned int aa)
{
	const char str16[]="0123456789abcdef";
    unsigned int bb;
    string str1;
    string str="";
    for(int i=0;i<4;i++)
    {
        str1="";
        bb=((aa>>i*8)%(1<<8))&0xff;   //逆序处理每个字节
        for (int j = 0; j < 2; j++)
        {
            str1.insert(0,1,str16[bb%16]);
            bb=bb/16;
        }
        str+=str1;
    }
    return str;
}

string md5(string str) {
		unsigned int *groups = add(str);	// pass
		a = A;
		b = B;
		c = C;
		d = D;
		for (int i = 0; i < group_num; i++) {	// every group
			A = a;
			B = b;
			C = c;
			D = d;
			for (int j = 0; j < 4; j++) {
				loop(groups+i*16, j);
			}
			a += A;
			b += B;
			c += C;
			d += D;
		}
		return changeHex(a) + changeHex(b) + changeHex(c) + changeHex(d);
}

int main() {
		string str;
		cin >> str;
		cout << md5(str) << endl;
}