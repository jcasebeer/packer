#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize2.h"

#define LEN(a) (sizeof((a))/sizeof(*(a)))

typedef uint8_t u8;
typedef int32_t i32;

typedef struct {
    u8 r, g, b, a;    
}pixel_t;

typedef struct {
    pixel_t  *data;
    i32 count;
    i32 width;
    i32 height;
    i32 chan;
}image_t;

image_t load_image(char *file)
{
    image_t out;
    i32 pack_size = 32;

    stbi_set_flip_vertically_on_load(1);
    char *data = stbi_load(file, &out.width, &out.height, &out.chan, 4);     
 
    if (out.data == NULL) {
        fprintf(stderr, "Failed to load file %s!\n", file);
        exit(1);
    }

    out.data = (pixel_t *)stbir_resize_uint8_linear( data,  out.width,  out.height, 0, 
                                0, pack_size, pack_size, 0,
                                STBIR_RGBA);
    out.width = pack_size;
    out.height = pack_size;
    out.count = pack_size*pack_size;

    stbi_image_free(data);

    return out;
}

int main(int argc, char **argv)
{
    char *name = argv[1];
    fprintf(stderr, "Packing %s %s %s %s %s\n", argv[2],argv[3], argv[4], argv[5], argv[6]);

    image_t albedo = load_image(argv[2]);
    image_t normal = load_image(argv[3]);
    image_t roughness = load_image(argv[4]);
    image_t metallic = load_image(argv[5]);
    image_t heightmap = load_image(argv[6]);

    image_t images[] = {albedo, normal, roughness, metallic, heightmap};

    i32 pixels = images[0].count;
    i32 width = images[0].width;
    i32 height = images[0].height;
    pixel_t *out = malloc(pixels * sizeof(*out));
    pixel_t *out2 = malloc(pixels * sizeof(*out2));
    
    for(i32 i = 0; i<pixels; i++) {
        pixel_t a = albedo.data[i % albedo.count]; 
        pixel_t n = normal.data[i % normal.count]; 
        pixel_t r = roughness.data[i % roughness.count]; 
        pixel_t m = metallic.data[i % metallic.count]; 
        pixel_t h = heightmap.data[i % heightmap.count]; 
        pixel_t *o = out + i;

        o->r = (u8)(powf(a.r/255.f, 2.2f)*255);
        o->g = (u8)(powf(a.g/255.f, 2.2f)*255);
        o->b = (u8)(powf(a.b/255.f, 2.2f)*255);
        o->a = m.r;

        o = out2 + i;

        float nx = (n.r/255.f)*2.0-1.0;
        float ny = (n.g/255.f)*2.0-1.0;
        float nz = (n.b/255.f)*2.0-1.0;

        float nm = sqrtf(nx*nx + ny*ny + nz*nz);
        if (nm!=0) {
            nx/=nm;
            ny/=nm;
            nz/=nm;
        }

        o->r = (nx + 1.f)*0.5f*255.f;
        o->g = (ny + 1.f)*0.5f*255.f;
        o->b = r.r;
        o->a = h.r;
    }

    char output_name[256] = {0};
    sprintf(output_name, "%s_am.png", name);

    char output_name2[256] = {0};
    sprintf(output_name2, "%s_nrh.png", name);
    
    stbi_write_png(output_name, width, height, 4, out, 0);
    stbi_write_png(output_name2, width, height, 4, out2, 0);
    
    return 0;
}
