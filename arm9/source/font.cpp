#include "font.h"
#include "font_default_frf.h"

u8 tileWidth = 0;
u8 tileHeight = 0;
u16 tileCount = 0;
u8 *fontTiles;
u16 *fontMap;
u8 textBuf[2][256 * 192];
int up =0;
int bom=0;


bool isWeak(char16_t c){
	return c < 'A' || (c > 'Z' && c < 'a') || (c > 'z' && c < 127);
}

bool isNumber(char16_t c){
	return c >= '0' && c <= '9';
}


u16 getCharIndex(char16_t c){// 字库找字
	int left = 0;
	int right = tileCount;
	while(left <= right){
		int mid = left + ((right - left) / 2);
		if(fontMap[mid] == c){
			return mid;
		}
		if(fontMap[mid] < c){
			left = mid + 1;
		}else{
			right = mid - 1;
		}
	}
	return 0;
}


void tonccpy(void *dst, const void *src, uint size){
    uint count;
    u16 *dst16;     // hword destination
    u8  *src8;      // byte source
    // Ideal case: copy by 4x words. Leaves tail for later.
    if( ((u32)src|(u32)dst)%4==0 && size>=4){
        u32 *src32= (u32*)src, *dst32= (u32*)dst;
        count= size/4;
        uint tmp= count&3;
        count /= 4;
        // Duff's Device, good friend!
        switch(tmp){
            do {    *dst32++ = *src32++;
        case 3:     *dst32++ = *src32++;
        case 2:     *dst32++ = *src32++;
        case 1:     *dst32++ = *src32++;
        case 0:     ; } while(count--);
        }
        // Check for tail
        size &= 3;
        if(size == 0)
            return;
        src8= (u8*)src32;
        dst16= (u16*)dst32;
    }else{        // Unaligned.
        uint dstOfs= (u32)dst&1;
        src8= (u8*)src;
        dst16= (u16*)(dst - dstOfs);
        // Head: 1 byte.
        if(dstOfs != 0){
            *dst16= (*dst16 & 0xFF) | *src8++<<8;
            dst16++;
            if(--size==0)
                return;
        }
    }
    // Unaligned main: copy by 2x byte.
    count= size/2;
    while(count--){
        *dst16++ = src8[0] | src8[1]<<8;
        src8 += 2;
    }
    // Tail: 1 byte.
    if(size&1)
        *dst16= (*dst16 &~ 0xFF) | *src8;
}


void Font(){
	const u8 *ptr = font_default_frf;
	ptr += 8;
	// check for and load META section
	if(memcmp(ptr, "META", 4) == 0){
		tileWidth = ptr[8];
		tileHeight = ptr[9];
		tonccpy(&tileCount, ptr + 10, sizeof(u16));
		
		u32 section_size;
		tonccpy(&section_size, ptr + 4, sizeof(u32));
		ptr += 8 + section_size;
	}
	// Character data
	if(memcmp(ptr, "CDAT", 4) == 0){
		fontTiles = new u8[tileHeight * tileCount];
		tonccpy(fontTiles, ptr + 8, tileHeight * tileCount);

		u32 section_size;
		tonccpy(&section_size, ptr + 4, sizeof(u32));
		ptr += 8 + section_size;
	} 
	// character map
	if(memcmp(ptr, "CMAP", 4) == 0){
		fontMap = new u16[tileCount];
		tonccpy(fontMap, ptr + 8, sizeof(u16) * tileCount);
		
		u32 section_size;
		tonccpy(&section_size, ptr + 4, sizeof(u32));
		ptr += 8 + section_size;
	} 
	// Copy palette to VRAM
	for(uint i = 0; i < sizeof(palette) / sizeof(palette[0]); i++){
		tonccpy(BG_PALETTE + i * 0x10, palette[i], 4);
		tonccpy(BG_PALETTE_SUB + i * 0x10, palette[i], 4);
	}
}


std::u16string utf8to16(std::string_view text){
	std::u16string out;
	for(uint i = 0; i < text.size();){
		char16_t c;
		if(!(text[i] & 0x80)){
			c = text[i++];
		}else if((text[i] & 0xE0) == 0xC0){
			c  = (text[i++] & 0x1F) << 6;
			c |=  text[i++] & 0x3F;
		}else if((text[i] & 0xF0) == 0xE0){
			c  = (text[i++] & 0x0F) << 12;
			c |= (text[i++] & 0x3F) << 6;
			c |=  text[i++] & 0x3F;
		}else{
			i++; // out of range or something (This only does up to U+FFFF since it goes to a U16 anyways)
		}
		out += c;
	}
	return out;
}


void print(bool top, std::u16string_view text, Palette palette) {
	int x = 0;
	int y = (top ? up : bom);
	auto ltrBegin = text.end(), ltrEnd = text.end();
	// Loop through string and print it
	for(auto it = text.begin(); true; it += 1){
		// If we hit the end of the string in an LTR section of an RTL
		// string, it may not be done, if so jump back to printing RTL
		if(it == text.end()){
			if(ltrBegin == text.end() || (ltrBegin == text.begin() && ltrEnd == text.end()))
				break;
		}
		// If at the end of an LTR section within RTL, jump back to the RTL
		if(it == ltrEnd && ltrBegin != text.end()){
			if(ltrBegin == text.begin() && (!isWeak(*ltrBegin) || isNumber(*ltrBegin)))
				break;
		}
		// Line break on newline or last space within 10 chars of edge in left align
		if(*it == '\n' || (*it == ' ' && 256 - x < tileWidth * 10 && text.end() - it > (256 - x) / tileWidth && *std::find(it + 1, std::min(it + (256 - x) / tileWidth, text.end()), ' ') != ' ')){
			x = 0;
			y += tileHeight;
		}
		if(x + tileWidth > 256){// Wrap at edge if left aligning
			x = 0;
			y += tileHeight;
			if(*it == ' ')// Skip to next char if a space
				it++;
		}
		u16 index = getCharIndex(*it);// 字库找字
		if(x >= 0 && x + tileWidth <= 256 && y >= 0 && y + tileHeight <= 192){// Don't draw off screen chars
			u8 *dst = textBuf[top] + x;
			for(int i = 0; i < tileHeight; i++){
				u8 px = fontTiles[(index * tileHeight) + i];
				for(int j = 0; j < tileWidth; j++){
					dst[(y + i) * 256 + j] = u8(palette) * 0x10 + ((px >> (7 - j)) & 1);
				}
			}
		}
		x += tileWidth;
	}
	(top ? up : bom) = y + tileHeight;
}




void printop(Palette palette, const char *format, ...){
	char str1[0x100];
	va_list va;
	va_start(va, format);
	vsniprintf(str1, 0x100, format, va);
	va_end(va);
	print(true, utf8to16(str1), palette);
	tonccpy(bgGetGfxPtr(2), textBuf[1], 256 * 192); 
}


void printf(Palette palette, const char *format, ...){
	char str2[0x100];
	va_list va;
	va_start(va, format);
	vsniprintf(str2, 0x100, format, va);
	va_end(va);
	print(false, utf8to16(str2), palette);
	tonccpy(bgGetGfxPtr(6), textBuf[0], 256 * 192); 
}


void clear(bool top){
	dmaFillWords(0, textBuf[top], 256 * 192); 
}
