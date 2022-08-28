#include <nds.h>
#include <stdio.h>
#include <fat.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <cstdarg>

#include "font.h"


int main() {
	
	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);
	vramSetPrimaryBanks(VRAM_A_MAIN_BG,VRAM_B_MAIN_SPRITE,VRAM_C_SUB_BG,VRAM_D_LCD);
	vramSetBankI(VRAM_I_SUB_SPRITE);
	bgInit(2, BgType_Bmp8, BgSize_B8_256x256, 1, 0);
	bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 1, 0);

	
	Font();
	iprintf(" nihao a 啊啊啊\n");//打印无效
	
	printf(3, 1,Palette::red,"你好，这是下屏显示的中文，后面是自动转行和符号n的转行\n这是转行符号之后的文字");
	printop(1, -22,Palette::green,"大家好，这是上屏显示的中文，后面是自动转行和符号n的转行\n这是转行符号之后的文字");


			while (1) {
				swiWaitForVBlank();
				scanKeys();
				if (!(keysHeld() & KEY_A)) break;
			}
	return 0;
}
