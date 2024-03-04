#include <stdio.h>
#include <csp/csp.h>
#include <csp/csp_iflist.h>
#include <csp/csp_rtable.h>
#include <csp/interfaces/csp_if_zmqhub.h>
#include <stdio.h>
#include <unistd.h>
#include <param/param_server.h>
#include <pthread.h>
#include "hooks.h"
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

static void * onehz_task(void * param) {
	while(1) {
		hook_onehz();
		sleep(1);
	}
	return NULL;
}

static void iface_init(void){
    csp_iface_t * iface = NULL;
    
	csp_zmqhub_init_filter2("ZMQ", "localhost", 3, 8, true, &iface, NULL, CSP_ZMQPROXY_SUBSCRIBE_PORT, CSP_ZMQPROXY_PUBLISH_PORT);

    iface->addr = 162;
    iface->netmask = 8;
    iface->name = "zmq";
    csp_rtable_set(0, 0, iface, CSP_NO_VIA_ADDRESS);
	csp_iflist_add(iface);
}

int main(void){
	printf("\nbootmsg\n");

	srand(time(NULL));
	
	void serial_init(void);
	serial_init();

	/* Gndwdt
	gndwdt_init();
    */

	/* Init CSP with */
	csp_conf.hostname = HOSTNAME;
	csp_init();

	/* Interfaces */
    iface_init();
    csp_print("Connection table\r\n");
    csp_conn_print_table();

    csp_print("Interfaces\r\n");
    csp_iflist_print();

    csp_print("Route table\r\n");
    csp_rtable_print();

    //server_start();
	//kiss_init();
	hook_init_early();
	/*csp_rt_init(); ROUTING TABLE?? */

	csp_bind_callback(csp_service_handler, CSP_ANY);
	csp_bind_callback(param_serve, PARAM_PORT_SERVER);

	vmem_file_init(&vmem_config);

	static pthread_t router_handle;
	pthread_create(&router_handle, NULL, &router_task, NULL);

	static pthread_t vmem_server_handle;
	pthread_create(&vmem_server_handle, NULL, &vmem_server_task, NULL);

	static pthread_t onehz_handle;
	pthread_create(&onehz_handle, NULL, &onehz_task, NULL);

	hook_init();
	while (1){
        sleep(10*1000); //TODO: Handle kbd interupt
    }

	//pthread_cancel(router_handle);
	//pthread_cancel(onehz_handle);
	//pthread_cancel(vmem_server_handle);
	//ptrhead_cancel(ftp_server_handle);

	//pthread_join(router_handle,NULL);
	//pthread_join(onehz_handle,NULL);
	//pthread_join(vmem_server_handle,NULL);
	//pthread_join(ftp_server_handle);
	
	return 0;
}
