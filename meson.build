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
	'protos/module_config.pb-c.c',
	'protos/pipeline_config.pb-c.c',
])

csp_dep = dependency('csp', fallback: ['csp', 'csp_dep'])
param_dep = dependency('param', fallback: ['param', 'param_dep'])
proto_c_dep = dependency('libprotobuf-c', fallback: ['protobuf-c', 'proto_c_dep'])
m_dep = meson.get_compiler('c').find_library('m', required : false)
deps = [csp_dep,param_dep,proto_c_dep,m_dep]

c_args = '-DHOSTNAME="@0@"'.format(get_option('hostname'))

pipeline_serv = executable('pipeline_serv', sources,
	dependencies : deps,
	install : true,
    c_args : c_args,
	link_args : ['-ldl'],
)
