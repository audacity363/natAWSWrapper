cmake_minimum_required(VERSION 3.15)

project(awsNatWrapper VERSION 1.0 LANGUAGES C CXX)

find_package(aws-lambda-runtime REQUIRED)
find_package(AWSSDK COMPONENTS s3)

include_directories(
    ./libs/rh4n_include 
    ./libs/rh4n_vars/include
    ./libs/rh4n_logging/include
    ./libs/rh4n_utils/include
    ./libs/rh4n_var2name/include
    ./libs/rh4n_ldaparser/include)

add_library(rh4nutils STATIC 
    ./libs/rh4n_utils/src/rh4n_utils)
#./libs/rh4n_utils/src/rh4n_utils_prop)

add_library(rh4nlogging STATIC
    ./libs/rh4n_logging/src/rh4n_logging)

add_library(rh4nvars STATIC 
    ./libs/rh4n_vars/src/rh4n_vars
    ./libs/rh4n_vars/src/rh4n_vars_array
    ./libs/rh4n_vars/src/rh4n_vars_bool
    ./libs/rh4n_vars/src/rh4n_vars_dump
    ./libs/rh4n_vars/src/rh4n_vars_float
    ./libs/rh4n_vars/src/rh4n_vars_free
    ./libs/rh4n_vars/src/rh4n_vars_group
    ./libs/rh4n_vars/src/rh4n_vars_integer
    ./libs/rh4n_vars/src/rh4n_vars_load
    ./libs/rh4n_vars/src/rh4n_vars_print
    ./libs/rh4n_vars/src/rh4n_vars_string
    ./libs/rh4n_vars/src/rh4n_vars_ustring)

add_library(rh4nldaparser STATIC
    ./libs/rh4n_ldaparser/src/rh4n_ldaparser.c
    ./libs/rh4n_ldaparser/src/rh4n_lda_filehandling
	./libs/rh4n_ldaparser/src/rh4n_lda_array
	./libs/rh4n_ldaparser/src/rh4n_lda_utils
	./libs/rh4n_ldaparser/src/rh4n_lda_variables
	./libs/rh4n_ldaparser/src/rh4n_lda_print
	./libs/rh4n_ldaparser/src/rh4n_lda_free)

add_library(rh4nvar2name STATIC
    ./libs/rh4n_var2name/src/rh4n_v2n
    ./libs/rh4n_var2name/src/rh4n_v2n_resolvenames
	./libs/rh4n_var2name/src/rh4n_v2n_resolveposition)

add_executable(lambdaHandler ./apps/bootstrapper/src/main.cpp)
target_link_libraries(lambdaHandler PUBLIC AWS::aws-lambda-runtime ${AWSSDK_LINK_LIBRARIES} rh4nutils rh4nlogging rh4nvars rh4nldaparser rh4nvar2name)
aws_lambda_package_target(lambdaHandler)
