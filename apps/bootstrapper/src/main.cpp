#include <iostream>
#include <aws/core/Aws.h>
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/LogMacros.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/HashingUtils.h>
#include <aws/core/platform/Environment.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/lambda-runtime/runtime.h>
#include <iostream>
#include <memory>

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

char const TAG[] = "VAR_DUMP";
char const NATTAG[] = "NATCALLER";
int gtest = 0;

using namespace aws::lambda_runtime;

static invocation_response callNaturalHandler(invocation_request const& req, const char *natlib, const char *natprog) {
    using namespace Aws::Utils::Json;

    JsonValue json(req.payload);

    if (!json.WasParseSuccessful()) {
        return invocation_response::failure("Failed to parse input JSON", "InvalidJSON");
    }

    AWS_LOGSTREAM_INFO(TAG, "gtest: " << gtest)
    
    JsonView view = json.View();

    Aws::Map<Aws::String, JsonView> rootNode = view.GetAllObjects();
    AWS_LOGSTREAM_INFO(TAG, "Length of map: [" << rootNode.size() << "]");

    for(std::map<Aws::String, JsonView>::iterator it = rootNode.begin(); it != rootNode.end(); ++it) {
        JsonView *targetView = &it->second;

        AWS_LOGSTREAM_INFO(TAG, "PID: " << getpid() << " - God Key: [" << it->first << "]");
        if(targetView->IsString()) {
            AWS_LOGSTREAM_INFO(TAG, "God a string");
            Aws::String tmp = targetView->AsString();
            AWS_LOGSTREAM_INFO(TAG, "God String: [" << tmp << "]");
        } else {
            AWS_LOGSTREAM_INFO(TAG, "Value is not a string");
        }

        AWS_LOGSTREAM_INFO(TAG, "==================================");
    }

    AWS_LOGSTREAM_INFO(TAG, "Try to fork myself an call someone else");

    //This is importent. When the log buffer get now flushed it gets completely 
    //cloded with the fork call and every message get's printed out twice
    AWS_LOG_FLUSH();

    pid_t childpid = fork();
    if(childpid == 0) {
        const char *execpath = NULL;

        AWS_LOGSTREAM_INFO(NATTAG, "Hello im an the child: " << getpid());
        AWS_LOGSTREAM_INFO(NATTAG, "Starting [" << natprog << "] from lib [" << natlib << "]");

        /*
        AWS_LOGSTREAM_INFO(NATTAG, "Delete old fuser");
        system("rm -r /opt/softwareag/Natural/fuser");
        
        AWS_LOGSTREAM_INFO(NATTAG, "Copy new fuser");
        execpath = getenv("LAMBDA_TASK_ROOT");
        if (execpath == NULL) {
            AWS_LOGSTREAM_ERROR(NATTAG, "LAMBDA_TASK_ROOT envvar is not set");
            exit(1);
        }
        AWS_LOGSTREAM_INFO(NATTAG, "Orig location: " << execpath);
        
        std::string copyLocation(execpath);

        copyLocation.append("/fuser");

        std::ostringstream s_execcmd;
        s_execcmd << "cp -r " << copyLocation << " " << "/opt/softwareag/Natural/";
        std::string execcmd = s_execcmd.str();

        AWS_LOGSTREAM_INFO(NATTAG, "Exec: [" << execcmd << "]");
        AWS_LOG_FLUSH();*/

        AWS_LOG_FLUSH();
        system("/opt/softwareag/Natural/bin/natbpsrv BPID=natbp");
        execl("/opt/softwareag/Natural/bin/main", "main", natlib, natprog, NULL);
        exit(0);
    }

    AWS_LOGSTREAM_INFO(TAG, "Wait for child [" << childpid << "]");
    int status = 0;
    waitpid(childpid, &status, 0);

    FILE *natoutputfile = fopen("/tmp/test", "r");
    char readbuff[100];
    memset(readbuff, 0x00, sizeof(readbuff));

    fread(readbuff, sizeof(readbuff), 1, natoutputfile);
    memset(readbuff, 0x20, 2);

    AWS_LOGSTREAM_INFO(TAG, "God value from natural: [" << readbuff << "]");
    
    AWS_LOG_FLUSH();

    Aws::String result(readbuff);


    JsonValue *returnobj = new JsonValue();

    returnobj->WithInteger("statusCode", 200);
    returnobj->WithBool("isBase64Encoded", false);

    returnobj->WithString("body", result);

    JsonView returnView = returnobj->View(); 
    return invocation_response::success(returnView.WriteReadable(), "json/text");
}

std::function<std::shared_ptr<Aws::Utils::Logging::LogSystemInterface>()> GetConsoleLoggerFactory() {
    return [] {
        return Aws::MakeShared<Aws::Utils::Logging::ConsoleLogSystem>(
            "console_logger", Aws::Utils::Logging::LogLevel::Trace);
    };
}

int main(int argc, char *argv[]) {
    using namespace Aws;


    SDKOptions options;

    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    options.loggingOptions.logger_create_fn = GetConsoleLoggerFactory();

    InitAPI(options);

    for(int i = 0; i < argc; i++) {
        AWS_LOGSTREAM_INFO(TAG, "God cli parm: [" << argv[i] << "]" << std::endl);
    }

    char *awsHandler = argv[1];
    char *pointPos = strchr(awsHandler, '.');
    const char *natlib = NULL, *natprog = NULL;
    if(pointPos == NULL) {
        std::cerr << "No Point in Handler found" << std::endl;
        return -1;
    }
    
    *pointPos = '\0';
    natlib = awsHandler;
    natprog = pointPos+1;

    {
        auto handler_fn = [natlib, natprog](aws::lambda_runtime::invocation_request const& req) {
            return callNaturalHandler(req, natlib, natprog);
        };
        run_handler(handler_fn);
    }

    ShutdownAPI(options);
}
