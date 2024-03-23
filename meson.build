project('pipeline', 'c',subproject_dir: 'lib',default_options: [
	'c_std=gnu11', 
	'b_lto=false',
	'default_library=static',
	'csp:packet_padding_bytes=42', 
	'csp:buffer_count=1000',
	'csp:buffer_size=2048',
	'csp:conn_max=20',
	'csp:conn_rxqueue_len=1000',
	'csp:qfifo_len=1000',
	'csp:rdp_max_window=1000',
	'csp:port_max_bind=16',
	'csp:use_rtable=true',
	'param:have_fopen=true', 
	'param:list_dynamic=true',
	'param:vmem_fram=false', 
])

sources = files([
	'src/main.c',
	'src/serial.c',
	'src/dipp_main.c',
	'src/dipp_error.c',
	'src/dipp_configs.c',
	'src/vmem_storage.c',
	'src/module_config.pb-c.c',
	'src/pipeline_config.pb-c.c',
])

dirs = include_directories('src/include')

csp_dep = dependency('csp', fallback: ['csp', 'csp_dep'])
param_dep = dependency('param', fallback: ['param', 'param_dep'])
proto_c_dep = dependency('libprotobuf-c', fallback: ['protobuf-c', 'proto_c_dep'])
m_dep = meson.get_compiler('c').find_library('m', required : false)
deps = [csp_dep,param_dep,proto_c_dep,m_dep]

c_args = '-DHOSTNAME="@0@"'.format(get_option('hostname'))

dipp = executable('dipp', sources,
	include_directories : dirs,
	dependencies : deps,
	install : true,
	c_args : c_args,
	link_args : ['-ldl'],
)
