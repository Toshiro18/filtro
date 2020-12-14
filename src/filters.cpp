#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>       /* sqrt */
#include <vector>
#include "filters.h"
#include <thread>  
#include <atomic>  

#define BLACK 0

using namespace std;

void blackWhitelimit(ppm* img, int start, int end)
{
    unsigned short int aux;

    for(int x = start; x < end; x++){

        for(int y = 0; y < img->width; y++){

            aux = img->getpixel(x,y).cumsum() / 3;
            img->setpixel(x,y,pixel(aux,aux,aux));
        }
    }
    return;
}
void blackWhite(ppm& img){

    short int fcalculo;

    for(int h = 0; h < img.height; h++){

        for(int w = 0; w < img.width; w++){
            fcalculo = img.getpixel(h,w).cumsum()/3;
            
            img.setpixel(h,w,pixel(fcalculo,fcalculo,fcalculo));
        }
    }
    return;
}

void threadedblackwhite(ppm& img, int nthreads) {

    if(nthreads==0){

        blackWhite(img);
        return;
    }

    int xthread = (int)(img.height / nthreads);
    int offset = img.height - (xthread * nthreads);
    vector<thread> thread_pool;

    for(int x = 0; x < nthreads; x++)
    {
        int start = x * xthread;
        int end = (x + 1) * xthread;
        if(x == nthreads - 1) {
            end += offset;
        }
        thread_pool.push_back(thread(blackWhitelimit, &img, start, end));
    }

    for(int x = 0; x < nthreads; x++)
    {
        thread_pool[x].join();
    }
    return;
}

void contrast(ppm& img,float contrast){

    short int r;
    short int g;
    short int b;
    float c = contrast;
    float f = (259 * (c + 255))/(255 * (259 - c));

    for(int h = 0; h < img.height; h++){

        for(int w = 0; w < img.width; w++){

            r = img.getpixel(h,w).r;
            g = img.getpixel(h,w).g;
            b = img.getpixel(h,w).b;
            r = f * (r-128) + 128;
            g = f * (g-128) + 128;
            b = f * (b-128) + 128;
            pixel pix = pixel(r,g,b);
            img.setpixel(h,w,pix.truncate());
        }
    }
    return;
}

void brightness(ppm& img, float br){

    short int r;
    short int g;
    short int b;
    float bright = br;

    for(int h = 0; h < img.height; h++){

        for(int w = 0; w < img.width; w++){

            img.getpixel(h,w).add(255*bright).truncate();
        }
    }
    return;
}
void convolution(ppm&img, ppm& img_target, short int ker[]){

    short int r;
    short int g;
    short int b;

    for(int h = 1; h < img.height - 1; h++){

        for(int w = 1; w < img.width - 1; w++){

            r = 0; 
            g = 0;
            b = 0;
            for(int kh = 0; kh < 3; kh++){

                for(int kw = 0; kw < 3; kw++){

                    r = r + img.getpixel(h + kh - 1, w + kw - 1).r * ker[kh * 3 + kw];
                    g = g + img.getpixel(h + kh - 1, w + kw - 1).g * ker[kh * 3 + kw];
                    b = b + img.getpixel(h + kh - 1, w + kw - 1).b * ker[kh * 3 + kw];
                }
            }
            img_target.setpixel(h - 1,w - 1, pixel(r, g, b).truncate());
        }
    }
    return;
}

void threadedconvolution(ppm* img,ppm* img_target, short int ker[], int start, int end)
{
    short int r;
    short int g;
    short int b;

    for(int h = start; h< end; h++){

        for(int w = 1; w < img->width - 1; w++){

            r = 0;
            g = 0;
            b = 0;

            for(int x = 0; x < 3; x++){

                for(int y = 0; y < 3; y++){

                    r += img->getpixel(h+x-1,w+y-1).r * ker[x*3+y];
                    g += img->getpixel(h+x-1,w+y-1).g * ker[x*3+y];
                    b += img->getpixel(h+x-1,w+y-1).b * ker[x*3+y];
                }
            }
            img_target->setpixel(h-1,w-1,pixel(r,g,b).truncate());
        }
    }
    return;
}

void sharpen(ppm& img){
    short int kernel[] = {0,-1,0,-1,5,-1,0,-1,0};
    ppm img_target(img.width-2,img.height-2);
    convolution(img,img_target,kernel);
    return;
}
void threadedsharpen(ppm& img,int nthreads){
    int xthread = (int)(img.height/ nthreads);
    int start, end, offset = img.height - (xthread * nthreads);
    vector<thread> thread_pool;

    short int kernel[] = {0,-1,0,-1,5,-1,0,-1,0};
    ppm img_target(img.width-2,img.height-2);
    for(int x = 0; x < nthreads; x++){
        start = x * xthread;
        if(start == 0)
            start=1;
        end = (x + 1) * xthread;
        if (x == nthreads - 1){
            end += offset -1;
        }
        thread_pool.push_back(thread(threadedconvolution,&img,&img_target,kernel,start,end));
    }
    for (int x = 0; x < nthreads; x++){
        thread_pool[x].join();
    }
    return;
}

void zoom(ppm &img, int n){
    ppm img_zoomed(img.height * n, img.width * n);
    for(size_t h = 0; h < img.height; h++){

        for(size_t w = 0; w < img.width; w++){

            for(size_t x = 0; x < n; x++){

                for(size_t y = 0; y < n; y++){

                    img_zoomed.setpixel((h*n) + x, (h*n) + y, img.getpixel(h,w));
                }
            }
        }
    }
    img = img_zoomed;
    return;
}

void crop(ppm &img, int k, int t){
    ppm result = ppm(img.width - t, img.height - k);
    for(size_t h = k; h < img.height; h++){
        for(size_t w = t; w < img.width; w++){
            result.setpixel(h-k,w-t,img.getpixel(h,w));
        }
    }
    img = result;
}

void negative(ppm& img){

    short int r;
    short int g;
    short int b;

    for(int h = 0; h < img.height; h++){

        for(int w = 0; w < img.width; w++){

            r = img.getpixel(h,w).r;
            g = img.getpixel(h,w).g;
            b = img.getpixel(h,w).b;
            r = 255 - r;
            g = 255 - g;
            b = 255 - b;
            pixel pix = pixel(r, g, b);
            img.setpixel(h,w,pix.truncate());
        }
    }
}