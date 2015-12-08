
INCLUDE=/usr/include/
LIBS=/usr/lib/
target:vm_status_monitor



INCLUDE+= -I /opt/vcloud/libvirt/libvirt_API/vm_status_monitor/thirdparty/libev-4.20/install/include
LIBS+= -L /opt/vcloud/libvirt/libvirt_API/vm_status_monitor/thirdparty/libev-4.20/install/lib

vm_status_monitor:thread_pool.c event_center_main.c
	gcc -g -o $@ $< -I ${INCLUDE} -L ${LIBS} -lvirt -lev

