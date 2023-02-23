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
#include <cmath>

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
        bool *motion_mask_pointer;
    // file handle
        FILE *fin;
        FILE *fout;
        FILE *flog;


    // subfuncition
        // main
        void pixelwise_denoising();

        // motion estimation
        void coarse_motion_estimation();
        void coarse_motion_estimation_full_search(int curr_xpos, int curr_ypos);
        int sad_calculator(int min_sad, int block_size_y, int block_size_x, int sx, int sy, int cx, int cy);
        // sad 
        void res_detection();
        double res_calculator(int i, int j);
        // optical flow 
        void lucas_kanada_flow();
        // bilateral filter
        PIXEL_FMT bilateral_filter(int i, int j);
        PIXEL_FMT gaussian_filter(int i, int j);
        PIXEL_FMT temporal_filter(int i, int j);
        // motion detection
        void motion_detection();
        // utils;
        bool check_border(int x, int y, int half);

        double gaussian(float x, double sigma);
        float distance(int x, int y, int i, int j);
        
    public:
    // parameter
        double res_thre = 1.1;
        int mv_thre = 0;
        double nr_spatialS = 1;
        double nr_spatialI = 15; // greater less effect
        float nr_temporal = 0.3;
    // function
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
        bool get_motion(int i, int j);
        void set_y_val(int i, int j, PIXEL_FMT val);
        void set_u_val(int i, int j, PIXEL_FMT val);
        void set_v_val(int i, int j, PIXEL_FMT val);
        void set_motion(int i, int j,bool val);

};