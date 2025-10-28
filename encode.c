#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    /*
     * get_image_size_for_bmp
     * ----------------------
     * Read BMP width and height from the header (offset 18) and
     * return the image capacity in bytes (width * height * 3).
     */
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    /* Informative message about image width */
    //printf("Image width: %u pixels\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    /* Informative message about image height */
    //printf("Image height: %u pixels\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

// check extension is in proper format or not
int checkExtension(char *str, char *extension)
{
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

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // check whether the file name is present or not(before .)
    //  Check Source file is having (.bmp) or not
    // encInfo -> src_image_fname = argv[2]
    if (checkExtension(argv[2], ".bmp"))
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        printf("Error: '%s' must have a .bmp extension\n", argv[2]);
        return e_failure;
    }

    // check whether the file name is present or not(before .)
    //  Check Source file is having (.txt or .c or .h or .sh) or not
    // encInfo -> secret_fname = argv[3]
    if (checkExtension(argv[3], ".txt") || checkExtension(argv[3], ".c") || checkExtension(argv[3], ".sh") || checkExtension(argv[3], ".h"))
    {
        encInfo->secret_fname = argv[3];
    }
    else
    {
        printf("Error: '%s' must have one of these extensions: .txt, .c, .sh, .h\n", argv[3]);
        return e_failure;
    }

    // check whether the argv[4] is having NULL or not
    // encInfor -> stego_image_fname = "default.bmp"
    // or
    // check whether the file name is present or not(before .)
    //  Check Source file is having (.bmp) or not
    // encInfor -> stego_image_fname = argv[4]
    if (argv[4] == NULL)
    {
        encInfo->stego_image_fname = "output.bmp";
    }
    else
    {
        if (checkExtension(argv[4], ".bmp"))
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            printf("Error: '%s' must have a .bmp extension\n", argv[4]);
            return e_failure;
        }
    }

    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error: unable to open '%s'\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error: unable to open '%s'\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error: unable to open '%s'\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

int extn_size;

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    char *extn;
    if (strstr(encInfo->secret_fname, ".txt") != NULL)
    {
        extn = ".txt";
    }
    else if (strstr(encInfo->secret_fname, ".c") != NULL)
    {
        extn = ".c";
    }
    else if (strstr(encInfo->secret_fname, ".sh") != NULL)
    {
        extn = ".sh";
    }
    else if (strstr(encInfo->secret_fname, ".h") != NULL)
    {
        extn = ".h";
    }
    strcpy(encInfo->extn_secret_file, extn);
    // printf("%s\n",encInfo->extn_secret_file);
    extn_size = (int)strlen(extn);
    // printf("%d\n",extn_size);
    //  54 header data of image file
    //  length of magic string
    // extension size (.txt) size is 4 store in integer so it takes 32 bytes
    // extension size no of character to store (.txt) = 4*8 or (.c) = 2*8 like that
    // stores the size of file int integer 36 so 32 bytes will needed
    // to store data from secret file so 36 characters * 8 that also added.
    int total_bytes = 54 + (strlen(MAGIC_STRING) * 8) + 32 + (extn_size * 8) + 32 + (encInfo->size_secret_file * 8);

    if (encInfo->image_capacity > total_bytes)
    {
        return e_success;
    }
    else
        return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];

    rewind(fptr_src_image);
    fread(buffer, 54, 1, fptr_src_image);
    fwrite(buffer, 54, 1, fptr_dest_image);

    if (ftell(fptr_src_image) == ftell(fptr_dest_image))
        return e_success;
    else
        return e_failure;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buffer[8];
    for (int i = 0; i < strlen(magic_string); i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(size, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char buffer[8];
    for (int i = 0; i < strlen(file_extn); i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // rewind it
    rewind(encInfo->fptr_secret);
    long size = encInfo->size_secret_file;
    char secret_data[size];
    fread(secret_data, encInfo->size_secret_file, 1, encInfo->fptr_secret);
    char buffer[8];
    for (int i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(secret_data[i], buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // char encode
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & (~1)) | (data >> i) & 1;
    }
    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    // integer encode
    for (int i = 0; i < 32; i++)
    {
        imageBuffer[i] = (imageBuffer[i] & (~1)) | (size >> i) & 1;
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    /* Orchestrate the encoding steps in sequence. Each helper returns
       e_success or e_failure. Print user-friendly messages for errors. */
    if (open_files(encInfo) == e_success)
    {
        /* Print a friendly header and input/output summary */
        printf("=============================================\n");
        printf("🖼️  IMAGE STEGANOGRAPHY USING LSB TECHNIQUE  \n");
        printf("=============================================\n");
        printf("\n=============================================\n");
        printf("🔐 ENCODING MODE SELECTED\n");
        printf("=============================================\n");
        printf("📂 Input BMP Image      : %s\n", encInfo->src_image_fname);
        printf("📄 Secret Message File  : %s\n", encInfo->secret_fname);
        printf("💾 Output Image (Stego) : %s\n", encInfo->stego_image_fname);
        printf("---------------------------------------------\n");
        if (check_capacity(encInfo) == e_success)
        {
            /* Capacity check and basic validation messages */
            printf("🔍 Checking file access and formats...\n");
            printf("✅ All files validated successfully!\n");
            printf("\n⚙️  Encoding Process Started...\n");
            printf("-------------------------------------------------\n");
            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                /* Inform user about header/read phase */
                printf("📦 Reading source image header...\n");
                if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    /* Inform user we're embedding the magic string / bits */
                    printf("💡 Embedding secret message bits into pixel data...\n");
                    if (encode_secret_file_extn_size(extn_size, encInfo) == e_success)
                    {
                        /* Extension size encoded */
                        printf("⏳ Encoding extension metadata...\n");
                        if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            /* Extension encoded */
                            printf("🔐 Extension encoded: %s\n", encInfo->extn_secret_file);
                            if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                /* Secret size encoded */
                                printf("📦 Secret file size: %ld bytes encoded.\n", encInfo->size_secret_file);
                                if (encode_secret_file_data(encInfo) == e_success)
                                {
                                    /* Secret data embedded */
                                    printf("⏳ Please wait, encoding in progress...\n");
                                    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        /* Finalize and report success with a friendly block */
                                        printf("\n🎯 Message successfully embedded into image!\n");
                                        printf("💾 Stego image created: %s\n", encInfo->stego_image_fname);
                                        printf("-------------------------------------------------\n");
                                        printf("✨ Encoding Completed Successfully! ✨\n");
                                        printf("✅ Your data is now hidden securely inside the image.\n");
                                        printf("-------------------------------------------------\n\n");

                                        fclose(encInfo->fptr_secret);
                                        fclose(encInfo->fptr_src_image);
                                        fclose(encInfo->fptr_stego_image);

                                        return e_success;
                                    }
                                    else
                                    {
                                        printf("\n⚠️ ERROR: failed while copying remaining image data.\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("\n⚠️ ERROR: failed to embed secret data into the image.\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("\n⚠️ ERROR: failed to encode secret file size.\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("\n⚠️ ERROR: failed to encode secret file extension.\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("\n⚠️ ERROR: failed to encode extension size.\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("\n⚠️ ERROR: failed to encode magic string.\n");
                    return e_failure;
                }
            }
            else
            {
                printf("\n⚠️ ERROR: failed to read/copy BMP header.\n");
                return e_failure;
            }
        }
        else
        {
            printf("\n🚫 Insufficient image capacity: image too small for the secret file.\n");
            return e_failure;
        }
    }
    else
    {
        printf("\n⚠️ ERROR: unable to open input/output files.\n");
        return e_failure;
    }
}
