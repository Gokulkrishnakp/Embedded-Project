#include <stdio.h>
#ifndef DECODE_H
#define DECODE_H

#include "types.h" 

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4


typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char *magic_data;
    char stego_image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *dec_fname;
    FILE *fptr_dec_secret;
    char dec_extn_secret_file[MAX_FILE_SUFFIX];
    char dec_secret_data[MAX_SECRET_BUF_SIZE];
    int  dec_extn_size;
    long dec_size_secret_file;
	
}DecodeInfo;


/* Read and validate Decode args from argv */
Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status_d do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status_d open_files_decode(DecodeInfo *decInfo);

/* Store Magic String */
Status_d decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extn size */
Status_d decode_secret_file_extn_size( DecodeInfo *decInfo);                          

/* Decode secret file extenstion */
Status_d decode_secret_file_extn(int size , DecodeInfo *decInfo);   

/* Decode secret file size */
Status_d decode_secret_file_size( DecodeInfo *decInfo);

/* Decode secret file data*/
Status_d decode_secret_file_data(int size ,DecodeInfo *decInfo);				//

/* Decode function, which does the real decoding */
Status_d decode_data_from_image(int size, char *data ,DecodeInfo *decInfo);   //++

/* Decode a byte into LSB of image data array */
Status_d decode_byte_from_lsb(char *data, char *image_buffer);

/* Generic func to decode integer to lsb */
Status_d decode_size_from_lsb( char *size_buffer, DecodeInfo *decInfo);


#endif
