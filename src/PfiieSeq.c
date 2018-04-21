#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


/*
#ifdef __linux__
#define fopen_s(fp, fmt, mode) *(fp)=fopen((fmt), (mode))
#define fclose_s(fp, fmt, mode) *(fp)=fclose((fmt), (mode))
#endif


#ifdef __unix
#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL
#endif
*/

//#include <windows.h>

//define GetTickCount()    ((1000.0 * (double)tickGet())/((double)sysClkRateGet())))

//#define STEPS 10
/*
double GetTickCount(void) 
{
  struct timespec now;
  if (clock_gettime(CLOCK_MONOTONIC, &now))
    return 0;
  return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
}
*/

typedef struct {
  int red;
  int green;
  int blue;
} PixelType; 

static double getLight(int p, int min, int max);
static double getDark(int p, int min, int max);

double getLight(int p, int min, int max) {
  double result = 0;

  if (min == max)
    result = p;
  else
    result = ((p - min) / (max - min)); 

  return result;
}

double getDark(int p, int min, int max) {
  return (1 - getLight(p, min, max));
}

PixelType newValue(PixelType nb[9]) {
  PixelType pt;
  int _tmpr, _tmpg, _tmpb;
  int red[9] = {nb[0].red, nb[1].red, nb[2].red, nb[3].red, nb[4].red, nb[5].red, nb[6].red, nb[7].red, nb[8].red};
  int green[9] = {nb[0].green,nb[1].green,nb[2].green,nb[3].green,nb[4].green,nb[5].green,nb[6].green,nb[7].green,nb[8].green};
  int blue[9] = {nb[0].blue,nb[1].blue,nb[2].blue,nb[3].blue,nb[4].blue,nb[5].blue,nb[6].blue,nb[7].blue,nb[8].blue};
  
  for (int i=0; i<9; i++) {
    for (int j=i+1; j<9; j++) {
      if (red[i] > red[j]) {
        _tmpr = red[i];
        red[i] = red[j];
        red[j] = _tmpr;
      } 
      if (green[i] > green[j]) {
        _tmpg = green[i];
        green[i] = green[j];
        green[j] = _tmpg;
      } 
      if (blue[i] > blue[j]) {
        _tmpb = blue[i];
        blue[i] = blue[j];
        blue[j] = _tmpb;
      } 
    } 
  }
  
  int red_median = red[4];
  int green_median = green[4];
  int blue_median = blue[4];
  int red_min = red[0];
  int green_min = green[0];
  int blue_min = blue[0];
  int red_max = red[8];
  int green_max = green[8];
  int blue_max = blue[8];
  
  double red_cll, red_cdd;
  double green_cll, green_cdd;
  double blue_cll, blue_cdd;
  
  double red_enhanced, red_neighbours;
  double green_enhanced, green_neighbours;
  double blue_enhanced, blue_neighbours;
 
  if(red_min == red_max) {
    red_cll = 1;
    red_cdd = 2;
  } else {
    red_cll = 1 + ((red_median - red_min) / (red_max - red_min));
    red_cdd = 2 - ((red_median - red_min) / (red_max - red_min));
  }

  if(green_min == green_max) {
    green_cll = 1;
    green_cdd = 2;
  } else {
    green_cll = 1 + ((green_median - green_min) / (green_max - green_min));
    green_cdd = 2 - ((green_median - green_min) / (green_max - green_min));
  }

  if(blue_min == blue_max) {
    blue_cll = 1;
    blue_cdd = 2;
  } else {
    blue_cll = 1 + ((blue_median - blue_min)/(blue_max - blue_min));
    blue_cdd = 2 - ((blue_median - blue_min)/(blue_max - blue_min));
  }


  for(int i=1; i<9; i++) {
    red_neighbours = red_neighbours + ((0.8*getDark(nb[i].red, red_min, red_max))+(0.8*getLight(nb[i].red, red_min, red_max)))
    /(red_cdd*getDark(nb[0].red, red_min, red_max)*getDark(nb[i].red, red_min, red_max)
      +0.8*getDark(nb[0].red, red_min, red_max)*getLight(nb[i].red, red_min, red_max)
      +0.8*getLight(nb[0].red, red_min, red_max)*getDark(nb[i].red, red_min, red_max)
      +red_cll*getLight(nb[0].red, red_min, red_max)*getLight(nb[i].red, red_min, red_max));

    green_neighbours = green_neighbours + ((0.8*getDark(nb[i].green, green_min, green_max))+(0.8*getLight(nb[i].green, green_min, green_max)))
    /(green_cdd*getDark(nb[0].green, green_min, green_max)*getDark(nb[i].green, green_min, green_max)
      +0.8*getDark(nb[0].green, green_min, green_max)*getLight(nb[i].green, green_min, green_max)
      +0.8*getLight(nb[0].green, green_min, green_max)*getDark(nb[i].green, green_min, green_max)
      +green_cll*getLight(nb[0].green, green_min, green_max)*getLight(nb[i].green, green_min, green_max));

    blue_neighbours = blue_neighbours + ((0.8*getDark(nb[i].blue, blue_min, blue_max))+(0.8*getLight(nb[i].blue, blue_min, blue_max)))
    /(blue_cdd*getDark(nb[0].blue, blue_min, blue_max)*getDark(nb[i].blue, blue_min, blue_max)
      +0.8*getDark(nb[0].blue, blue_min, blue_max)*getLight(nb[i].blue, blue_min, blue_max)
      +0.8*getLight(nb[0].blue, blue_min, blue_max)*getDark(nb[i].blue, blue_min, blue_max)
      +blue_cll*getLight(nb[0].blue, blue_min, blue_max)*getLight(nb[i].blue, blue_min, blue_max));
  }

  red_enhanced = getLight(nb[0].red, red_min, red_max)*(1/8)*red_neighbours;
  green_enhanced = getLight(nb[0].green, green_min, green_max)*(1/8)*green_neighbours;
  blue_enhanced = getLight(nb[0].blue, blue_min, blue_max)*(1/8)*blue_neighbours;

  if (red_enhanced < 0.5 && nb[0].red > 0)             // P is dark, so reduce P's color  
    pt.red = nb[0].red-1;                              // don’t let newp be less than 0
  else if (red_enhanced > 0.5 && nb[0].red < 255)      // P is light, so increase P's color
    pt.red = nb[0].red+1;                              // don’t let newp be great than 255
  else
    pt.red = nb[0].red;                                // equally split, we leave P alone

  if (green_enhanced < 0.5 && nb[0].green > 0)         // P is dark, so reduce P's color  
    pt.green = nb[0].green-1;                          // don’t let newp be less than 0
  else if (green_enhanced > 0.5 && nb[0].green < 255)  // P is light, so increase P's color
    pt.green = nb[0].green+1;                          // don’t let newp be great than 255
  else
    pt.green = nb[0].green;                            // equally split, we leave P alone

  if (blue_enhanced < 0.5 && nb[0].blue > 0)           // P is dark, so reduce P's color  
    pt.blue = nb[0].blue-1;                            // don’t let newp be less than 0
  else if (blue_enhanced > 0.5 && nb[0].blue < 255)    // P is light, so increase P's color
    pt.blue = nb[0].blue+1;                            // don’t let newp be great than 255
  else
    pt.blue = nb[0].blue;                              // equally split, we leave P alone

 // printf("pt: %d %d %d\n",pt.red, pt.green, pt.blue);
  return pt;
}

int main (int argc, char *argv[])
{
  int i = 0, j = 0, niter = 0, rpg_count = 0;
  char fname[40];
  char finput[40];
  FILE *out;
  int rows;
  int cols;
  PixelType _tmpPixel; 
  FILE* fp;
  char buf[50];
  int iterations = 10;

  if(argc == 3) {
    printf("The input ppm is %s\n", argv[1]);
    printf("The output ppm will be %s\n", argv[2]);
    printf("Default number of iterations %d\n", iterations);
  } else if(argc == 4) {
    printf("The input ppm is %s\n", argv[1]);
    printf("The output ppm will be %s\n", argv[2]);
    iterations = atoi(argv[3]);
    printf("Number of iterations %d\n", iterations);
  } else if(argc > 4) {
    printf("Too many arguments supplied.\n");
    return 1;
  } else {
    printf("PPM Input file and Output file expected. Number of Iterations is optional\n");
    printf("Example 1: ./PfiieSeq res/Harbor.ppm harborSeqResult.pgm\n");
    printf("Example 2: ./PfiieSeq res/Harbor.ppm harborSeqResult.pgm 30\n");
    return 1;
  }

  snprintf(finput, 40, argv[1]);
  snprintf(fname, 40, argv[2]);

  // get rows and cols
  PixelType **a; 
  PixelType **_aTmp;

  if ((fp = fopen(finput, "r")) == NULL) { /* Open source file. */
    perror("PPM source file not found!");
    return 1;
  }

  while (fgets(buf, sizeof(buf), fp) != NULL && niter < 3) {
    if(niter == 0) {
      printf("PixMap: %s",buf);
    } else if (niter == 1) {
      char **res = NULL;
      char *p = strtok (buf, " ");
      int n_spaces = 0;
      
      /* split string and append tokens to 'res' */
      while (p) {
        res = realloc (res, sizeof (char*) * ++n_spaces);
        if (res == NULL)
          exit (-1); /* memory allocation failed */
      
        res[n_spaces-1] = p;
        p = strtok (NULL, " ");
      }
      
      /* realloc one extra element for the last NULL */
      res = realloc (res, sizeof (char*) * (n_spaces+1));
      res[n_spaces] = 0;
   
      cols = atoi(res[0]);
      rows = atoi(res[1]);

      free(res);

      printf("Width: %d | height: %d\n",cols,rows);
  
      a = (PixelType**) malloc(rows*sizeof(PixelType*));  
      for (int i = 0; i < rows; i++)  
        a[i] = (PixelType*) malloc(cols*sizeof(PixelType)); 

      _aTmp = (PixelType**) malloc(rows*sizeof(PixelType*));  
      for (int i = 0; i < rows; i++)  
        _aTmp[i] = (PixelType*) malloc(cols*sizeof(PixelType)); 

    } else if (niter == 2) {
      printf("Maximum value for each color: %s",buf);
      buf[strlen(buf) - 1] = '\0'; // eat the newline fgets() stores	
      break;
    }
    buf[strlen(buf) - 1] = '\0'; // eat the newline fgets() stores	
    niter++;
  }

  niter = 0;

  while (fgets(buf, sizeof(buf), fp) != NULL && niter < (cols*rows*3)) {
    if(rpg_count == 0) {
      _tmpPixel.red = atoi(strdup(buf));
      rpg_count++;		
    } else if(rpg_count == 1) {
      _tmpPixel.green = atoi(strdup(buf));
      rpg_count++;		
    } else if(rpg_count == 2) {
      _tmpPixel.blue = atoi(strdup(buf));
      rpg_count = 0;
      a[i][j] = _tmpPixel;
      if((j+1) == (int)cols) {
        j = 0;
        i++;
      } else {
        j++;
      }
    }
    buf[strlen(buf) - 1] = '\0'; // eat the newline fgets() stores	
    niter++;
  }
  fclose(fp);
  
  int step = 0;
  int converged = 0;
  int diffs;
  while (!converged && step < iterations) {    
    step++;     
    diffs = 0;

    for(int r=1;r<rows-1;r++) {
      for(int c=1;c<cols-1;c++) {
	PixelType neighbors[9] = {a[r][c], a[r-1][c-1], a[r-1][c], a[r-1][c+1],a[r][c-1],a[r][c+1],a[r+1][c-1], a[r+1][c], a[r+1][c+1]};
        _aTmp[r][c] = newValue(neighbors);
        if (_aTmp[r][c].red != a[r][c].red && _aTmp[r][c].green != a[r][c].green && _aTmp[r][c].blue != a[r][c].blue) 
          diffs++;
      }
    }	
    converged = (diffs == 0);     

    for(int r=1;r<rows-1;r++) {
      for(int c=1;c<cols-1;c++) {		
        a[r][c] = _aTmp[r][c];
      }
    }
  }
  
  // use a well-defined image standard: Netpbm
  out = fopen(fname, "w+b");
  // need a header
  fprintf(out, "P3\n%d %d\n255\n", (int)cols, (int)rows);
  for (i=0; i<rows; i++)
    for (j=0; j<cols; j++)
      fprintf(out,"%d\n%d\n%d\n", a[i][j].red, a[i][j].green, a[i][j].blue);
  fclose(out);
 // etime = GetTickCount();

  printf("Nr. of operations: %d\n", niter);
  //printf("Time = %.3f s\nDone\n", (etime-stime)/1000.0);
  return 0;
}
