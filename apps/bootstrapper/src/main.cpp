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

#include "rh4n.h"
#include "rh4n_utils.h"

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

    AWS_LOGSTREAM_INFO(TAG, "Try to fork myself and call someone else");

    //This is importent. When the log buffer get now flushed it gets completely 
    //cloded with the fork call and every message get's printed out twice
    AWS_LOG_FLUSH();

    pid_t childpid = fork();
    if(childpid == 0) {
        const char *execpath = NULL;

        RH4nProperties props; memset(&props, 0x00, sizeof(props));

        strncpy(props.natlibrary, natlib, sizeof(props.natlibrary));
        strncpy(props.natprogram, natprog, sizeof(props.natprogram));

        props.natparms = (char*)malloc(1);
        *props.natparms = 0x00;
        props.i_loglevel = RH4N_DEVELOP;
        strcpy(props.httpreqtype, "GET");

        props.natsrcpath = (char*)malloc(20);
        memset(props.natsrcpath, 0x00, 20);
        strcpy(props.natsrcpath, "/var/task/fuser/");

        props.outputfile = (char*)malloc(20);
        memset(props.outputfile, 0x00, 20);
        strcpy(props.outputfile, "/tmp/outputfile");

        props.logpath = (char*)malloc(1);
        *props.logpath = 0x00;

        strcpy(props.errorrepresentation, "JSON");
        
        rh4nUtilsDumpProperties("/tmp/rh4n.props", &props);

        AWS_LOGSTREAM_INFO(NATTAG, "Hello im am the child: " << getpid());
        AWS_LOGSTREAM_INFO(NATTAG, "Starting [" << natprog << "] from lib [" << natlib << "]");

        //Start Natural buffer pool
        system("/opt/softwareag/Natural/bin/natbpsrv BPID=natbp");

        AWS_LOGSTREAM_INFO(NATTAG, "Calling the nni caller");
        AWS_LOG_FLUSH();

        std::ostringstream execcmd;
        execcmd << "/opt/softwareag/Natural/bin/main " << natlib << " " << natprog;
        system("/opt/softwareag/Natural/bin/natcaller /tmp/rh4n.props");
        //int returncode = execl("/opt/softwareag/Natural/bin/main", "main", natlib, natprog, NULL);
        //AWS_LOGSTREAM_ERROR(NATTAG, "someting on execl went wrong: " << returncode);
        fflush(stdout);
        AWS_LOG_FLUSH();
        exit(0);
    }

    AWS_LOGSTREAM_INFO(TAG, "Wait for child [" << childpid << "]");
    int status = 0;
    waitpid(childpid, &status, 0);
    int exitcode = WEXITSTATUS(status);
    AWS_LOGSTREAM_INFO(TAG, "child exited with code: [" << exitcode << "]");

    FILE *natoutputfile = fopen("/tmp/outputfile", "r");
    if(natoutputfile == NULL) {
        AWS_LOG_FLUSH();
        return invocation_response::failure("Output file does not exist", "Devliver Response");
    }
    char readbuff[2000];
    memset(readbuff, 0x00, sizeof(readbuff));

    fread(readbuff, sizeof(readbuff), 1, natoutputfile);
    //memset(readbuff, 0x20, 2);

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
