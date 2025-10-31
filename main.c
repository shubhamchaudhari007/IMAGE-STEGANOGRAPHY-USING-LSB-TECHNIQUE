#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
#include <string.h>

/*
 
Name        : Shubham Sanjay Chaudhari
Date        : 17_09_2025
Batch ID    : 25021C_306
Language    : C Programming
Project     : 

  Image Steganography using LSB Technique
  ---------------------------------------
  A C-based project to hide and retrieve secret data inside image files 
  using the Least Significant Bit (LSB) encoding technique, ensuring 
  data confidentiality without affecting image quality.

  Project Overview:
    - This program provides a secure and efficient way to conceal text data
      within a BMP image file by modifying the least significant bits of 
      pixel data.
    - The hidden information can be retrieved accurately during decoding 
      without altering the original image content perceptibly.
    - Ensures reliability with proper input validation, file size checking,
      and memory-safe operations.

  Core Features:
    1. Encode Mode:
       - Embeds secret text data into the image pixels.
       - Checks image capacity before encoding.
       - Supports dynamic message length and custom output filename.
    2. Decode Mode:
       - Extracts and reconstructs hidden text data from encoded images.
       - Automatically generates decoded text files.
    3. File Validation:
       - Validates BMP headers, file extensions, and readable file states.
    4. Error Handling:
       - Handles missing files, insufficient image size, and invalid arguments.
    5. Progress Feedback:
       - Displays step-by-step encoding and decoding process for user clarity.

  Working Algorithm:
    - Each character of the secret message is broken down into bits.
    - Each bit replaces the least significant bit (LSB) of a pixel byte.
    - The process continues until the entire message is encoded.
    - During decoding, bits are read back and reassembled to recover the message.

  File Structure:
    - main.c        : Contains main() and handles encode/decode mode selection.
    - encode.c/h    : Functions for embedding secret data into the image.
    - decode.c/h    : Functions for retrieving hidden data from the stego-image.
    - common.c/h    : Shared helper functions for file I/O, bit manipulation, etc.
    - makefile      : Used to compile and link all modules together easily.

  Extensibility:
    - Can be upgraded to support password protection and AES encryption.
    - Extendable to PNG or JPEG formats with minor modifications.
    - GUI or command-line argument parsing can be integrated for automation.

  Suitable For:
    - Students exploring image processing, file handling, and bitwise operations.
    - Developers interested in basic cryptography and data hiding techniques.
    - Projects focusing on secure communication and information protection.

*/


OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    /*
     * Simple CLI entry point for the steganography tool.
     * Usage examples:
     *  - Encoding: a.out -e <source.bmp> <secret.txt> [output.bmp]
     *  - Decoding: a.out -d <stego.bmp> [output_secret_base]
     */

    printf("=============================================\n");
    printf("🖼️  IMAGE STEGANOGRAPHY USING LSB TECHNIQUE  \n");
    printf("=============================================\n");
    // Step 1 : Check the argc >= 4 true - > step 2
    if (argc >= 4)
    {
        // Step 2 : Call the check_operation_type((argv[1]) == e_encode ) true - > step 3
        if (check_operation_type(argv[1]) == e_encode)
        {
            // Step 3 : Declare structure variable EncodeInfo enc_info
            EncodeInfo enc_Info;
            // Step 4 : Call the read_and_validate_encode_args(argv,&enc_info)== e_success)))))
            if (read_and_validate_encode_args(argv, &enc_Info) == e_success)
            {
                //  true -> Step 5 ,
                // Step 5 : Call the do_encoding (&encInfo);
                if (do_encoding(&enc_Info) == e_success)
                {
                    printf("\n✨ Encoding Completed Successfully! ✨\n");
                    printf("\n-----------------------------------\n");
                    printf("💾 Output stego file: %s\n", enc_Info.stego_image_fname);
                    printf("-----------------------------------\n");
                }
                else{
                    printf("\n------------------------------------------------------\n");
                    printf(" ❌ Error: decoding failed.");
                    printf("\n------------------------------------------------------\n");
                }
            }
            else
            {
                // false - > terminate the program

                return 0;
            }
        }
        else if (check_operation_type(argv[1]) == e_decode)
        {
            // for decoding
            DecodeInfo dcd_Info;

            printf("=============================================\n");
            printf("🔓 DECODING MODE SELECTED\n");
            printf("=============================================\n");

            if (read_and_validate_decode_args(argv, &dcd_Info) == e_success)
            {
                // open .bmp file to read magic string and check the file is encoded or not
                open_file_decode(&dcd_Info);

                // check magic string is present or not
                if (decode_magic_string(&dcd_Info) == e_success)
                {
                    printf("Input file appears to contain embedded data. Starting decode...\n");
                    printf("---------------------------------------------\n");
                    // do decoding
                    if (do_decoding(&dcd_Info) == e_success)
                    {
                        printf("\n✨ Decoding Completed Successfully! ✨\n");
                        printf("\n-----------------------------------\n");
                        printf("💾 Output: %s\n", dcd_Info.secret_fname);
                        printf("-----------------------------------\n");
                    }
                    else
                    {
                        printf("\n------------------------------------------------------\n");
                        printf(" ❌ Error: decoding failed.");
                        printf("\n=======================================================\n");
                    }
                }
                else
                {
                    printf("\n------------------------------------------------------\n");
                    printf(" ❌ The provided file does not appear to be encoded. Please supply a valid encoded BMP.");
                    printf("\n==========================================================\n");
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            printf("\n------------------------------------------------------\n");
            printf(" ❌ Unsupported operation. Use -e to encode or -d to decode.");
            printf("\n========================================================\n");
            return 0;
        }
    }
    else
    {
        printf("\n------------------------------------------------------\n");
        printf(" ❌ Insufficient arguments. \nSee usage: \na.out -e <source.bmp> <secret> [output.bmp]  OR  \na.out -d <stego.bmp> [output_secret_base]\n");
        printf("==========================================================\n");
        return 0;
    }
}

OperationType check_operation_type(char *symbol)
{

    if (!strcmp(symbol, "-e"))
    {
        // Step 1 : Check whether the symbol is -e or not true - > return e_encode false -> Step 2
        return e_encode;
    }
    else if (!strcmp(symbol, "-d"))
    {
        // Step 2 : Check whether the symbol is -d or not true - > return e_decode
        return e_decode;
    }
    else
    {
        // false -> return e_unsupported
        return e_unsupported;
    }
}
