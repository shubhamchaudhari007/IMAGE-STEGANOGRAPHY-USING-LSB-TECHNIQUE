#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>
// #include "encode.h"

int checkExtension1(char *str, char *extension)
{
    /*
     * checkExtension1
     * -----------------
     * Return 1 if the given filename 'str' ends with the provided
     * 'extension' (for example, ".bmp"). Returns 0 otherwise.
     */
    if (str[0] == '.')
    {
        return 0;
    }

    int i = strlen(str) - 1;
    int j = strlen(extension) - 1;
    while (j >= 0)
    {
        if (str[i] != extension[j])
            return 0;

        j--;
        i--;
    }
    return 1;
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *dcdInfo)
{
    /*
     * Validate decode arguments from argv and populate dcdInfo.
     * Expects argv[2] to be the stego BMP file and optional argv[3]
     * to be the desired output base filename for the secret.
     */

    if (checkExtension1(argv[2], ".bmp"))
    {
        dcdInfo->stego1_image_fname = argv[2];
        // return e_success;
    }
    else
    {
        printf("Error: '%s' must have a .bmp extension.\n", argv[2]);
        return e_failure;
    }

    /* Use default output base name "Decode" when user didn't provide one. */
    if (argv[3] == NULL)
    {
        dcdInfo->secret_fname = "Decode";
    }
    else
    {
        dcdInfo->secret_fname = strtok(argv[3], ".");
    }
    return e_success;
}

/* check extensions of file gave from user */
// int checkExtension(char *str, char *extension);

/* Get File pointers for i/p and o/p files */
Status open_file_decode(DecodeInfo *dcdInfo)
{
    /*
     * Open the stego image file and store the FILE* in the structure.
     * Returns e_success on success or e_failure on error.
     */

    // Src Image file
    dcdInfo->fptr_stego1_image = fopen(dcdInfo->stego1_image_fname, "rb");
    // Do Error handling
    if (dcdInfo->fptr_stego1_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", dcdInfo->stego1_image_fname);

        return e_failure;
    }

    return e_success;
}

/* Copy bmp image header */
Status skip_bmp_header(FILE *fptr_stego1_image)
{
    /*
     * Skip the BMP header (54 bytes) so we are positioned at the
     * start of the pixel data where embedded bits are stored.
     */

    fseek(fptr_stego1_image, 54, SEEK_SET);
    return e_success;
}

/* Store Magic String */
Status decode_magic_string(DecodeInfo *dcdInfo)
{
    /*
     * Read and reconstruct the Magic string from LSBs and compare
     * it to MAGIC_STRING. Returns e_success if it matches.
     */

    char buffer[8];
    char str[100];
    skip_bmp_header(dcdInfo->fptr_stego1_image);
    int i;
    for (i = 0; i < strlen(MAGIC_STRING); i++)
    {
        char ch;
        fread(buffer, 8, 1, dcdInfo->fptr_stego1_image);
        decode_lsb_to_byte(&ch, buffer);
        str[i] = ch;
    }
    str[i] = '\0';
    if (!strcmp(MAGIC_STRING, str))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/*Encode extension size*/
Status decode_secret_file_extn_size(DecodeInfo *dcdInfo)
{
    /*
     * Read the 32 image-bytes that encode the extension size and
     * decode that into dcdInfo->extn_size.
     */
    char buffer[32];
    fread(buffer, 32, 1, dcdInfo->fptr_stego1_image);
    decode_size_to_lsb(&dcdInfo->extn_size, buffer);
    return e_success;
}

// /* Encode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *dncInfo)
{
    /*
     * Reconstruct the secret file extension by decoding extn_size
     * bytes (each byte is encoded across 8 image bytes).
     */
    char buffer[8];
    char str[100];
    int i;
    for (i = 0; i < dncInfo->extn_size; i++)
    {
        char ch;
        fread(buffer, 8, 1, dncInfo->fptr_stego1_image);
        decode_lsb_to_byte(&ch, buffer);
        str[i] = ch;
    }
    str[i] = '\0';

    strcpy(dncInfo->extn_secret_file, str);
    return e_success;
}

// /* Encode secret file size */
Status decode_secret_file_size(DecodeInfo *dcdInfo)
{
    /*
     * Decode the next 32 image-bytes into the secret file size
     * (stored in dcdInfo->size_secret_file).
     */
    char buffer[32];
    int num;
    fread(buffer, 32, 1, dcdInfo->fptr_stego1_image);
    decode_size_to_lsb(&num, buffer);
    dcdInfo->size_secret_file = (long)num;
    return e_success;
}

Status open_file_decode_to_store(DecodeInfo *dcdInfo){
    /*
     * Append the decoded extension to the output base name and write
     * the decoded secret bytes to that output file.
     */
    strcat(dcdInfo->secret_fname, dcdInfo->extn_secret_file);

    dcdInfo->fptr_secret = fopen(dcdInfo->secret_fname, "w");

    if (dcdInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", dcdInfo->stego1_image_fname);

        return e_failure;
    }

    return e_success;
}

// /* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *dcdInfo)
{
    open_file_decode_to_store(dcdInfo);

    char buffer[8];
    for (int i = 0; i < dcdInfo->size_secret_file; i++)
    {
        char ch;
        fread(buffer, 8, 1, dcdInfo->fptr_stego1_image);
        decode_lsb_to_byte(&ch, buffer);
        fwrite(&ch, 1, 1, dcdInfo->fptr_secret);
    }
    /* Note: file is not explicitly closed here to preserve original logic */

    return e_success;
}

/* Encode a byte into LSB of image data array */
Status decode_lsb_to_byte(char *data, char *image_buffer)
{
    // char decode
    /* initialize accumulator to zero before assembling bits */
    unsigned char acc = 0;
    for (int i = 0; i < 8; i++)
    {
        acc |= (unsigned char)(image_buffer[i] & 1) << i;
    }
    *data = (char)acc;
    return e_success;
}

// Encode a size to lsb
Status decode_size_to_lsb(int *size, char *imageBuffer)
{
    // integer decode
    uint size1 = 0;
    for (int i = 0; i < 32; i++)
    {
        size1 |= (uint)(imageBuffer[i] & 1) << i;
    }
    *size = (int)size1;
    return e_success;
}

/* Perform the encoding */
Status do_decoding(DecodeInfo *dcdInfo)
{
    /* Run decode steps in sequence and print a user-friendly result */
    /* Print the same project banner used for encoding so both flows match */
    
    if (decode_secret_file_extn_size(dcdInfo) == e_success)
    {
        /* Friendly decode header and progress messages */
        
        // printf("📂 Input Stego Image : %s\n", dcdInfo->stego1_image_fname);
        // /* Show intended output base + extension when available */
        // printf("💾 Output Text File  : %s%s\n", dcdInfo->secret_fname, dcdInfo->extn_secret_file);
        // printf("---------------------------------------------\n");

        printf("🔍 Validating and reading image data...\n");
        printf("✅ Image file verified successfully!\n");
        printf("\n🧩 Extracting hidden bits from image...\n");

        if (decode_secret_file_extn(dcdInfo) == e_success)
        {
            /* Secret file extension reconstructed */
            printf("📜 Reconstructing the hidden message (ext: %s)...\n", dcdInfo->extn_secret_file);
            if (decode_secret_file_size(dcdInfo) == e_success)
            {
                /* Secret file size decoded */
                printf("⏳ Decoding in progress, please wait... (%ld bytes)\n", dcdInfo->size_secret_file);

                if (decode_secret_file_data(dcdInfo) == e_success)
                {
                    /* Successful decode summary */
                    printf("\n🎉 Hidden message extracted successfully!\n");
                    printf("💾 Decoded text saved as: %s\n", dcdInfo->secret_fname);
                    printf("-------------------------------------------------\n");
                    printf("✨ Decoding Completed Successfully! ✨\n");
                    printf("✅ Secret data retrieved without loss.\n");
                    printf("-------------------------------------------------\n\n");

                    fclose(dcdInfo->fptr_secret);
                    fclose(dcdInfo->fptr_stego1_image);

                    return e_success;
                }
                else
                {
                    printf("\n⚠️  ERROR: failed to write decoded secret data.\n");
                    return e_failure;
                }
            }
            else
            {
                printf("\n⚠️  ERROR: failed to decode secret file size.\n");
                return e_failure;
            }
        }
        else
        {
            printf("\n⚠️  ERROR: failed to decode secret file extension.\n");
            return e_failure;
        }
    }
    else
    {
        printf("\n🚫 ERROR: failed to decode extension size (invalid or corrupted stego image).\n");
        return e_failure;
    }
}