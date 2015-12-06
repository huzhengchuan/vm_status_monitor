#include <stdio.h>
#include <ev.h>




ev_io stdin_watcher;
ev_timer timeout_watcher;

void stdin_cb(EV_P_ ev_io *w, int revents)
{
	puts("stdin ready");
	char buf[128] = {0x00};

	int index = 0;
	
	fgets(buf, 128, stdin);

	printf("the end %s...........\r\n", buf);

	//	ev_io_stop(EV_A_ w);

	ev_break(EV_A_ EVBREAK_CANCEL);
}

void timeout_cb(EV_P_ ev_timer *w, int revents)
{
	puts("timeout");

	ev_break(EV_A_ EVBREAK_CANCEL);
}

int main()
{
	struct ev_loop *loop = EV_DEFAULT;

	ev_io_init(&stdin_watcher, stdin_cb, 0, EV_READ);
	ev_io_start(loop, &stdin_watcher);

	ev_timer_init(&timeout_watcher, timeout_cb, 2, 0);
	ev_timer_start(loop, &timeout_watcher);

	ev_run(loop, 0);

	//break was called, so exit
	return 0;

}
