#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"
#include "rh4n_v2n.h"

#define OUTPUTFILE "./jsonoutput.json"

int checkResult(char *testname, char *expectedresult, RH4nProperties *props) {
    char resultbuff[5000];
    int returncode = -1;

    FILE *test = fopen("jsonoutput.json", "rb");

    if(test == NULL) { exit(1); };

    char buff[4000], cur;
    int i = 0;

    memset(buff, 0x00, sizeof(buff));

    /*while((cur = fgetc(test)) != EOF && i < 4000) {
        buff[i++] = cur;
    }*/

    fgets(buff, sizeof(buff), test);

    fclose(test);

    //int size = read(fileno(outputfile), resultbuff, sizeof(resultbuff));
    rh4n_log_develop(props->logging, "Bytes read: %d", i-1);

   if(strcmp(buff, expectedresult) != 0) {
        rh4n_log_develop(props->logging, "Test %s failed.", testname);
        rh4n_log_develop(props->logging, "Expected: %s", expectedresult);
        rh4n_log_develop(props->logging, "God:      %s", buff);
    } else {
        returncode = 0;
        rh4n_log_develop(props->logging, "Test %s succeeded", testname);
    }

    fclose(test);

    rh4n_log_develop(props->logging, "--------------------------------------------------------------------------------------------------------------------------------------------");
    return(returncode);
} 

RH4nVarList* preTest() {
    static RH4nVarList varlist;

    rh4nvarInitList(&varlist);
    return(&varlist);
}

void afterTest(RH4nVarList *varlist) {
    rh4nvarFreeList(varlist);
}

int runTest(char *testname, RH4nVarList *varlist, RH4nProperties *props) {
    rh4n_log_develop(props->logging, "---------------------------------------------------------Running Test %s---------------------------------------------------------", testname);
    rh4nvarPrintList(varlist, props);
    FILE *out = fopen(OUTPUTFILE, "w");
    rh4njsonPrintJSON(varlist, out, props);
    fflush(out);
    fclose(out);
}

int main(int argc, char *argv[]) {
    RH4nVarList *varlist;
    RH4nProperties props;
    RH4nLDAEntry_t *entries = NULL;
    char errorstr[2048];
    int arrlength[3] = {2, 0, 0};

    props.logging = rh4nLoggingCreateStreamingRule("VARLIB", "TEST", RH4N_DEVELOP, "");

    //rh4nvarInitList(&varlist);

#if 0
/*
 * 1 str1 (A) DYNAMIC
 * 1 str2 (A) DYNAMIC
 * 1 str3 (A) DYNAMIC
 * 1 str4 (A) DYNAMIC
 */
#if 1
    varlist = preTest();

    rh4nvarCreateNewString(varlist, NULL, "str1", "val1");
    rh4nvarCreateNewString(varlist, NULL, "str2", "val2");
    rh4nvarCreateNewString(varlist, NULL, "str3", "val3");
    rh4nvarCreateNewString(varlist, NULL, "str4", "val3");

    runTest("Test 1", varlist, &props);
    if(checkResult("Test 1", "{\"str1\":\"val1\",\"str2\":\"val2\",\"str3\":\"val3\",\"str4\":\"val3\"}", &props) != 0) {
        exit(1);
    }
    afterTest(varlist);
#endif

/*
 * 1 grp1 (1:2)  
 *   2 str1 (A) DYNAMIC
 *   2 str2 (A) DYNAMIC
 *   2 str3 (A) DYNAMIC
 *   2 str4 (A) DYNAMIC
 *
 *   Expected Output:
 *      
 *      [{"grp1":{"str1":"","str2":"","str3":"","str4":""}},{"grp1":{"str1":"","str2":"","str3":"","str4":""}}]
 */
#if 1
    varlist = preTest();

    rh4nvarCreateNewGroup(varlist, "grp1");
    rh4nvarCreateNewStringArray(varlist, "grp1", "str1", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str2", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str3", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str4", 1, arrlength);

    runTest("Test 2", varlist, &props);
    if(checkResult("Test 2", 
        "[{\"grp1\":{\"str1\":\"\",\"str2\":\"\",\"str3\":\"\",\"str4\":\"\"}},{\"grp1\":{\"str1\":\"\",\"str2\":\"\",\"str3\":\"\",\"str4\":\"\"}}]",
        &props) != 0) {
        exit(1);
    }
    afterTest(varlist);
#endif

/*
 * 1 grp1 (1:2)  
 *   2 str1 (A) DYNAMIC
 *   2 str2 (A) DYNAMIC
 *   2 str3 (A) DYNAMIC
 *   2 str4 (A) DYNAMIC
 * 1 str5 (A/1:2) DYNAMIC
 *
 *   Expected Output:
 *      [{"grp1":{"str1":"","str2":"","str3":"","str4":""},"str5":""},{"grp1":{"str1":"","str2":"","str3":"","str4":""},"str5":""}] 
 */
#if 1
    varlist = preTest();

    rh4nvarCreateNewGroup(varlist, "grp1");
    rh4nvarCreateNewStringArray(varlist, "grp1", "str1", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str2", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str3", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str4", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, NULL, "str5", 1, arrlength);

    runTest("Test 3", varlist, &props);
    if(checkResult("Test 3", 
        "[{\"grp1\":{\"str1\":\"\",\"str2\":\"\",\"str3\":\"\",\"str4\":\"\"},\"str5\":\"\"},{\"grp1\":{\"str1\":\"\",\"str2\":\"\",\"str3\":\"\",\"str4\":\"\"},\"str5\":\"\"}]",
        &props) != 0) {
        exit(1);
    }
    afterTest(varlist);
#endif

/*
 * 1 str1 (A/1:2) DYNAMIC
 * 1 str2 (A/1:2) DYNAMIC
 * 1 str3 (A/1:2) DYNAMIC
 * 1 str4 (A/1:2) DYNAMIC
 * 1 str5 (A/1:2) DYNAMIC
 *
 *  Expected Output:
 *      [{"str1":"","str2":"","str3":"","str4":"","str5":""},{"str1":"","str2":"","str3":"","str4":"","str5":""}]
 */
#if 1
    varlist = preTest();

    rh4nvarCreateNewStringArray(varlist, NULL, "str1", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, NULL, "str2", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, NULL, "str3", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, NULL, "str4", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, NULL, "str5", 1, arrlength);

    runTest("Test 4", varlist, &props);
    if(checkResult("Test 4", 
        "[{\"str1\":\"\",\"str2\":\"\",\"str3\":\"\",\"str4\":\"\",\"str5\":\"\"},{\"str1\":\"\",\"str2\":\"\",\"str3\":\"\",\"str4\":\"\",\"str5\":\"\"}]",
        &props) != 0) {
        exit(1);
    }
    afterTest(varlist);
#endif

/* 1 grp1
 *   2 str1 (A/1:2) DYNAMIC
 *   2 str2 (A/1:2) DYNAMIC
 *   2 str3 (A/1:2) DYNAMIC
 *   2 str4 (A/1:2) DYNAMIC
 *   2 str5 (A) DYNAMIC
 *
 *   Expected Output:
 *      {"grp1":{"str2":["",""],"str3":["",""],"str4":["",""],"str5":["",""],"str1":"str1"}}
 */
#if 1
    varlist = preTest();

    rh4nvarCreateNewGroup(varlist, "grp1");
    //rh4nvarCreateNewString(&varlist, "grp1", "str1", "str1");
    rh4nvarCreateNewStringArray(varlist, "grp1", "str2", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str3", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str4", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str5", 1, arrlength);
    rh4nvarCreateNewString(varlist, "grp1", "str1", "str1");

    runTest("Test 5", varlist, &props);
    if(checkResult("Test 5", 
"{\"grp1\":{\"str2\":[\"\",\"\"],\"str3\":[\"\",\"\"],\"str4\":[\"\",\"\"],\"str5\":[\"\",\"\"],\"str1\":\"str1\"}}",
        &props) != 0) {
        exit(1);
    }
    afterTest(varlist);
#endif

/* 1 grp1
 *   2 str1 (A) DYNAMIC
 *   2 str2 (A/1:2) DYNAMIC
 *   2 grp2
 *     3 str3 (A/1:2) DYNAMIC
 *     3 str4 (A) DYNAMIC
 *   2 str5 (A/1:2) DYNAMIC
 *
 *   Expected Output:
 *      {"grp1":{"str1":"","str2":["",""],"grp2":{"str3":["",""],"str5":""},"str4":["",""]}
 */
#if 1
{
    varlist = preTest();

    char *parents[] = { "grp1", NULL, NULL };
    rh4nvarCreateNewGroup(varlist, "grp1");

    rh4nvarCreateNewString(varlist, "grp1", "str1", "");
    rh4nvarCreateNewStringArray(varlist, "grp1", "str2", 1, arrlength);

    rh4nvarCreateNewGroup_m(varlist, parents, "grp2");

    parents[1] = "grp2";
    rh4nvarCreateNewStringArray(varlist, NULL, "str3", 1, arrlength);
    rh4nvarMoveVarToGroup_m(varlist, "str3", parents);
    rh4nvarCreateNewString(varlist, NULL, "str4", "");
    rh4nvarMoveVarToGroup_m(varlist, "str4", parents);

    rh4nvarCreateNewStringArray(varlist, "grp1", "str5", 1, arrlength);

    runTest("Test 6", varlist, &props);
    if(checkResult("Test 6", 
"{\"grp1\":{\"str1\":\"\",\"str2\":[\"\",\"\"],\"grp2\":{\"str3\":[\"\",\"\"],\"str4\":\"\"},\"str5\":[\"\",\"\"]}}",
        &props) != 0) {
        exit(1);
    }
    afterTest(varlist);
}
#endif

/* 1 grp1 (1:2)
 *   2 str1 (A) DYNAMIC
 *   2 str2 (A) DYNAMIC
 *   2 grp2
 *     3 str3 (A/1:2) DYNAMIC
 *     3 str4 (A) DYNAMIC
 *   2 str5 (A) DYNAMIC
 *
 *   Expected Output:
 *   {"grp1":[{"str1":"","str2":"","grp2":{"str3":["",""],"str4":""},"str5":""},{"str1":"","str2":"","grp2":{"str3":["",""],"str4":""},"str5":""}]}
 */
#if 1
{
    varlist = preTest();

    char *parents[] = { "grp1", NULL, NULL };
    rh4nvarCreateNewGroup(varlist, "grp1");

    rh4nvarCreateNewStringArray(varlist, "grp1", "str1", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str2", 1, arrlength);

    rh4nvarCreateNewGroup_m(varlist, parents, "grp2");

    parents[1] = "grp2";
    arrlength[1] = 2;
    rh4nvarCreateNewStringArray(varlist, NULL, "str3", 2, arrlength);
    arrlength[1] = 0;
    rh4nvarMoveVarToGroup_m(varlist, "str3", parents);
    rh4nvarCreateNewStringArray(varlist, NULL, "str4", 1, arrlength);
    rh4nvarMoveVarToGroup_m(varlist, "str4", parents);

    rh4nvarCreateNewStringArray(varlist, "grp1", "str5", 1, arrlength);

    runTest("Test 7", varlist, &props);
    if(checkResult("Test 7", 
"[{\"grp1\":{\"str1\":\"\",\"str2\":\"\",\"grp2\":{\"str3\":[\"\",\"\"],\"str4\":\"\"},\"str5\":\"\"}},{\"grp1\":{\"str1\":\"\",\"str2\":\"\",\"grp2\":{\"str3\":[\"\",\"\"],\"str4\":\"\"},\"str5\":\"\"}}]",
        &props) != 0) {
        exit(1);
    }
    afterTest(varlist);
}
#endif

/*
 * 1 grp1 (1:2)  
 *   2 str1 (A) DYNAMIC
 *   2 str2 (A) DYNAMIC
 *   2 str3 (A) DYNAMIC
 *   2 str4 (A) DYNAMIC
 * 1 grp2
 *   2 str5 (A) DYNAMIC
 *
 *   Expected Output:
 *      {"grp1":[{"str1":"","str2":"","str3":"","str4":""},{"str1":"","str2":"","str3":"","str4":""}],"str5":["",""]} 
 */
#if 1
    varlist = preTest();

    rh4nvarCreateNewGroup(varlist, "grp1");
    rh4nvarCreateNewGroup(varlist, "grp2");
    rh4nvarCreateNewStringArray(varlist, "grp1", "str1", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str2", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str3", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str4", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp2", "str5", 1, arrlength);

    runTest("Test 8", varlist, &props);
    if(checkResult("Test 8", 
"{\"grp1\":[{\"str1\":\"\",\"str2\":\"\",\"str3\":\"\",\"str4\":\"\"},{\"str1\":\"\",\"str2\":\"\",\"str3\":\"\",\"str4\":\"\"}],\"grp2\":[{\"str5\":\"\"},{\"str5\":\"\"}]}",
        &props) != 0) {
        exit(1);
    }
    afterTest(varlist);
#endif

/*
 * 1 grp1 (1:2)  
 *   2 str1 (A) DYNAMIC
 *   2 str2 (A) DYNAMIC
 *   2 str3 (A) DYNAMIC
 *   2 str4 (A) DYNAMIC
 *
 *   Expected Output:
 *      {"grp1":[{"str1":"","str2":"","str3":"","str4":""},{"str1": "","str2":"","str3":"","str4":""}],"str5":["",""]}
 */
#if 1
    varlist = preTest();

    rh4nvarCreateNewGroup(varlist, "grp1");
    rh4nvarCreateNewStringArray(varlist, "grp1", "str1", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str2", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str3", 1, arrlength);
    rh4nvarCreateNewStringArray(varlist, "grp1", "str4", 1, arrlength);
    rh4nvarCreateNewString(varlist, "grp1", "str5", "");

    runTest("Test 9", varlist, &props);
    if(checkResult("Test 9", 
"{\"grp1\":{\"str1\":[\"\",\"\"],\"str2\":[\"\",\"\"],\"str3\":[\"\",\"\"],\"str4\":[\"\",\"\"],\"str5\":\"\"}}",
        &props) != 0) {
        exit(1);
    }
    afterTest(varlist);
#endif

/*
 * 1 grp1 (1:2)
 *   2 str1 (A) DYNAMIC
 * 1 grp2 (1:2)
 *   2 str2 (A) DYNAMIC
 *
 *   Expected Output:
 *      {"grp1":[{"str1":""},{"str1":""}],"grp2":[{"str2":""},{"str2":""}]}
 */

#if 1
    varlist = preTest();

    rh4nvarCreateNewGroup(varlist, "grp1");
    rh4nvarCreateNewStringArray(varlist, "grp1", "str1", 1, arrlength);
    rh4nvarCreateNewGroup(varlist, "grp2");
    rh4nvarCreateNewStringArray(varlist, "grp2", "str2", 1, arrlength);

    runTest("Test 10", varlist, &props);
    if(checkResult("Test 10", 
"{\"grp1\":[{\"str1\":\"\"},{\"str1\":\"\"}],\"grp2\":[{\"str2\":\"\"},{\"str2\":\"\"}]}",
        &props) != 0) {
        exit(1);
    }
    afterTest(varlist);
#endif


/*
 * 1 grp1 (1:2)
 *   2 str1 (A) DYNAMIC
 *   2 grp2 (1:2)
 *      3 str2 (A) DYNMAIC
 *      3 str3 (A) DYNAMIC
 * Expected Ouput:
 *    [{"str1": "", "grp2": [{"str2": "", "str3": ""}],[{"str2": "", "str3": ""}]}, *     {"str1": "", "grp2": [{"str2": "", "str3": ""}],[{"str2": "", "str3": ""}]}] */

#if 1
{
    varlist = preTest();

    char *parents[] = { "grp1", NULL, NULL };
    //rh4nvarCreateNewGroup(&varlist, "grp1");
    rh4nvarCreateNewStringArray(varlist, NULL, "str1", 1, arrlength);

    arrlength[1] = 2;
    //rh4nvarCreateNewGroup_m(&varlist, parents, "grp2");
    rh4nvarCreateNewGroup(varlist, "grp2");
    parents[1] = "grp2";
    rh4nvarCreateNewStringArray(varlist, "grp2", "str2", 2, arrlength);
    //rh4nvarMoveVarToGroup_m(&varlist, "str2", parents);
    rh4nvarCreateNewStringArray(varlist, "grp2", "str3", 2, arrlength);
    //rh4nvarMoveVarToGroup_m(&varlist, "str3", parents);

    runTest("Test 11", varlist, &props);
    if(checkResult("Test 11", 
"[{\"str1\":\"\",\"grp2\":[{\"str2\":\"\",\"str3\":\"\"}],[{\"str2\":\"\",\"str3\":\"\"}]},{\"str1\":\"\",\"grp2\":[{\"str2\":\"\",\"str3\":\"\"}],[{\"str2\":\"\",\"str3\":\"\"}]}]",
        &props) != 0) {
        exit(1);
    }
    afterTest(varlist);
}
#endif

/*
 * 1 grp1 (1:1)
 *  2 str1 (A) DYNAMIC
 *  2 str2 (A/1:1) DYNMAIC
 * Expected Output
 *  [{"str1": "", "str2": [""]}, {"str1": "", "str2": [""]}]
 */
#if 1
    varlist = preTest();

    arrlength[0] = 1;
    rh4nvarCreateNewStringArray(varlist, NULL, "str1", 1, arrlength);
    arrlength[1] = 1;
    rh4nvarCreateNewStringArray(varlist, NULL, "str2", 2, arrlength);

    runTest("Test 12", varlist, &props);
    if(checkResult("Test 12", 
        "[{\"str1\":\"\",\"str2\":[\"\"]},{\"str1\":\"\",\"str2\":[\"\"]}]",
        &props) != 0) {
        exit(1);
    }
    afterTest(varlist);
#endif
#endif

/*
 * 1 grp1 
 *   2 grp2 (1:1)
 *     3 var1 (A) DYNAMIC
 *     3 var2 (A) DYNAMIC
 *     3 grp3 
 *       4 var3 (A4)
 *       4 var4 (A4)
 *  2 grp4 (1:1)
 *    3 var5 (A) DYNAMIC
 *    3 var6 (A) DYNAMIC
 */
#if 1
{
    char *parents[] = { "grp1", NULL, NULL, NULL };
    arrlength[0] = 1;

    varlist = preTest();
    rh4nvarCreateNewGroup(varlist, "grp1");
    rh4nvarCreateNewGroup_m(varlist, parents, "grp2");

    

    parents[1] = "grp2";
    rh4nvarCreateNewStringArray(varlist, NULL, "var1", 1, arrlength);
    rh4nvarMoveVarToGroup_m(varlist, "var1", parents);
    rh4nvarCreateNewStringArray(varlist, NULL, "var2", 1, arrlength);
    rh4nvarMoveVarToGroup_m(varlist, "var2", parents);
    rh4nvarCreateNewGroup_m(varlist, parents, "grp3");

    parents[2] = "grp3";
    rh4nvarCreateNewStringArray(varlist, NULL, "var3", 1, arrlength);
    rh4nvarMoveVarToGroup_m(varlist, "var3", parents);
    rh4nvarCreateNewStringArray(varlist, NULL, "var4", 1, arrlength);
    rh4nvarMoveVarToGroup_m(varlist, "var4", parents);

    runTest("Test 13", varlist, &props);

    afterTest(varlist);

}
#endif

    /*rh4nvarPrintList(&varlist, &props);
    rh4njsonPrintJSON(&varlist, stderr, &props);

    rh4nvarFreeList(&varlist);*/
    rh4n_del_log_rule(props.logging);
    return(0);
}
