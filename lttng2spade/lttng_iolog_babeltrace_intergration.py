import re


def getParameters(name, line):
    # Qianhui: 这里在name 前加一个空格，防止匹配newfd, oldfd, dfd 等参数
    p1 = re.compile(" " + name + " = -?[0-9]*", re.S)
    res = re.findall(p1, line)
    if len(res) < 1:
        return None
    res = re.findall(r"\d+\.?\d*", res[0])
    return res[0]

def getTimeStamp(line):
    p1 = re.compile("[0-9]*:[0-9]*:[0-9]*.[0-9]*", re.S)
    res = re.findall(p1, line)
    if len(res) < 1:
        return None
    return res[0]


# Qianhui: line1 都改成line_io, line2 都改成 line_bbt, 方便区分

# 读取Lttng Analysis的数据
f1 = open("iolog.log")  # 返回一个文件对象
line_io = f1.readline()  # 调用文件的 readline()方法

# 读取Babel Trace的数据
f2 = open("babeltrace.log")  # 返回一个文件对象
line_bbt = f2.readline()  # 调用文件的 readline()方法

# 写出文件的路径
output_filename = 'intergration.log'
f3 = open(output_filename, 'w')
'''
 遍历的基本思想就是扫描babeltrace和Lttng-log里面的起始时间戳和pid是否匹配，如果匹配，则可以直接将filename拼接到后面去,
 鉴于时间戳的粒度很细，可以跳过fd的比较
 整体的时间复杂度应该可以做到 O(M + N)，M是Babeltrace的日志数量，N是Lttng-iolog的日志数量
 因为 
 1. b_time_stamp < l_start_time
     当前行直接输出（无可拼接项），并且babeltrace 日志往下读一位
 2. b_time_stamp == l_start_time and l_pid == b_pid
     拼接filename
 3. b_time_stamp > l_start_time
     Lttng-iolog向下读一行

重复循环，直到两个文件都读到末尾
'''
# Qianhui: line_io 和 line_bbt 计数，方便debug
line_io_count = 1
line_bbt_count = 1
# Qianhui: 置换了一下 line_bbt(line2) 和 line_io(line1), 因为line2 比较多
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
        #print("")
        b_time_stamp = getTimeStamp(line_bbt)
        b_pid = getParameters("pid", line_bbt)
        b_fd = getParameters("fd", line_bbt)
        
        #print("Now comparing lttng-iolog(%s) and babeltrace(%s)" % (line_io_count, line_bbt_count))
        #print("l_start_time = " + l_start_time)
        #print("l_pid = " + l_pid)
        #print("l_fileName = " + l_fileName)
        # Qianhui: 这里用了str(), 因为None 不能和string 相加
        #print("b_time_stamp = " + str(b_time_stamp))
        #print("b_pid = " + str(b_pid))
        #print("b_fd = " + str(b_fd))

        if b_time_stamp < l_start_time:
            line3 = line_bbt
        elif b_time_stamp > l_start_time:
            break
        elif b_time_stamp == l_start_time:
            if b_pid == l_pid:
                #print("hit")
                line3 = line_bbt[:-3] + ", filename = \"" + l_fileName + "\" }\n"
            else:
                #print("time matched, but pid not matched")
                line3 = line_bbt
        f3.write(line3)
        line_bbt = f2.readline()
        line_bbt_count += 1

    line_io = f1.readline()
    line_io_count += 1
    
# Qianhui: 输出babeltrace 剩余行
while line_bbt:
    #print("")
    b_time_stamp = getTimeStamp(line_bbt)
    b_pid = getParameters("pid", line_bbt)
    b_fd = getParameters("fd", line_bbt)

    #print("Now outputing babeltrace(%s)" % (line_bbt_count))
    #print("b_time_stamp = " + str(b_time_stamp))
    #print("b_pid = " + str(b_pid))
    #print("b_fd = " + str(b_fd))

    line3 = line_bbt
    
    f3.write(line3)
    line_bbt = f2.readline()
    line_bbt_count += 1


f1.close()
f2.close()
f3.close()
print("Done")