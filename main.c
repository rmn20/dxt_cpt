#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "stb_image.h"
#include "dxtencode.h"

#define DDPF_FOURCC 0x4

typedef struct {
	int dwSize;
	int dwFlags;
	char dwFourCC[4];
	int dwRGBBitCount;
	int dwRBitMask;
	int dwGBitMask;
	int dwBBitMask;
	int dwABitMask;
} DDS_PIXELFORMAT;

#define DDSD_CAPS 0x1
#define DDSD_HEIGHT 0x2
#define DDSD_WIDTH 0x4
#define DDSD_PIXELFORMAT 0x1000
#define DDSD_MIPMAPCOUNT 0x20000

#define DDSCAPS_COMPLEX 0x8
#define DDSCAPS_MIPMAP 0x400000
#define DDSCAPS_TEXTURE 0x1000

typedef struct {
	int dwSize;
	int dwFlags;
	int dwHeight;
	int dwWidth;
	int dwPitchOrLinearSize;
	int dwDepth;
	int dwMipMapCount;
	int dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	int dwCaps;
	int dwCaps2;
	int dwCaps3;
	int dwCaps4;
	int dwReserved2;
} DDS_HEADER;

#define DDS_MAGIC 0x20534444

typedef struct {
	int dwMagic;
	DDS_HEADER header;
} DDS_FILE;

int main(int argc, char **argv) {
	if(argc == 1) {
		printf("dxt_cpt - BC1 encoder for channel packed textures\n");
		printf("dxt_cpt [options] input.png [output.dds]\n");
		
		printf("\nOptions:\n");
		printf("-mip - Generate mipmaps\n");
		
		return 0;
	}
	
	//Parse arguments
	bool mipMapping = false;
	int fileArgIndex = 0;
	
	for(int i = 1; i < argc; i++) {
		fileArgIndex = i;
		
		if(argv[i][0] != '-') break;
		
		if(strcmp(argv[i], "-mip") == 0) mipMapping = true;
	}
	
	//Load image
	char* filename = argv[fileArgIndex];
	int w, h, n;
	uint8_t* image = stbi_load(filename, &w, &h, &n, 3);
	
	if(!image) {
		printf("Can't load image %s!\n", filename);
		return 1;
	}
	
	//Allocate memory and fill dds header
	int blocksW = w / 4 + ((w & 3) > 0 ? 1 : 0);
	int blocksH = h / 4 + ((h & 3) > 0 ? 1 : 0);
	int mipLevels = 1;
	
	size_t dataSize = sizeof(DDS_FILE) + blocksW * blocksH * 8;
	
	if(mipMapping) {
		for(
			int mW = w / 2, mH = h / 2; 
			mW > 0 || mH > 0; 
			mW /= 2, mH /= 2
		) {
			int mipBW = mW / 4 + ((mW & 3) > 0 ? 1 : 0);
			int mipBH = mH / 4 + ((mH & 3) > 0 ? 1 : 0);
			
			if(mipBW <= 0) mipBW = 1;
			if(mipBH <= 0) mipBH = 1;
			
			dataSize += mipBW * mipBH * 8;
			mipLevels++;
		}
	}
	
	uint8_t* outputData = malloc(dataSize);
	assert(outputData);
	
	DDS_FILE* ddsFile = (DDS_FILE*) outputData;
	
	*ddsFile = (DDS_FILE) {
		.dwMagic = DDS_MAGIC,
		.header = {
			.dwSize = sizeof(DDS_HEADER),
			.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT,
			.dwHeight = h,
			.dwWidth = w,
			.dwDepth = 1,
			.dwPitchOrLinearSize = blocksH * blocksW / 2,
			.dwMipMapCount = mipLevels,
			.ddspf = {
				.dwSize = sizeof(DDS_PIXELFORMAT),
				.dwFlags = DDPF_FOURCC,
				.dwFourCC = {'D', 'X', 'T', '1'}
			},
			.dwCaps = DDSCAPS_TEXTURE
		}
	};
	
	//Encode texture
	encodeDXT1(outputData + sizeof(DDS_FILE), image, w, h);
	
	if(mipMapping) {
		ddsFile->header.dwFlags |= DDSD_MIPMAPCOUNT;
		ddsFile->header.dwCaps |= DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
		
		uint8_t* outputMip = outputData + sizeof(DDS_FILE) + blocksW * blocksH * 8;
		
		for(int i = 1; i < mipLevels; i++) {
			int prevW = w >> (i - 1), nextW = w >> i;
			int prevH = h >> (i - 1), nextH = h >> i;
			
			if(prevW <= 0) prevW = 1;
			if(prevH <= 0) prevH = 1;
			if(nextW <= 0) nextW = 1;
			if(nextH <= 0) nextH = 1;
			
			for(int y = 0; y < nextH; y++) {
				int y1 = y * prevH / nextH;
				int y2 = (y + 1) * prevH / nextH;
				
				for(int x = 0; x < nextW; x++) {
					int x1 = x * prevW / nextW;
					int x2 = (x + 1) * prevW / nextW;
					
					int r = 0, g = 0, b = 0;
					
					for(int yy = y1; yy < y2; yy++) {
						uint8_t* imgPtr = &image[(x1 + yy * prevW) * 3];
						
						for(int xx = x1; xx < x2; xx++, imgPtr += 3) {
							r += imgPtr[0];
							g += imgPtr[1];
							b += imgPtr[2];
						}
					}
					
					int pxCnt = (x2 - x1) * (y2 - y1);
					uint8_t* imgPtr = &image[(x + y * nextW) * 3];
					
					imgPtr[0] = (r + (pxCnt >> 1)) / pxCnt;
					imgPtr[1] = (g + (pxCnt >> 1)) / pxCnt;
					imgPtr[2] = (b + (pxCnt >> 1)) / pxCnt;
				}
			}
			
			encodeDXT1(outputMip, image, nextW, nextH);
			
			int mipBW = nextW / 4 + ((nextW & 3) > 0 ? 1 : 0);
			int mipBH = nextH / 4 + ((nextH & 3) > 0 ? 1 : 0);
			outputMip += mipBW * mipBH * 8;
		}
	}
	
	//Save file
	char* outputFname;
	
	if(argc > fileArgIndex + 1) {
		outputFname = argv[fileArgIndex + 1];
	} else {
		char* extension = ".dds";
		size_t nameLen = strlen(filename);
		size_t extLen = strlen(extension);
		
		char* dotPtr = strrchr(filename, '.');
		if(!dotPtr) dotPtr = filename + nameLen;
		
		outputFname = malloc(dotPtr - filename + extLen + 1);
		assert(outputFname);
		
		memcpy(outputFname, filename, dotPtr - filename);
		memcpy(&outputFname[dotPtr - filename], extension, extLen + 1);
	}
	
	FILE* file = fopen(outputFname, "wb");
	
	if(!file) {
		printf("Can't open file %s\n", outputFname);
	} else {
		size_t writeSize = fwrite(outputData, 1, dataSize, file);

		if(writeSize != dataSize) {
			printf("Can't write file %s\n", outputData);
		}

		fclose(file);
	}
	
	//Free memory & exit
	free(image);
	free(outputData);
	if(!(argc > fileArgIndex + 1)) free(outputFname);
	
	return 0;
}