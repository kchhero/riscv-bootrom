#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <fcntl.h>

#include <usb.h>


/* Global variables */


/* Function Define */
static usb_dev_handle *get_usb_dev_handle(int vid, int pid);
int send_data(int vid, int pid, unsigned char *data, int size);
int receive_data(int vid, int pid, unsigned char *data, int size);


int main(int argc, char **argv)
{
	int vid = 0x04e8, pid = 0x1234;
	int i, ret = 0;
	unsigned int data[512/4];

	for (i = 0; i < 512/4; i++)
		data[i] = i;

	printf("test start.....\n");

//	if ((ret = send_data(vid, pid, (unsigned char *)data, 512)) == 0)
	if ((ret = receive_data(vid, pid, (unsigned char *)data, 512)) == 0)
		printf("receive data size is %d\n", ret);
	else
		printf("receive fail. ret = %d\n", ret);

	for (i = 0; i < 512/4; i++) {
		if ((i % 4) == 0)
			putchar ('\n');
		printf("%08X ", data[i]);
	}
	putchar ('\n');

	printf("test done\n");

	return 0;
}


static usb_dev_handle *get_usb_dev_handle(int vid, int pid)
{
	struct usb_bus *bus;
	struct usb_device *dev;
	static int is_init_usb = 0;

	if (!is_init_usb) {
		usb_init();
		is_init_usb = 1;
	}

	usb_find_busses();
	usb_find_devices();

	for (bus = usb_get_busses(); bus; bus = bus->next) {
		for (dev = bus->devices; dev; dev = dev->next) {
			if (dev->descriptor.idVendor == vid &&
				dev->descriptor.idProduct == pid) {
				printf("device found vid:%04X pid:%04X\n",
						vid, pid);
				return usb_open(dev);
			}
		}
	}

	return NULL;
}


int send_data(int vid, int pid, unsigned char *data, int size)
{
	int ret;
	usb_dev_handle *dev_handle;

	dev_handle = get_usb_dev_handle(vid, pid);

	if (NULL == dev_handle) {
		printf("Cannot found matching USB device."
				"(vid=0x%04x, pid=0x%04x)\n", vid, pid);
		return -1;
	}

	if (usb_claim_interface(dev_handle, 0) < 0) {
		printf("usb_claim_interface() fail!!!\n");
		usb_close(dev_handle);
		return -1;
	}

	printf("=> Downloading %d bytes\n", size);

	ret = usb_bulk_write(dev_handle, 2, (void *)data, size, 5*1000*1000);

	if (ret == size) {
		printf("=> Download Success!!!\n");
	} else {
		printf("=> Download Failed!!(ret=%d)\n", ret);
		usb_close(dev_handle);
		return -1;
	}

	usb_release_interface(dev_handle, 0);
	usb_close( dev_handle );

	return 0;
}

int receive_data(int vid, int pid, unsigned char *data, int size)
{
	int ret = 0;
	usb_dev_handle *dev_handle;

	dev_handle = get_usb_dev_handle(vid, pid);

	if (NULL == dev_handle) {
		printf("Cannot found matching USB device."
				"(vid=0x%04x, pid=0x%04x)\n", vid, pid);
		return -1;
	}

	if (usb_claim_interface(dev_handle, 0) < 0) {
		printf("usb_claim_interface() fail!!!\n");
		usb_close(dev_handle);
		return -1;
	}
	printf("=> try get %d bytes\n", size);

	ret = usb_bulk_read(dev_handle, 1, (void *)data, size, 5*1000*1000);

	if (ret == size) {
		printf("=> data received. ret:%d, size:%d\n", ret, size);
	} else {
		printf("=> cannot get data!!(ret=%d)\n", ret);
		usb_close(dev_handle);
		return ret;
	}

	usb_release_interface(dev_handle, 0);
	usb_close(dev_handle);

	return 0;
}

