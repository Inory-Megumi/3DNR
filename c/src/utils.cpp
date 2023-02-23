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
    this->motion_mask_pointer = (bool *)malloc(this->resolution * sizeof(bool));
}

// Function
// Read YUV FILES
void NrClass::specify_file(FILE *fin, FILE *fout, FILE *flog)
{
    
    this->fin = fin;
    this->fout = fout;
    this->flog = flog;
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
    case REF:
        frame_pointer = this->ref_pointer;
        break;
    case CUR:
        frame_pointer = this->cur_pointer;
        break;
    default:
        assert(0);
        break;
    }
    return *(frame_pointer + offset);
}

PIXEL_FMT NrClass::get_cur_y_val(int i, int j)
{
    int offset = i * this->width + j;
    return get_pixel(offset, CUR);
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
    return get_pixel(offset, CUR);
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
    return get_pixel(offset, CUR);
}
PIXEL_FMT NrClass::get_ref_y_val(int i, int j)
{
    int offset = i * this->width + j;
    return get_pixel(offset, REF);
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
    return get_pixel(offset, REF);
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
    return get_pixel(offset, REF);
}


void NrClass::set_y_val(int i, int j, PIXEL_FMT val)
{
    int offset = i * this->width + j;
    *(this->tmp_pointer + offset) = val;
}
void NrClass::set_u_val(int i, int j, PIXEL_FMT val)
{
    int offset = 0;
    switch (this->format)
    {
    case v420:
        offset = i / 2 * this->width / 2 + j / 2 + this->resolution;
        *(this->tmp_pointer + offset) = val;
        break;

    default:
        assert(0);
        break;
    }
}
void NrClass::set_v_val(int i, int j, PIXEL_FMT val)
{
    int offset = 0;
    switch (this->format)
    {
    case v420:
        offset = i / 2 * this->width / 2 + j / 2 + this->resolution * 5 / 4;
        *(this->tmp_pointer + offset) = val;
        break;

    default:
        assert(0);
        break;
    }
}

bool NrClass::get_motion(int i, int j)
{
    int offset = i * this->width + j;
    return *(this->motion_mask_pointer + offset);
}

void NrClass::set_motion(int i, int j, bool val)
{
    int offset = i * this->width + j;
    *(this->motion_mask_pointer + offset) = val;
}

// Function
// Process Image
void NrClass::init_buffer()
{
    memset(this->cur_pointer,0,this->size);
    memset(this->motion_mask_pointer,0,this->resolution);
    fread(this->ref_pointer, sizeof(PIXEL_FMT), this->size, this->fin);
    fwrite(this->ref_pointer, sizeof(PIXEL_FMT), this->size, this->fout);
}


void NrClass::process(int n){
    int i = 0;
    while (!feof(this->fin) && (i < n))
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
    // TODO: Denosing Function
    recursive_denoising();    
    // Swap Pointer
    PIXEL_FMT *tmp = this->ref_pointer;
    this->ref_pointer = this->tmp_pointer;
    this->tmp_pointer = tmp;
    // Write Reference Frame
    fwrite(this->ref_pointer, sizeof(PIXEL_FMT), this->size, this->fout);    
}


void NrClass::recursive_denoising(){
    // TODO:
    // 1. Coarse Motion Estimation
    // 2. Store Coarse MV
    // 3. Line Temperal Denoising
    motion_detection();
    pixelwise_denoising();
}

void NrClass::pixelwise_denoising()
{
    for(int i = 0; i < this->height; i++){
        for(int j = 0; j < this->width; j++){
            if(get_motion(i,j))
                set_y_val(i,j,bilateral_filter(i,j));
            else
                set_y_val(i,j,temporal_filter(i,j));
            if((i%2)&&(j%2)){
                set_u_val(i,j,get_cur_u_val(i,j));
                set_v_val(i,j,get_cur_v_val(i,j));
            }
        }
    }
}



//////////////////////////////////
//
//  topic : bilateral filter && Temporal filter
//
/////////////////////////////////
PIXEL_FMT NrClass::temporal_filter(int x, int y){
    float k = this->nr_temporal;
    if((x == 67) && (y == 153))
        {   
            printf("hi!\n");
            printf("REF_Y = %d,CUR_Y= %d\n",get_ref_y_val(x,y),get_cur_y_val(x,y));}
                
    float pixel_n = k * get_cur_y_val(x,y) + (1.0 - k) * get_ref_y_val(x,y);
    // fprintf(this->flog,"pixel= %f \n",pixel_n);
    return (PIXEL_FMT) pixel_n;
}


#define BF_SZ 5
PIXEL_FMT NrClass::bilateral_filter(int x, int y)
{
    double sigmaS = this->nr_spatialS;
    double sigmaI = this->nr_spatialI;
    
    double iFiltered = 0;
    double wP = 0;
    int neighbor_x = 0;
    int neighbor_y = 0;
    int half = BF_SZ / 2;
    // DEBUG

    if(check_border(x,y,half))
        return get_cur_y_val(x,y); 
    // border_check
    for(int i = 0; i < BF_SZ; i++) {
        for(int j = 0; j < BF_SZ; j++) {
            neighbor_x = x - (half - i);
            neighbor_y = y - (half - j);
            double gi = gaussian(get_cur_y_val(neighbor_x, neighbor_y) - get_cur_y_val(x, y), sigmaI);
            double gs = gaussian(distance(x, y, neighbor_x, neighbor_y), sigmaS);
            double w = gi * gs;
            iFiltered = iFiltered + get_cur_y_val(neighbor_x,neighbor_y) * w;
            wP = wP + w;
        }
    }
    iFiltered = iFiltered / wP;
    return  (PIXEL_FMT)iFiltered;
}

PIXEL_FMT NrClass::gaussian_filter(int x, int y)
{
    double sigmaS = this->nr_spatialS;
    double sigmaI = this->nr_spatialI;
    
    double iFiltered = 0;
    double wP = 0;
    int neighbor_x = 0;
    int neighbor_y = 0;
    int half = BF_SZ / 2;
    if(check_border(x,y,half))
        return get_cur_y_val(x,y); 
    // border_check
    for(int i = 0; i < BF_SZ; i++) {
        for(int j = 0; j < BF_SZ; j++) {
            neighbor_x = x - (half - i);
            neighbor_y = y - (half - j);
            double gs = gaussian(distance(x, y, neighbor_x, neighbor_y), sigmaS);
            double w = gs;
            iFiltered = iFiltered + get_cur_y_val(neighbor_x,neighbor_y) * w;
            wP = wP + w;
        }
    }
    iFiltered = iFiltered / wP;
    return  (PIXEL_FMT)iFiltered;
}




//////////////////////////////////
//
//  topic : optical flow
//
/////////////////////////////////
void NrClass::lucas_kanada_flow()
{



}
//////////////////////////////////
//
//  topic : motion estimation
//
/////////////////////////////////
void NrClass::motion_detection(){
    res_detection();
}
#define RES_SZ 5
void NrClass::res_detection(){
    double thre = this->res_thre;
    for(int i = 0; i < this->height; i++)
        for(int j = 0; j < this->width; j++){
            if(abs(res_calculator(i,j)) > thre)
                set_motion(i,j,1);
            else
                set_motion(i,j,0);
        }
}

double NrClass::res_calculator(int x, int y){
    int half = RES_SZ / 2;
    if (check_border(x,y,half))    return 0;
    double res = 0;
    double rn = 0;
    double a_res = 0;
    int neighbor_x = 0;
    int neighbor_y = 0;
    for(int i = 0; i < BF_SZ; i++) {
        for(int j = 0; j < BF_SZ; j++) {
            neighbor_x = x - (half - i);
            neighbor_y = y - (half - j);
            res += (get_cur_y_val(neighbor_x, neighbor_y) - get_ref_y_val(neighbor_x, neighbor_y));
            rn++;
        }
    }
    a_res = res / rn;
    fprintf(this->flog,"%f\n",a_res);
    return a_res;
}

//////////////////////////////////
//
//  topic : motion estimation
//
/////////////////////////////////

// Function
// Full Search 
#define SEARCH_RANGE 16
#define BLOCK_SIZE 16
void NrClass::coarse_motion_estimation(){
    int curr_ypos, curr_xpos;
    int subblock_size_x = BLOCK_SIZE, subblock_size_y = BLOCK_SIZE;
    for (curr_ypos = 0; curr_ypos < this->height; curr_ypos += subblock_size_y)
    {
        for (curr_xpos = 0; curr_xpos < this->width; curr_xpos += subblock_size_x)
        {
            coarse_motion_estimation_full_search(curr_xpos, curr_ypos);
            // FULL_Search(subblock_size_x, subblock_size_y, curr_ypos, curr_xpos, ref_y, ref_cb, ref_cr, curr_y);
        }
    }
}


// Function
// Block Matching Full Search Motion Estimation
void NrClass::coarse_motion_estimation_full_search(int curr_xpos, int curr_ypos){
    // PARAMETER
    int subblock_size_x = BLOCK_SIZE;
    int subblock_size_y = BLOCK_SIZE;
    int ORI_W = this->width;
    int ORI_H = this->height;
    // BLOCK MATCHING FULL_SEARCH
    int min_sad_16x16, sad_16x16;
    int min_mv_x, min_mv_y;
    int v_curr_x_min, v_curr_y_min;
    int v_curr_x_max, v_curr_y_max;
    int sw_x, sw_y;
    min_sad_16x16 = 255 * subblock_size_x * subblock_size_y;
    min_mv_x = 0;
    min_mv_y = 0;
    // v_curr_x(y) 
    // v_curr_x_min x x x x x ... x x x x v_curr_x_max
    // ...............................................
    // v_curr_y_max x x x x x ... x x x x x x x x x x 
    if ((v_curr_x_min = curr_xpos - SEARCH_RANGE) <= 0)
    {
        v_curr_x_min = 0;
    }
    else{
        v_curr_x_min = curr_xpos - SEARCH_RANGE;
    }
    if ((v_curr_y_min = curr_ypos - SEARCH_RANGE) <= 0)
    {
        v_curr_y_min = 0;
    }
    else{
        v_curr_y_min = curr_ypos - SEARCH_RANGE;
    }
    if ((v_curr_x_max = curr_xpos + (SEARCH_RANGE - 1) + subblock_size_x) >= ORI_W)
    {
        v_curr_x_max = ORI_W - subblock_size_x;
    }
    else
    {
        v_curr_x_max = curr_xpos + (SEARCH_RANGE);
    }
    if ((v_curr_y_max = curr_ypos + (SEARCH_RANGE - 1) + subblock_size_y) >= ORI_H)
    {
        v_curr_y_max = ORI_H - subblock_size_y;
    }
    else
    {
        v_curr_y_max = curr_ypos + (SEARCH_RANGE);
    }
    // search begin sw -- search window block
    for (sw_y = v_curr_y_min; sw_y < v_curr_y_max; sw_y++)
    {
        for (sw_x = v_curr_x_min; sw_x < v_curr_x_max; sw_x++)
        {
            sad_16x16 = sad_calculator(min_sad_16x16, subblock_size_y, subblock_size_x, sw_x, sw_y, curr_xpos, curr_ypos);
            // fprintf(this->flog,"sad = %d, swx = %d , swy = %d\n",sad_16x16,sw_x,sw_y);
            if (sad_16x16 < min_sad_16x16)
            {
                min_sad_16x16 = sad_16x16;
                min_mv_x = sw_x - curr_xpos;
                min_mv_y = sw_y - curr_ypos;
            }
            
        }
    }
    //exit(-1);
    fprintf(this->flog,"mv_x = %d, mv_y =%d ,min_sad_16x16 = %d\n",min_mv_x,min_mv_y,min_sad_16x16);
}
#define FULL_SEARCH_EARLY_TERMINATION_EN 1
// Function : sad_calculator
// Argument :
// (x,y) : reference block leftmost and upmost pixel index

int NrClass::sad_calculator(int min_sad, int block_size_y, int block_size_x, int sx, int sy, int cx, int cy)
{
    int ref_y, ref_x, cur_y, cur_x;
    int sad = 0;
    for (ref_y = sy, cur_y = cy; ref_y - sy< block_size_y; ref_y++, cur_y++)
    {
        for (ref_x = sx, cur_x = cx; ref_x - sx < block_size_x; ref_x++, cur_x++)
        {
            sad = sad + abs(get_cur_y_val(cur_y,cur_x) - get_ref_y_val(ref_y,ref_x));
            // fast full search
            /*
            if (FULL_SEARCH_EARLY_TERMINATION_EN == 1)
            {
                if (sad > min_sad)
                {
                    return sad;
                }
            }
            */
        }
    }
    return sad;
}












// UTILS FUNCTION
// add_noise
void NrClass::add_noise(float sigma,int n){
    int cnt = 0;
    init_rand();
    while (!feof(this->fin) && (cnt < n))
    {   
        fread(this->cur_pointer, sizeof(PIXEL_FMT), this->size, this->fin);
        // add_noise
        for(int i = 0; i < this->size; i++){
            float noise = getNoise(sigma);
            float pixel = (float)cur_pointer[i];
            pixel = pixel + noise;
            pixel = pixel < 0 ? 0 : (pixel > 255 ? 255 : pixel);
            cur_pointer[i] = (PIXEL_FMT)pixel;
        }
        printf("Process NO.%d Frame\n",cnt);
        fwrite(this->cur_pointer, sizeof(PIXEL_FMT), this->size, this->fout);
        cnt ++;
    } 
}

float NrClass::distance(int x, int y, int i, int j) {
    return float(sqrt(pow(x - i, 2) + pow(y - j, 2)));
}

double NrClass::gaussian(float x, double sigma) {
    return exp(-(pow(x, 2))/(2 * pow(sigma, 2))) / (2 * M_PI * pow(sigma, 2));

}

bool NrClass::check_border(int x, int y, int half)
{
    return (x - half < 0) || (x + half > this->height) || (y + half > this->width) || (y - half < 0);
}