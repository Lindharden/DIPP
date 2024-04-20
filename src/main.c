#include <stdio.h>
#include <csp/csp.h>
#include <csp/csp_iflist.h>
#include <csp/csp_rtable.h>
#include <csp/interfaces/csp_if_zmqhub.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <param/param_server.h>
#include <pthread.h>
#include <vmem/vmem_server.h>
#include <vmem/vmem_file.h>
#include "vmem_storage.h"
#include "dipp_run.h"
#include <csp/drivers/usart.h>
#include <csp/drivers/can_socketcan.h>

void *vmem_server_task(void *param)
{
	vmem_server_loop(param);
	return NULL;
}

void *router_task(void *param)
{
	while (1)
	{
		csp_route_work();
	}
	return NULL;
}

int p_all = 0;

static void iface_init(int argc, char *argv[])
{
	csp_iface_t *iface = NULL;
	char *interface = "ZMQ";		  // Default interface
	char *port = "localhost";		  // Default port
	char *kiss_device = "/dev/ttyS1"; // Default KISS device
	char *can_device = "vcan0";		  // Default CAN device
	int pipeline_addr = 162;		  // Default pipeline address

	int opt;
	while ((opt = getopt(argc, argv, "i:p:a:q")) != -1)
	{
		switch (opt)
		{
		case 'i':
			// Use the provided interface instead of "ZMQ"
			interface = optarg; 
			break;
		case 'p':
			// Use the provided port/devices
			port = optarg;
			kiss_device = optarg;
			can_device = optarg;
			break;
		case 'a':
			// Use the pipeline address
			pipeline_addr = atoi(optarg);
			break;
		case 'q':
			p_all = 1;
			break;
		default:
			fprintf(stderr, "Usage: %s [-i <interface>] [-p <port/device>] [-a <pipeline_address<´>]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (strcmp(interface, "ZMQ") == 0)
	{
		/* ZMQ setup */
		csp_zmqhub_init_filter2("ZMQ", port, 3, 8, true, &iface, NULL, CSP_ZMQPROXY_SUBSCRIBE_PORT, CSP_ZMQPROXY_PUBLISH_PORT);
		iface->name = "zmq";
	}
	else if (strcmp(interface, "KISS") == 0)
	{
		/* KISS setup */
		csp_usart_conf_t conf = {
			.device = kiss_device,
			.baudrate = 115200,
			.databits = 8,
			.stopbits = 1,
			.paritysetting = 0,
			.checkparity = 0};

		int error = csp_usart_open_and_add_kiss_interface(&conf, CSP_IF_KISS_DEFAULT_NAME, &iface);
		if (error != CSP_ERR_NONE)
		{
			csp_print("failed to add KISS interface [%s], error: %d\n", kiss_device, error);
			exit(1);
		}

		iface->name = "KISS";
	}
	else if (strcmp(interface, "CAN") == 0)
	{
		int error = csp_can_socketcan_open_and_add_interface(can_device, "CAN", pipeline_addr, 1000000, 0, &iface);
		if (error != CSP_ERR_NONE)
		{
			csp_print("failed to add CAN interface [%s], error: %d\n", can_device, error);
			return;
		}

		iface->name = "CAN";
	}

	iface->addr = pipeline_addr;
	iface->netmask = 8;
	csp_rtable_set(0, 0, iface, CSP_NO_VIA_ADDRESS);
	csp_iflist_add(iface);
}

int main(int argc, char *argv[])
{
	// printf("\nbootmsg\n");

	srand(time(NULL));

	void serial_init(void);
	serial_init();

	/* Init CSP with */
	csp_conf.hostname = HOSTNAME; // HOSTNAME defined in meson_options.txt
	csp_init();

	/* Interfaces */
	iface_init(argc, argv);
	// csp_print("Connection table\r\n");
	// csp_conn_print_table();

	// csp_print("Interfaces\r\n");
	// csp_iflist_print();

	// csp_print("Route table\r\n");
	// csp_rtable_print();

	csp_bind_callback(csp_service_handler, CSP_ANY);
	csp_bind_callback(param_serve, PARAM_PORT_SERVER);

	vmem_file_init(&vmem_storage);

	static pthread_t router_handle;
	pthread_create(&router_handle, NULL, &router_task, NULL);

	static pthread_t vmem_server_handle;
	pthread_create(&vmem_server_handle, NULL, &vmem_server_task, NULL);

	if (p_all) process_all(1);
	else process_one(1);
	
	return 0;
}
