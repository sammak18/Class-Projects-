/* tab:4
 *
 * photo.c - photo display functions
 *
 * "Copyright (c) 2011 by Steven S. Lumetta."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
 * EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:        Steve Lumetta
 * Version:       3
 * Creation Date: Fri Sep 9 21:44:10 2011
 * Filename:      photo.c
 * History:
 *    SL    1    Fri Sep 9 21:44:10 2011
 *        First written(based on mazegame code).
 *    SL    2    Sun Sep 11 14:57:59 2011
 *        Completed initial implementation of functions.
 *    SL    3    Wed Sep 14 21:49:44 2011
 *        Cleaned up code for distribution.
 */


#include <string.h>

#include "assert.h"
#include "modex.h"
#include "photo.h"
#include "photo_headers.h"
#include "world.h"


/* types local to this file(declared in types.h) */

/*
 * A room photo.  Note that you must write the code that selects the
 * optimized palette colors and fills in the pixel data using them as
 * well as the code that sets up the VGA to make use of these colors.
 * Pixel data are stored as one-byte values starting from the upper
 * left and traversing the top row before returning to the left of
 * the second row, and so forth.  No padding should be used.
 */
struct photo_t {
    photo_header_t hdr;            /* defines height and width */
    uint8_t        palette[192][3];     /* optimized palette colors */
    uint8_t*       img;                 /* pixel data               */
};

/*
 * An object image.  The code for managing these images has been given
 * to you.  The data are simply loaded from a file, where they have
 * been stored as 2:2:2-bit RGB values(one byte each), including
 * transparent pixels(value OBJ_CLR_TRANSP).  As with the room photos,
 * pixel data are stored as one-byte values starting from the upper
 * left and traversing the top row before returning to the left of the
 * second row, and so forth.  No padding is used.
 */
struct image_t {
    photo_header_t hdr;  /* defines height and width */
    uint8_t*       img;  /* pixel data               */
};

typedef struct node{
    unsigned int r_sum;
    unsigned int g_sum;
    unsigned int b_sum;
    int count;
    int identity;
}octree_node;

#define OCTREE_4_LENGTH 4096;
#define OCTREE_2_LENGTH 64;


int octree_compare(const void * a, const void * b);
int findclosestmatch(uint16_t pixel, octree_node* level4octree, octree_node* level2octree);

#define REP_OUTSB(port, source, count)                  \
do {                                                    \
    asm volatile("                                    \n\
        1: movb 0(%1), %%al                           \n\
        outb %%al, (%w2)                              \n\
        incl %1                                       \n\
        decl %0                                       \n\
        jne 1b                                        \n\
        "                                               \
        : /* no outputs */                              \
        : "c"((count)), "S"((source)), "d"((port))      \
        : "eax", "memory", "cc"                         \
    );                                                  \
} while (0)

/* macro used to write a byte to a port */
#define OUTB(port, val)                                 \
do {                                                    \
    asm volatile("                                    \n\
        outb %b1, (%w0)                               \n\
        "                                               \
        : /* no outputs */                              \
        : "d"((port)), "a"((val))                       \
        : "memory", "cc"                                \
    );                                                  \
} while (0)


/*
 * The room currently shown on the screen.  This value is not known to
 * the mode X code, but is needed when filling buffers in callbacks from
 * that code(fill_horiz_buffer/fill_vert_buffer).  The value is set
 * by calling prep_room.
 */
static const room_t* cur_room = NULL;


/*
 * fill_horiz_buffer
 *   DESCRIPTION: Given the(x,y) map pixel coordinate of the leftmost
 *                pixel of a line to be drawn on the screen, this routine
 *                produces an image of the line.  Each pixel on the line
 *                is represented as a single byte in the image.
 *
 *                Note that this routine draws both the room photo and
 *                the objects in the room.
 *
 *   INPUTS:(x,y) -- leftmost pixel of line to be drawn
 *   OUTPUTS: buf -- buffer holding image data for the line
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void fill_horiz_buffer(int x, int y, unsigned char buf[SCROLL_X_DIM]) {
    int            idx;   /* loop index over pixels in the line          */
    object_t*      obj;   /* loop index over objects in the current room */
    int            imgx;  /* loop index over pixels in object image      */
    int            yoff;  /* y offset into object image                  */
    uint8_t        pixel; /* pixel from object image                     */
    const photo_t* view;  /* room photo                                  */
    int32_t        obj_x; /* object x position                           */
    int32_t        obj_y; /* object y position                           */
    const image_t* img;   /* object image                                */

    /* Get pointer to current photo of current room. */
    view = room_photo(cur_room);

    /* Loop over pixels in line. */
    for (idx = 0; idx < SCROLL_X_DIM; idx++) {
        buf[idx] = (0 <= x + idx && view->hdr.width > x + idx ? view->img[view->hdr.width * y + x + idx] : 0);
    }

    /* Loop over objects in the current room. */
    for (obj = room_contents_iterate(cur_room); NULL != obj; obj = obj_next(obj)) {
        obj_x = obj_get_x(obj);
        obj_y = obj_get_y(obj);
        img = obj_image(obj);

        /* Is object outside of the line we're drawing? */
        if (y < obj_y || y >= obj_y + img->hdr.height || x + SCROLL_X_DIM <= obj_x || x >= obj_x + img->hdr.width) {
            continue;
        }

        /* The y offset of drawing is fixed. */
        yoff = (y - obj_y) * img->hdr.width;

        /*
         * The x offsets depend on whether the object starts to the left
         * or to the right of the starting point for the line being drawn.
         */
        if (x <= obj_x) {
            idx = obj_x - x;
            imgx = 0;
        }
        else {
            idx = 0;
            imgx = x - obj_x;
        }

        /* Copy the object's pixel data. */
        for (; SCROLL_X_DIM > idx && img->hdr.width > imgx; idx++, imgx++) {
            pixel = img->img[yoff + imgx];

            /* Don't copy transparent pixels. */
            if (OBJ_CLR_TRANSP != pixel) {
                buf[idx] = pixel;
            }
        }
    }
}


/*
 * fill_vert_buffer
 *   DESCRIPTION: Given the(x,y) map pixel coordinate of the top pixel of
 *                a vertical line to be drawn on the screen, this routine
 *                produces an image of the line.  Each pixel on the line
 *                is represented as a single byte in the image.
 *
 *                Note that this routine draws both the room photo and
 *                the objects in the room.
 *
 *   INPUTS:(x,y) -- top pixel of line to be drawn
 *   OUTPUTS: buf -- buffer holding image data for the line
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void fill_vert_buffer(int x, int y, unsigned char buf[SCROLL_Y_DIM]) {
    int            idx;   /* loop index over pixels in the line          */
    object_t*      obj;   /* loop index over objects in the current room */
    int            imgy;  /* loop index over pixels in object image      */
    int            xoff;  /* x offset into object image                  */
    uint8_t        pixel; /* pixel from object image                     */
    const photo_t* view;  /* room photo                                  */
    int32_t        obj_x; /* object x position                           */
    int32_t        obj_y; /* object y position                           */
    const image_t* img;   /* object image                                */

    /* Get pointer to current photo of current room. */
    view = room_photo(cur_room);

    /* Loop over pixels in line. */
    for (idx = 0; idx < SCROLL_Y_DIM; idx++) {
        buf[idx] = (0 <= y + idx && view->hdr.height > y + idx ? view->img[view->hdr.width *(y + idx) + x] : 0);
    }

    /* Loop over objects in the current room. */
    for (obj = room_contents_iterate(cur_room); NULL != obj; obj = obj_next(obj)) {
        obj_x = obj_get_x(obj);
        obj_y = obj_get_y(obj);
        img = obj_image(obj);

        /* Is object outside of the line we're drawing? */
        if (x < obj_x || x >= obj_x + img->hdr.width ||
            y + SCROLL_Y_DIM <= obj_y || y >= obj_y + img->hdr.height) {
            continue;
        }

        /* The x offset of drawing is fixed. */
        xoff = x - obj_x;

        /*
         * The y offsets depend on whether the object starts below or
         * above the starting point for the line being drawn.
         */
        if (y <= obj_y) {
            idx = obj_y - y;
            imgy = 0;
        }
        else {
            idx = 0;
            imgy = y - obj_y;
        }

        /* Copy the object's pixel data. */
        for (; SCROLL_Y_DIM > idx && img->hdr.height > imgy; idx++, imgy++) {
            pixel = img->img[xoff + img->hdr.width * imgy];

            /* Don't copy transparent pixels. */
            if (OBJ_CLR_TRANSP != pixel) {
                buf[idx] = pixel;
            }
        }
    }
}


/*
 * image_height
 *   DESCRIPTION: Get height of object image in pixels.
 *   INPUTS: im -- object image pointer
 *   OUTPUTS: none
 *   RETURN VALUE: height of object image im in pixels
 *   SIDE EFFECTS: none
 */
uint32_t image_height(const image_t* im) {
    return im->hdr.height;
}


/*
 * image_width
 *   DESCRIPTION: Get width of object image in pixels.
 *   INPUTS: im -- object image pointer
 *   OUTPUTS: none
 *   RETURN VALUE: width of object image im in pixels
 *   SIDE EFFECTS: none
 */
uint32_t image_width(const image_t* im) {
    return im->hdr.width;
}

/*
 * photo_height
 *   DESCRIPTION: Get height of room photo in pixels.
 *   INPUTS: p -- room photo pointer
 *   OUTPUTS: none
 *   RETURN VALUE: height of room photo p in pixels
 *   SIDE EFFECTS: none
 */
uint32_t photo_height(const photo_t* p) {
    return p->hdr.height;
}


/*
 * photo_width
 *   DESCRIPTION: Get width of room photo in pixels.
 *   INPUTS: p -- room photo pointer
 *   OUTPUTS: none
 *   RETURN VALUE: width of room photo p in pixels
 *   SIDE EFFECTS: none
 */
uint32_t photo_width(const photo_t* p) {
    return p->hdr.width;
}


/*
 * prep_room
 *   DESCRIPTION: Prepare a new room for display.  You might want to set
 *                up the VGA palette registers according to the color
 *                palette that you chose for this room.
 *   INPUTS: r -- pointer to the new room
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes recorded cur_room for this file
 */
void prep_room(const room_t* r) {
    /* Record the current room. */
    cur_room = r;
    fill_palette(room_photo(cur_room)->palette);
}


/*
 * read_obj_image
 *   DESCRIPTION: Read size and pixel data in 2:2:2 RGB format from a
 *                photo file and create an image structure from it.
 *   INPUTS: fname -- file name for input
 *   OUTPUTS: none
 *   RETURN VALUE: pointer to newly allocated photo on success, or NULL
 *                 on failure
 *   SIDE EFFECTS: dynamically allocates memory for the image
 */
image_t* read_obj_image(const char* fname) {
    FILE*    in;        /* input file               */
    image_t* img = NULL;    /* image structure          */
    uint16_t x;            /* index over image columns */
    uint16_t y;            /* index over image rows    */
    uint8_t  pixel;        /* one pixel from the file  */

    /*
     * Open the file, allocate the structure, read the header, do some
     * sanity checks on it, and allocate space to hold the image pixels.
     * If anything fails, clean up as necessary and return NULL.
     */
    if (NULL == (in = fopen(fname, "r+b")) ||
        NULL == (img = malloc(sizeof (*img))) ||
        NULL != (img->img = NULL) || /* false clause for initialization */
        1 != fread(&img->hdr, sizeof (img->hdr), 1, in) ||
        MAX_OBJECT_WIDTH < img->hdr.width ||
        MAX_OBJECT_HEIGHT < img->hdr.height ||
        NULL == (img->img = malloc
        (img->hdr.width * img->hdr.height * sizeof (img->img[0])))) {
        if (NULL != img) {
            if (NULL != img->img) {
                free(img->img);
            }
            free(img);
        }
        if (NULL != in) {
            (void)fclose(in);
        }
        return NULL;
    }

    /*
     * Loop over rows from bottom to top.  Note that the file is stored
     * in this order, whereas in memory we store the data in the reverse
     * order(top to bottom).
     */
    for (y = img->hdr.height; y-- > 0; ) {

        /* Loop over columns from left to right. */
        for (x = 0; img->hdr.width > x; x++) {

            /*
             * Try to read one 8-bit pixel.  On failure, clean up and
             * return NULL.
             */
            if (1 != fread(&pixel, sizeof (pixel), 1, in)) {
                free(img->img);
                free(img);
                (void)fclose(in);
                return NULL;
            }

            /* Store the pixel in the image data. */
            img->img[img->hdr.width * y + x] = pixel;
        }
    }

    /* All done.  Return success. */
    (void)fclose(in);
    return img;
}


/*
 * read_photo
 *   DESCRIPTION: Read size and pixel data in 5:6:5 RGB format from a
 *                photo file and create a photo structure from it.
 *                Code provided simply maps to 2:2:2 RGB.  You must
 *                replace this code with palette color selection, and
 *                must map the image pixels into the palette colors that
 *                you have defined.
 *   INPUTS: fname -- file name for input
 *   OUTPUTS: none
 *   RETURN VALUE: pointer to newly allocated photo on success, or NULL
 *                 on failure
 *   SIDE EFFECTS: dynamically allocates memory for the photo
 */
photo_t* read_photo(const char* fname) {
    FILE*    in;    /* input file               */
    photo_t* p = NULL;    /* photo structure          */
    uint16_t x;        /* index over image columns */
    uint16_t y;        /* index over image rows    */
    uint16_t pixel;    /* one pixel from the file  */

        
    octree_node level2octree[64];
    octree_node level4octree[4096];
    /* file-scope variables */
    int i;
    for(i=0;i<4096;i++){
        level4octree[i].r_sum = 0;
        level4octree[i].g_sum = 0;
        level4octree[i].b_sum = 0;
        level4octree[i].count = 0;
        level4octree[i].identity = i;
    }

    for(i=0;i<64;i++){
        level2octree[i].r_sum = 0;
        level2octree[i].g_sum = 0;
        level2octree[i].b_sum = 0;
        level2octree[i].count = 0;
        level2octree[i].identity = i;

    }



    /*
     * Open the file, allocate the structure, read the header, do some
     * sanity checks on it, and allocate space to hold the photo pixels.
     * If anything fails, clean up as necessary and return NULL.
     */
    if (NULL == (in = fopen(fname, "r+b")) ||
        NULL == (p = malloc(sizeof (*p))) ||
        NULL != (p->img = NULL) || /* false clause for initialization */
        1 != fread(&p->hdr, sizeof (p->hdr), 1, in) ||
        MAX_PHOTO_WIDTH < p->hdr.width ||
        MAX_PHOTO_HEIGHT < p->hdr.height ||
        NULL == (p->img = malloc
        (p->hdr.width * p->hdr.height * sizeof (p->img[0])))) {
        if (NULL != p) {
            if (NULL != p->img) {
                free(p->img);
            }
            free(p);
        }
        if (NULL != in) {
            (void)fclose(in);
        }
        return NULL;
    }

    /*
     * Loop over rows from bottom to top.  Note that the file is stored
     * in this order, whereas in memory we store the data in the reverse
     * order(top to bottom).
     */
    for (y = p->hdr.height; y-- > 0; ) {

        /* Loop over columns from left to right. */
        for (x = 0; p->hdr.width > x; x++) {

            /*
             * Try to read one 16-bit pixel.  On failure, clean up and
             * return NULL.
             */
            if (1 != fread(&pixel, sizeof (pixel), 1, in)) {
                free(p->img);
                free(p);
                (void)fclose(in);
                return NULL;
            }
            /*
             * 16-bit pixel is coded as 5:6:5 RGB(5 bits red, 6 bits green,
             * and 6 bits blue).  We change to 2:2:2, which we've set for the
             * game objects.  You need to use the other 192 palette colors
             * to specialize the appearance of each photo.
             *
             * In this code, you need to calculate the p->palette values,
             * which encode 6-bit RGB as arrays of three uint8_t's.  When
             * the game puts up a photo, you should then change the palette
             * to match the colors needed for that photo.
             */
            uint32_t red_four;
            uint32_t green_four;
            uint32_t blue_four;
            uint32_t power_ranger;  // variable to combine RGB
            
            // pixel contains 16 bits in the form RRRR RGGG GGGB BBBB
            // mask = 0000 0000 0001 1110
            blue_four = pixel & 0x001e;
            // mask = 0000 0111 1000 0000
            green_four = pixel & 0x0780;
            // mask = 1111 0000 0000 0000 
            red_four = pixel & 0xf000;

            // 0000 0000 000B BBB0 = 0000 0000 0000 BBBB
            blue_four = blue_four>>1;
            // 0000 0GGG G000 0000 = 0000 0000 GGGG 0000
            green_four = green_four>>3;
            // RRRR 0000 0000 0000 = 0000 RRRR 0000 0000
            red_four = red_four>>4;

            power_ranger = power_ranger & 0x0;
            power_ranger = power_ranger | green_four;
            power_ranger = power_ranger | red_four;
            power_ranger = power_ranger | blue_four;
            // power_ranger  = xxxx RRRR GGGG BBBB
            
            //power_ranger = (int) power_ranger;

            uint32_t red_five = 0xf800;
            uint32_t green_six = 0x07e0;
            uint32_t blue_five = 0x001f;


            // red mask = 1111 1000 0000 0000 
            // shift by 11 to get 0000 0000 000R RRRR
            level4octree[(int)power_ranger].r_sum += (unsigned int)((pixel & red_five)>>11);          
            // green mask = 0000 0111 1110 0000 
            // shift by 5 to get 0000 0000 00GG GGGG
            level4octree[(int)power_ranger].g_sum += (unsigned int)((pixel & green_six)>>5);
            // blue mask = 0000 0000 0001 1111  
            // we have 0000 0000 000B BBBB
            level4octree[(int)power_ranger].b_sum += (unsigned int)((pixel & blue_five));
            level4octree[(int)power_ranger].count += 1;


            // for two level octree
            uint32_t red_two;
            uint32_t green_two;
            uint32_t blue_two;
            uint32_t combined_six;

            //pixel looks like RRRR RGGG GGGB BBBB

            // bit mask: 0000 0000 0001 1000
            blue_two = pixel & 0x0018;

            // bit mask: 0000 0110 0000 0000
            green_two = pixel & 0x0600;

            // bit mask: 1100 0000 0000 0000
            red_two = pixel & 0xc000;

            blue_two = blue_two>>3;
            green_two = green_two>>7;
            red_two = red_two>>10;

            // combining to form six bits
            combined_six = combined_six & 0x0;
            combined_six = combined_six | green_two;
            combined_six = combined_six | red_two;
            combined_six = combined_six | blue_two;

            combined_six = (int) combined_six;

            level2octree[(int) combined_six].r_sum += (unsigned int)((pixel & red_five)>>11);          
            level2octree[(int) combined_six].g_sum += (unsigned int)((pixel & green_six)>>5);
            level2octree[(int) combined_six].b_sum += (unsigned int)((pixel & blue_five));
            level2octree[(int) combined_six].count += 1;
            





            //p->img[p->hdr.width * y + x] = (((pixel >> 14) << 4) | (((pixel >> 9) & 0x3) << 2) | ((pixel >> 3) & 0x3));
        }
    }

    // sort the array
    qsort (level4octree, 4096, sizeof(octree_node), &octree_compare);

    // put level twooctree in palette
    for(i=0;i<64;i++){
        // check for division by 0 error
        if(level2octree[i].count == 0){
            p->palette[i][0]= 0;
            p->palette[i][1]= 0;
            p->palette[i][2]= 0;
            continue;
        }
        p->palette[i][0]= ((uint8_t) (level2octree[i].r_sum/level2octree[i].count))<<1;
        p->palette[i][1]= ((uint8_t) (level2octree[i].g_sum/level2octree[i].count));
        p->palette[i][2]= ((uint8_t) (level2octree[i].b_sum/level2octree[i].count))<<1;
    }
    // put 128 best level4octrees in palette
    for(i=0;i<128;i++){
        if(level4octree[i].count == 0){
            p->palette[i][0]= 0;
            p->palette[i][1]= 0;
            p->palette[i][2]= 0;
            continue;
        }
        p->palette[i+64][0]= ((uint8_t) (level4octree[i].r_sum/level4octree[i].count))<<1;
        p->palette[i+64][1]= ((uint8_t) (level4octree[i].g_sum/level4octree[i].count));
        p->palette[i+64][2]= ((uint8_t) (level4octree[i].b_sum/level4octree[i].count))<<1;
    }

    fseek(in,0,SEEK_SET);

    for (y = p->hdr.height; y-- > 0; ) {
    /* Loop over columns from left to right. */
        for (x = 0; p->hdr.width > x; x++) {

        /*
            * Try to read one 16-bit pixel.  On failure, clean up and
            * return NULL.
            */
        if (1 != fread(&pixel, sizeof (pixel), 1, in)) {
            free(p->img);
            free(p);
            (void)fclose(in);
            return NULL;
        }
        
        uint32_t red_four;
        uint32_t green_four;
        uint32_t blue_four;
        uint32_t power_ranger;
        
        // pixel contains 16 bits in the form RRRR RGGG GGGB BBBB
        // mask = 0000 0000 0001 1110
        blue_four = pixel & 0x001e;
        // mask = 0000 0111 1000 0000
        green_four = pixel & 0x0780;
        // mask = 1111 0000 0000 0000 
        red_four = pixel & 0xf000;

        // 0000 0000 000B BBB0 = 0000 0000 0000 BBBB
        blue_four = blue_four>>1;
        // 0000 0GGG G000 0000 = 0000 0000 GGGG 0000
        green_four = green_four>>3;
        // RRRR 0000 0000 0000 = 0000 RRRR 0000 0000
        red_four = red_four>>4;

        power_ranger = power_ranger & 0x0;
        power_ranger = power_ranger | green_four;
        power_ranger = power_ranger | red_four;
        power_ranger = power_ranger | blue_four;
        // bit is now 0000 RRRR GGGG BBBB


        p->img[p->hdr.width * y + x] =  findclosestmatch(power_ranger,level4octree,level2octree) + 64;
        }
    }






    /* All done.  Return success. */
    (void)fclose(in);
    return p;
}

/*
 * octree compare
 *   DESCRIPTION: compares two octrees on the basis of their count and places the higher octree first
 *   INPUTS: a,b the two octree nodes
 *   OUTPUTS: return a +ve or -ve number depending on what is greater
 *   RETURN VALUE: int number 
 */

int octree_compare(const void * a, const void * b){
    // return -ve number if a's count is more
    if(((octree_node*)a)->count > ((octree_node*)b)->count){
        return -1;
    }
    // return +ve number if a's count is less
    if(((octree_node*)a)->count < ((octree_node*)b)->count){
        return 1;
    }
    // if equal return 0
    return 0;
}

/*
 * findclosestmatch
 *   DESCRIPTION: looks for the colour in the pallete
 *   INPUTS: pixel, level4 octree array, level 2 octree array
 *   OUTPUTS: returns the index of the colour in the pallete
 *   RETURN VALUE: int index 
 */
int findclosestmatch(uint16_t pixel, octree_node* level4octree, octree_node* level2octree){
    //loop variables
    int i;
    // search in level 4
    for(i=0;i<128;i++){
        if (level4octree[i].identity == pixel){
            return i+ 64;
        }
    }
    uint16_t r,g,b;
    // modify pixel to become 6 bits
    // pixel looks like 0000 RRRR GGGG BBBB
    r = pixel & 0x0c00;
    g = pixel & 0x00c0;
    b = pixel & 0x000c;
    r = r>>6;
    g = g>>4;
    b = b>>2;
    pixel = pixel & 0x0;
    pixel = pixel | r;
    pixel = pixel | g;
    pixel = pixel | b;


    // look in level 2 for colour
    for(i=0;i<64;i++){
        if (level2octree[i].identity == pixel){
            return i;
        }
    }

    //error
    return 64;

}
