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
	
	printop(Palette::white,"零");
	printop(Palette::green,"第一行大苏打撒旦阿松大十大十大收到阿松大阿松大啊实打实打赏大苏打阿松大啊撒打算阿萨大的撒啊大的撒字");
	printop(Palette::green,"第二行大苏打撒旦阿松大十大十大收到阿松大阿松大啊实打实打赏大苏打阿松大啊撒打算阿萨大的撒啊大的撒字");
	printop(Palette::green,"第三行大苏打撒旦阿松大十大十大收到阿松大阿松大啊实打实打赏大苏打阿松大啊撒打算阿萨大的撒啊大的撒字");
	printop(Palette::green,"第四行大苏打撒旦阿松大十大十大收到阿松大阿松大啊实打实打赏大苏打阿松大啊撒打算阿萨大的撒啊大的撒字");

	
	
	u16 a =32;
	printf(Palette::white,"零");
	printf(Palette::red,"数字1只能答应一四川，覅二尺救兵1 1231\n23 \t带t%d",a);
	printf(Palette::red,"数字2只能答应一四川，覅二尺救兵1 1231\n23 \t带t%d",a);
	printf(Palette::red,"数字3只能答应一四川，覅二尺救兵1 1231\n23 \t带t%d",a);
	printf(Palette::red,"数字4只能答应一四川，覅二尺救兵1 1231\n23 \t带t%d",a);
	
	

			while (1) {
				swiWaitForVBlank();
				scanKeys();
				if (!(keysHeld() & KEY_A)) break;
			}
	return 0;
}
