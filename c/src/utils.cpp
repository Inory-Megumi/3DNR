/*
 * @Filename:
 * @Author: Kehongbo
 * @Created:
 * @Description:   
 */
#include "utils.h"
// Function
// Init Class and allocate space
NrClass::NrClass(int w, int h, int fmt)
{
    this->width = w;
    this->height = h;
    this->format = fmt;
    this->resolution = w * h;
    // allocate space for frame buffer
    switch (fmt)
    {
    case v420:
        this->size = w * h * 3 / 2;
        break;
    case v422:
        this->size = w * h * 2;
        break;
    case v444:
        this->size = w * h * 3;
        break;

    default:
        assert(0);
        break;
    }
    this->ref_pointer = (PIXEL_FMT *)malloc(this->size * sizeof(PIXEL_FMT));
    this->cur_pointer = (PIXEL_FMT *)malloc(this->size * sizeof(PIXEL_FMT));
    this->tmp_pointer = (PIXEL_FMT *)malloc(this->size * sizeof(PIXEL_FMT));
}

// Function
// Read YUV FILES
void NrClass::specify_yuv_file(FILE *fin, FILE *fout)
{
    
    this->fin = fin;
    this->fout = fout;
}



// Function
// Get Pixel Value
// YUV 

PIXEL_FMT NrClass::get_pixel(int offset, int id)
{
    assert(offset < this->size);
    PIXEL_FMT *frame_pointer;
    switch (id)
    {
    case ref:
        frame_pointer = this->ref_pointer;
        break;
    case cur:
        frame_pointer = this->cur_pointer;
        break;
    default:
        break;
    }
    return *(frame_pointer + offset);
}

PIXEL_FMT NrClass::get_cur_y_val(int i, int j)
{
    int offset = i * this->width + j;
    return get_pixel(offset, cur);
}
PIXEL_FMT NrClass::get_cur_u_val(int i, int j)
{
    int offset = 0;
    switch (this->format)
    {
    case v420:
        offset = i / 2 * this->width / 2 + j / 2 + this->resolution;
        break;

    default:
        assert(0);
        break;
    }
    return get_pixel(offset, cur);
}
PIXEL_FMT NrClass::get_cur_v_val(int i, int j)
{
    int offset = 0;
    switch (this->format)
    {
    case v420:
        offset = i / 2 * this->width / 2 + j / 2 + this->resolution * 5 / 4;
        break;

    default:
        assert(0);
        break;
    }
    return get_pixel(offset, cur);
}
PIXEL_FMT NrClass::get_ref_y_val(int i, int j)
{
    int offset = i * this->width + j;
    return get_pixel(offset, ref);
}
PIXEL_FMT NrClass::get_ref_u_val(int i, int j)
{
    int offset = 0;
    switch (this->format)
    {
    case v420:
        offset = i / 2 * this->width / 2 + j / 2 + this->resolution;
        break;
    default:
        assert(0);
        break;
    }
    return get_pixel(offset, ref);
}
PIXEL_FMT NrClass::get_ref_v_val(int i, int j)
{
    int offset = 0;
    switch (this->format)
    {
    case v420:
        offset = i / 2 * this->width / 2 + j / 2 + this->resolution * 5 / 4;
        break;

    default:
        assert(0);
        break;
    }
    return get_pixel(offset, ref);
}

// Function
// Process Image
void NrClass::init_buffer()
{
    memset(this->cur_pointer,0,this->size);
    fread(this->ref_pointer, sizeof(PIXEL_FMT), this->size, this->fin);
    fwrite(this->ref_pointer, sizeof(PIXEL_FMT), this->size, this->fout);
}

void NrClass::process(int n){
    int i = 0;
    while (!feof(this->fin) ||(i < n))
    {   
        if(i == 0){
            printf("Process NO.%d Frame\n",i);
            init_buffer();
            i++;
        }
        else{
        printf("Process NO.%d Frame\n",i);
        process_once();
        i++;}
    } 
}

void NrClass::process_once(){
    // Read Current Frame
    fread(this->cur_pointer, sizeof(PIXEL_FMT), this->size, this->fin);
    // Processing
    // TODO:
    for(int i = 0; i < this->size; i++)
        this->tmp_pointer[i] = (this->cur_pointer[i] + this->ref_pointer[i])/2;
    
    // Swap Pointer
    PIXEL_FMT *tmp = this->ref_pointer;
    this->ref_pointer = this->tmp_pointer;
    this->tmp_pointer = tmp;
    // Write Reference Frame
    fwrite(this->ref_pointer, sizeof(PIXEL_FMT), this->size, this->fout);
}