#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

// Function prototypes. Don't change these.
uint8_t *read_card(char *fname, int *size);
void save_jpeg(uint8_t *data, int size, char *filename);
void recover(uint8_t *data, int size);

#define RAW_FILE "card.raw"

int main()
{
    //while(1){
        // Read the card.raw file into an array of bytes (uint8_t)
        int card_length;
        uint8_t *card_data = read_card(RAW_FILE, &card_length);
        if(card_data != NULL){
            // Recover the images
            recover(card_data, card_length);
        }

        else{
             //break;
        }
    //}
}

uint8_t *read_card(char *filename, int *size)
{

    struct stat st;
    if (stat(filename, &st) == -1)
    {
        fprintf(stderr, "Can't get info about %s\n", filename);
        exit(1);
    }
    int len = st.st_size;
    uint8_t *raw = malloc(len * sizeof(uint8_t));
    
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        fprintf(stderr, "Can't open %s for reading\n", filename);
        exit(1);
    }
    
    fread(raw, 1, len, fp);
    fclose(fp);
    
    *size = len;
    return raw;
}

void save_jpeg(uint8_t *data, int size, char *filename)
{
    //printf("it worked\n");
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "Can't write to %s\n", filename);
        exit(1);
    }
    
    fwrite(data, 1, size, fp);
    fclose(fp);   
}

void recover(uint8_t *data, int size){
    int temp = 0;
    int tempB = 0;
    int tempC;
    int location[1000];
    int jpgPlaceA = 0;
    int jpgPlaceB = 0;
    int jpgPlaceC = 0;

    // Calculate JPEG length
    for(int i = 0; i < size; i++){
        if(data[i]==255 && data[i+1]==216 && data[i+2]==255 && (data[i+3]==224 || data[i+3]==225)){
            //printf("%i\n",data[i+1]);
            location[temp] = i; // store position in location array (index 0 is the first start, 1 is the second)
            temp++;
        }

        // if two starts were found
        if(temp == 2){
            temp = 0;
            tempC = 0; // <-- very important we include this or else the images created will find end markers in the midst of other jpegs
            // loop backward for an ending marker, filter through the slack space:
            for(int j = location[1]-1; j > location[0]; j--){
                
                //tempC = 0;
                if(tempC == 0 && data[j]==217 && data[j-1]==255){
                    tempB = j; // j is the end marker position
                    uint8_t substr[(tempB - location[0])+1]; // create substring to store jpeg chunk
                    
                    // char copy function from ground up because why not!!
                    int m = 0;
                    for(int k = location[0]; k < tempB + 1; k++){
                        substr[m] = data[k];
                        m++;
                    }
                    char buffer[20];
                    sprintf(buffer,"%i%i%i.jpg",jpgPlaceA,jpgPlaceB,jpgPlaceC); 
                    save_jpeg(substr,(tempB - location[0])+1,buffer); // throw her into save_jpeg
                    //printf("%s\n",buffer);
                    // code for changing the jpg number
                    if(jpgPlaceC < 9){
                        jpgPlaceC++;
                    }
                    else{
                        jpgPlaceC = 0;
                        jpgPlaceB++;
                    }
                    if(jpgPlaceB > 8){
                        jpgPlaceB = 0;
                        jpgPlaceA++;
                    }
                    tempC++;
                }
                
            }
            tempB = 0;
        }

    }

}







    // Construct filename to save to (write)

    // call save_jpeg to save the file image to,
    // by passing in the address of the start of the JPEG, the length, and the filenam