//This file is a part of dxt_cpt - a BC1 encoder for channel packed textures
//dxt_cpt is based on stb_dxt
//See end of file for license information

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>

static const unsigned char stb__OMatch5[256][2] = {
   {  0,  0 }, {  0,  0 }, {  0,  1 }, {  0,  1 }, {  1,  0 }, {  1,  0 }, {  1,  0 }, {  1,  1 },
   {  1,  1 }, {  1,  1 }, {  1,  2 }, {  0,  4 }, {  2,  1 }, {  2,  1 }, {  2,  1 }, {  2,  2 },
   {  2,  2 }, {  2,  2 }, {  2,  3 }, {  1,  5 }, {  3,  2 }, {  3,  2 }, {  4,  0 }, {  3,  3 },
   {  3,  3 }, {  3,  3 }, {  3,  4 }, {  3,  4 }, {  3,  4 }, {  3,  5 }, {  4,  3 }, {  4,  3 },
   {  5,  2 }, {  4,  4 }, {  4,  4 }, {  4,  5 }, {  4,  5 }, {  5,  4 }, {  5,  4 }, {  5,  4 },
   {  6,  3 }, {  5,  5 }, {  5,  5 }, {  5,  6 }, {  4,  8 }, {  6,  5 }, {  6,  5 }, {  6,  5 },
   {  6,  6 }, {  6,  6 }, {  6,  6 }, {  6,  7 }, {  5,  9 }, {  7,  6 }, {  7,  6 }, {  8,  4 },
   {  7,  7 }, {  7,  7 }, {  7,  7 }, {  7,  8 }, {  7,  8 }, {  7,  8 }, {  7,  9 }, {  8,  7 },
   {  8,  7 }, {  9,  6 }, {  8,  8 }, {  8,  8 }, {  8,  9 }, {  8,  9 }, {  9,  8 }, {  9,  8 },
   {  9,  8 }, { 10,  7 }, {  9,  9 }, {  9,  9 }, {  9, 10 }, {  8, 12 }, { 10,  9 }, { 10,  9 },
   { 10,  9 }, { 10, 10 }, { 10, 10 }, { 10, 10 }, { 10, 11 }, {  9, 13 }, { 11, 10 }, { 11, 10 },
   { 12,  8 }, { 11, 11 }, { 11, 11 }, { 11, 11 }, { 11, 12 }, { 11, 12 }, { 11, 12 }, { 11, 13 },
   { 12, 11 }, { 12, 11 }, { 13, 10 }, { 12, 12 }, { 12, 12 }, { 12, 13 }, { 12, 13 }, { 13, 12 },
   { 13, 12 }, { 13, 12 }, { 14, 11 }, { 13, 13 }, { 13, 13 }, { 13, 14 }, { 12, 16 }, { 14, 13 },
   { 14, 13 }, { 14, 13 }, { 14, 14 }, { 14, 14 }, { 14, 14 }, { 14, 15 }, { 13, 17 }, { 15, 14 },
   { 15, 14 }, { 16, 12 }, { 15, 15 }, { 15, 15 }, { 15, 15 }, { 15, 16 }, { 15, 16 }, { 15, 16 },
   { 15, 17 }, { 16, 15 }, { 16, 15 }, { 17, 14 }, { 16, 16 }, { 16, 16 }, { 16, 17 }, { 16, 17 },
   { 17, 16 }, { 17, 16 }, { 17, 16 }, { 18, 15 }, { 17, 17 }, { 17, 17 }, { 17, 18 }, { 16, 20 },
   { 18, 17 }, { 18, 17 }, { 18, 17 }, { 18, 18 }, { 18, 18 }, { 18, 18 }, { 18, 19 }, { 17, 21 },
   { 19, 18 }, { 19, 18 }, { 20, 16 }, { 19, 19 }, { 19, 19 }, { 19, 19 }, { 19, 20 }, { 19, 20 },
   { 19, 20 }, { 19, 21 }, { 20, 19 }, { 20, 19 }, { 21, 18 }, { 20, 20 }, { 20, 20 }, { 20, 21 },
   { 20, 21 }, { 21, 20 }, { 21, 20 }, { 21, 20 }, { 22, 19 }, { 21, 21 }, { 21, 21 }, { 21, 22 },
   { 20, 24 }, { 22, 21 }, { 22, 21 }, { 22, 21 }, { 22, 22 }, { 22, 22 }, { 22, 22 }, { 22, 23 },
   { 21, 25 }, { 23, 22 }, { 23, 22 }, { 24, 20 }, { 23, 23 }, { 23, 23 }, { 23, 23 }, { 23, 24 },
   { 23, 24 }, { 23, 24 }, { 23, 25 }, { 24, 23 }, { 24, 23 }, { 25, 22 }, { 24, 24 }, { 24, 24 },
   { 24, 25 }, { 24, 25 }, { 25, 24 }, { 25, 24 }, { 25, 24 }, { 26, 23 }, { 25, 25 }, { 25, 25 },
   { 25, 26 }, { 24, 28 }, { 26, 25 }, { 26, 25 }, { 26, 25 }, { 26, 26 }, { 26, 26 }, { 26, 26 },
   { 26, 27 }, { 25, 29 }, { 27, 26 }, { 27, 26 }, { 28, 24 }, { 27, 27 }, { 27, 27 }, { 27, 27 },
   { 27, 28 }, { 27, 28 }, { 27, 28 }, { 27, 29 }, { 28, 27 }, { 28, 27 }, { 29, 26 }, { 28, 28 },
   { 28, 28 }, { 28, 29 }, { 28, 29 }, { 29, 28 }, { 29, 28 }, { 29, 28 }, { 30, 27 }, { 29, 29 },
   { 29, 29 }, { 29, 30 }, { 29, 30 }, { 30, 29 }, { 30, 29 }, { 30, 29 }, { 30, 30 }, { 30, 30 },
   { 30, 30 }, { 30, 31 }, { 30, 31 }, { 31, 30 }, { 31, 30 }, { 31, 30 }, { 31, 31 }, { 31, 31 },
};

static const unsigned char stb__OMatch6[256][2] = {
   {  0,  0 }, {  0,  1 }, {  1,  0 }, {  1,  1 }, {  1,  1 }, {  1,  2 }, {  2,  1 }, {  2,  2 },
   {  2,  2 }, {  2,  3 }, {  3,  2 }, {  3,  3 }, {  3,  3 }, {  3,  4 }, {  4,  3 }, {  4,  4 },
   {  4,  4 }, {  4,  5 }, {  5,  4 }, {  5,  5 }, {  5,  5 }, {  5,  6 }, {  6,  5 }, {  6,  6 },
   {  6,  6 }, {  6,  7 }, {  7,  6 }, {  7,  7 }, {  7,  7 }, {  7,  8 }, {  8,  7 }, {  8,  8 },
   {  8,  8 }, {  8,  9 }, {  9,  8 }, {  9,  9 }, {  9,  9 }, {  9, 10 }, { 10,  9 }, { 10, 10 },
   { 10, 10 }, { 10, 11 }, { 11, 10 }, {  8, 16 }, { 11, 11 }, { 11, 12 }, { 12, 11 }, {  9, 17 },
   { 12, 12 }, { 12, 13 }, { 13, 12 }, { 11, 16 }, { 13, 13 }, { 13, 14 }, { 14, 13 }, { 12, 17 },
   { 14, 14 }, { 14, 15 }, { 15, 14 }, { 14, 16 }, { 15, 15 }, { 15, 16 }, { 16, 14 }, { 16, 15 },
   { 17, 14 }, { 16, 16 }, { 16, 17 }, { 17, 16 }, { 18, 15 }, { 17, 17 }, { 17, 18 }, { 18, 17 },
   { 20, 14 }, { 18, 18 }, { 18, 19 }, { 19, 18 }, { 21, 15 }, { 19, 19 }, { 19, 20 }, { 20, 19 },
   { 20, 20 }, { 20, 20 }, { 20, 21 }, { 21, 20 }, { 21, 21 }, { 21, 21 }, { 21, 22 }, { 22, 21 },
   { 22, 22 }, { 22, 22 }, { 22, 23 }, { 23, 22 }, { 23, 23 }, { 23, 23 }, { 23, 24 }, { 24, 23 },
   { 24, 24 }, { 24, 24 }, { 24, 25 }, { 25, 24 }, { 25, 25 }, { 25, 25 }, { 25, 26 }, { 26, 25 },
   { 26, 26 }, { 26, 26 }, { 26, 27 }, { 27, 26 }, { 24, 32 }, { 27, 27 }, { 27, 28 }, { 28, 27 },
   { 25, 33 }, { 28, 28 }, { 28, 29 }, { 29, 28 }, { 27, 32 }, { 29, 29 }, { 29, 30 }, { 30, 29 },
   { 28, 33 }, { 30, 30 }, { 30, 31 }, { 31, 30 }, { 30, 32 }, { 31, 31 }, { 31, 32 }, { 32, 30 },
   { 32, 31 }, { 33, 30 }, { 32, 32 }, { 32, 33 }, { 33, 32 }, { 34, 31 }, { 33, 33 }, { 33, 34 },
   { 34, 33 }, { 36, 30 }, { 34, 34 }, { 34, 35 }, { 35, 34 }, { 37, 31 }, { 35, 35 }, { 35, 36 },
   { 36, 35 }, { 36, 36 }, { 36, 36 }, { 36, 37 }, { 37, 36 }, { 37, 37 }, { 37, 37 }, { 37, 38 },
   { 38, 37 }, { 38, 38 }, { 38, 38 }, { 38, 39 }, { 39, 38 }, { 39, 39 }, { 39, 39 }, { 39, 40 },
   { 40, 39 }, { 40, 40 }, { 40, 40 }, { 40, 41 }, { 41, 40 }, { 41, 41 }, { 41, 41 }, { 41, 42 },
   { 42, 41 }, { 42, 42 }, { 42, 42 }, { 42, 43 }, { 43, 42 }, { 40, 48 }, { 43, 43 }, { 43, 44 },
   { 44, 43 }, { 41, 49 }, { 44, 44 }, { 44, 45 }, { 45, 44 }, { 43, 48 }, { 45, 45 }, { 45, 46 },
   { 46, 45 }, { 44, 49 }, { 46, 46 }, { 46, 47 }, { 47, 46 }, { 46, 48 }, { 47, 47 }, { 47, 48 },
   { 48, 46 }, { 48, 47 }, { 49, 46 }, { 48, 48 }, { 48, 49 }, { 49, 48 }, { 50, 47 }, { 49, 49 },
   { 49, 50 }, { 50, 49 }, { 52, 46 }, { 50, 50 }, { 50, 51 }, { 51, 50 }, { 53, 47 }, { 51, 51 },
   { 51, 52 }, { 52, 51 }, { 52, 52 }, { 52, 52 }, { 52, 53 }, { 53, 52 }, { 53, 53 }, { 53, 53 },
   { 53, 54 }, { 54, 53 }, { 54, 54 }, { 54, 54 }, { 54, 55 }, { 55, 54 }, { 55, 55 }, { 55, 55 },
   { 55, 56 }, { 56, 55 }, { 56, 56 }, { 56, 56 }, { 56, 57 }, { 57, 56 }, { 57, 57 }, { 57, 57 },
   { 57, 58 }, { 58, 57 }, { 58, 58 }, { 58, 58 }, { 58, 59 }, { 59, 58 }, { 59, 59 }, { 59, 59 },
   { 59, 60 }, { 60, 59 }, { 60, 60 }, { 60, 60 }, { 60, 61 }, { 61, 60 }, { 61, 61 }, { 61, 61 },
   { 61, 62 }, { 62, 61 }, { 62, 62 }, { 62, 62 }, { 62, 63 }, { 63, 62 }, { 63, 63 }, { 63, 63 },
};

typedef struct {
	uint16_t endpoint0;
	uint16_t endpoint1;
	uint32_t indices;
} DXTBlock;

typedef struct {
	int x1, y1, x2, y2;
	int pixelsCount;
} Bounds2D;

Bounds2D calculateBounds(int x, int y, int w, int h, int seamSize) {
	int minx = x - seamSize, miny = y - seamSize;
	int maxx = x + 4 + seamSize, maxy = y + 4 + seamSize;
	
	if(minx < 0) minx = 0;
	if(miny < 0) miny = 0;
	if(maxx > w) maxx = w;
	if(maxy > h) maxy = h;
	
	return (Bounds2D) {
		minx, miny, maxx, maxy,
		(maxx - minx) * (maxy - miny)
	};
}

const int COL_STRIDE = 3;

float clampf(float v, float min, float max) {
	return fmaxf(min, fminf(max, v));
}

float distanceSquare(float x1, float y1, float z1, float x2, float y2, float z2) {
	return 
		(x1 - x2) * (x1 - x2) + 
		(y1 - y2) * (y1 - y2) +
		(z1 - z2) * (z1 - z2);
}

void normalize(float* vec) {
	float invLev = vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];
	invLev = 1 / fmaxf(sqrtf(invLev), 0.000001f);
	
	vec[0] *= invLev;
	vec[1] *= invLev;
	vec[2] *= invLev;
}

uint16_t encodeRGB565(float* col) {
	return 
		(((int) roundf(clampf(col[0] * 31 / 255, 0, 31))) << 11) |
		(((int) roundf(clampf(col[1] * 63 / 255, 0, 63))) << 5) |
		(((int) roundf(clampf(col[2] * 31 / 255, 0, 31))));
}

void decodeRGB565(uint16_t col, float* output) {
	output[0] = (((col & 0xF800) >> 8) | ((col & 0xF800) >> 13));
	output[1] = (((col & 0x07E0) >> 3) | ((col & 0x07E0) >> 9));
	output[2] = (((col & 0x001F) << 3) | ((col & 0x001F) >> 2));
}

void calculatePalette(uint16_t endpoint0, uint16_t endpoint1, float* colors) {
	decodeRGB565(endpoint0, &colors[0]);
	decodeRGB565(endpoint1, &colors[3]);
	
	colors[6] = roundf((colors[0] * 2 + colors[3]) / 3); 
	colors[7] = roundf((colors[1] * 2 + colors[4]) / 3); 
	colors[8] = roundf((colors[2] * 2 + colors[5]) / 3); 
	
	colors[9]  = roundf((colors[3] * 2 + colors[0]) / 3); 
	colors[10] = roundf((colors[4] * 2 + colors[1]) / 3); 
	colors[11] = roundf((colors[5] * 2 + colors[2]) / 3); 
}

int pickClosestColor(float r, float g, float b, float* colors) {
	float c0Dist = distanceSquare(colors[0], colors[1], colors[2], r, g, b);
	float c1Dist = distanceSquare(colors[3], colors[4], colors[5], r, g, b);
	float c2Dist = distanceSquare(colors[6], colors[7], colors[8], r, g, b);
	float c3Dist = distanceSquare(colors[9], colors[10], colors[11], r, g, b);
	
	float dist = c0Dist;
	int index = 0;
	
	if(c1Dist < dist) {dist = c1Dist; index = 1;}
	if(c2Dist < dist) {dist = c2Dist; index = 2;}
	if(c3Dist < dist) {dist = c3Dist; index = 3;}
	
	return index;
}

uint32_t calculateIndices(
	uint8_t* image, int x, int y, int w, int h, 
	float* endpoint0, float* endpoint1
) {
	float colors[3 * 4];
	
	calculatePalette(
		encodeRGB565(endpoint0),
		encodeRGB565(endpoint1),
		colors
	);
	
	uint32_t indices = 0;
	
	int blockW = (x + 4 > w) ? (w - x) : 4;
	int blockH = (y + 4 > h) ? (h - y) : 4;
	
	for(int yy = 0; yy < blockH; yy++) {
		uint8_t* ptr = &image[(x + (y + yy) * w) * COL_STRIDE];
		
		for(int xx = 0; xx < blockW; xx++, ptr += COL_STRIDE) {
			float r = ptr[0], g = ptr[1], b = ptr[2];
			
			int offset = (xx | (yy << 2)) << 1;
			int index = pickClosestColor(r, g, b, colors);
			
			indices |= index << offset;
		}
	}
	
	return indices;
}

int compareInts(const void* a, const void* b) {
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

void encodeDXT1(void* output, void* image, int w, int h) {
	uint8_t* img = image;
	
	int outputWStep = (w / 4) * sizeof(DXTBlock);
	if((w & 3) != 0) outputWStep += sizeof(DXTBlock);
	
	for(int y = 0; y < h; y += 4, output += outputWStep) {
		#pragma omp parallel for
		for(int x = 0; x < w; x += 4) {
			//Determine block bounds
			Bounds2D bounds = calculateBounds(x, y, w, h, 0);
			
			//Collect block stats
			float mean[3] = {0};
			float min[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
			float max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
			
			int sorted[16 * 3];
			
			for(int yy = bounds.y1, i = 0; yy < bounds.y2; yy++) {
				uint8_t* ptr = &img[(bounds.x1 + yy * w) * COL_STRIDE];
				
				for(int xx = bounds.x1; xx < bounds.x2; xx++, ptr += COL_STRIDE, i++) {
					for(int rgb = 0; rgb < 3; rgb++) {
						float v = ptr[rgb];
						
						mean[rgb] += v;
						min[rgb] = fminf(min[rgb], v);
						max[rgb] = fmaxf(max[rgb], v);
						
						sorted[i + rgb * 16] = ptr[rgb];
					}
				}
			}

			mean[0] /= bounds.pixelsCount;
			mean[1] /= bounds.pixelsCount;
			mean[2] /= bounds.pixelsCount;
			
			qsort(&sorted[0], bounds.pixelsCount, sizeof(int), compareInts);
			qsort(&sorted[16], bounds.pixelsCount, sizeof(int), compareInts);
			qsort(&sorted[32], bounds.pixelsCount, sizeof(int), compareInts);
			
			//Find principal axis using covariance matrix and power iteration
			float cov[6] = {0};
			
			for(int yy = bounds.y1; yy < bounds.y2; yy++) {
				uint8_t* ptr = &img[(bounds.x1 + yy * w) * COL_STRIDE];
				
				for(int xx = bounds.x1; xx < bounds.x2; xx++, ptr += COL_STRIDE) {
					float r = ptr[0] - mean[0];
					float g = ptr[1] - mean[1];
					float b = ptr[2] - mean[2];

					cov[0] += r*r;
					cov[1] += r*g;
					cov[2] += r*b;
					cov[3] += g*g;
					cov[4] += g*b;
					cov[5] += b*b;
				}
			}
			
			float axis[3] = {max[0] - min[0], max[1] - min[1], max[2] - min[2]};
			
			for(int i = 0; i < 20; i++) {
				float r = axis[0] * cov[0] + axis[1] * cov[1] + axis[2] * cov[2];
				float g = axis[0] * cov[1] + axis[1] * cov[3] + axis[2] * cov[4];
				float b = axis[0] * cov[2] + axis[1] * cov[4] + axis[2] * cov[5];
				
				float invLen = 1 / fmaxf(0.000001f, fmaxf(fabsf(r), fmaxf(fabsf(g), fabsf(b))));

				axis[0] = r * invLen;
				axis[1] = g * invLen;
				axis[2] = b * invLen;
			}
			
			normalize(axis);
			
			//Find first endpoints
			float endpoint0[3], endpoint1[3];
			float minDotDist = FLT_MAX, maxDotDist = -FLT_MAX;
			
			for(int yy = bounds.y1; yy < bounds.y2; yy++) {
				uint8_t* ptr = &img[(bounds.x1 + yy * w) * COL_STRIDE];
				
				for(int xx = bounds.x1; xx < bounds.x2; xx++, ptr += COL_STRIDE) {
					float r = ptr[0] - mean[0], g = ptr[1] - mean[1], b = ptr[2] - mean[2];
					float dot = r * axis[0] + g * axis[1] + b * axis[2];
					
					if(dot < minDotDist) minDotDist = dot;
					if(dot > maxDotDist) maxDotDist  = dot;
				}
			}
			
			endpoint0[0] = axis[0] * minDotDist + mean[0];
			endpoint0[1] = axis[1] * minDotDist + mean[1];
			endpoint0[2] = axis[2] * minDotDist + mean[2];
			
			endpoint1[0] = axis[0] * maxDotDist + mean[0];
			endpoint1[1] = axis[1] * maxDotDist + mean[1];
			endpoint1[2] = axis[2] * maxDotDist + mean[2];
			
			//Refine endpoints using indices and pixel data
			//By solving a least squares system via normal equations + Cramer's rule
			static const int w1Tab[4] = {3,0,2,1};
			static const float xxTab[4] = {0, 1, 4, 9};
			static const float yyTab[4] = {9, 4, 1, 0};
			static const float xyTab[4] = {0, 2, 2, 0};
			
			for(int i = 0; i < 20; i++) {
				uint16_t oldEndpoint0 = encodeRGB565(endpoint0);
				uint16_t oldEndpoint1 = encodeRGB565(endpoint1);
				
				float colors[3 * 4];
				calculatePalette(oldEndpoint0, oldEndpoint1, colors);
				
				bool singleColorBlock = true;
				int prevIdx = -1;
				
				int sortedIdx[16];
				
				float At1_r = 0, At1_g = 0, At1_b = 0;
				float At2_r = 0, At2_g = 0, At2_b = 0;
				float xxAccum = 0, yyAccum = 0, xyAccum = 0;
				
				for(int yy = bounds.y1, i = 0; yy < bounds.y2; yy++) {
					uint8_t* ptr = &img[(bounds.x1 + yy * w) * COL_STRIDE];
					
					for(int xx = bounds.x1; xx < bounds.x2; xx++, ptr += COL_STRIDE, i++) {
						float r = ptr[0], g = ptr[1], b = ptr[2];
					
						int index = pickClosestColor(r, g, b, colors);
						int w1 = w1Tab[index];
						sortedIdx[i] = w1;
						
						if(prevIdx != -1 && prevIdx != w1) singleColorBlock = false;
						prevIdx = w1;
					}
				}

				if(singleColorBlock) {
					//All pixels have the same index?
					//Yes, linear system would be singular;
					//Single-color match on average color
					int r = (int) roundf(mean[0]);
					int g = (int) roundf(mean[1]);
					int b = (int) roundf(mean[2]);
					
					endpoint0[0] = stb__OMatch5[r][0] * 255 / 31.0f;
					endpoint0[1] = stb__OMatch6[g][0] * 255 / 63.0f;
					endpoint0[2] = stb__OMatch5[b][0] * 255 / 31.0f;
					
					endpoint1[0] = stb__OMatch5[r][1] * 255 / 31.0f;
					endpoint1[1] = stb__OMatch6[g][1] * 255 / 63.0f;
					endpoint1[2] = stb__OMatch5[b][1] * 255 / 31.0f;
				} else {
					//TODO: Just count each index?
					qsort(&sortedIdx[0], bounds.pixelsCount, sizeof(int), compareInts);
					
					for(int yy = bounds.y1, i = 0; yy < bounds.y2; yy++) {
						for(int xx = bounds.x1; xx < bounds.x2; xx++, i++) {
							int w1 = sortedIdx[i];

							xxAccum += xxTab[w1];
							yyAccum += yyTab[w1];
							xyAccum += xyTab[w1];
							
							float r, g, b;
							
							if(axis[0] < 0) r = sorted[i];
							else r = sorted[bounds.pixelsCount - 1 - i];
							
							if(axis[1] < 0) g = sorted[i + 16];
							else g = sorted[bounds.pixelsCount - 1 - i + 16];
							
							if(axis[2] < 0) b = sorted[i + 32];
							else b = sorted[bounds.pixelsCount - 1 - i + 32];
							
							At1_r   += w1 * r;
							At1_g   += w1 * g;
							At1_b   += w1 * b;
							At2_r   += r;
							At2_g   += g;
							At2_b   += b;
						}
					}
					
					At2_r = 3 * At2_r - At1_r;
					At2_g = 3 * At2_g - At1_g;
					At2_b = 3 * At2_b - At1_b;
					
					float f = 3 / (xxAccum*yyAccum - xyAccum*xyAccum);
					
					endpoint0[0] = clampf((At1_r*yyAccum - At2_r * xyAccum) * f, 0, 255);
					endpoint0[1] = clampf((At1_g*yyAccum - At2_g * xyAccum) * f, 0, 255);
					endpoint0[2] = clampf((At1_b*yyAccum - At2_b * xyAccum) * f, 0, 255);
					
					endpoint1[0] = clampf((At2_r*xxAccum - At1_r * xyAccum) * f, 0, 255);
					endpoint1[1] = clampf((At2_g*xxAccum - At1_g * xyAccum) * f, 0, 255);
					endpoint1[2] = clampf((At2_b*xxAccum - At1_b * xyAccum) * f, 0, 255);
				}
				
				uint16_t newEndpoint0 = encodeRGB565(endpoint0);
				uint16_t newEndpoint1 = encodeRGB565(endpoint1);
				
				if(oldEndpoint0 == newEndpoint0 && oldEndpoint1 == newEndpoint1) break;
			}
			
			//Encode endpoints
			uint32_t indices = calculateIndices(img, x, y, w, h, endpoint0, endpoint1);
			
			uint16_t endpoint0q = encodeRGB565(endpoint0);
			uint16_t endpoint1q = encodeRGB565(endpoint1);
			
			if(endpoint0q <= endpoint1q) {
				if(endpoint0q == endpoint1q) {
					indices = 0;
				} else {
					uint16_t tmp = endpoint0q;
					endpoint0q = endpoint1q;
					endpoint1q = tmp;
					
					indices ^= 0x55555555;
				}
			}
			
			DXTBlock* block = (DXTBlock*) (output + (x / 4 * sizeof(DXTBlock)));
			block->endpoint0 = endpoint0q;
			block->endpoint1 = endpoint1q;
			block->indices = indices;
		}
	}
}

/*
MIT License

Copyright (c) 2017 Sean Barrett
Copyright (c) 2026 Roman Lahin

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/