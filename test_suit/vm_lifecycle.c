/**
 * vm_lifecycle.c 
 * vm life cycle demo
 * run shell command:
 * author: zhengchuanhu@gmail.com
 * **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>



#define VCLOUD_FREEMEM(buffer) \
	do{  \
		free(buffer); \
		buffer = NULL; \
	}while(0)


typedef enum _VM_CMD_TYPE{
	VM_CMD_START = 0,
	VM_CMD_SHUTDOWN = 1,
	VM_CMD_STATUS = 2,

	VM_CMD_BUTT,
}VM_CMD_TYPE;


int vm_cmd_start(virConnectPtr conn, virDomainPtr dom)
{
	int flag = -1;
	virErrorPtr error = NULL;

	flag = virDomainCreate(dom);
	if(flag != 0)
	{
		error = virGetLastError();
		printf("virDoaminCreate fialed:%s\r\n", error->message);
		return -1;
	}
	return 0;
}

int vm_cmd_shutdown(virConnectPtr conn, virDomainPtr dom)
{
	int flag = -1;
	virErrorPtr error = NULL;

	flag = virDomainShutdown(dom);
	if(flag != 0)
	{
		error = virGetLastError();
		printf("virDoaminShutdown failed:%s \r\n", error->message);
		return -1;
	}
	return 0;
}

int vm_cmd_status(virConnectPtr conn, virDomainPtr dom)
{
	char *status = NULL;
	virErrorPtr error = NULL;
	int vcpus = 0;
	unsigned long long node_free_memory = 0;
	virNodeInfo nodeinfo;
	virDomainInfo info;
	int id = 0;
	const char *name = NULL;

	/* get the capabilities of conn */
	status = virConnectGetCapabilities(conn);
	if(NULL == status)
	{
		fprintf(stderr, "virConnectGetCapabilities failed %s\r\n", error->message);
		virFreeError(error);
		return -1;
	}
	fprintf(stdout, "Capabilities of connection:%s\r\n", status);
	VCLOUD_FREEMEM(status);

	/* get the hostname reported from conn */
	status = virConnectGetHostname(conn);
	if(NULL == status)
	{
		error = virGetLastError();
		fprintf(stderr, "virConnectGetHostname failed:%s\r\n", error->message);
		virFreeError(error);
		return -1;
	}
	fprintf(stdout, "Connection hostname:%s\r\n", status);
	VCLOUD_FREEMEM(status);

	vcpus = virConnectGetMaxVcpus(conn, NULL);
	if(vcpus < 0)
	{
		error = virGetLastError();
		fprintf(stderr, "virConnectGetMaxVcpus failed:%s\r\n", error->message);
		virFreeError(error);
		return -1;
	}
	fprintf(stdout, "The maxVcpus is %d\r\n", vcpus);

	/* get the amount of free memory avaliable on the node from conn */
	node_free_memory = virNodeGetFreeMemory(conn);
	if(node_free_memory <= 0)
	{
		error = virGetLastError();
		fprintf(stderr, "virNodeGetFreeMemory failed:%s\r\n", error->message);
		virFreeError(error);
		return -1;
	}
	fprintf(stdout, "The node free_memory is %llu\r\n", node_free_memory);
	
	if(virNodeGetInfo(conn, &nodeinfo) < 0)
	{
		error = virGetLastError();
		fprintf(stderr, "virNodeGetInfo failed:%s\r\n", error->message);
		virFreeError(error);
		return -1;
	}

	fprintf(stdout, "Node info from connection\r\n");
	fprintf(stdout, "Model:%s\r\n", nodeinfo.model);
	fprintf(stdout, "Memory size:%lukb\r\n", nodeinfo.memory);
	fprintf(stdout, "Number of CPUs:%d\r\n", nodeinfo.cpus);
	fprintf(stdout, "MHz of CPUs:%d\r\n", nodeinfo.mhz);
	fprintf(stdout, "Number of NUMA nodes:%d\r\n", nodeinfo.nodes);
	fprintf(stdout, "Number of CPU sockets:%u\r\n", nodeinfo.sockets);
	fprintf(stdout, "Number of CPU cores per socket:%d\r\n", nodeinfo.cores);
	fprintf(stdout, "Number of CPU threads per core:%d\r\n", nodeinfo.threads);
	
	id = virDomainGetID(dom);
	name = virDomainGetName(dom);
	fprintf(stdout, "Id:%u  name:%s\r\n", id, name);

	if(virDomainGetInfo(dom, &info) < 0)
	{
		error = virGetLastError();
		fprintf(stderr, "virDomainGetInfo failed:%s\r\n", error->message);
		virFreeError(error);
		return -1;
	}
	fprintf(stdout, "status:%d\r\n", info.state);
	return 0;
}

#define ASSERT_ISNULL_RETURN(cmdStr, returnERR) \
	do{                                         \
		if(NULL == cmdStr)                      \
		{                                       \
			return returnERR;                   \
		}                                       \
	}while(0)                                  


VM_CMD_TYPE cmdStrToType(const char *cmdStr)
{
	ASSERT_ISNULL_RETURN(cmdStr, VM_CMD_BUTT);
	
	if(!strcmp(cmdStr, "start") || !strcmp(cmdStr, "START"))
	{
		return VM_CMD_START;
	}
	if(!strcmp(cmdStr, "shutdown") || !strcmp(cmdStr, "SHUTDOWN"))
	{
		return VM_CMD_SHUTDOWN;
	}
	if(!strcmp(cmdStr, "status") || !strcmp(cmdStr, "STATUS"))
	{
		return VM_CMD_STATUS;
	}

	return VM_CMD_BUTT; 

}
int main(int argc, char *argv[])
{
	virConnectPtr conn;
	virDomainPtr dom;
	VM_CMD_TYPE cmdType = VM_CMD_BUTT;
	char cmdStr[64] = {0x00};

	if(argc != 3)
	{
		printf("Usage:./vm_lifecycle guestos_name \{status|start|shutdown\}\r\n");
		return -1;
	}
	
	conn = virConnectOpen("qemu:///system");
	if(NULL == conn)
	{
		fprintf(stderr, "Failed to open connection to qemu://system\n");
		return -1;
	}
	
	dom = virDomainLookupByName(conn, argv[1]);
	if(NULL == dom)
	{
		printf("virDomainLookUpByName failed.");
		virConnectOpen(conn);
		return -1;
	}
	
	strncpy(cmdStr, argv[2], 63);

BEGIN:
	
	cmdType = cmdStrToType(cmdStr);
	switch(cmdType)
	{
		case VM_CMD_START:
			vm_cmd_start(conn, dom);
			break;
		case VM_CMD_SHUTDOWN:
			vm_cmd_shutdown(conn, dom);
			break;
		case VM_CMD_STATUS:
			vm_cmd_status(conn, dom);
			break;
		default:
			printf("The cmd is valid!");
			break;
	}

	do{
		printf("input cmd[start|shutdown|status|quit]:");
		scanf("%s", cmdStr);
		if(!strcmp(cmdStr, "quit"))
		{
			printf("Thanks, Good Bye");
			goto END;
		}
		goto BEGIN;
	}while(0);

END:
	if(dom != NULL) 
	{
	//	virDomainFree(dom);
		virDomainDestroy(dom);
	}
	if(conn != NULL)
	{
		virConnectClose(conn);
	}
	return 0;
}
