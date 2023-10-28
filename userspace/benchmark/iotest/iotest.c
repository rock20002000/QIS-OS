
#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>


#define RESOURCE_NAME "/dev/hello"
#define APP_NAME 	  "app_dev"

const size_t BUF_MAX = 200;
int fd;

char *str_wr;

char buf[1024];


bool simple_test(void);
bool overflow_test(void);
bool boundary_test(void);
bool buffer_full_test(void);
bool alternate_rw(void);
bool dataIntegrityTest(void);
bool otherCharacterTest(void);
bool strIsEqual(char *strWrite,char *strRead,unsigned int len);
void errorTest();


int main(int argc, char *argv[])
{
	int i = 0;
	int mode = 0;

	printf_s("Welcome to the test tool for dev.\n");

	// 1. open the device.
	fd = open(RESOURCE_NAME, FILEOPEN_READWRITE);
	if (fd < 0)
	{
		perror( RESOURCE_NAME );
		exit(ERR_NOT_FOUND);//EXIT_FAILURE);
	}
	else
	{
		printf_s(APP_NAME "device opened successfully\n");
	}


#if 0
	for( i = 0; i < 300 ; i++)
	{
		bool flag[7] = {0};
		bool success = true;

		memset(buf, 0, sizeof(buf));
		flag[0] = simple_test();
		success = success && flag[0];

		memset(buf, 0, sizeof(buf));
		flag[1] = overflow_test();
		success = success && flag[1];

		memset(buf, 0, sizeof(buf));
		flag[2] = boundary_test();
		success = success && flag[2];

		memset(buf, 0, sizeof(buf));
		flag[3] = buffer_full_test();
		success = success && flag[3];

		memset(buf, 0, sizeof(buf));
		flag[4] = alternate_rw();
		success = success && flag[4];

		memset(buf, 0, sizeof(buf));
		flag[5] = dataIntegrityTest();
		success = success && flag[5];

		memset(buf, 0, sizeof(buf));
		flag[6] = otherCharacterTest();
		success = success && flag[6];
		if (!success)
		{
			printf("There must be a problem.\n");
			printf("Cycle index is %d.\n",i);
			break;
		}
	}
	printf("--------ALL PASSED--------- \n");
#endif


#if 1
	if (argc <= 1) {
		simple_test();
	} else if (argc <= 2) {
		mode = atoi(argv[1]);

		memset(buf, 0, sizeof(buf));
		read(fd, buf, 200);

		switch (mode) 
		{
		case 2:
			overflow_test();
			break;
		case 3:
			boundary_test();
			break;
		case 4:
			buffer_full_test();
			break;
		case 5:
			alternate_rw();
			break;
		case 9:
			dataIntegrityTest();
			break;
		case 10:
			otherCharacterTest();
			break;
		default:
			printf("Error\n");
			break;
		}
	} else {
		printf("Error\n");
	}

#endif
	close(fd);
	return EOK;	//	EXIT_SUCCESS;
}

char temp[7] = "abcd123";

bool simple_test(void)
{
	int ret = 0;
	str_wr = "abcd";
	bool success = false;
	char szTmp[1024] = {0};

	printf_s(APP_NAME ":Simple Write Test.\n");
	ret = write(fd, str_wr, strlen(str_wr));
	sprintf(szTmp, "return value:%d send length:%d\n", ret, strlen(str_wr));
	printf_s(szTmp);

	if (ret != strlen(str_wr))
	{
		printf_s(APP_NAME ": Step1 :: Write Fail.\n");
		return success;
	}
	else
	{
		printf_s(APP_NAME ": Step1 :: Write OK.\r\n");
	}

	str_wr = "123";
	ret = write(fd, str_wr, strlen(str_wr));
	if (ret != strlen(str_wr))
	{
		printf_s(APP_NAME ": Step2 :: Write Fail\r\n");
		return success;
	}
	else
	{
		printf_s(APP_NAME ": Step2 :: Write OK.\r\n");
	}

	printf(APP_NAME "Simple Read Test.\n");
	memset(buf, 0, sizeof(buf));
	
	ret = read(fd, buf, 5);
	if (ret < 0) {
		printf_s(APP_NAME ":Step3:: Read fail\n");
	}
	else
	{
		if (ret != 5)
		{
			printf_s(APP_NAME ": Step3:: Read Error\r\n");
			return success;
		}
		else
		{
			success = strIsEqual(temp,buf,ret);
			if(!success)
			{
				return success;
			}
			printf_s(APP_NAME ": Step3:: Read OK\r\n");
		}
	}

	memset(buf, 0, sizeof(buf));
	
	ret = read(fd, buf, 3);
	if (ret < 0)
	{
		printf_s(APP_NAME ":Step4:: Read fail\n");
	}
	else
	{
		success = strIsEqual(temp+5,buf,ret);
		if(!success)
		{
			return success;
		}
		success = true;
		printf_s(APP_NAME ": Step3:: Read OK\r\n");
	}
	printf_s( "TEST CASE PASSED simple_test\r\n");
	return success;
}


bool overflow_test(void)
{
	int ret = 0;
	str_wr = "abcd";
	int index = 0;
	bool success = false;

	printf_s(APP_NAME ":Overflow Write Test.\n");
	ret = write(fd, str_wr, strlen(str_wr));

	if (ret < 0)
	{
		printf_s(APP_NAME ":Step 1 :: Write fail\n");
		return success;
	}
	else
	{
		printf_s(APP_NAME ":Step 1 :: Write OK.\r\n");
	}
	str_wr = malloc(BUF_MAX);
	if(str_wr==NULL)
	{
		printf_s(APP_NAME ":Step 1 :: memory allocation error \r\n");
		exit(1);
	}

	for (index = 0; index < 20; index++)
	{
		memcpy(str_wr + (index * 10), "01234abcde", 10);
	}

	ret = write(fd, str_wr, 196);
	if (ret < 0)
	{
		printf_s(APP_NAME ":Step 2 :: Write Fail\r\n");
		printf_s(APP_NAME "data length write to buffer %d\n",ret);
		return success;
	}
	else
	{
		if (ret != 196)
		{
			printf_s(APP_NAME ":Step 2 :: Write Error\r\n");
			return success;
		}
		else
		{
			printf_s(APP_NAME ":Step 2 :: Write OK\r\n");
		}
	}

	memset(buf, 0, sizeof(buf));

	ret = read(fd, buf, 4);
	if (ret < 0)
	{
		printf_s(APP_NAME ":Step 3 :: Read Fail\r\n");
		return success;
	}
	else
	{
		char *strTemp = "abcd";
		success = strIsEqual(strTemp,buf,ret);
		if(!success)
		{
			return success;
		}
		printf_s(APP_NAME ": Step 3 :: Read OK\r\n");
	}

	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, 197);
	if (ret < 0)
	{
		printf_s(APP_NAME ":Step4 :: Read Fail\r\n");
		return success;
	}
	else
	{
		if ( ret == 196)
		{
			success = strIsEqual(str_wr,buf,ret);
			if(!success)
			{
				return success;
			}
			printf_s(APP_NAME ":Step4 :: Read OK\r\n");
		}
		else
		{
			return success;
			printf_s(APP_NAME ":Step4 :: Read Error\r\n");
		}
	}

	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, 10);
	if (ret != 0)
	{
		printf_s(APP_NAME "Step 5: Read Fail.\n");
	}
	else
	{
		printf_s(APP_NAME "Step 5: Read OK.\r\n");
		success = true;
	}

	free(str_wr);
	str_wr = NULL;
	printf_s("TEST CASE PASSED overflow_test\r\n");
	return success;
}


bool boundary_test(void)
{
	int ret = 0;
	int index = 0;
	bool success = false;
	printf_s(APP_NAME ":Boundary test\n");

	str_wr = malloc(BUF_MAX);
	if(str_wr==NULL)
	{
		printf_s(APP_NAME ":Step 1 :: memory allocation error \r\n");
		exit(1);
	}

	for (index = 0; index < 20; index++)
	{
		memcpy(str_wr + (index * 10), "abcdefghij", 10);
	}

	ret = write(fd, str_wr, 50);
	if (ret < 0)
	{
		printf_s(APP_NAME ": Step 1: Write Fail.\r\n");
		return success;
	}
	else
	{
		if (ret != 50)
		{
			printf_s(APP_NAME ": Step 1: Write Fail.\r\n");
			return success;
		}
		else
		{
			printf(APP_NAME ": Step 1: Write OK.\r\n");
		}
	}

	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, 200);
	if (ret < 0)
	{
		printf_s(APP_NAME ":Step 2:: Read 200 bytes fail.\n");
		return success;
	}
	else
	{
		if (ret != 50)
		{
			printf_s (APP_NAME ": Step 2 :: Read Error\r\n");
			return success;
		}
		else
		{
			success = strIsEqual(str_wr,buf,ret);
			if(!success)
			{
				return success;
			}
			printf_s (APP_NAME ": Step 2 :: Read OK\r\n");
		}
	}

	memset(str_wr, 0, sizeof(str_wr));
	if (str_wr != NULL)
	{
		free(str_wr);
		str_wr = NULL;
	}

	str_wr = "abc";
	ret = write(fd, str_wr, 3);
	if(ret < 0 )
	{
		printf_s(APP_NAME ":Step3 :: Write Fail\r\n");
		return success;
	}
	else
	{
		if (ret != 3)
		{
			printf_s(APP_NAME ": Step3:: Write Error.\r\n");
			return success;
		}
		else
		{
			printf_s(APP_NAME ": Step3:: Write OK.\r\n");
		}
	}

	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, 0);
	if (ret != 0)
	{
		printf_s(APP_NAME ": Step4 :: Read Fail.\r\n");
		return success;
	}
	else
	{
		printf_s(APP_NAME ": Step4 :: Read OK.\r\n");
	}

	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, 4);
	if (ret < 0)
	{
		printf_s(APP_NAME ": Step5 :: Read Fail.\r\n");
		return success;

	}
	else
	{
		if (ret != 3)
		{
			printf_s(APP_NAME ": Step5 :: Read Error.\r\n");
			return success;
		}
		else
		{
			success = strIsEqual(str_wr,buf,ret);
			if(!success)
			{
				return success;
			}
			success = true;
			printf_s(APP_NAME ": Step5 :: Read OK.\r\n");
		}
	}
	printf_s("TEST CASE PASSED boundary_test\r\n");
	return success;
}

bool buffer_full_test(void)
{
	int index = 0;
	int ret = 0;
	bool success = false;

	printf(APP_NAME "buffer_full_test.\n");

	char str_tmp[] = "abcd";

	ret = write(fd, str_tmp, strlen(str_tmp));
	if (ret< 0) 
	{
		printf_s(APP_NAME ":Step1 :: Write abcd failed.\r\n");
		return success;
	} 
	else 
	{
		if (ret != 4) 
		{
			printf_s(APP_NAME ":Step1 :: Write abcd failed.\r\n");
			return success;
		} else 
		{
			printf_s(APP_NAME ":Step1 :: Write abcd OK.\r\n");
		}
	}	
	str_wr = malloc(BUF_MAX);
	if(str_wr == NULL)
	{
		printf_s(APP_NAME ":Step 1 :: memory allocation error \r\n");
		exit(1);
	}
	for (index = 0; index < 20; index++) 
	{
		memcpy(str_wr + (index * 10), "abcdefghij", 10);
	}

	ret = write(fd, str_wr, 200);
	if (ret < 0) 
	{
		printf_s(APP_NAME ": Step2 :: Write Fail\r\n");
		return success;
	} 
	else 
	{
		if (ret != 196) 
		{
			printf_s(APP_NAME ": Step2 :: Write Error\r\n");
			return success;
		} 
		else 
		{
			printf_s(APP_NAME ": Step2 :: Write OK\r\n");
		}
	}	

	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, BUF_MAX);
	if (ret < 0) 
	{
		printf_s(APP_NAME ": Step3 :: Read Fail.\r\n");
		return success;
	} 
	else 
	{
		if (ret != 200) 
		{
			printf_s(APP_NAME ": Step3 :: Read Error.\r\n");
		} 
		else 
		{
			char strtmp[250] = "";
			char str[10] = "abcd";
			strncpy(strtmp, str, 4);
			strcat(strtmp, str_wr);
			success = strIsEqual(strtmp,buf,ret);
			if (!success)
			{
				return success;
			}
			printf_s(APP_NAME ": Step3 :: Write OK.\r\n");
		}
	}

	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, 4);
	if (ret < 0) 
	{
		printf_s(APP_NAME ":Step4 :: Read Error.\r\n");
		return success;

	} else 
	{
		if (ret != 0) 
		{
			printf_s(APP_NAME ": Step4 :: Read Error.\r\n");
			return success;
		} 
		else 
		{
			printf_s(APP_NAME ": Step4 :: Read OK.\r\n");
			success = true;
		}
	}

	if (str_wr != NULL)
	{
		free(str_wr);
		str_wr = NULL;
	}
	printf_s("TEST CASE PASSED buffer_full_test\r\n");
	return success;
}

bool alternate_rw(void)
{
	int index = 0;
	int ret = 0;
	bool success = false;
	char data1[110] = "";
	printf_s(APP_NAME ":alternate test.\n");

	str_wr = malloc(BUF_MAX);
	if (str_wr == NULL)
	{
		printf_s(APP_NAME ":Step 1 :: memory allocation error \r\n");
		exit(1);
	}
	for (index = 0; index < 20; index++) 
	{
		memcpy(str_wr + (index * 10), "abcdefghij", 10);
		memcpy(data1 + (index * 5), "A1B2C", 5);
	}

	ret = write(fd, data1, 100);
	if (ret < 0) 
	{
		printf_s(APP_NAME ": Step1: Write Fail.\n");
		return false;
	}
	else 
	{
		if (ret != 100) 
		{
			printf_s(APP_NAME ":Step1 Write Error.\n");
			return success;
		} 
		else 
		{
			printf_s(APP_NAME ":Step1 Write OK.\n");
		}
	}

	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, 50);
	if (ret < 0) 
	{
		printf_s(APP_NAME ": Step2 :: Read Fail.\r\n");
		return success;
	}
	else 
	{
		if (ret != 50)
		{
			return success;
		}

		success = strIsEqual(data1,buf,50);
		if (!success)
		{
			return success;
		}		
		printf_s(APP_NAME ": Step2 :: Read OK.\r\n");
	}

	ret = write(fd, str_wr, BUF_MAX);
	if (ret < 0) 
	{
		printf_s(APP_NAME ": Step3 :: Write Fail\r\n");
		return success;
	}
	else 
	{
		if (ret != 150) 
		{
			printf_s(APP_NAME ": Step3 :: Write Error\r\n");
			return success;
		}
		else 
		{
			printf_s(APP_NAME ": Step3 ::Write OK.\r\n");
		}
	}

	memset(buf, 0, sizeof(buf));

	ret = read(fd, buf, 201);
	if (ret < 0) 
	{
		printf_s(APP_NAME ": Step4 :: Read Fail\r\n");
		return success;
	}
	else 
	{	
		char strdest[320] = "";
		strncpy(strdest,data1 + 50 , 50);
		strcat(strdest, str_wr);
		success = strIsEqual(strdest,buf,ret);
		if (!success)
		{
			return success;
		}
		printf_s(APP_NAME ": Step4 :: Read OK.\r\n");
	}

	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, 3);
	if (ret < 0) 
	{
		printf_s(APP_NAME ": Step5:: Read 3 bytes fail.\n");
		return success;
	}
	else 
	{
		if (ret != 0) 
		{
			printf_s(APP_NAME ": Step5:: Read 3 Byte Error.\r\n");
			return success;
		} 
		else 
		{
			printf_s(APP_NAME ": Step5:: Read 3 Byte OK.\r\n");
			success = true;
		}
	}
	if (str_wr != NULL)
	{
		free(str_wr);
		str_wr = NULL;
	}
	printf_s("TEST CASE PASSED alternate_rw\r\n");
	return success;
}


bool dataIntegrityTest(void)
{
	int index = 0;
	int ret;
	bool success = false;
	// attention: malloc result should be checked.
	printf_s(APP_NAME ":dataIntegrityTest.\n");
	str_wr = malloc(BUF_MAX);
	if (str_wr == NULL)
	{
		printf_s(APP_NAME ":Step 1 :: memory allocation error \r\n");
		exit(1);
	}

	for (index = 0; index < 20; index++) 
	{
		memcpy(str_wr + (index * 10), "abcdefghij", 10);
	}

	do 
	{
		ret = write(fd, str_wr, 200);
		if (ret < 0) 
		{
			printf_s(APP_NAME ": Step1: Write Fail.\n");
			return success;
		} 
		else 
		{
			if (ret != 200) 
			{
				printf_s(APP_NAME ":Step1 Write Error.\n");
				printf_s("cycle index %d\n", index);
				return success;
			} 
			else 
			{
				printf_s(APP_NAME ":Step1 Write OK.\n");
			}
		}

		memset(buf, 0, sizeof(buf));
		ret = read(fd, buf, 201);
		if (ret < 0) 
		{
			printf_s(APP_NAME ": Step5:: Read 3 bytes fail.\n");
			return success;
		} 
		else 
		{
			if (ret != 200) 
			{
				printf_s(APP_NAME ": Step5:: Read 3 Byte Error.\r\n");
				return success;
			} 
			else 
			{
				success = strIsEqual(str_wr,buf,200);
				if (!success)
				{
					return success;
				}
				printf_s(APP_NAME ": Step5:: Read  Byte OK.\r\n");
			}
		}
		index++;
	} while(index < 20000);
	success = true;
	if (str_wr != NULL)
	{
		free(str_wr);
		str_wr = NULL;
	}
	printf_s("TEST CASE PASSED dataIntegrityTest\r\n");
	return success;
}

bool otherCharacterTest(void)
{
	int index = 0;
	int ret;
	int count = 0;
	bool success = false;
	char strtmp[] = "���ֺ������ַ�����\01234566()*&^%$#@!~`.,':|[]{}-����/-+";


	printf_s(APP_NAME ":otherCharacterTest.\n");

	str_wr = malloc(200);
	if (str_wr == NULL)
	{
		printf_s(APP_NAME ":Step 1 :: memory allocation error \r\n");
		exit(1);
	}

	int n = strlen(strtmp) - 1;
	int m = 200 / n;
	memset(str_wr,0,sizeof(str_wr));
	for (index = 0; index < m; index++)
		{
			memcpy(str_wr + (index * n), strtmp, n);
		}

	for (index = 0; index < m; index++)
	{
		memcpy(str_wr + (index * n), strtmp, n);
	}
	
	srand(1);

	count = rand() % 200 + 1;
	do {
		count = rand() % 200 + 1;
		ret = write(fd, str_wr, count);
		if (ret < 0) 
		{
			printf_s(APP_NAME ": Step1: Write Fail.\n");
			return success;
		} 
		else 
		{
			if (ret != count) 
			{
				printf_s(APP_NAME ":Step1 Write Error.\n");
				return success;
			} 
			else 
			{
				printf_s(APP_NAME "cycle index.\n",index);
			}
		}

		memset(buf, 0, sizeof(buf));
		ret = read(fd, buf, count);
		if (ret < 0) 
		{
			printf_s(APP_NAME ": Step2:: Read  bytes fail.\n");
			return success;
		} 
		else
		{
			if (ret != count) 
			{
				printf_s(APP_NAME ": Step2:: Read Byte Error.\r\n");
				return success;
			}
			else 
			{
				success = strIsEqual(str_wr, buf,count);
				if (!success)
				{
					return success;
				}
				printf_s(APP_NAME ": Step2:: Read  Byte OK.\r\n");
			}
		}

		index++;
	} while (index < 1000);
	success = true;
	if (str_wr != NULL)
	{
		free(str_wr);
		str_wr = NULL;
	}
	printf_s("TEST CASE PASSED otherCharacterTest\r\n");
	return success;
}

//this function is used to test whether the program handles exceptions or errors
void errorTest()
{
	///TO DO

}

bool strIsEqual(char *strWrite,char *strRead,unsigned int len)
{
	if(strncmp(strWrite,strRead,len) != 0)
	{
		printf_s("DATA READ IS NOT EQUAL TO DATA WRITTN\n");
		printf_s("WRITE DATA: %s;DATA LENGTH: %d\n",strWrite,len);
		printf_s("DATA READ: %s\n",strRead);
		return false;
	}
	return true;
}
