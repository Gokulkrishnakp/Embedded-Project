#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "common.h"


/* Function Definitions */
/* To check & validate
 * the extension of files passed 
 * and to store in the  respective
 * structure pointers */ 
Status_d read_and_validate_decode_args ( char *argv[] , DecodeInfo *decInfo)
{
    printf("Entered into the function\n");
    if(strcmp(strstr(argv[2],"."),".bmp") == 0 )
        decInfo -> stego_image_fname = argv[2];
    else
        return d_failure;

    if( argv[3] == NULL )
        decInfo -> dec_fname = "decode.txt";
    else
        decInfo -> dec_fname = argv[3];

    return d_success;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status_d open_files_decode(DecodeInfo *decInfo)
{


    // Src Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return d_failure;
    }


    // Secret message file
    decInfo->fptr_dec_secret = fopen(decInfo->dec_fname, "w");
    // Do Error handling
    if (decInfo->fptr_dec_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->dec_fname);

    	return d_failure;
    }

    // No failure return d_success
    return d_success;
}


/* 
 * Decode Magic string from file
 * Inputs: char pointer , Entire structure
 * Return Value: d_success, d_failure
 * Description: Takes the MAGIC_STRING from header
 * and calling the decoding data from image function
 * if the decoded is not same as encoded then returns failed
 */

Status_d decode_magic_string(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
    int len = strlen(MAGIC_STRING);
    decInfo -> magic_data = malloc( len + 1 );

    decode_data_from_image(len,decInfo -> magic_data, decInfo);
  
    
    decInfo -> magic_data[len]='\0';
    printf("%s\n",decInfo -> magic_data);
    if(strcmp(decInfo -> magic_data,MAGIC_STRING) == 0 )
        return d_success;
    else
        return d_failure;
}


/* 
 * Decoding data from image 
 * Inputs: number of data (length of data), char data to be decoded (secret file) , Structure 
 * Return Value: d_success
 * Description: reads 8 bytes of data from stego image
 * and calls decode byte from lsb function for decoding each bit for length times
 */


Status_d decode_data_from_image( int size, char *data, DecodeInfo *decInfo)
{
    for ( int i=0; i < size ; i++)
    {
        fread(decInfo -> stego_image_data,8,1,decInfo -> fptr_stego_image);
        decode_byte_from_lsb(&data[i], decInfo -> stego_image_data);
    }
    return d_success;

}


/* 
 * Decoding byte to lsb 
 * Inputs: char data to be encoded (from stego), char data to be encoded ( to be decoded in file)
 * Return Value: d_success
 * Description: reads 1 bytes of data,
 * takes the LSB and decode each bit, forms a character
 */

Status_d decode_byte_from_lsb( char *data , char *image_buffer)
{
    unsigned char ch = 0x00;
    for ( int i = 0 ; i < 8 ; i++ )
    {
        ch = ((image_buffer[i] & 0x01 ) << i ) | ch;
    }
    *data=ch;
    return d_success;
        
}


/* 
 * decode Secret file extension size
 * Inputs: Structure 
 * Return Value: d_success
 * Description: as int is of 32 bit
 * takes 32 bytes of data from src image to str
 * pass theat to decode size from lsb function to 
 * decode each bit from the image data
 */

Status_d decode_secret_file_extn_size( DecodeInfo *decInfo )
{
    char str[32];
    fread(str,32,1,decInfo -> fptr_stego_image);
    decode_size_from_lsb(str, decInfo);
    decInfo -> dec_extn_size = decInfo -> dec_size_secret_file;     
  //  printf("%d\n",decInfo -> dec_extn_size);
    return d_success;
}


/* 
 * Encode size to lsb
 * Inputs: char str read from stego ,  Structure 
 * Return Value: d_success
 * Description: as int is of 32 bit
 * takes 32 bytes of data from src image 
 * runs a loop 32 times to decode each bit 
 * takes the LSB and decode each bit
 */

Status_d decode_size_from_lsb ( char *size_buffer,DecodeInfo *decInfo)
{
    int s = 0x00;
    for (int i = 0 ; i < 32 ; i++ )
    {
        s = ((size_buffer[i] & 0x01 ) << i ) | s;
    }
    decInfo -> dec_size_secret_file = s;
    return d_success;

}


/* 
 * Decode extn from file
 * Inputs: int size of extn , Entire structure
 * Return Value: d_success, d_failure
 * Description: calls decode data from image function
 * after getting the extension it is then checked 
 * if the decoded is not same as encoded extn then returns failed
 */


Status_d decode_secret_file_extn (int size, DecodeInfo *decInfo)
{
    decode_data_from_image(size,decInfo -> dec_extn_secret_file,decInfo);
    decInfo -> dec_extn_secret_file[size] = '\0';
    //printf("%s\n",decInfo ->dec_extn_secret_file);
    if ( strcmp( strstr( decInfo -> dec_fname,"."), decInfo -> dec_extn_secret_file) == 0  )
        return d_success;
    else
        return d_failure;
}


/* 
 * Decode Secret file size
 * Inputs: Structure 
 * Return Value: d_success
 * Description: since size is int reads 32 bytes of data from stego
 * and calls decode size from lsb function
 */

Status_d decode_secret_file_size ( DecodeInfo  *decInfo)
{
    char str[32];
    fread(str,32,1,decInfo -> fptr_stego_image);
    decode_size_from_lsb(str, decInfo);
  //  printf("%d\n",decInfo -> dec_size_secret_file);
    return d_success;

}

/* 
 * Decode Secret file data
 * Inputs:int size of secret file, Entire  Structure 
 * Return Value: d_success
 * Description: declares ch,run a loop for size time, 
 * then reads 8 bytes of data from stego 
 * then calls the decode byte to function with ch having the return value
 * writes this in the decode file
 */

Status_d decode_secret_file_data ( int size, DecodeInfo *decInfo)
{
    char ch;
    for ( int i=0; i < size ; i++)
    {
        fread(decInfo -> stego_image_data,8,1,decInfo -> fptr_stego_image);
        decode_byte_from_lsb(&ch, decInfo -> stego_image_data);
        fputc(ch,decInfo -> fptr_dec_secret);

    }
    return d_success;

    
}

/* 
 * Decoding the data
 * Inputs: Entire  Structure 
 * Return Value: d_success, d_failure
 * Description: Calls each programs for doing decoding 1 by 1 
 * if successfull, then calls next program
 * if not prints error message
 */

Status_d do_decoding (DecodeInfo *decInfo)
{
    if(open_files_decode(decInfo)==d_success)
    {
        printf("opened files for decoding successfully\n");
        if (decode_magic_string(decInfo) == d_success)
        {
            printf("Decoded Magic string successfully\n");
            if ( decode_secret_file_extn_size ( decInfo ) == d_success)
            {
                printf("Decoded extension size successfully\n");
                if ( decode_secret_file_extn(decInfo -> dec_extn_size,decInfo) == d_success)
                {
                    printf("Decoded the file extension successfully\n");
                    if ( decode_secret_file_size( decInfo ) == d_success)
                    {
                        printf("Decoded secret file size successfully\n");
                        if ( decode_secret_file_data(decInfo -> dec_size_secret_file , decInfo) == d_success )
                        {
                            printf("Decoded the secret file successfully\n");
                        }
                        else
                        {
                            printf("failed to decode the secret file \n");
                            return d_failure;
                        }
                    }
                    else
                    {
                        printf("Failed to decode  secret file size \n");
                        return d_failure;
                    }
                }
                else
                {
                    printf("Failed to decode the file extension\n");
                    return d_failure;
                }
            }
            else
            {
                printf("Failed to decode the extension size \n");
                return d_failure;
            }
        }
        else
        {
            printf("failed to decode the  Magic string\n");
            return d_failure;
        }
    }
    else
    {
        printf("Failed to open files for decoding\n");
        return d_failure;
    }
    return d_success;
}
