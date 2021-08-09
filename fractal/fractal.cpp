#include <cstdlib>
#include <sys/time.h>
#include <cmath>
#include "fractal.h"

static const double Delta = 1;
static const double xMid =  0.30001005304301999;
static const double yMid =  0.0250000000006;

int main(int argc, char *argv[])

{
  if (argc != 3) {fprintf(stderr, "usage: %s frame_width num_frames\n", argv[0]); exit(-1);}
  int width = atoi(argv[1]);
  if (width < 10) {fprintf(stderr, "error: frame_width must be at least 10\n"); exit(-1);}
  int frames = atoi(argv[2]);
  if (frames < 1) {fprintf(stderr, "error: num_frames must be at least 1\n"); exit(-1);}

  int height = width;
  unsigned char* pic = new unsigned char[frames * height * (width*3)];

  timeval start, end;
  gettimeofday(&start, NULL);

  double delta = Delta;
  for (int frame = 0; frame < frames; frame++) {
    const double xMin = xMid - delta;
    const double yMin = yMid - delta;
    const double dw = 2.0 * delta / width;
    for (int row = 0; row < width; row++) {
      const double cy = yMin + row * dw;
      for (int col = 0; col < width; col++) {
        const double cx = xMin + col * dw;
        double x = cx;
        double y = cy;
        int depth = 256;
        double x2, y2;
        do {
          x2 = x * x;
          y2 = y * y;
          y = 2 * x * y + cy;
          x = x2 - y2 + cx;
          depth--;
        } while ((depth > 0) && ((x2 + y2) < 5.0));


        pic[ (frame * height * (width*3)) + (row * (width*3)) + (col*3)+1] = (unsigned char)256-depth;

      }
    }

    delta *= 0.98;
  }

  gettimeofday(&end, NULL);
  double runtime = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;

  printf("%d,%d,%d,%.4f\n", 1, width, frames, runtime);


  for(int frame = 0; frame < frames; frame++)
  {
    char name[32];
    sprintf(name, "image/fractal%d.bmp", frame + 1000);
    generateBitmapImage(&pic[frame * height * (width *3)], width, height, name);
  }

  delete [] pic;
  return 0;
}
