project('pipeline', 'c',subproject_dir: 'lib',default_options: [
	'csp:packet_padding_bytes=42', 
	'csp:buffer_count=1000',
	'csp:buffer_size=2048',
	'csp:conn_max=20',
	'csp:conn_rxqueue_len=1000',
	'csp:qfifo_len=1000',
	'csp:rdp_max_window=1000',
	'csp:port_max_bind=16',
	'param:have_fopen=true', 
	'param:vmem_fram=false', 
	'param:list_dynamic=true',
])

sources = files([
	'src/main.c',
	'src/hooks.c',
    'src/serial.c',
    'src/csp_hooks.c',
    'src/pipeline/pipeline.c',
    'src/vmem_config.c',
    'src/arb_params.c',
])


csp_dep = dependency('csp', fallback: ['csp', 'csp_dep'])
param_dep = dependency('param', fallback: ['param', 'param_dep'])

deps = [csp_dep,param_dep]

c_args = '-DHOSTNAME="@0@"'.format(get_option('hostname'))

pipeline_serv = executable('pipeline_serv', sources,
	dependencies : deps,
	install : true,
    c_args : c_args,
)
