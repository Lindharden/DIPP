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
#include "vmem_config.h"
#include <csp/drivers/usart.h>

void * vmem_server_task(void * param) {
	vmem_server_loop(param);
	return NULL;
}

void * router_task(void * param) {
	while(1) {
		csp_route_work();
	}
	return NULL;
}

static void iface_init(int argc, char *argv[]){
    csp_iface_t * iface = NULL;
	char *port = "localhost"; // Default port is "localhost"

    // Check if a port argument is provided in the command line
    if (argc > 1) {
        port = argv[1]; // Use the provided port instead of "localhost"
    }
    
	csp_zmqhub_init_filter2("ZMQ", port, 3, 8, true, &iface, NULL, CSP_ZMQPROXY_SUBSCRIBE_PORT, CSP_ZMQPROXY_PUBLISH_PORT);

    iface->addr = 162;
    iface->netmask = 8;
    iface->name = "zmq";
    csp_rtable_set(0, 0, iface, CSP_NO_VIA_ADDRESS);
	csp_iflist_add(iface);
}

int main(int argc, char *argv[]){
	printf("\nbootmsg\n");

	srand(time(NULL));
	
	void serial_init(void);
	serial_init();

	/* Init CSP with */
	csp_conf.hostname = HOSTNAME; // HOSTNAME defined in meson_options.txt
	csp_init();

	/* Interfaces */
    iface_init(argc, argv);
    csp_print("Connection table\r\n");
    csp_conn_print_table();

    csp_print("Interfaces\r\n");
    csp_iflist_print();

    csp_print("Route table\r\n");
    csp_rtable_print();

	csp_bind_callback(csp_service_handler, CSP_ANY);
	csp_bind_callback(param_serve, PARAM_PORT_SERVER);

	vmem_file_init(&vmem_config);

	static pthread_t router_handle;
	pthread_create(&router_handle, NULL, &router_task, NULL);

	static pthread_t vmem_server_handle;
	pthread_create(&vmem_server_handle, NULL, &vmem_server_task, NULL);

	while (1){
        sleep(10*1000); //TODO: Handle kbd interupt
    }

	return 0;
}
