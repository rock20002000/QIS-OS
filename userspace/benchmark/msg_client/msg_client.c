/*
 * msg_client.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */

#include <platform/include/platform.h>

int main(int argc, char **argv)
{
        int coid, i;
        char szTmp[1024] = {0};
        char buf[128] = {0};
	path_object_t obj;

        //if (argc < 3) {
        //        fprintf(stderr, "Usage: simple_client <pid> <chid>\n");
        //        return -1;
        //}
        //spid = atoi(argv[1]);
        //chid = atoi(argv[2]);


    	int iRet = PathmgrRequire("/dev/MsgServer", &obj);
    	sprintf(szTmp, "MsgClient get Path: get pid = %d, chid = %x iRet:%d\n", obj.pid, obj.chid, iRet);
    	printf_s(szTmp);
    	if(iRet!=EOK) return iRet;


        if ((coid = ConnectAttach(0, obj.pid, obj.chid, 0, 0)) == -1) {
                perror("ConnectAttach");
                return -1;
        }

        // sent 3 pairs of "Hello" and "Ni Hao"
        for (i = 0; i < 3; i++) {
                sprintf(buf, "Hello");
                printf_s("client: sent '%s'\n", buf);
                if (MsgSend(coid, buf, strlen(buf) + 1, buf, sizeof(buf)) != EOK) {
                        perror("MsgSend");
                        return -1;
                }
                printf_s("client: returned '%s'\n", buf);
                //screen_printf(szTmp);

                sprintf(buf, "Ni Hao");
                printf_s( "client: sent '%s'\n", buf);
                //screen_printf(szTmp);

                if (MsgSend(coid, buf, strlen(buf) + 1, buf, sizeof(buf)) != EOK) {
                        perror("MsgSend");
                        return -1;
                }
                printf_s("client: returned '%s'\n", buf);
                //screen_printf(szTmp);
        }

        // sent a bad message, see if we get an error
        sprintf(buf, "Unknown");
        printf_s("client: sent '%s'\n", buf);
        //screen_printf(szTmp);

        if (MsgSend(coid, buf, strlen(buf) + 1, buf, sizeof(buf)) != EOK) {
                perror("MsgSend");
                return -1;
        }

        ConnectDetach(coid);

        return 0;
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/
