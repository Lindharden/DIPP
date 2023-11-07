# C Data Processing Pipeline
Data Processing Pipeline for the Disco-2 CubeSat satellite.

## Compile External Function
To compile the external functions from C files, use the following command:
```
gcc -shared -o <compiled_name>.so <filename>.c
```
The resulting `.so` files can be referenced directly in the code.

The function within each module should have the name `run()`. This is the function that will be called within the pipeline.

## Configuring the Pipeline Modules
To include modules in the pipeline, add their names, without extension and path, to `modules.txt` in the desired order.

## Build the Pipeline
After the external modules have been compiled into .so files, the entire pipeline can be built. This is done with the command:
```
make
```

## Run the Pipeline
After the pipeline is built, execute it with the command:
```
./bin/binary <initial_value>
```