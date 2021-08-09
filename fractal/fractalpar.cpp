/*
Fractal code for CS 4380 / CS 5351

Copyright (c) 2018, Texas State University. All rights reserved.

Redistribution and usage in source and binary form, with or without
modification, is only permitted for educational use.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Author: Martin Burtscher
*/

#include <cstdlib>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <sys/time.h>
#include "fractal.h"

static const double Delta = 1;
static const double xMid =  0.30001005304301999;
static const double yMid =  0.0250000000006;

int main(int argc, char *argv[])
{
    int my_rank = 0;
    int size;

    if (argc != 3)
    {
        fprintf(stderr, "usage: %s frame_width num_frames\n", argv[0]);
        exit(-1);
    }
    int width = atoi(argv[1]);
    if (width < 10)
    {
        fprintf(stderr, "error: frame_width must be at least 10\n");
        exit(-1);
    }
    int frames = atoi(argv[2]);
    if (frames < 1)
    {
        fprintf(stderr, "error: num_frames must be at least 1\n");
        exit(-1);
    }

    MPI_Init(&argc, &argv); /*START MPI */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int step = floor(((float)(frames - 1) / (float)size));
    int missing_parts = ((frames - 1) - (step * size));

    int start_frame_i = step * my_rank;
    int end_frame_i = (step * (my_rank + 1)) - 1;

    if (my_rank == 0)
    {
        end_frame_i++;
    }
    else
    {
        start_frame_i++;
        end_frame_i++;
    }

    if (missing_parts > 0 && my_rank >= 1)
    {
        if (my_rank <= missing_parts)
        {
            start_frame_i += my_rank - 1;
            end_frame_i += my_rank;
        }
        else
        {
            start_frame_i += missing_parts;
            end_frame_i += missing_parts;
        }
    }

    int workload = end_frame_i - start_frame_i + 1;
    int height = width;
    unsigned char *pic = new unsigned char[workload * width * (width*3)];

    double delta = Delta;
    for(int frame = 1; frame <= start_frame_i; frame++)
    {
        delta*=  0.98;
    }

    printf("Processo %d ira processar os frames de %d ate %d com workload de %d \n", my_rank, start_frame_i, end_frame_i, end_frame_i - start_frame_i + 1);

    timeval start, end;
    gettimeofday(&start, NULL);

    for(int frame = 0; frame < workload; frame++)
    {
        const double xMin = xMid - delta;
        const double yMin = yMid - delta;
        const double dw = 2.0 * delta/ width;
        for (int row = 0; row < width; row++)
        {
            const double cy = yMin + row * dw;
            for (int col = 0; col < width; col++)
            {
                const double cx = xMin + col * dw;
                double x = cx;
                double y = cy;
                int depth = 256;
                double x2, y2;
                do
                {
                    x2 = x * x;
                    y2 = y * y;
                    y = 2 * x * y + cy;
                    x = x2 - y2 + cx;
                    depth--;
                } while ((depth > 0) && ((x2 + y2) < 5.0));
                pic[ (frame * height * (width*3)) + (row * (width*3)) + (col*3)+1] = (unsigned char)256-depth;
            }
        }
        delta = delta*0.98;
    }

    MPI_Finalize();

    if(my_rank == 0)
    {
        gettimeofday(&end, NULL);
        double runtime = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
        printf("%d,%d,%d,%.4f\n", size, width, workload, runtime);
    }

    //   verify result by writing frames to BMP files
    if ((width <= 900000) && (frames <= 10000))
    {
        for (int frame = 0; frame < workload; frame++)
        {
            char name[32];
            sprintf(name, "images2/fractal%d.bmp", frame + 1000 + start_frame_i);
            generateBitmapImage(&pic[frame * height * (width *3)], width, height, name);
        }
    }

    delete[] pic;

    return 0;
}
