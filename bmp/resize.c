/** 
 * Wahaj Chaudhry
 * wchaudhry@college.harvard.edu 
 * resize.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Resizes a BMP piece by piece.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"


int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize scale_factor infile outfile\n");
        return 1;
    }
    
    // intialize scale factor and convert to int
    int scale_factor = atoi(argv[1]);
    
    // handles values less than 1 and greater than 100
    if (scale_factor < 1 || scale_factor > 100)
    {
        printf("Enter a valid Integer between 1 and 100 \n");
        return 2;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 5;
    }

    // differentiates between old and new width 
    int old_width = bi.biWidth; 
    bi.biWidth *= scale_factor; 
    
    // differentiates between old and new height 
    int old_height = bi.biHeight; 
    bi.biHeight *= scale_factor; 

    // determine padding for scanlines
    int padding =  (4 - (old_width * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // determines new padding after resize 
    int new_padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // changes size of the image relative to padding 
    bi.biSizeImage = ((bi.biWidth * sizeof(RGBTRIPLE))
     + new_padding) * abs(bi.biHeight);
    
    // changes size of the file structure 
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPINFOHEADER) 
    + sizeof(BITMAPFILEHEADER);
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    // memory allocated to buffer which holds scanline pixels 
    RGBTRIPLE * img_buffer = malloc(sizeof(RGBTRIPLE) * (bi.biWidth));

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(old_height); i < biHeight; i++)
    {
        // counter for movement through scanlines     
        int cursor_start = 0;             
        
        // iterate over pixels in scanline  
        for (int j = 0; j < old_width; j++) 
        {   
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                    
            // loops through buffer for each unit of the scale resize 
            for(int k = 0; k < scale_factor; k++)
            {
                *(img_buffer + cursor_start) = triple;
                cursor_start++;
            }
        }
            
        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);
                
        for (int triple = 0; triple < scale_factor; triple++)
        {
            // write RGB triple to outfile
            fwrite(img_buffer, sizeof(RGBTRIPLE), bi.biWidth, outptr);
                

            // then add it back 
            for (int pad = 0; pad < new_padding; pad++)
            {
                fputc(0x00, outptr);
            }
        }
               
                              
    }
    

    // free buffer memory allocation
    free(img_buffer);
    
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
