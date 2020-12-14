#ifndef FILTERS_H
#define FILTERS_Hsssss
#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include "ppm.h"
#include <atomic>


// SINGLE-THREAD FILTERS

void blackWhite(ppm& img);
void contrast(ppm& img, float contrast);
void brightness(ppm& img, float br);
void shades(ppm& img, unsigned char shades);
void merge(ppm& img1, ppm& img2, float alpha);
void frame(ppm& img, pixel color, int x);
void boxBlur(ppm &img);
void zoom(ppm &img, int n);
void edgeDetection(ppm &img);
void sharpen(ppm &img);
void crop(ppm &img, int k, int t);
void negative(ppm &img);

// MULTI-THREAD FILTERS
void threadedblackwhite(ppm& img, int nthreads);
void threadedsharpen(ppm& img, int nthreads);

#endif