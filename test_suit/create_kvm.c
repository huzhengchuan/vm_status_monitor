/**
 * create_kvm.c 
 * create kvm machine based on the simple.xml
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

char * open_file(char *filename)
{
	FILE *fp = NULL;
	int flag = 0;	
	int length = 0;
	char *buffer = NULL;

	fp = fopen(filename, "r+");
	if(NULL == fp)
	{
		printf("open file failed.");
		return NULL;
	}
	
	flag = fseek(fp, 0, SEEK_END);
	if(flag != 0)
	{
		printf("The content in file is valid.\r\n");
		fclose(fp);
		return NULL;
	}
	
	length = ftell(fp);
	buffer = (char *)malloc((length+1)*sizeof(char));
	if(NULL == buffer)
	{
		printf("Allocate buffer failed.");
		fclose(fp);
		return NULL;
	}
	
	memset(buffer, 0x00, (length + 1)*sizeof(char));
	flag = fseek(fp, 0, SEEK_SET);
	if(flag != 0)
	{
		printf("Seek file failed.");
		fclose(fp);
		return NULL;
	}
	
	length = fread(buffer, 1, length, fp);
	fclose(fp);
	return buffer;
}

virDomainPtr create_kvm(virConnectPtr conn, char *xml)
{
	virErrorPtr error = NULL;
	virDomainPtr dom = NULL;

	if(NULL == xml || NULL == conn)
	{
		printf("The xml is invalid para");
		return NULL;
	}

	dom = virDomainDefineXML(conn, xml);
	if(!dom)
	{
		error = virGetLastError();
		fprintf(stderr, "virDomainDefineXML failed:%s!\r\n", error->message);
		virFreeError(error);
		return NULL;
	}
	
	if(virDomainCreate(dom) < 0)
	{
		error = virGetLastError();
		fprintf(stderr, "virDomainCreate failed:%s!\r\n", error->message);
		virDomainUndefine(dom);
		virFreeError(error);
		return NULL;
	}
	return dom;
}


int get_status(virConnectPtr conn, virDomainPtr dom)
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

int main(int argc, char *argv[])
{
	char *xml_buf = NULL;
	virConnectPtr conn;
	virDomainPtr dom;
	char end[64] = {0x00};

	if(argc != 2)
	{
		fprintf(stderr, "parameters are wrong, please checkout!\r\n");
		return -1;
	}
	
	conn = virConnectOpen("qemu:///system");
	if(NULL == conn)
	{
		fprintf(stderr, "Failed to open connection to qemu://system\n");
		return -1;
	}
	
	xml_buf = open_file(argv[1]);
	if(NULL == xml_buf)
	{
		fprintf(stderr, "open failed \n");
		virConnectClose(conn);
		return -1;
	}
	
	dom = create_kvm(conn, xml_buf);
	if(!dom)
	{
		virConnectClose(conn);
		free(xml_buf);
		return -1;
	}
	
	free(xml_buf);

	if(get_status(conn, dom) != 0)
	{
		fprintf(stderr, "get_status failed!");
		virDomainFree(dom);
		virConnectClose(conn);
		return -1;
	}

	do{
		fprintf(stdout, "input quit to end:");
		scanf("%s", end);
	}while(strcmp(end, "quit"));
	
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
