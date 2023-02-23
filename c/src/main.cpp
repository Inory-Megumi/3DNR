/*
 * @Filename:   
 * @Author: Kehongbo
 * @Created:         
 * @Description:   
 */
/*
 * @Filename:   
 * @Author: Kehongbo
 * @Created:         
 * @Description:   
 */
#include <cstdio>
#include "utils.h"
int main(int argc, char **argv)
{
    // Open YUV File
    FILE *file_in = fopen(argv[1], "rb");
    if (file_in == NULL)
    {
        printf("READ YUV ERROR! ARG=%s\n",argv[1]);
        return -1;
    }
    // Create Write Files
    FILE *file_out = fopen(argv[2],"wb");
    FILE *log_out = fopen(argv[3],"w+");
    // Create NR Process
    NrClass MainProcess(176,144,v420);
    MainProcess.specify_file(file_in,file_out,log_out);
    // MainProcess.add_noise(5,50);
    MainProcess.process(20);
    // int i = 26;int j = 32;
    // printf("y_value = %d\n",MainProcess.get_cur_y_val(i,j));
    // printf("u_value = %d\n",MainProcess.get_cur_u_val(i,j));
    // printf("v_value = %d\n",MainProcess.get_cur_v_val(i,j));
    fclose(file_in);
    fclose(file_out);
    fclose(log_out);
    return 0;
}