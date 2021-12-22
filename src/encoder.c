/*
 * encoder.c
 *
 *  Created on: Dec 22, 2021
 *      Author: bill
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/input.h>

int main(int argc, char *argv[])
{
	int enc_fd;
	int sw_fd;
	int max_fd;
	//******** IMPORTANT!!!!
	// Code must first verify the /eventX index for the rotary encoder and switch!!!!
	// It is hard coded here as we know the assignment - do not depend on this!!!!
	char encoder_dev[] = "/dev/input/event1\0";
	char switch_dev[] = "/dev/input/event0\0";
	struct input_event enc_ev[64];
	struct input_event sw_ev[64];
	int i, rd;
	fd_set rdfs;

	enc_fd = open(encoder_dev, O_RDONLY | O_NONBLOCK);
	if (enc_fd < 0)
	{
		printf("failed to open %s...\n", encoder_dev);
		return -1;
	}
	printf("%s has been opened...\n", encoder_dev);

	max_fd = enc_fd;

	sw_fd = open(switch_dev, O_RDONLY | O_NONBLOCK);
	if (sw_fd < 0)
	{
		printf("failed to open %s...\n", switch_dev);
		return -1;
	}
	printf("%s has been opened...\n", switch_dev);

	if (sw_fd > max_fd)
	{
		max_fd = sw_fd;
	}

	unsigned int enc_type, sw_type, enc_code, sw_code;
	int enc_value = 0, sw_value = 0;
	while(1)
	{
		FD_ZERO(&rdfs);
		FD_SET(enc_fd, &rdfs);
		FD_SET(sw_fd, &rdfs);

		select(max_fd + 1, &rdfs, NULL, NULL, NULL);

		if(FD_ISSET(enc_fd, &rdfs))
		{
			rd = read(enc_fd, enc_ev, sizeof(enc_ev));
			if (rd < (int) sizeof(struct input_event)) {
				printf("ENCODER: expected %d bytes, got %d\n", (int) sizeof(struct input_event), rd);
				return -1;
			}
			for (i = 0; i < (rd / sizeof(struct input_event)); i++)
			{

				enc_type = enc_ev[i].type;
				enc_code = enc_ev[i].code;
				enc_value = enc_ev[i].value;
				//printf("type = %d   code=%d  count=%d\n", type, code, i);
				if ((enc_type == EV_REL) && (enc_code == REL_HWHEEL))
				{
					printf("%+d\n", enc_value);
				}
			}
		}
		if(FD_ISSET(sw_fd, &rdfs))
		{
			rd = read(sw_fd, sw_ev, sizeof(sw_ev));
			if (rd < (int) sizeof(struct input_event)) {
				printf("SWITCH: expected %d bytes, got %d\n", (int) sizeof(struct input_event), rd);
				return -1;
			}
			for (i = 0; i < (rd / sizeof(struct input_event)); i++)
			{

				sw_type = sw_ev[i].type;
				sw_code = sw_ev[i].code;
				sw_value = sw_ev[i].value;

				if ((sw_type == EV_KEY) && (sw_code == KEY_ENTER))
				{
					if (sw_value == 1)
					{
						printf("ENTER PRESS\n");
					}
					else
					{
						printf("ENTER RELEASE\n");
					}
				}
			}
		}
	}
	close(enc_fd);
	return 0;
}

