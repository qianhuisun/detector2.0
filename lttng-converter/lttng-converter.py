import re
import os
from time import *
from datetime import *

def get_timeStamp(line):
    p1 = re.compile("[0-9]*:[0-9]*:[0-9]*.[0-9]*", re.S)
    res = re.findall(p1, line)
    if len(res) < 1:
        return None
    return res[0]

def get_digital_parameters(name, line):
    p1 = re.compile(" " + name + " = -?[0-9]*", re.S)
    res = re.findall(p1, line)
    if len(res) < 1:
        return None
    res = re.findall(r"\d+\.?\d*", res[0])
    return res[0]

def get_syscall(line):
    str_list = re.split('\[|\,|\]|\s+', line)
    return str_list[5][0:-1]
    
def get_filename(line):
    # "之间不匹配空格
    p1 = re.compile('filename = "[^ ]*?"', re.S)
    res = re.findall(p1, line)
    if len(res) == 0:
        return "filename not found"
    # syscall_entry_openat 目前输出中有两个filename 值是一样的，仅返回第一个
    filename = re.findall(r'".*"', res[0])[0][1:-1]
    # 去掉开头可能存在的 ./
    if filename.startswith("./"):
        filename = filename[2:]
    return filename

def convert_time(datetime_str):
    datetime_obj = datetime.strptime("08/12/19 " + datetime_str[:-6], '%d/%m/%y %H:%M:%S.%f')
    time_flt = mktime(datetime_obj.timetuple()) + datetime_obj.microsecond / 1E6
    return time_flt


# 从lttng-iolog 输出中提取filename 匹配到babeltrace 输出中的fd, 生成临时文件intergration.log
def lttng_iolog_babeltrace_intergration():
    # 读取lttng-iolog 的数据
    f1 = open(sys.argv[1]) 
    line_io = f1.readline()

    # 读取babeltrace 的数据
    f2 = open(sys.argv[2]) 
    line_bbt = f2.readline()

    # 写出文件的路径
    output_filename = 'intergration.log'
    f3 = open(output_filename, 'w')

    # 对能在lttng-iolog 匹配到时间戳的行，加上filename 参数，否则保留原行
    while line_io:
        str_list = re.split('\[|\,|\]|\s+', line_io)
        length = len(str_list)
        l_start_time = ""
        l_pid = ""
        l_fileName = ""
        if length == 13:
            l_start_time = str_list[1]
            l_pid = str_list[9]
            l_fileName = str_list[10]
        elif length == 14:
            l_start_time = str_list[1]
            l_pid = str_list[10]
            l_fileName = str_list[11]
        elif length == 15:
            l_start_time = str_list[1]
            l_pid = str_list[9]
            l_fileName = str_list[10] + str_list[11]
        elif length == 16:
            l_start_time = str_list[1]
            l_pid = str_list[10]
            l_fileName = str_list[11] + str_list[12]
        
        while line_bbt:
            b_time_stamp = get_timeStamp(line_bbt)
            b_pid = get_digital_parameters("pid", line_bbt)
            b_fd = get_digital_parameters("fd", line_bbt)
            
            if b_time_stamp < l_start_time:
                line3 = line_bbt
            elif b_time_stamp > l_start_time:
                break
            elif b_time_stamp == l_start_time:
                if b_pid == l_pid:
                    line3 = line_bbt[:-3] + ", filename = \"" + l_fileName + "\" }\n"
                else:
                    line3 = line_bbt
            f3.write(line3)
            line_bbt = f2.readline()

        line_io = f1.readline()
        
    # 输出babeltrace 剩余行
    while line_bbt:
        b_time_stamp = get_timeStamp(line_bbt)
        b_pid = get_digital_parameters("pid", line_bbt)
        b_fd = get_digital_parameters("fd", line_bbt)

        line3 = line_bbt
        
        f3.write(line3)
        line_bbt = f2.readline()

    f1.close()
    f2.close()
    f3.close()
    print("Step1: lttng_iolog_babeltrace_intergration Done\n")


# 从lttng_iolog_babeltrace_intergration() 生成的临时文件intergration.log 中提取点边
def lttng_intergration_spade():

    f1 = open("intergration.log")
    line1 = f1.readline()

    f2 = open(sys.argv[3], 'w')

    event_id = 0

    file_set = []
    proc_set = []

    while line1:
        syscall = get_syscall(line1)
        # 提取命令行
        if syscall == "syscall_entry_execve":
            filename = get_filename(line1)
            pid = get_digital_parameters("pid", line1)
            time_str = get_timeStamp(line1)
            time_flt = convert_time(time_str)
            # write this proc vertex line because it may contains cmd line
            line2 = "{ID=%s, command line=%s, egid=0, euid=0, gid=0, pid=%s, ppid=0, seen time=%.3f, source=syscall, type=Process, uid=0, unit=0}\n" % (pid, filename, pid, time_flt)
            f2.write(line2)
            if pid not in proc_set:
                proc_set.append(pid)
        # 目前只提取 open read write 三种边
        elif syscall == "syscall_entry_openat":
            operation = "open"
            _type = "Used"
            filename = get_filename(line1)
            if filename == "filename not found":
                line1 = f1.readline()
                continue
            dest_id = hash(filename)
            pid = get_digital_parameters("pid", line1)
            src_id = pid
            time_str = get_timeStamp(line1)
            time_flt = convert_time(time_str)
            # add file vertex line when filename is not in file_set
            if filename not in file_set:
                file_set.append(filename)
                line2 = "{ID=%s, epoch=0, path=%s, source=syscall, subtype=file, type=Artifact}\n" % (dest_id, filename)
                f2.write(line2)
            # add proc vertex line when pid is not in proc_set
            if pid not in proc_set:
                proc_set.append(pid)
                line2 = "{ID=%s, egid=0, euid=0, gid=0, pid=%s, ppid=0, seen time=%.3f, source=syscall, type=Process, uid=0, unit=0}\n" % (pid, pid, time_flt)
                f2.write(line2)
            event_id += 1
            line2 = "{destID=%s, event id =%s, operation=%s, source=syscall, srcID=%s, time=%.3f, type=%s}\n" % (dest_id, str(event_id), operation, src_id, time_flt, _type)
            f2.write(line2)
        elif syscall == "syscall_entry_read":
            operation = "read"
            _type = "Used"
            filename = get_filename(line1)
            if filename == "filename not found":
                line1 = f1.readline()
                continue
            dest_id = hash(filename)
            pid = get_digital_parameters("pid", line1)
            src_id = pid
            time_str = get_timeStamp(line1)
            time_flt = convert_time(time_str)
            # add file vertex line when filename is not in file_set
            if filename not in file_set:
                file_set.append(filename)
                line2 = "{ID=%s, epoch=0, path=%s, source=syscall, subtype=unknown, type=Artifact}\n" % (dest_id, filename)
                f2.write(line2)
            # add proc vertex line when pid is not in proc_set
            if pid not in proc_set:
                proc_set.append(pid)
                line2 = "{ID=%s, egid=0, euid=0, gid=0, pid=%s, ppid=0, seen time=%.3f, source=syscall, type=Process, uid=0, unit=0}\n" % (pid, pid, time_flt)
                f2.write(line2)
            event_id += 1
            line2 = "{destID=%s, event id =%s, operation=%s, source=syscall, srcID=%s, time=%.3f, type=%s}\n" % (dest_id, str(event_id), operation, src_id, time_flt, _type)
            f2.write(line2)
        elif syscall == "syscall_entry_write":
            operation = "write"
            _type = "WasGeneratedBy"
            filename = get_filename(line1)
            if filename == "filename not found":
                line1 = f1.readline()
                continue
            src_id = hash(filename)
            pid = get_digital_parameters("pid", line1)
            dest_id = pid
            time_str = get_timeStamp(line1)
            time_flt = convert_time(time_str)
            # add file vertex line when filename is not in file_set
            if filename not in file_set:
                file_set.append(filename)
                line2 = "{ID=%s, epoch=0, path=%s, source=syscall, subtype=file, type=Artifact}\n" % (src_id, filename)
                f2.write(line2)
            # add proc vertex line when pid is not in proc_set
            if pid not in proc_set:
                proc_set.append(pid)
                line2 = "{ID=%s, egid=0, euid=0, gid=0, pid=%s, ppid=0, seen time=%.3f, source=syscall, type=Process, uid=0, unit=0}\n" % (pid, pid, time_flt)
                f2.write(line2)
            event_id += 1
            line2 = "{destID=%s, event id =%s, operation=%s, source=syscall, srcID=%s, time=%.3f, type=%s}\n" % (dest_id, str(event_id), operation, src_id, time_flt, _type)
            f2.write(line2)
        line1 = f1.readline()

    f1.close()
    f2.close()
    os.remove("intergration.log")
    print("Step2: lttng_intergration_spade Done\n")


def convert_lttng_to_spade():
    if len(sys.argv) != 4:
        print("Usage:\n\tpython3 lttng-converter.py iolog_filename babeltrace_filename output_filename\n")
        return
    lttng_iolog_babeltrace_intergration()
    lttng_intergration_spade()
    print("Done")


if __name__ == "__main__":
    convert_lttng_to_spade()