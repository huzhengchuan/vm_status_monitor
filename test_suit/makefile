
INCLUDE=/usr/include/
LIBS=/usr/lib/
target:create_kvm vm_lifecycle msg_task msg_task_0

create_kvm:create_kvm.c
	gcc -g -o $@ $< -I ${INCLUDE} -L ${LIBS} -lvirt


vm_lifecycle:vm_lifecycle.c
	gcc -g -o $@ $< -I ${INCLUDE} -L ${LIBS} -lvirt


INCLUDE+= -I /opt/vcloud/libvirt/libvirt_API/vm_status_monitor/thirdparty/libev-4.20/install/include
LIBS+= -L /opt/vcloud/libvirt/libvirt_API/vm_status_monitor/thirdparty/libev-4.20/install/lib

msg_task:msg_task.c
	gcc -g -o $@ $< -I ${INCLUDE} -L ${LIBS} -lvirt -lev

msg_task_0:msg_task_0.c
	gcc -g -o $@ $< -I ${INCLUDE} -L ${LIBS} -lvirt -lev
