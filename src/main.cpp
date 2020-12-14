#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include "filters.h"
#include <thread>
#define ONE_OVER_BILLION 1E-9


// Un esqueleto como base, modificar a gusto pero respetando la consigna

int main(int argc , char* argv[]){
	
	// Pasamos 0 en caso de no utilizar p2 o  "" en img2
	if(string(argv[1]) == "-help")
	{
		cout << "Uso: ./tp <filtro> <nthreads> <p1> <p2> <img1> <img2>" << endl;
		return 0; 
	}
	string filtro = argv[1];
	int nthreads = atoi(argv[2]);
	int p1 = atof(argv[3]);
	int p2 = atof(argv[4]);
	string img1(argv[5]);
	ppm img(img1);
	bool singlethread = nthreads == 1;	
	ppm img_target(img.width-2,img.height-2);
	cout << "Aplicando filtro" << endl;
	struct timespec start, stop;
    double accum;
	clock_gettime(CLOCK_REALTIME, &start);
	cout << nthreads << endl;
	cout << p1 << endl;
	cout << filtro << endl;
	if(filtro == "bw"){
		if (singlethread)
			blackWhite(img);
		else	
		threadedblackwhite(img, nthreads);
	}
	else if(filtro == "br"){

		brightness(img, p1);
	}
	else if(filtro == "ct"){

		contrast(img, p1);
	}
	else if(filtro == "sn"){

		if(singlethread)
			sharpen(img);
		else
			threadedsharpen(img, nthreads);

	}
	else if(filtro == "zm"){

		zoom(img,p1);
	}
	else if(filtro == "cp"){

		crop(img,p1,p2);
	}

	else if(filtro == "neg"){
		negative(img);
	}
	clock_gettime(CLOCK_REALTIME, &stop);
	accum = ( stop.tv_sec - start.tv_sec )
	        + ( stop.tv_nsec - start.tv_nsec ) * ONE_OVER_BILLION;
	printf( "Duración del filtrado: %lf s\n", accum);
	cout << "Escribiendo imagen" << endl;
	string out = "out/salida.ppm";
	img.write(out);  
	img_target.write(out); 
	cout << "Listo" << endl;
	return 0;
}