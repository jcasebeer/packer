#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "stb_image.h"
#include "stb_image_write.h"

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

    stbi_set_flip_vertically_on_load(1);
    out.data = (pixel_t *)stbi_load(file, &out.width, &out.height, &out.chan, 4);     
    out.count = out.width * out.height;
    return out;
}

int main(int argc, char **argv)
{

    image_t albedo = load_image("albedom.png");
    image_t normal = load_image("normal.png");
    image_t roughness = load_image("roughness.png");
    image_t metallic = load_image("metallic.png");

    image_t images[] = {albedo, normal, roughness, metallic};

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
        pixel_t *o = out + i;

        o->r = (u8)(powf(a.r/255.f, 2.2f)*255);
        o->g = (u8)(powf(a.g/255.f, 2.2f)*255);
        o->b = (u8)(powf(a.b/255.f, 2.2f)*255);
        o->a = m.r;

        o = out2 + i;

        o->r = n.r;
        o->g = n.g;
        o->b = n.b;
        o->a = r.r;
    }
    
    stbi_write_png("albedo.png", width, height, 4, out, 0);
    stbi_write_png("normrough.png", width, height, 4, out2, 0);
    
    return 0;
}
