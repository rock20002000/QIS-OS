/*
 * msg_server.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */

#include <platform/include/platform.h>
int main()
{
        int chid, rcvid, status;
        char szTmp[1024] 	= {0};
        char buf[128] 		= {0};

		if ((chid = ChannelCreateWithPath(0, "/dev/MsgServer")) == -1) {	//	if ((chid = ChannelCreate(0)) == -1) {
                perror("ChannelCreate");
                return -1;
        }

        sprintf(szTmp, "Server is ready, pid = %d, chid = %x\n", getpid(), chid);
        screen_printf(szTmp);

        for (;;) {
                if ((rcvid = MsgReceive(chid, buf, sizeof(buf), NULL)) == -1) {
                        perror("MsgReceive");
                        return -1;
                }

                sprintf(szTmp, "Server: Received '%s'\n", buf);
                screen_printf(szTmp);

                // Based on what we receive, return some message
                if (strcmp(buf, "Hello") == 0) {
                        MsgReply(rcvid, 0, "World", strlen("World") + 1);
                } else if (strcmp(buf, "Ni Hao") == 0) {
                        MsgReply(rcvid, 0, "Zhong Guo", strlen("Zhong Guo") + 1);
                } else {
                        MsgError(rcvid, EINVAL);
                }
        }

        ChannelDestroy(chid);
        return 0;
}


/*
int main(int argc, char **argv) {
	return 0;
}
*/
