#ifdef COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <malloc.h>

#include "./SlimEngine/core/types.h"
#include "./SlimEngine/math/vec4.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
//typedef struct BITMAPFILEHEADER
//{
//    WORD bfType;  //specifies the file type
//    DWORD bfSize;  //specifies the size in bytes of the bitmap file
//    WORD bfReserved1;  //reserved; must be 0
//    WORD bfReserved2;  //reserved; must be 0
//    DWORD bfOffBits;  //specifies the offset in bytes from the bitmapfileheader to the bitmap bits
//} BITMAPFILEHEADER;
//typedef struct BITMAPINFOHEADER
//{
//    DWORD biSize;  //specifies the number of bytes required by the struct
//    LONG biWidth;  //specifies width in pixels
//    LONG biHeight;  //specifies height in pixels
//    WORD biPlanes;  //specifies the number of color planes, must be 1
//    WORD biBitCount;  //specifies the number of bits per pixel
//    DWORD biCompression;  //specifies the type of compression
//    DWORD biSizeImage;  //size of image in bytes
//    LONG biXPelsPerMeter;  //number of pixels per meter in x axis
//    LONG biYPelsPerMeter;  //number of pixels per meter in y axis
//    DWORD biClrUsed;  //number of colors used by the bitmap
//    DWORD biClrImportant;  //number of colors that are important
//}BITMAPINFOHEADER;

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
    FILE *filePtr;  //our file pointer
    BITMAPFILEHEADER bitmapFileHeader;  //our bitmap file header
    unsigned char *bitmapImage;  //store image data
    int imageIdx=0;  //image index counter
    unsigned char tempRGB;  //our swap variable

    //open file in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return NULL;

    //read the bitmap file header
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);

    //verify that this is a .BMP file by checking bitmap id
    if (bitmapFileHeader.bfType !=0x4D42)
    {
        fclose(filePtr);
        return NULL;
    }

    //read the bitmap info header
    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr);

    //move file pointer to the beginning of bitmap data
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    if (bitmapInfoHeader->biSizeImage == 0) {
        bitmapInfoHeader->biSizeImage = bitmapInfoHeader->biBitCount * bitmapInfoHeader->biHeight * bitmapInfoHeader->biWidth;
    }
    //allocate enough memory for the bitmap image data
    bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

    //verify memory allocation
    if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    //read in the bitmap image data
    fread(bitmapImage,bitmapInfoHeader->biSizeImage,1,filePtr);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        return NULL;
    }

    //swap the R and B values to get RGB (bitmap is BGR)
    u8 step = bitmapInfoHeader->biBitCount == 32 ? 4 : 3;
    for (imageIdx = 0;imageIdx < bitmapInfoHeader->biSizeImage;imageIdx+=step)
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    //close file and return bitmap image data
    fclose(filePtr);
    return bitmapImage;
}

typedef struct BitmapHeader {
    u16 file_type;  // Type of the file
    u32 file_size;  // Size of the file (in bytes)

    u16 reserved1;  // Reserved (0)
    u16 reserved2;  // Reserved (0)

    u32 data_offset;  // Offset to the data (in bytes)

    u32 struct_size;  // Struct size (in bytes )

    i32 width;    // Bitmap width  (in pixels)
    i32 height;   // Bitmap height (in pixels)

    u16 planes;      // Color planes count (1)
    u16 bit_depth;   // Bits per pixel

    u32 compression;  // Compression type
    u32 image_size;   // Image size (in bytes)

    i32 x_pixels_per_meter;   // X Pixels per meter
    i32 y_pixels_per_meter;   // Y pixels per meter

    u32 colors_used;      // User color count
    u32 colors_important; // Important color count
} BitmapHeader;

//void loadBitmapHeader(BitmapHeader *header, char* file_path, Platform *platform) {
//    void *file = platform->openFileForReading(file_path);
//    platform->readFromFile(header, sizeof(BitmapHeader), file);
//    platform->closeFile(file);
//}
//
//void loadBitmapFile(Bitmap *bitmap, char* file_path, u8 *file_data, Platform *platform, Memory *memory) {
//    void *file = platform->openFileForReading(file_path);
//    platform->readFromFile(file_data, sizeof(BitmapHeader), file);
//    bitmap->header = *((BitmapHeader*)file_data);
//    platform->readFromFile(file_data, bitmap->header.file_size - sizeof(BitmapHeader), file);
//    platform->closeFile(file);
//
//    u32 pixel_data_size = bitmap->header.width * bitmap->header.height * 4;
//    u8 *trg_pixel_data = bitmap->pixel_data = (u8*)allocateMemory(memory, pixel_data_size);
//    u8 *src_pixel_data = file_data + (bitmap->header.data_offset - sizeof(BitmapHeader));
//    for (u32 i = 0; i < pixel_data_size; i++) *trg_pixel_data = *src_pixel_data;

void initTextureMip(TextureMip *mip, u16 width, u16 height) {
    mip->width = width;
    mip->height = height;
    mip->texels = (vec4*)malloc(sizeof(vec4) * width * height);

    width++;
    height++;

    mip->texel_quads      = (TexelQuad* )malloc(sizeof(TexelQuad ) * width * height);
    mip->texel_quad_lines = (TexelQuad**)malloc(sizeof(TexelQuad*) * height);
    mip->texel_quad_lines[0] = mip->texel_quads;
    for (u16 y = 1; y < height; y++) mip->texel_quad_lines[y] = mip->texel_quad_lines[y - 1] + width;
}

void loadTextureMip(TextureMip *mip, bool wrap) {
    TexelQuad *top_texel_quad_line    = mip->texel_quad_lines[0];
    TexelQuad *bottom_texel_quad_line = mip->texel_quad_lines[mip->height];
    TexelQuad *TL, *TR, *BL, *BR;
    bool L, R, T, B;
    const u16 last_y = mip->height - 1;
    const u16 last_x = mip->width - 1;
    const u32 l = 0;
    const u32 r = mip->width;
    vec4 *texel = mip->texels;
    TexelQuad *current_line, *next_line;
    for (u16 y = 0; y < mip->height; y++) {
        current_line = mip->texel_quad_lines[y];
        next_line = mip->texel_quad_lines[y + 1];

        T = (y == 0);
        B = (y == last_y);

        for (u16 x = 0; x < mip->width; x++, texel++) {
            TL = current_line + x;
            TR = current_line + x + 1;
            BL = next_line + x;
            BR = next_line + x + 1;

            L = (x == 0);
            R = (x == last_x);

            TL->BR = TR->BL = BL->TR = BR->TL = *texel;
            if (     L) TL->BL = BL->TL = *texel;
            else if (R) TR->BR = BR->TR = *texel;
            if (wrap) {
                if (     L) current_line[r].BR = next_line[r].TR = *texel;
                else if (R) current_line[l].BL = next_line[l].TL = *texel;
                if (T) {
                    bottom_texel_quad_line[x].BR = bottom_texel_quad_line[x + 1].BL = *texel;
                    if (     L) bottom_texel_quad_line[r].BR = *texel;
                    else if (R) bottom_texel_quad_line[l].BL = *texel;
                } else if (B) {
                    top_texel_quad_line[x].TR = top_texel_quad_line[x + 1].TL = *texel;
                    if (     L) top_texel_quad_line[r].TR = *texel;
                    else if (R) top_texel_quad_line[l].TL = *texel;
                }
            } else {
                if (T) {
                    TL->TR = TR->TL = *texel;

                    if (     L) TL->TL = *texel;
                    else if (R) TR->TR = *texel;

                } else if (B) {
                    BL->BR = BR->BL = *texel;

                    if (     L) BL->BL = *texel;
                    else if (R) BR->BR = *texel;
                }
            }
        }
    }
}

void loadTexture(Texture *texture, u16 width, u16 height, bool wrap, bool mipmap, bool filter, u8 *texel_components, u8 bit_count) {
    texture->wrap = wrap;
    texture->mipmap = mipmap;
    texture->filter = filter;
    texture->width = width;
    texture->height = height;
    texture->mip_count = 1;

    u16 mip_width  = texture->width;
    u16 mip_height = texture->height;
    if (texture->mipmap)
        while (mip_width > 4 && mip_height > 4) {
            mip_width /= 2;
            mip_height /= 2;
            texture->mip_count++;
        }

    texture->mips = (TextureMip*)malloc(sizeof(TextureMip) * texture->mip_count);

    initTextureMip(texture->mips, width, height);

    vec4 *texel = texture->mips->texels;
    u8 i, *texel_component = texel_components;
    u16 x, y;
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++, texel++) {
            if (bit_count == 32) {
                for (i = 0; i < 4; i++, texel_component++)
                    texel->components[i] = (f32)(*texel_component) * COLOR_COMPONENT_TO_FLOAT;
            } else {
                for (i = 0; i < 3; i++, texel_component++)
                    texel->components[i] = (f32)(*texel_component) * COLOR_COMPONENT_TO_FLOAT;
                texel->components[3] = texel->components[2];
            }
        }

    loadTextureMip(texture->mips, texture->wrap);

    if (!texture->mipmap) return;

    TextureMip *current_mip = texture->mips;
    TextureMip *next_mip = current_mip + 1;

    mip_width  = texture->width;
    mip_height = texture->height;

    TexelQuad *colors_quad;
    u32 stride, offset, start;

    while (mip_width > 4 && mip_height > 4) {
        start = mip_width + 1;
        stride = start * 2;

        mip_width  /= 2;
        mip_height /= 2;

        initTextureMip(next_mip, mip_width, mip_height);

        for (y = 0; y < mip_height; y++) {
            offset = start + 1;

            for (x = 0; x < mip_width; x++) {
                colors_quad = current_mip->texel_quads + offset;
                texel = next_mip->texels + (mip_width * y + x);
                *texel = colors_quad->quadrants[0];
                for (i = 1; i < 4; i++) *texel = addVec4(*texel, colors_quad->quadrants[i]);
                *texel = scaleVec4(*texel, 0.25f);
                offset += 2;
            }

            start += stride;
        }

        loadTextureMip(next_mip, wrap);

        current_mip++;
        next_mip++;
    }
}

int bmp2texture(char* bmp_file_path, char* texture_file_path, bool mipmap, bool wrap, bool filter) {
    BITMAPINFOHEADER bitmapInfoHeader;
    u8* texel_components = LoadBitmapFile(bmp_file_path, &bitmapInfoHeader);

    FILE* file;
    Texture texture;
    loadTexture(&texture, bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, wrap, mipmap, filter, texel_components, bitmapInfoHeader.biBitCount);

    file = fopen(texture_file_path, "wb");

    fwrite(&texture.width,  sizeof(u16), 1, file);
    fwrite(&texture.height, sizeof(u16), 1, file);
    fwrite(&texture.filter,      sizeof(bool), 1, file);
    fwrite(&texture.mipmap,      sizeof(bool), 1, file);
    fwrite(&texture.wrap,      sizeof(bool), 1, file);
    fwrite(&texture.mip_count, sizeof(u8), 1, file);

    TextureMip *texture_mip = texture.mips;
    for (u8 mip_index = 0; mip_index < texture.mip_count; mip_index++, texture_mip++) {
        fwrite(&texture_mip->width,  sizeof(u16), 1, file);
        fwrite(&texture_mip->height, sizeof(u16), 1, file);
        fwrite(texture_mip->texels, sizeof(vec4), texture_mip->width * texture_mip->height, file);

        if (texture.filter)
            fwrite(texture_mip->texel_quads, sizeof(TexelQuad), ((texture_mip->width + 1) * (texture_mip->height + 1)), file);
    }

    fclose(file);

    return 0;
}

int main(int argc, char *argv[]) {
    //return error if user does not provide exactly 4 arguments
    if (argc == 6)
        return bmp2texture(argv[1], argv[2], argv[3][0] == 'm', argv[4][0] == 'w', argv[5][0] == 'f');

    printf("Exactly 2 file paths need to be provided: A '.bmp' file (input) then a '.text' file (output)");
    return 1;
}