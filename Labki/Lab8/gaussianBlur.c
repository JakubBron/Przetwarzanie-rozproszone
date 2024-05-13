#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <png.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define RADIUS 15
int img_width, img_height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers = NULL;
png_bytep *dst_row_pointers = NULL;


// prototypes
void read_png_file(char *filename);
void write_png_file(char *filename);
void process_png_file(int x, int y, int width, int height, double **kernel);
void makePixels(int width, int height, png_bytep **row_pointers);
void freePixels(int width, int height, png_bytep **row_pointers);
double** makeKernel();
void freeKernel(double** kernel);
void fillKernel(double** kernel);
void gaussianBlur(int x, int y, png_bytep outputPixel, double **kernel);
int getPixelValue(int x, int y, int color);



// from https://gist.github.com/niw/5963798
void read_png_file(char *filename)
{
    FILE *fp = fopen(filename, "rb");

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
        abort();

    png_infop info = png_create_info_struct(png);
    if (!info)
        abort();

    if (setjmp(png_jmpbuf(png)))
        abort();

    png_init_io(png, fp);
    png_read_info(png, info);

    img_width = png_get_image_width(png, info);
    img_height = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    makePixels(img_width, img_height, &row_pointers);
    makePixels(img_width, img_height, &dst_row_pointers);

    png_read_image(png, row_pointers);

    fclose(fp);

    png_destroy_read_struct(&png, &info, NULL);
}

void write_png_file(char *filename)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
        abort();

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
        abort();

    png_infop info = png_create_info_struct(png);
    if (!info)
        abort();

    if (setjmp(png_jmpbuf(png)))
        abort();

    png_init_io(png, fp);

    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
        png,
        info,
        img_width, img_height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    // png_set_filler(png, 0, PNG_FILLER_AFTER);

    if (dst_row_pointers == NULL)
        abort();

    png_write_image(png, dst_row_pointers);
    png_write_end(png, NULL);

    freePixels(img_width, img_height, &row_pointers);
    freePixels(img_width, img_height, &dst_row_pointers);

    fclose(fp);

    png_destroy_write_struct(&png, &info);
}

// edited from https://gist.github.com/niw/5963798
void process_png_file(int x, int y, int width, int height, double **kernel)
{
    for (int i = y; i < height; i++)
    {
        png_bytep dst_row = dst_row_pointers[i];
        for (int j = x; j < width; j++)
        {
            png_bytep outputPixel = &(dst_row[j * 4]);
            gaussianBlur(j, i, outputPixel, kernel);
        }
    }
}

// memory allocation
void makePixels(int width, int height, png_bytep **row_pointers)
{
    if (*row_pointers != NULL)
        abort();

    png_byte *pixels = (png_byte *)malloc(sizeof(png_byte) * height * width * 4);
    if (pixels == NULL)
    {
        abort();
    }
    
    *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
    if (*row_pointers == NULL)
    {
        abort();
    }
    
    for (int y = 0; y < height; y++)
    {
        (*row_pointers)[y] = pixels + y * width * 4;
    }
}

void freePixels(int width, int height, png_bytep **row_pointers)
{
    if (*row_pointers == NULL)
    {
        abort();
    }
    free(*row_pointers[0]);
    free(*row_pointers);
}

double** makeKernel()
{
    double **kernel = (double **)malloc(sizeof(double *) * (2 * RADIUS + 1));
    if(kernel == NULL)
    {
        abort();
    }

    for (int i = 0; i < 2 * RADIUS + 1; i++)
    {
        kernel[i] = (double *)malloc(sizeof(double) * (2 * RADIUS + 1));
        if(kernel[i] == NULL)
        {
            abort();
        }
    }
    return kernel;
}

void freeKernel(double** kernel)
{
    for (int i = 0; i < 2 * RADIUS + 1; i++)
    {
        free(kernel[i]);
    }
    free(kernel);
}

// gaussian blur
void fillKernel(double** kernel)
{
    double sigma = fmax(RADIUS / 2.0f, 1.0f);
    double exponentDenominator = 2 * sigma * sigma;

    for (int i = -RADIUS; i <= RADIUS; ++i)
    {
        for (int j = -RADIUS; j <= RADIUS; ++j)
        {
            double exponentNumerator = -(i * i + j * j);        
            double eExpression = exp(exponentNumerator / exponentDenominator);
            double kernelValue = (eExpression / (2 * M_PI * sigma * sigma));
            kernel[j + RADIUS][i + RADIUS] = kernelValue;
        }
    }
}

void gaussianBlur(int x, int y, png_bytep outputPixel, double **kernel)
{
    for (int color = 0; color < 4; color++)
    {
        double val = 0;
        double div_val = 0;
        for (int i = -RADIUS; i <= RADIUS; ++i)
        {
            for (int j = -RADIUS; j <= RADIUS; ++j)
            {
                int px_val = getPixelValue(x + i, y + j, color);
                if (px_val == -1)
                    continue;
                val += (double)px_val * kernel[j + RADIUS][i + RADIUS];
                div_val += kernel[j + RADIUS][i + RADIUS];
            }
        }
        val /= div_val;
        outputPixel[color] = val;
    }
}

// helpers
int getPixelValue(int x, int y, int color)
{
    if (x < 0 || x >= img_width)
        return -1;
    if (y < 0 || y >= img_height)
        return -1;
    png_bytep px = &(row_pointers[y][4 * x]);
    return px[color];
}

// multi-threading
struct workerArgs
{
    int x, y, width, height;
    double** kernel;
};

void *jobs(void *inputArgs)
{
    struct workerArgs *args = (struct workerArgs *)inputArgs;
    process_png_file(args->x, args->y, args->width, args->height, args->kernel);
    return NULL;
}

void dispatchProcesses(int threadsNumber, double **kernel)
{
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * threadsNumber);
    if (threads == NULL)
    {
        abort();
    }

    struct workerArgs *args = (struct workerArgs *)malloc(sizeof(struct workerArgs) * threadsNumber);
    if (args == NULL)
    {
        abort();
    }

    for (int i = 0; i < threadsNumber; i++)
    {
        args[i].x = 0;
        args[i].y = i * img_height / threadsNumber;
        args[i].width = img_width;
        args[i].height = fmin((i + 1) * img_height / threadsNumber, img_height);
        args[i].kernel = kernel;
        pthread_create(&threads[i], NULL, jobs, (void *)&args[i]);
    }
    for (int i = 0; i < threadsNumber; i++)
    {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(args);
}

// main
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: ./a.out input output\n");
        return 0;
    }

    double **kernel = makeKernel();
    fillKernel(kernel);
    read_png_file(argv[1]);

    dispatchProcesses(14, kernel);

    write_png_file(argv[2]);
    freeKernel(kernel);
    return 0;
}