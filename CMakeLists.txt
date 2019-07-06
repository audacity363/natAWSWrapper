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
    ./libs/rh4n_ldaparser/include
    ./libs/rh4n_jsongenerator/include
    ./apps/natuser/include)

add_library(rh4nutils STATIC 
    ./libs/rh4n_utils/src/rh4n_utils)
#./libs/rh4n_utils/src/rh4n_utils_prop)
set_property(TARGET rh4nutils PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(rh4nlogging STATIC
    ./libs/rh4n_logging/src/rh4n_logging)
set_property(TARGET rh4nlogging PROPERTY POSITION_INDEPENDENT_CODE ON)


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
set_property(TARGET rh4nvars PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(rh4nldaparser STATIC
    ./libs/rh4n_ldaparser/src/rh4n_ldaparser.c
    ./libs/rh4n_ldaparser/src/rh4n_lda_filehandling
	./libs/rh4n_ldaparser/src/rh4n_lda_array
	./libs/rh4n_ldaparser/src/rh4n_lda_utils
	./libs/rh4n_ldaparser/src/rh4n_lda_variables
	./libs/rh4n_ldaparser/src/rh4n_lda_print
	./libs/rh4n_ldaparser/src/rh4n_lda_free)
set_property(TARGET rh4nldaparser PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(rh4nvar2name STATIC
    ./libs/rh4n_var2name/src/rh4n_v2n
    ./libs/rh4n_var2name/src/rh4n_v2n_resolvenames
	./libs/rh4n_var2name/src/rh4n_v2n_resolveposition)
set_property(TARGET rh4nvar2name PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(realHTML4Natural SHARED
    ./apps/natuser/src/rh4n_nat_jumptab
    ./apps/natuser/src/ncuxinit
    ./apps/natuser/src/rh4n_nat_genjson
    ./apps/natuser/src/rh4n_nat_init
    ./apps/natuser/src/rh4n_nat_utils
    ./apps/natuser/src/rh4n_nat_variablereadout
    ./apps/natuser/src/rh4n_nat_geturlobj
    ./apps/natuser/src/rh4n_nat_mbn
    ./apps/natuser/src/rh4n_nat_getbodyobj
    ./apps/natuser/src/rh4n_nat_getbodyvar
    ./apps/natuser/src/rh4n_nat_geturlvar
    ./apps/natuser/src/rh4n_nat_getuser
    ./apps/natuser/src/rh4n_nat_debug_init

    ./apps/natuser/src/var_parm_handling/rh4n_nat_a_handling
	./apps/natuser/src/var_parm_handling/rh4n_nat_i_handling
	./apps/natuser/src/var_parm_handling/rh4n_nat_l_handling
	./apps/natuser/src/var_parm_handling/rh4n_nat_f_handling
	./apps/natuser/src/var_parm_handling/rh4n_nat_dt_handling
	./apps/natuser/src/var_parm_handling/rh4n_nat_np_handling
	./apps/natuser/src/var_parm_handling/rh4n_nat_u_handling

    ./apps/natuser/src/put_var_handling/rh4n_nat_put_a
	./apps/natuser/src/put_var_handling/rh4n_nat_put_i
	./apps/natuser/src/put_var_handling/rh4n_nat_put_l
	./apps/natuser/src/put_var_handling/rh4n_nat_put_f
	./apps/natuser/src/put_var_handling/rh4n_nat_put_aton)
target_link_libraries(realHTML4Natural PRIVATE rh4nutils rh4nlogging rh4nvars rh4nldaparser rh4nvar2name)


add_executable(lambdaHandler ./apps/bootstrapper/src/main.cpp)
target_link_libraries(lambdaHandler PUBLIC AWS::aws-lambda-runtime ${AWSSDK_LINK_LIBRARIES} rh4nutils rh4nlogging rh4nvars rh4nldaparser rh4nvar2name)

aws_lambda_package_target(lambdaHandler)
