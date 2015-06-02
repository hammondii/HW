/* Version 2:
***Request user to enter row and input string.  
***Writes successfully to buffer
***Prints out buffer until \0*/

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"

#define MAX_STR 255

int main(int argc, char* argv[])
{
	int res;
	unsigned int temp;
	unsigned char buf[65];
	unsigned char message[65];
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i=0;

	// Initialize the hidapi library
	res = hid_init();

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	handle = hid_open(0x4d8, 0x3f, NULL);

	// Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	wprintf(L"Manufacturer String: %s\n", wstr);

	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	wprintf(L"Product String: %s\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);

	// Read Indexed String 1
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	wprintf(L"Indexed String 1: %s\n", wstr);

/* 	Toggle LED (cmd 0x80). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x80;
	res = hid_write(handle, buf, 65);

	// Request state (cmd 0x81). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x81;
	res = hid_write(handle, buf, 65);

	// Read requested state
	res = hid_read(handle, buf, 65);

	// Print out the returned buffer.
	for (i = 0; i < 4; i++)
		printf("buf[%d]: %d\n", i, buf[i]); */

	//ADDED CODE FOR HW8
	/*Toggle LED (cmd 0x80). The first byte is the report number (0x0).
	**Request information from the user and send to PIC for OLED write
	*/

	printf("Enter Starting Row Number: ");
	scanf("%d",&temp);
	printf("You entered %d\n",temp);
	// printf("Enter Message: ");
	// scanf("%25s",message);
	
	buf[0] = 0x0;
	buf[1] = 0x80;
	buf[2] = temp;
	int stored = buf[2];
	printf("Buffer has %d\n",stored);
	// for (i=0;i<400000;i++);
/* 	for(i=0;i<strlen( (char *)temp) -1;i++)
		{
			buf[i+2] = temp[i];
		} */

	/*for (i = 0; i<strlen( (char *) message)+3; i++){
		if(i<2)printf("buf[%d]: %d\n", i, buf[i]);
		else printf("buf[%d]: %c\n", i, message[i-3]); 
	}*/
	res = hid_write(handle, buf, 65);

	// TO BE CHANGED Request state (cmd 0x81). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x81;
	res = hid_write(handle, buf, 65);

	// Read requested state
	res = hid_read(handle, buf, 65);

	// Print out the returned buffer.
	for (i = 0; i < 4; i++)
		printf("buf[%d]: %d\n", i, buf[i]);
	
	

/* 	FILE *ofp;

	ofp = fopen("accels.txt", "w");

	for (i=0; i<DATAPNTS; i++) {

	fprintf(ofp,"your data\r\n",x[i],y[i],z[i]);

	}

	fclose(ofp); */
	
	// Finalize the hidapi library

	res = hid_exit();
	
	return 0;
}

