#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <vector>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstdint>
#include <fstream>
#include <bitset>
#include <cstring>
#include <iomanip>
#include <chrono>
#include <ratio>
#include <jpeglib.h>

#include "../../vp/tlm/address.hpp"

#define IP_COMMAND_LOAD_LETTER_DATA			0x0001
#define IP_COMMAND_LOAD_LETTER_MATRIX		0x0002
#define IP_COMMAND_LOAD_TEXT            	0x0004
#define IP_COMMAND_LOAD_POSSITION			0x0008
#define IP_COMMAND_LOAD_PHOTO       		0x0010
#define IP_COMMAND_PROCESSING		        0x0020
#define IP_COMMAND_SEND_FROM_BRAM			0x0040
#define IP_COMMAND_RESET            		0x0080

using namespace std;
using namespace chrono;

uint16_t letterData_0[214];
uint16_t letterData_1[214];
uint16_t letterData_2[214];
uint16_t letterData_3[214];
uint16_t letterData_4[214];

uint16_t possition_0[106];
uint16_t possition_1[106];
uint16_t possition_2[106];
uint16_t possition_3[106];
uint16_t possition_4[106];

uint16_t letterMatrix_0[16602];
uint16_t letterMatrix_1[22716];
uint16_t letterMatrix_2[29792];
uint16_t letterMatrix_3[37569];
uint16_t letterMatrix_4[46423];

uint16_t input_image_0[691200];
uint16_t input_image_1[1555200];
uint16_t input_image_2[2764800];
uint16_t input_image_3[4320000];
uint16_t input_image_4[6220800];

uint16_t text[55];


void extract_data();
void write_ip(int command, int dimension, int offset);
int read_ip();
void saveJPEG(const char *filename, uint16_t *image, int width, int height);

int main(int argc, char* argv[])
{ 
    int fd;
    int *p;
    int part_of_photo = 1;
    int start_tmp;
    int end_frame = 0;
    uint16_t temp;

    int width, height, bram_row;

    if(argc != 3)
    {
        cerr << "[app] Invalid number of arguments in command line!!" << endl;
        return 1;
    }
    
    int dimension = atoi(argv[1]);
    const char* output_file = argv[2];

    switch(dimension)
    {
        case 0:
            width = D0_WIDTH;
            height = D0_HEIGHT;
            bram_row = D0_BRAM;
            break;
        case 1:
            width = D1_WIDTH;
            height = D1_HEIGHT;
            bram_row = D1_BRAM;
            break;
        case 2:
            width = D2_WIDTH;
            height = D2_HEIGHT;
            bram_row = D2_BRAM;
            break;
        case 3:
            width = D3_WIDTH;
            height = D3_HEIGHT;
            bram_row = D3_BRAM;
            break;
        case 4:
            width = D4_WIDTH;
            height = D4_HEIGHT;
            bram_row = D4_BRAM;
            break;   
    }

	/* ------------------------ */
	/* ------Extract data------ */
	/* ------------------------ */

	extract_data();
	
    /* ------------------------ */
	/* --------Reset IP-------- */
	/* ------------------------ */

	write_ip(IP_COMMAND_RESET, dimension, 0);
	
	/* ------------------------ */
	/* ----Send letterData----- */
	/* ------------------------ */
	
    fd = open("/dev/dma", O_RDWR | O_NDELAY);
    if(fd < 0)
    {
        cout << "[app] Cannot open /dev/dma for write" << endl;
        return -1;
    }
    
    p = (int*)mmap(0, 214*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    switch(dimension)
    {   
        case 0:
            memcpy(p, letterData_0, 214*2);
            break;
        case 1:
            memcpy(p, letterData_1, 214*2);
            break;
        case 2:
            memcpy(p, letterData_2, 214*2);
            break;
        case 3:
            memcpy(p, letterData_3, 214*2);
            break;
        case 4:
            memcpy(p, letterData_4, 214*2);
            break;
    }
    munmap(p, 214*2);

    write_ip(IP_COMMAND_LOAD_LETTER_DATA, dimension, 0);

    /*close(fd);
    if(fd < 0)
    {
        cout << "[app] Cannot close /dev/dma for write" << endl;
        return -1;
    }
    */

	/* ------------------------ */
	/* ---Send letterMatrix---- */
	/* ------------------------ */
	
    /*fd = open("/dev/dma", O_RDWR | O_NDELAY);
    if(fd < 0)
    {
        cout << "[app] Cannot open /dev/dma for write" << endl;
        return -1;
    }
    */

    switch(dimension)
    {   
        case 0:
            p = (int*)mmap(0, 16602*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            memcpy(p, letterMatrix_0, 16602*2);
            munmap(p, 16602*2);
            break;
        case 1:
            p = (int*)mmap(0, 22716*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            memcpy(p, letterMatrix_1, 22716*2);
            munmap(p, 22716*2);
            break;
        case 2:
            p = (int*)mmap(0, 29792*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            memcpy(p, letterMatrix_2, 29792*2);
            munmap(p, 29792*2);
            break;
        case 3:
            p = (int*)mmap(0, 37569*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            memcpy(p, letterMatrix_3, 37569*2);
            munmap(p, 37569*2);
            break;
        case 4:
            p = (int*)mmap(0, 46423*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            memcpy(p, letterMatrix_4, 46423*2);
            munmap(p, 46423*2);
            break;
    }

    write_ip(IP_COMMAND_LOAD_LETTER_MATRIX, dimension, 0);
    
    /*close(fd);
    if(fd < 0)
    {
        cout << "[app] Cannot close /dev/dma for write" << endl;
        return -1;
    }
    */

    /* ------------------------ */
	/* -----Send possition----- */
	/* ------------------------ */
	
    /*fd = open("/dev/dma", O_RDWR | O_NDELAY);
    if(fd < 0)
    {
        cout << "[app] Cannot open /dev/dma for write" << endl;
        return -1;
    }
    */

    p = (int*)mmap(0, 106*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    switch(dimension)
    {   
        case 0:
            memcpy(p, possition_0, 106*2);
            break;
        case 1:
            memcpy(p, possition_1, 106*2);
            break;
        case 2:
            memcpy(p, possition_2, 106*2);
            break;
        case 3:
            memcpy(p, possition_3, 106*2);
            break;
        case 4:
            memcpy(p, possition_4, 106*2);
            break;
    }
    munmap(p, 106*2);

    write_ip(IP_COMMAND_LOAD_POSSITION, dimension, 0);
    
    /*close(fd);
    if(fd < 0)
    {
        cout << "[app] Cannot close /dev/dma for write" << endl;
        return -1;
    }
    */

    /* ------------------------ */
	/* -------Send text-------- */
	/* ------------------------ */

    /*fd = open("/dev/dma", O_RDWR | O_NDELAY);
    if(fd < 0)
    {
        cout << "[app] Cannot open /dev/dma for write" << endl;
        return -1;
    }
    */

    p = (int*)mmap(0, 55*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    memcpy(p, text, 55*2);
    munmap(p, 55*2);

    write_ip(IP_COMMAND_LOAD_TEXT, 55, 0);
    
    close(fd);
    if(fd < 0)
    {
        cout << "[app] Cannot close /dev/dma for write" << endl;
        return -1;
    }
    

	/* ------------------------- */
	/* Send photo and processing */
	/* ------------------------- */
    
    do
    {   
        start_tmp = bram_row * part_of_photo;
        start_tmp = (start_tmp > height) ? height : start_tmp;
        uint16_t *source;
        switch(dimension)
        {   
            case 0:
                source = input_image_0 + ((height - start_tmp) * width * 3); 
                break;
            case 1:
                source = input_image_1 + ((height - start_tmp) * width * 3); 
                break;
            case 2:
                source = input_image_2 + ((height - start_tmp) * width * 3); 
                break;
            case 3:
                source = input_image_3 + ((height - start_tmp) * width * 3); 
                break;
            case 4:
                source = input_image_4 + ((height - start_tmp) * width * 3); 
                break;
        }
        
        fd = open("/dev/dma", O_RDWR | O_NDELAY);
        if(fd < 0)
        {
            cout << "[app] Cannot open /dev/dma for write" << endl;
            return -1;
        }

        p = (int*)mmap(0, bram_row*width*3*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(p == MAP_FAILED)
        {
            cout << "[app] Memory mapped failed. Error: " << strerror(errno) << endl;
            close(fd);
            return -1;
        }

        memcpy(p, source, bram_row*width*3*2);
        munmap(p, bram_row*width*3*2);

        write_ip(IP_COMMAND_LOAD_PHOTO, dimension, 0);
    
        close(fd);
        if(fd < 0)
        {
            cout << "[app] Cannot close /dev/dma for write" << endl;
            return -1;
        }

        //Write possitiony to title-ip
        write_ip(start_tmp, dimension, 1);
        
        //Processing
        write_ip(IP_COMMAND_PROCESSING, dimension, 0);

        //Read interupt from ip
        end_frame = read_ip();
        cout << "[app] interupt frame: " << end_frame << endl;
        
        //Reading processed data from ip
        write_ip(IP_COMMAND_SEND_FROM_BRAM, dimension, 0);

        fd = open("/dev/dma", O_RDWR | O_NDELAY);
        if(fd < 0)
        {
            cout << "[app] Cannot open /dev/dma for write" << endl;
            return -1;
        }
        
        p = (int*)mmap(0, bram_row*width*3*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        
        for(int i = 0; i < bram_row*width*3/2; i++)
        {
            temp = (uint16_t)((uint32_t)*(p+i) & 0x0000ffff);
            switch(dimension)
            {   
                case 0:
                    input_image_0[((height - start_tmp) * width *3) + (i*2)] = temp; 
                    break;
                case 1:
                    input_image_1[((height - start_tmp) * width *3) + (i*2)] = temp; 
                    break;
                case 2:
                    input_image_2[((height - start_tmp) * width *3) + (i*2)] = temp;
                    break;
                case 3:
                    input_image_3[((height - start_tmp) * width *3) + (i*2)] = temp;
                    break;
                case 4:
                    input_image_4[((height - start_tmp) * width *3) + (i*2)] = temp;
                    break;
            }

            temp = (uint16_t)(((uint32_t)*(p+i) & 0xffff0000) >> 16);
            switch(dimension)
            {   
                case 0:
                    input_image_0[((height - start_tmp) * width *3) + (i*2+1)] = temp; 
                    break;
                case 1:
                    input_image_1[((height - start_tmp) * width *3) + (i*2+1)] = temp; 
                    break;
                case 2:
                    input_image_2[((height - start_tmp) * width *3) + (i*2+1)] = temp;
                    break;
                case 3:
                    input_image_3[((height - start_tmp) * width *3) + (i*2+1)] = temp;
                    break;
                case 4:
                    input_image_4[((height - start_tmp) * width *3) + (i*2+1)] = temp;
                    break;
            }
        }
        
        munmap(p, bram_row*width*3*2);
        close(fd);
        if(fd < 0)
        {
            cout << "[app] Cannot close /dev/dma for write" << endl;
            return -1;
        }

        cout << "[app] Obradjen je " << part_of_photo << ". dio slike" << endl; 
        part_of_photo++;

    }while(!end_frame);
        
    cout << "[app] Finished processing of photo !!!" << endl; 
	
    switch(dimension)
    {   
        case 0:
            saveJPEG(output_file, input_image_0, width, height); 
            break;
        case 1:
            saveJPEG(output_file, input_image_1, width, height); 
            break;
        case 2:
            saveJPEG(output_file, input_image_2, width, height); 
            break;
        case 3:
            saveJPEG(output_file, input_image_3, width, height); 
            break;
        case 4:
            saveJPEG(output_file, input_image_4, width, height); 
            break;
    }
    
    /*
    FILE *fajl = fopen("output4.txt", "w");
    if(fajl != NULL)
    {
        for(int i = 0; i < width * height * 3; i++)
            fprintf(fajl, "%d\n", input_image_4[i]);      
    }
    fclose(fajl);
    */

	return 0;
}


void write_ip(int command, int dimension, int offset)
{	
	int ret;
	FILE *title_file;
	
	title_file = fopen("/dev/title-ip", "w");
	
	// cout << "[app] Opened title-ip for " << hex << command << endl;	
	if(title_file == NULL) cout << "[app] Could not open /dev/title-ip" << endl;
	// else cout << "[app] Opened /dev/title-ip" << endl;
	
	ret = fprintf(title_file, "%d,%d,%d\n", command, dimension, offset);
	
	if(fclose(title_file)) cout << "[app] Cannot close /dev/title-ip" << endl;
	// else cout << "[app] Successfully closed /dev/title-ip after " << command << endl
}



void extract_data()
{
	int temp;
	FILE *input;

	// Extracting letterData for every dimension

	input = fopen("../../data/ver_data/letterData_1.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open letterData_1.txt" << endl;
    }

	for(int i = 0; i < 214; i++)
	{
		fscanf(input, "%d", &temp);
		letterData_0[i] = (uint16_t)temp;
	}
	fclose(input);


	input = fopen("../../data/ver_data/letterData_2.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open letterData_2.txt" << endl;
    }

	for(int i = 0; i < 214; i++)
	{
		fscanf(input, "%d", &temp);
		letterData_1[i] = (uint16_t)temp;
	}
	fclose(input);
	

	input = fopen("../../data/ver_data/letterData_3.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open letterData_3.txt" << endl;
    }

	for(int i = 0; i < 214; i++)
	{
		fscanf(input, "%d", &temp);
		letterData_2[i] = (uint16_t)temp;
	}
	fclose(input);
	

	input = fopen("../../data/ver_data/letterData_4.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open letterData_4.txt" << endl;
    }

	for(int i = 0; i < 214; i++)
	{
		fscanf(input, "%d", &temp);
		letterData_3[i] = (uint16_t)temp;
	}
	fclose(input);
	

	input = fopen("../../data/ver_data/letterData_5.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open letterData_5.txt" << endl;
    }

	for(int i = 0; i < 214; i++)
	{
		fscanf(input, "%d", &temp);
		letterData_4[i] = (uint16_t)temp;
	}
	fclose(input);
	

	// Extracting possition for every dimension	

	input = fopen("../../data/ver_data/possition_1.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open possition_1.txt" << endl;
    }

	for(int i = 0; i < 106; i++)
	{
		fscanf(input, "%d", &temp);
		possition_0[i] = (uint16_t)temp;
	}
	fclose(input);
	
    
    input = fopen("../../data/ver_data/possition_2.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open possition_2.txt" << endl;
    }

	for(int i = 0; i < 106; i++)
	{
		fscanf(input, "%d", &temp);
		possition_1[i] = (uint16_t)temp;
	}
	fclose(input);
	

	input = fopen("../../data/ver_data/possition_3.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open possition_3.txt" << endl;
    }

	for(int i = 0; i < 106; i++)
	{
		fscanf(input, "%d", &temp);
		possition_2[i] = (uint16_t)temp;
	}
	fclose(input);
	
    
    input = fopen("../../data/ver_data/possition_4.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open possition_4.txt" << endl;
    }

	for(int i = 0; i < 106; i++)
	{
		fscanf(input, "%d", &temp);
		possition_3[i] = (uint16_t)temp;
	}
	fclose(input);

    
    input = fopen("../../data/ver_data/possition_5.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open possition_5.txt" << endl;
    }

	for(int i = 0; i < 106; i++)
	{
		fscanf(input, "%d", &temp);
		possition_4[i] = (uint16_t)temp;
	}
	fclose(input);
	
    
    // Extracting letterMatrix for every dimensiom	
	
    input = fopen("../../data/ver_data/letterMatrix_1.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open letterMatrix_1.txt" << endl;
    }

	for(int i = 0; i < 16602; i++)
	{
		fscanf(input, "%d", &temp);
		letterMatrix_0[i] = (uint16_t)temp;
	}
	fclose(input);

	
    input = fopen("../../data/ver_data/letterMatrix_2.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open letterMatrix_2.txt" << endl;
    }

	for(int i = 0; i < 22716; i++)
	{
		fscanf(input, "%d", &temp);
		letterMatrix_1[i] = (uint16_t)temp;
	}
	fclose(input);

	
    input = fopen("../../data/ver_data/letterMatrix_3.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open letterMatrix_3.txt" << endl;
    }

	for(int i = 0; i < 29792; i++)
	{
		fscanf(input, "%d", &temp);
		letterMatrix_2[i] = (uint16_t)temp;
	}
	fclose(input);

	
    input = fopen("../../data/ver_data/letterMatrix_4.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open letterMatrix_4.txt" << endl;
    }

	for(int i = 0; i < 37569; i++)
	{
		fscanf(input, "%d", &temp);
		letterMatrix_3[i] = (uint16_t)temp;
	}
	fclose(input);

	
    input = fopen("../../data/ver_data/letterMatrix_5.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open letterMatrix_5.txt" << endl;
    }

	for(int i = 0; i < 46423; i++)
	{
		fscanf(input, "%d", &temp);
		letterMatrix_4[i] = (uint16_t)temp;
	}
	fclose(input);


	// Extracting input image for every dimension 
    
    input = fopen("../../data/ver_data/input_image_640x360.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open input_image_640x360.txt" << endl;
    }

	for(int i = 0; i < 691200; i++)
	{
		fscanf(input, "%d", &temp);
		input_image_0[i] = (uint16_t)temp;
	}
	fclose(input);


    input = fopen("../../data/ver_data/input_image_960x540.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open input_image_960x540.txt" << endl;
    }

	for(int i = 0; i < 1555200; i++)
	{
		fscanf(input, "%d", &temp);
		input_image_1[i] = (uint16_t)temp;
	}
	fclose(input);


    input = fopen("../../data/ver_data/input_image_1280x720.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open input_image_12800x720.txt" << endl;
    }

	for(int i = 0; i < 2764800; i++)
	{
		fscanf(input, "%d", &temp);
		input_image_2[i] = (uint16_t)temp;
	}
	fclose(input);


    input = fopen("../../data/ver_data/input_image_1600x900.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open input_image_1600x900.txt" << endl;
    }

	for(int i = 0; i < 4320000; i++)
	{
		fscanf(input, "%d", &temp);
		input_image_3[i] = (uint16_t)temp;
	}
	fclose(input);


    input = fopen("../../data/ver_data/input_image_1920x1080.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open input_image_1920x1080.txt" << endl;
    }

	for(int i = 0; i < 6220800; i++)
	{
		fscanf(input, "%d", &temp);
		input_image_4[i] = (uint16_t)temp;
	}
	fclose(input);
    
    //Extracting text
	
    input = fopen("../../data/ver_data/text.txt", "r");
    if(input == NULL)
    {
        cout << "[extract_data] Cannot open text.txt" << endl;
    }

	for(int i = 0; i < 55; i++)
	{
		fscanf(input, "%d", &temp);
		text[i] = (uint16_t)temp;
	}
	fclose(input);
          
}

int read_ip()
{
    FILE *title_file;
    int command;
    int frame;

    title_file = fopen("/dev/title-ip", "r");
    if(title_file == NULL)
    {
        cout << "[app] Cannot open /dev/title-ip for read" << endl;
        return -1;
    }

    int ret = fscanf(title_file, "%d %d", &frame, &command);
    if(ret == EOF)
    {
        cout << "[app] Error reading from /dev/title-ip" << endl;
        fclose(title_file);
        return -1;
    }

    fclose(title_file);
    
    if(command && frame)
        return 0;

    return frame;
    
}

void saveJPEG(const char *filename, uint16_t *image, int width, int height) {

    FILE *outfile = fopen(filename, "wb");  

    if (!outfile) {
        cerr << "Error opening output file." << endl;
        return;
    }

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3; // Broj komponenti po pikselu (RGB)
    cinfo.in_color_space = JCS_RGB; // Boja slike

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 90, TRUE);
    jpeg_start_compress(&cinfo, TRUE);
    
    JSAMPROW row_pointer[1];
    uint8_t* row = new uint8_t[width * 3];

    for(int y = 0; y < height; ++y)
    {
        for(int x = 0; x < width; ++x)
        {
            int index = (y * width + x) * 3;
            row[x*3] = static_cast<uint8_t>(image[index]);
            row[x*3+1] = static_cast<uint8_t>(image[index+1]);
            row[x*3+2] = static_cast<uint8_t>(image[index+2]);
        }
        row_pointer[0] = row;
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    
    delete[] row;
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
}
