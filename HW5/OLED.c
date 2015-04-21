#include "OLED.h"


void OLED_WRITE(char input){

        int row, col;
        int temp;
        int i =0, j=0, k=0;
        int val;
        int rowstart=28;
        int colstart=32;
        while(input[k]){
            for(k=0;k<strlen(input);k++){
                val=input[k];
                for(col=0;col<5;col++){
                    for(row=0;row<8;row++){
                        temp=ASCII[val-0x20][i];
                        //
                        temp>>=j;
                        display_pixel_set(row+rowstart,col+colstart,temp&1);
                        j++;
                    }

                    i++;
                    j=0;
                }                
                i=0;
                colstart+=5;
            }
            display_draw();
        }
}

int SET_START(int rowstart, int colstart){
    int temp[2];
    temp[0] = rowstart;
    temp[1] = colstart;

    return (temp);
}