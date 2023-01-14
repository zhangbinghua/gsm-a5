#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

/* bitmap header */
#include "bitmap.h"
#include "base.h"
#include "print_bin.h"

/* 
 *	eg 1: 使用代码默认参数
 *	./gsm-a5
 *	
 *	eg 2: 使用命令行传参
 *	第一个参数为key
 *	第二个参数为frameCounter
 * ./gsm-a5 0x123 0x45
 *
 *	eg 3:
 *	第三个参数为开启debug开关，可以看到很多打印和转换的具体数据内容
 *	./gsm-a5 0x123 0x45 1
 */

/*
 * 编译方法：
 * 编译：make
 * 清除：make clean
 */

struct convert_gsm {
	long getKey;
	long getFrameCounter;
	int getKeyLen;
	int getFrameCounterLen;
	bool need_covert;
	bool debug;
};

struct convert_gsm gsm_convert;

int tapsLen[3] = {4,2,4};
int regLen[3] = {19,22,23};
int XREG_TAPS[4] = {13, 16, 17, 18};
int YREG_TAPS[2] = {20, 21};
int ZREG_TAPS[4] = {7, 20, 21, 22};
int key[64] = {0,1,1,1,1,1,1,1,1,0,1,0,1,1,0,1,1,1,1,1,0,0,1,0,1,0,1,0,0,1,1,1,1,0,0,0,1,0,1,1,0,1,1,0,1,0,0,0,1,1,0,0,0,1,0,1,1,1,1,0,1,0,0,1};
int frameCounter[22] = {0,1,0,1,0,0,1,0,1,1,0,0,1,0,0,1,1,1,1,0,1,0};
int xReg[19] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int yReg[22] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int zReg[23] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int keyStream[228];


//获取当前xReg yReg zReg的taps做异或运算
int getXorValueWithTaps(int reg[], int taps[], int length)
{
	int i;
	//int len =  sizeof(taps) / sizeof(taps[0]);
	int result = reg[taps[0]];
	for(i = 1;i < length; i++){
		result = result^reg[taps[i]];
	}
	return result;
}

void shiftRegister(int reg[], int toInsert, int length)
{
	int i;
	//int len =  sizeof(reg) / sizeof(reg[0]);
	for (i = length - 1; i > 0; i--) {
		reg[i] = reg[i - 1];
	}
	reg[0] = toInsert;
}

void mixKeyIntoRegisters(void)
{
	int i;
	for (i = 0;i < 64; i++) {

		int bitX0 = getXorValueWithTaps(xReg, XREG_TAPS, tapsLen[0]);
		int bitY0 = getXorValueWithTaps(yReg, YREG_TAPS, tapsLen[1]);
		int bitZ0 = getXorValueWithTaps(zReg, ZREG_TAPS, tapsLen[2]);

		bitX0 = bitX0^key[i];
		bitY0 = bitY0^key[i];
		bitZ0 = bitZ0^key[i];

		shiftRegister(xReg, bitX0, regLen[0]);
		shiftRegister(yReg, bitY0, regLen[1]);
		shiftRegister(zReg, bitZ0, regLen[2]);
	}
}

void mixFrameCounterIntoRegisters(void)
{
	int i;
	for (i=0;i<22;i++) {
		shiftRegister(xReg, getXorValueWithTaps(xReg, XREG_TAPS, tapsLen[0])^frameCounter[i], regLen[0]);
		shiftRegister(yReg, getXorValueWithTaps(yReg, YREG_TAPS, tapsLen[1])^frameCounter[i], regLen[1]);
		shiftRegister(zReg, getXorValueWithTaps(zReg, ZREG_TAPS, tapsLen[2])^frameCounter[i], regLen[2]);
	}
}

int getMajority(void)
{
	//少数服从多数原则
	int sum = xReg[8] + yReg[10] + zReg[10];
	int resurst =  (sum==2||sum==3)?1:0;//2、3:rue  1:false
	return resurst;
}

void clock1Time(void) 
{
	int majority = getMajority();
	//与majority相等就移动 不相等就不移地
	if (xReg[8] == majority) {
		shiftRegister(xReg, getXorValueWithTaps(xReg, XREG_TAPS, tapsLen[0]), regLen[0]);
	}
	if (yReg[10] == majority) {
		shiftRegister(yReg, getXorValueWithTaps(yReg, YREG_TAPS, tapsLen[1]), regLen[1]);
	}
	if (zReg[10] == majority) {
		shiftRegister(zReg, getXorValueWithTaps(zReg, ZREG_TAPS, tapsLen[2]), regLen[2]);
	}
}

void clock100Times(void) 
{
	int i;
	for (i = 0; i < 100; i++) {
		clock1Time();
	}
}

void generate(void) 
{
	int i;
	for (i = 0; i < 228; i++) {
		keyStream[i] = xReg[18]^yReg[21]^zReg[22];
		clock1Time();
	}
}

void initialize(void) 
{
	mixKeyIntoRegisters();//与key（Kc 8byte 64bit） 64次
	mixFrameCounterIntoRegisters();//与frame（Fn 22bit） 22次
	clock100Times();//与majority 值比较循环100次
	generate();//生成228位密钥流
}

void start_to_convert(void)
{
	/* Start to convert getKey(hex) to key(int array). */
	printf("\nStart to convert getKey(hex) to key(int array).\n");
	if (gsm_convert.debug == true) {
		printf("conver before---->\n");
		printIntBinary(key, gsm_convert.getKeyLen);
	}

	covert_hex_to_binary(&gsm_convert.getKey, key, gsm_convert.getKeyLen);

	if (gsm_convert.debug == true) {
		printf("convert after---->\n");
		printIntBinary(key, gsm_convert.getKeyLen);
	}

	/* Start to convert getFrameCounter(hex) to frameCounter(int array). */
	printf("\nStart to convert getFrameCounter(hex) to frameCounter(int array). \n");

	if (gsm_convert.debug == true) {
		printf("conver before---->\n");
		printIntBinary(frameCounter, gsm_convert.getFrameCounterLen);
	}

	covert_hex_to_binary(&gsm_convert.getFrameCounter, frameCounter, gsm_convert.getFrameCounterLen);

	if (gsm_convert.debug == true) {
		printf("convert after---->\n");
		printIntBinary(frameCounter, gsm_convert.getFrameCounterLen);
	}
}

int main(int argc, char *argv[])
{
	int i, ret;
	long getKey, getFrameCounter;
	int getKeyLen, getFrameCounterLen;
	long debug;

	gsm_convert.need_covert = false;// do not need conver.
	gsm_convert.debug = false;
	
   /*  Write C code in this online editor and run it. */
	printf("Hello, World! \n");

    switch (argc) {
	case 1:
		printf("Use default key and frameCounter.\n");
		break;
	case 3:
		getKey = strtoul(argv[1], 0, 0);
		getFrameCounter = strtoul(argv[2], 0, 0);
		gsm_convert.need_covert = true;
		printf("getKey=%#lx getFrameCounter=%#lx\n", getKey, getFrameCounter);
		break;
	case 4:
		getKey = strtoul(argv[1], 0, 0);
		getFrameCounter = strtoul(argv[2], 0, 0);
		debug = strtoul(argv[3], 0, 0);
		gsm_convert.need_covert = true;
		gsm_convert.debug = true;
		printf("getKey=%#lx getFrameCounter=%#lx debug=%ld\n", getKey, getFrameCounter, debug);
		break;
	default:
		printf("ERROR input must be one or three params!!! \n"
			   "Like this: ./gsm-a5 0x123(key) 0x45(frameCounter) \n"
			   "Like this: ./gsm-a5 0x123(key) 0x45(frameCounter)  1(debug on)\n");
		exit(1);
    }

	if (gsm_convert.need_covert == false)
		printf("Do not need conver.\n");
	else
		printf("Need conver.\n");

	getKeyLen = sizeof(key)/sizeof(key[0]);
	getFrameCounterLen = sizeof(frameCounter)/sizeof(frameCounter[0]);
	printf("getKeyLen=%d getFrameCounterLen=%d\n", getKeyLen, getFrameCounterLen);

	gsm_convert.getKeyLen = getKeyLen;
	gsm_convert.getFrameCounterLen = getFrameCounterLen;

	if (gsm_convert.need_covert == true) {
		gsm_convert.getKey = getKey;
		gsm_convert.getFrameCounter = getFrameCounter;
		start_to_convert();
	}

	initialize();
	for(i = 0;i < 228; i++) {
		printf("%d", keyStream[i]);
	}
	printf("\n");

	return 0;
}