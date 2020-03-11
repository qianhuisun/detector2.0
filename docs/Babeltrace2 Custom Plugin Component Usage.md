# [Babeltrace2](https://babeltrace.org/) Custom Plugin Usage

## Description

A Custom Plugin (called [csobj](http://118.126.94.181/xlh/linux-collector/blob/tcp_send/csobj.c)
) of Babeltrace2 is designed to analyze online LTTng traces and send custom event objects to the collector through TCP connection.

## Details

### Step 1. Install Babeltrace2

#### 1. source

* Tarball: [Babeltrace 2.0.0](https://www.efficios.com/files/babeltrace/babeltrace-2.0.0.tar.bz2) (recommended)
* Github: [Babeltrace 2.0](https://github.com/efficios/babeltrace/tree/master)

#### 2. build and install

```cmd
$ BABELTRACE_DEV_MODE=1 BABELTRACE_MINIMAL_LOG_LEVEL=TRACE ./configure
$ make
$ make install
```

> Some libraries may be missed on your machine. Please check the error message to "configure" if any.

### Step 2. Compile Custom Plugin of Babeltrace2

#### 1. source

* [csobj.c](http://118.126.94.181/xlh/linux-collector/blob/tcp_send/csobj.c)

> This component can be represented as ***sink.object.csobj*** where **sink is the component type, object is the plugin name, csobj is the component name**.

#### 2. build the plugin

```cmd
$ cc csobj.c -fPIC -c $(pkg-config --cflags babeltrace2)
$ ld csobj.o -o csobj.so -shared $(pkg-config --libs babeltrace2)
```

This will create a share object csobj.so in the current folder.

### Step 3. Run Babeltrace2 with Custom Plugin Component csobj

For example, the compiled file csobj.so and the executable server is in the current folder.

```cmd
$ lttng create sample-session --live
$ lttng enable-event --kernel --all
$ lttng add-context -k -t tid
```
If you don't know your host name or session name, use this command:
```cmd
$ babeltrace2 --input-format=lttng-live net://localhost
```
For example, my hostname is **toby-VirtualBox** and my session name is **qianhui-session**. So I use the following commands to run the server (**1st**), Babeltrace2 with custom component (2nd/3rd), and Lttng live (3rd/2nd) sequentially:
```cmd
$ ./server
$ babeltrace2 --plugin-path=. --input-format=lttng-live net://localhost/host/toby-VirtualBox/qianhui-session --component=sink.object.csobj
$ lttng start
```
To stop and destroy lttng:
```cmd
lttng destroy
```

> 1. The [server.c](http://118.126.94.181/xlh/linux-collector/blob/tcp_send/server.c) must be running before running the Babeltrace2 command above. It is hardcoded to listen on 127.0.0.1:5026.
> 2. Since it's LTTng live usage, an LTTng live session must be __created__ before running the Babeltrace2 command above. [LTTng live details](https://lttng.org/docs/v2.11/#doc-lttng-live)
> 3. The Babeltrace2 command will first connect to 127.0.0.1:5026 (hardcoded in csobj.c) and then receive traces from LTTng live and send event objects to the server.

### Receive event objects

Here is the example of receiving event objects: [server.c](http://118.126.94.181/xlh/linux-collector/blob/tcp_send/server.c)