#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>

#include "types.h" // Contains user defined types

typedef struct decodeInfo{

    char *stego1_image_fname; // To store the dest file name
    FILE *fptr_stego1_image;

    /* Secret File Info */
    char *secret_fname;       // To store the secret file name
    FILE *fptr_secret;        // To store the secret file address
    char extn_secret_file[5]; // To store the Secret file extension
    //char secret_data[100];    // To store the secret data
    long size_secret_file;    // To store the size of the secret data
    int extn_size;

}DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *dcdInfo);

/* check extensions of file gave from user */
//int checkExtension(char *str, char *extension);
int checkExtension1(char *str, char *extension);

/* Perform the encoding */
Status do_decoding(DecodeInfo *dcdInfo);

/* Get File pointers for i/p and o/p files */
Status open_file_decode(DecodeInfo *dcdInfo);


Status open_file_decode_to_store(DecodeInfo *dcdInfo);


/* Copy bmp image header */
Status skip_bmp_header(FILE *fptr_stego_image);

/* Store Magic String */
Status decode_magic_string(DecodeInfo *dcdInfo);

// /*Encode extension size*/
 Status decode_secret_file_extn_size(DecodeInfo *dcdInfo);

// /* Encode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *dncInfo);

// /* Encode secret file size */
Status decode_secret_file_size(DecodeInfo *dcdInfo);

// /* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *dcdInfo);

/* Encode a byte into LSB of image data array */
Status decode_lsb_to_byte(char *data, char *image_buffer);

// Encode a size to lsb
Status decode_size_to_lsb(int *size, char *imageBuffer);

#endif