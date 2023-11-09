# C Data Processing Pipeline
MARIO stands for: Modular, Adaptive, Reconfigurable, Image, Orchestrator.

MARIO is an modular image processing pipeline tailored to run on the Disco-2 CubeSat.

## Compile External Function
Modules for the pipeline should be written in C, and compiled to .so (shared object) files using the following command:
```
gcc -shared -o <compiled_name>.so <filename>.c
```

The function within each module should have the name `run()`. This is the function that will be called within the pipeline.

The shared object files should be placed within the `external_modules` directory.

## Configuring the Pipeline Modules
To include modules in the pipeline, add their names, without extension and path, to `modules.txt` in the desired order.

## Build the Pipeline
To build the project run the following commands:
```
meson . builddir
cd builddir
ninja
```

## Run the Pipeline
To run the pipeline you first have to configure the `iface_init()` function within the `main.c` file. The default configuration looks like this:
```C
static void iface_init(void){
    csp_iface_t * iface = NULL;
    
	csp_zmqhub_init_filter2("ZMQ", "localhost", 3, 8, true, &iface);

    iface->addr = 162;
    iface->netmask = 8;
    iface->name = "zmq";
    csp_rtable_set(0, 0, iface, CSP_NO_VIA_ADDRESS);
	csp_iflist_set_default(iface);
}
```

Run the `pipeline_serv` binary from the `builddir` directory using `./builddir/pipeline_serv`.

Now run a `zmqproxy` which is found by building [CSH](https://github.com/discosat/csh), in a different terminal.

Run `CSH` from a different directory (i.e. make a CSH clone), in a different terminal, using `.csh -i ../init/zmq.csh` from the `builddir` directory with a `zmq.csh` file that is configured to the correct port as defined by the ZMQ proxy.

Now access the MARIO node from the resently created CSH terminal by accessing `node 162` (default port), and download the list of parameters using `list download`. Now set the `pipeline_run` parameter to 1 using the command:
```
set pipeline_run 1
```

This should run the pipeline using the modules defined in `modules.txt`.
