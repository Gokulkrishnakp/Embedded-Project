/* Name: Gokul Krishna K P
* Date : 10/06/2023
* Description : Steganography Project
* input : ./a.out -e beautiful.bmp secret.txt
* 	  ./a.out -d stego.bmp 
*
*/

#include "encode.h"
#include "decode.h"
#include "types.h"
#include<stdio.h>
#include<string.h>

int main(int argc, char *argv[])
{
	// to check if sufficient arguments are passed
    if ( argc >= 3 ) 
    {
	//checking operation
        if ( check_operation_type(argv) == e_encode)
        {
            printf ( "Selected Encoding\n" );
	    //structure variable declaration
            EncodeInfo encInfo;
	   // passing the file along with structure variable for validation
           if ( read_and_validate_encode_args(argv,&encInfo) == e_success)
           {
               printf ("Read and Validate encode arg func is a success\n");
		//after successfull validation calling encoding function
               if ( do_encoding(&encInfo) == e_success)
               {
                   printf("Done Encoding successfully\n");
               }
               else
               {
                   printf("Failed to do Encode\n");
                   return -1;
               }

           }
           else
           {
               printf ("Read and Validate encode arg func is a failure\n");
               return -1;
           }
        }

	//same as of encoding but for decoding
        else if (check_operation_type(argv) == e_decode)
        {
            printf ( "Selected Decoding\n" );
            DecodeInfo decInfo;
            if ( read_and_validate_decode_args(argv,&decInfo) == d_success)
           {
               printf ("Read and Validate decode arg func is a success\n");
		if ( do_decoding(&decInfo) == d_success)
               {
                   printf("Done Decoding successfully\n");
               }
               else
               {
                   printf("Failed to do Decoding\n");
                   return -1;
               }
           }
           else
           {
               printf ("Read and Validate decode arg func is a failure\n");
               return -1;
           } 
        }
        else
        {
            printf("Invalid input\nplease pass \nfor Encoding : ./a.out -e beautiful.bmp secret.txt stego.bmp\nfor Decoding : ./a.out -d stego.bmp decode.txt\n");
        }
    }
    else
    {
       printf ("Pass the sufficient number of arguments \nplease pass \nfor Encoding : ./a.out -e beautiful.bmp secret.txt stego.bmp\nfor Decoding : ./a.out -d stego.bmp decode.txt\n");
       return -1;
    }

    return 0;
}
/* Function declaration */
/* 
 * Checking operation
 * Inputs: Files passed through CLA
 * Return Value: e_encode, e_decode,e_unsupported
 * Description: check the CLA at pos 1
 * check with the given conditions
 * and return results 
 */
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e") == 0)
        return e_encode;
    else if(strcmp (argv[1],"-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}

