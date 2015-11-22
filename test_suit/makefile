
INCLUDE=/usr/include/
LIBS=/usr/lib/
target:create_kvm vm_lifecycle

create_kvm:create_kvm.c
	gcc -g -o $@ $< -I ${INCLUDE} -L ${LIBS} -lvirt


vm_lifecycle:vm_lifecycle.c
	gcc -g -o $@ $< -I ${INCLUDE} -L ${LIBS} -lvirt
