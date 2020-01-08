使用说明:
1. lttng 收集二进制数据
2. babeltrace 生成数据
3. lttng-iolog 生成数据
4. python3 lttng-converter.py iolog_filename babeltrace_filename output_filename

Example:
babeltrace 生成数据在 download_execution/babeltrace.log 
lttng-iolog 生成数据在 download_execution/iolog.log 
运行
	python3 lttng-converter.py download_execution/iolog.log download_execution/babeltrace.log download_execution.log
即可生成输出：
	download_execution.log