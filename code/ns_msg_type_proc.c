#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ns_base.h>

#define NS_MSG_TYPE_LINE_LEN (256)

ULONG empty_check(CHAR *pcLine, INT iLen)
{
    CHAR *pcPos = NULL;

    for (pcPos = pcLine; ((*pcPos != '\0') && ((pcPos - pcLine) < iLen)); pcPos++) {
        if (_isblank(*pcPos)) {
            continue;
        }
        if (*pcPos == '#') {
            return 1;
        }
        return 0;
    }

    return 1;
}

STATIC const char *types_str_list[] = {
	[AT_NONE]   = "NONE",
	[AT_CHAR]   = "CHAR",
	[AT_UCHAR]  = "UCHAR",
	[AT_SZ]     = "SZ",
	[AT_SHORT]  = "SHORT",
	[AT_USHORT] = "USHORT",
	[AT_INT]    = "INT",
	[AT_UINT]   = "UINT",
	[AT_LONG]   = "LONG",
	[AT_ULONG]  = "ULONT",
	[AT_LLONG]  = "LLONG",
	[AT_ULLONG] = "ULLONG",
	[AT_FLOAT]  = "FLOAT",
	[AT_DOUBLE] = "DOUBLE",
	[AT_BOOL_T] = "BOOL_T",
};

ULONG read_MsgType(VOID)
{
    CHAR *pcPos  = NULL;
    FILE *dotHfp = NULL;
    CHAR acBufLine[NS_MSG_TYPE_LINE_LEN + 1];
    INT iBegin = 0;
    CHAR acStBuf[128];
    INT  iStLen = 0;

    dotHfp = fopen("include/lib/ns_msg_type.h", "r");
    if (dotHfp == NULL) {
        return ERROR_FAILE;
    }

    while(fgets(acBufLine, NS_MSG_TYPE_LINE_LEN, dotHfp)) {
        if (empty_check(acBufLine, sizeof(acBufLine))) {
            continue;
        }
        //printf("len=%d:%s", strlen(acBufLine), acBufLine);
        //memset(acBufLine, 0, sizeof(acBufLine));
        //for (pcPos = acBufLine; ((*pcPos != '\0') && ((pcPos - acBufLine) < NS_MSG_TYPE_LINE_LEN)); pcPos++) {
        //}
        //printf("%s", acBufLine);
        //fflush(stdout);
        pcPos = strstr(acBufLine, "typedef");
        if (pcPos != NULL) {
            //pcPos += strlen("typedef");
            //printf("//%s", pcPos);
            //fflush(stdout);
            continue;
        }
        pcPos = strstr(acBufLine, "{");
        if (pcPos != NULL) {
            iBegin = 1;
            //printf("%s \n\t{\n", acBufLine);
            //fflush(stdout);
            continue;
        }
        pcPos = strstr(acBufLine, "}MSG_");
        if (pcPos != NULL) {
            //pcPos += strlen("typedef");
            printf("\t%s\n", "0xdeadbaef,");
            pcPos[0] = '\t';
            //printf("\t},\n");
            pcPos = strstr(acBufLine, "_ST;");
            if (pcPos != NULL) {
                pcPos[0] = '\0';
                //printf("\t%s,\n},\n", acBufLine);
                printf("%s,\n", acBufLine);
                fflush(stdout);
                printf("%s", acStBuf);
                iStLen = 0;
                continue;
            }
        }
        if (iBegin == 1) {
            for (pcPos = acBufLine; _isblank (*pcPos); pcPos++);
            if (pcPos != NULL) {
                pcPos = strstr(pcPos, " ");
                if (pcPos != NULL) {
                    pcPos[0] = '\0';
                    //printf("%s ,", pcPos);
                    INT i          = 0;
                    CHAR *pcEnd    = NULL;
                    INT  iArrayLen = 0;
                    /* is a array */
                    pcPos++;
                    pcPos = strstr(pcPos, "[");
                    if (pcPos != NULL) {
                        pcEnd = strstr(pcPos, "]");
                        if (pcEnd != NULL) {
                            pcPos++;
                            pcEnd = '\0';
                            iArrayLen = atoi(pcPos);
                        }
                    } else {
                        iArrayLen = 1;
                    }
                    pcPos = NULL;
                    while(i < ARRAY_SIZE(types_str_list)) {
                        pcPos = strstr(acBufLine, types_str_list[i]);
                        if (pcPos != NULL) {
                            break;
                        }
                        i++;
                    }
                    if (pcPos != NULL) {
                        //printf("\t\t%d,\n", i);
                        //fflush(stdout);
                        /* 0xFFFFAAAA; 
                         * 0xFFFF: array len
                         * 0xAAAA: type (not msg type, just type int, etc.)
                         * */
                        i = (i & 0xFFFF) | ((iArrayLen & 0xFFFF) << 16);
                        iStLen += snprintf(acStBuf + iStLen, sizeof(acStBuf) - iStLen, "\t0x%x,\n", (unsigned int)i);
                    }
                    continue;
                }
            }
        }

        printf("%s", acBufLine);
        fflush(stdout);
    }
    printf("\t%s\n", "0xdeadbaef,");

    fclose(dotHfp);
    dotHfp = NULL;

    return ERROR_SUCCESS;
}

int main(void)
{
    read_MsgType();

    return 0;
}