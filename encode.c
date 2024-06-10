#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */
/* To check & validate
 * the extension of files passed 
 * and to store in the  respective
 * structure pointers */  
Status read_and_validate_encode_args (char *argv[],EncodeInfo *encInfo )
{
    //checking the first argv and to store it in src 
    if (strcmp(strstr(argv[2],"."),".bmp") == 0)			
        encInfo->src_image_fname = argv[2];
    else
        return e_failure;

    if (strcmp(strstr(argv[3],"."),".txt") == 0)
        encInfo->secret_fname = argv[3];
    else
        return e_failure;

    if ( argv[4] == NULL )
        encInfo->stego_image_fname = "stego.bmp";
    else
        encInfo->stego_image_fname = argv[4];

    return e_success;
 
}


/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}


/* 
 * Check capacity of the file to be encoded
 * Inputs: Structure pointer variables
 * Return Value: e_success or e_failure
 * Description: check the image capacity with all
 * the values to be encoded
 */

Status check_capacity(EncodeInfo *encInfo)
{
	//gets image size
    encInfo-> image_capacity =  get_image_size_for_bmp(encInfo -> fptr_src_image);
	//gets size of secret file
   encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
   if ( encInfo -> image_capacity > (54 + 16 + 32 + 32 + 32 + ( encInfo -> size_secret_file * 8 )))
      return e_success;
   else
      return e_failure; 

}


/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}



/* Get file size
 * Input: file ptr
 * Output: file size in bytes
 */
uint get_file_size( FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);

}


/* 
 * Copy BMP header to stego.bmp
 * Inputs: file pointer of src image,file pointer of destination image
 * Return Value: e_success
 * Description: Copy the first 54 byte data from src
 * and write that data in destination file
 */


Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char str[54];
    fseek(fptr_src_image,0,SEEK_SET);
    fread(str,54,1,fptr_src_image);
    fwrite(str,54,1,fptr_dest_image);
    return e_success;
}

/* 
 * Encode Magic string to file
 * Inputs: char pointer , Entire structure
 * Return Value: e_success
 * Description: Takes the MAGIC_STRING from header
 * and calling the encoding data to image function
 */

Status encode_magic_string ( char *magic_string, EncodeInfo *encInfo )
{
    encode_data_to_image(magic_string,strlen(magic_string), encInfo);
    return e_success;
}

/* 
 * Encoding data to image 
 * Inputs: char data to be encoded (secret file), number of data (length of data) , Structure 
 * Return Value: e_success
 * Description: reads 8 bytes of data from src
 * and calls byte to lsb function for encoding each bit for length times
 * writes the image data to stego.bmp ( encoding file)
 */

Status encode_data_to_image( char *data , int size, EncodeInfo *encInfo)
{
    for (int i =0; i < size; i++)
    {
        fread( encInfo -> image_data,8,1, encInfo -> fptr_src_image );
        encode_byte_to_lsb ( data[i],encInfo -> image_data );
        fwrite( encInfo -> image_data,8,1, encInfo -> fptr_stego_image); 
    }
    return e_success;
}

/* 
 * Encoding byte to lsb 
 * Inputs: char data to be encoded (from user), char data to be encoded ( to be encoded in stego)
 * Return Value: e_success
 * Description: reads 1 bytes of data,
 * takes the LSB and encoding each bit with image data (LSB)
 */

Status encode_byte_to_lsb ( char data, char *image_buffer )
{
    for (int i =0;i < 8; i++)
    {
        image_buffer[i] = ( image_buffer[i] & 0xFE ) | (( data>>i ) & 1 );
    }
    return e_success;
}

/* 
 * Encode Secret file extension size
 * Inputs: size of extension,  Structure 
 * Return Value: e_success
 * Description: pass the values
 * and calls size to lsb function 
 */

Status encode_secret_file_extn_size (int size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(size,encInfo);
    return e_success;
}

/* 
 * Encode size to lsb
 * Inputs: size of extension,  Structure 
 * Return Value: e_success
 * Description: as int is of 32 bit
 * takes 32 bytes of data from src image to str
 * runs a loop to encode each bit to the image data LSB
 */


Status encode_size_to_lsb(int size , EncodeInfo *encInfo)
{
    char str[32];
    fread( str,32,1, encInfo -> fptr_src_image );
    for ( int i = 0; i <32 ;i++)
    {
        str[i] = ( str[i] & 0xFE ) | (( size>>i ) & 1 );
        
    }
    fwrite( str,32,1, encInfo -> fptr_stego_image); 
    return e_success;
}

/* 
 * Encode Secret file extension
 * Inputs: char exten of secret file,  Structure 
 * Return Value: e_success
 * Description: pass the extention a 
 * and calls data to image function 
 */

Status encode_secret_file_extn (char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image( file_extn,strlen(file_extn),encInfo);
    return e_success;
}    

/* 
 * Encode Secret file size
 * Inputs: int size of secret file,  Structure 
 * Return Value: e_success
 * Description: takes secret file size , entire structure elements
 * and size to lsb function 
 */

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(file_size,encInfo);
    return e_success;
}

/* 
 * Encode Secret file data
 * Inputs: Entire  Structure 
 * Return Value: e_success
 * Description: declares str of file size and fix the file pointer 
 * in 0th position of secret file and reads byte upto file size
 * calls data to image function for encoding 
 */

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char str[encInfo -> size_secret_file];
    fseek(encInfo -> fptr_secret,0,SEEK_SET);
    fread(str,encInfo -> size_secret_file,1,encInfo -> fptr_secret);
    encode_data_to_image(str,encInfo -> size_secret_file,encInfo);
    return e_success;
}

/* 
 * Copy remaining data
 * Inputs: Entire  Structure 
 * Return Value: e_success
 * Description: after encoding the rest of the data from 
 * image is copied to the stego image
 */

Status copy_remaining_img_data ( EncodeInfo *encInfo)
{
    int len = encInfo -> image_capacity + 55 - ftell(encInfo -> fptr_src_image);
    char ch[len];
    fread(ch,len,1,encInfo -> fptr_src_image);
    fwrite(ch,len,1,encInfo -> fptr_stego_image);
    return e_success;
}


/* 
 * Encoding the data
 * Inputs: Entire  Structure 
 * Return Value: e_success, e_failure
 * Description: Calls each programs for encoding 1 by 1 
 * if succesfull calls next program
 * if not prints error message
 */

Status do_encoding(EncodeInfo *encInfo)
{
    if ( open_files(encInfo) == e_success )
    {
        printf("Open files is a Success\n");
        if( check_capacity(encInfo) == e_success)
        {
            printf("Check Capacity is a Success\n");
            if ( copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
            {
                printf("copy bmp header is a success\n");
                if ( encode_magic_string(MAGIC_STRING,encInfo ) == e_success )
                {
                    printf("Encoded magic string successfully\n");
                    strcpy( encInfo -> extn_secret_file,strstr(encInfo -> secret_fname,"."));
                    if ((encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file),encInfo)) == e_success)
                    {
                        printf("Encoded secret file extn size\n");
                        if (encode_secret_file_extn(encInfo -> extn_secret_file,encInfo) == e_success)
                        {
                            printf("Encoded secret file extn \n");
                            if ( encode_secret_file_size (encInfo -> size_secret_file,encInfo) == e_success)
                            {
                                printf("Encoded secret file size\n");
                                if ( encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf ("Encoded secret file data\n");
                                    if (copy_remaining_img_data(encInfo) == e_success)
                                    {
                                        printf("Copied remaining data\n");
                                    }
                                    else
                                    {
                                        printf("Failed to copy the remaining data\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf ("Failed to encoded secret file data\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("failed to encode secret file size\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Failed to encoded secret file extn\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Failed to encode secret file extn size\n");
                        return e_failure;

                    }
                }
                else
                {
                    printf("failed to encode the magic string\n");
                    return e_failure;
                }
            }
            else
            {
                printf("copy bmp header is a falure\n");
                return e_failure;
            }
        }
        else
        {
            printf("Check Capacity is a Failure!!!\n");
            return e_failure;

        }

    }
    else
    {
        printf("Open files is a Failure!!!\n");
        return e_failure;
    }
    return e_success;
}

