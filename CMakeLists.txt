cmake_minimum_required(VERSION 3.15)

project(awsNatWrapper VERSION 1.0 LANGUAGES C CXX)

find_package(aws-lambda-runtime REQUIRED)
find_package(AWSSDK COMPONENTS s3)

include_directories(
    ./libs/rh4n_include 
    ./libs/rh4n_vars/include
    ./libs/rh4n_logging/include)

add_library(rh4nlogging STATIC
    ./libs/rh4n_logging/src/rh4n_logging.c)

add_library(rh4nvars STATIC 
    ./libs/rh4n_vars/src/rh4n_vars.c
    ./libs/rh4n_vars/src/rh4n_vars_array.c
    ./libs/rh4n_vars/src/rh4n_vars_bool.c
    ./libs/rh4n_vars/src/rh4n_vars_dump.c
    ./libs/rh4n_vars/src/rh4n_vars_float.c
    ./libs/rh4n_vars/src/rh4n_vars_free.c
    ./libs/rh4n_vars/src/rh4n_vars_group.c
    ./libs/rh4n_vars/src/rh4n_vars_integer.c
    ./libs/rh4n_vars/src/rh4n_vars_load.c
    ./libs/rh4n_vars/src/rh4n_vars_print.c
    ./libs/rh4n_vars/src/rh4n_vars_string.c
    ./libs/rh4n_vars/src/rh4n_vars_ustring.c)

add_executable(lambdaHandler ./apps/bootstrapper/src/main.cpp)
target_link_libraries(lambdaHandler PUBLIC AWS::aws-lambda-runtime ${AWSSDK_LINK_LIBRARIES})
aws_lambda_package_target(lambdaHandler)
