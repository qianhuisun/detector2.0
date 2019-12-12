import re
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
    # 不匹配两个"之间不匹配空格
    p1 = re.compile('filename = "[^ ]*?"', re.S)
    res = re.findall(p1, line)
    if len(res) == 0:
        return "filename not found"
    # syscall_entry_openat 目前输出中有两个filename 值是一样的，仅返回一个
    filename = re.findall(r'".*"', res[0])
    return filename[0][1:-1]

def convert_time(datetime_str):
    datetime_obj = datetime.strptime("08/12/19 " + datetime_str[:-6], '%d/%m/%y %H:%M:%S.%f')
    time_flt = mktime(datetime_obj.timetuple()) + datetime_obj.microsecond / 1E6
    return time_flt


f1 = open("intergration.log")  # 返回一个文件对象
line1 = f1.readline()  # 调用文件的 readline()方法

f2 = open("output.log", 'w')

event_id = 0

file_set = []
proc_set = []

while line1:
    syscall = get_syscall(line1)
    if syscall == "syscall_entry_openat":
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
print("Done")