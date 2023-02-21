/*
 * @Filename:  utils.h 
 * @Author: Kehongbo
 * @Created:         
 */
#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cstring>
#include "libauxiliar.h"

#define PIXEL_FMT unsigned char
enum VIDEO_FMT {v420,v422,v444} ;
enum FRAME_ID {REF,CUR};
class NrClass {
    private:
    // video_info
        int width;
        int height;
        int format;
        int resolution;
        int size;
        PIXEL_FMT *ref_pointer;
        PIXEL_FMT *cur_pointer;
        PIXEL_FMT *tmp_pointer;//store intermidiate frame
        PIXEL_FMT get_pixel(int offset,int id);
    // file handle
        FILE *fin;
        FILE *fout;
        FILE *flog;


    // subfuncition

        void coarse_motion_estimation();
        void coarse_motion_estimation_full_search(int curr_xpos, int curr_ypos);
        int sad_calculator(int min_sad, int block_size_y, int block_size_x, int sx, int sy, int cx, int cy);
    public:
        NrClass(int w,int h,int fmt);

        void specify_file(FILE *fin,FILE *fout,FILE *flog);
        void init_buffer();
        void add_noise(float sigma, int n);

        void process(int n);
        void process_once();
        void recursive_denoising();


        //get value
        PIXEL_FMT get_cur_y_val(int i ,int j);
        PIXEL_FMT get_cur_u_val(int i ,int j);
        PIXEL_FMT get_cur_v_val(int i ,int j);
        PIXEL_FMT get_ref_y_val(int i ,int j);
        PIXEL_FMT get_ref_u_val(int i ,int j);
        PIXEL_FMT get_ref_v_val(int i ,int j);

};