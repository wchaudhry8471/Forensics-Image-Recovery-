/**
 * Wahaj Chaudhry 
 * wchaudhry@college.harvard.edu 
 * recover.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Recovers JPEGs from a forensic image.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BLOCK 512

#define NAME 8

// unsigned integer for length of 8 bits 
typedef uint8_t BYTE;

int main(int argc, char* argv[])
{
    // opens the infile, hardcoded for now 
    FILE* file = fopen("card.raw", "r");
    
    // handles an empty file 
    if (file == NULL)
    {
        printf("Nothing on file!");
        return 1;
    }
         
    
    // establishes 512 block size 
    BYTE block[BLOCK];
    
    // array for jpg name 
    char name[NAME];
    
    // create pointers for outfile storage    
    FILE* jpeg = NULL;
    
    // initialize counter for jpg name 
    int jpg_counter = 0;
    
    // runs while block size is 512 
    while(fread(&block, 1 , BLOCK, file) == BLOCK)
    {
        // checks for jpg start sequence 
        if (block[0] == 0xff && block[1] == 0xd8 && block[2] == 0xff 
        && (block[3] == 0xe0 || block[3] == 0xe0))
        {
            // closes if output file is directed elsewhere 
            if (jpeg != NULL)
            {
                fclose(jpeg);
            }
                
            // prints file ot current directory 
            sprintf(name, "%03d.jpg", jpg_counter); 
                
            // counter for file naming 
            jpg_counter++;
                
            // opens next file for writing 
            jpeg = fopen(name, "w");
                
            // writes in newly named jpg file from block to jpeg
            fwrite(block, BLOCK, 1, jpeg);                              
        }
            
        // continues to add to existing file 
        else if (jpeg != NULL)
        {
            fwrite(block, 1, BLOCK, jpeg);
        }
            
    } 
     
    // closes outfile   
    fclose(jpeg);
    
    // closes infile 
    fclose(file);
 
    // The End is Nigh! Jk... 
    return 0;
 
 }
