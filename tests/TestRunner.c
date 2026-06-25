#include <stdio.h>
#include <stdint.h>

#include "HTTPRequestParsingTests.c"

int main() {
    size_t totalTests = 0;
    size_t failedTests = 0;

    printf("------------------------------RUNNING TESTS------------------------------\n\n\n");

    printf("------------------------------PARSING TESTS-------------------------------\n");
    totalTests++;
    if (ParseRequestLine_ValidRequestString_GET_ParseSuccesful() != 0) {
        failedTests += 1;
        printf("!!TEST FAILED!!\n");
    }
    totalTests++;
    if (ParseRequestLine_ValidRequestString_PUT_ParseSuccesful() != 0) {
        failedTests += 1;
        printf("!!TEST FAILED!!\n");
    }
    totalTests++;
    if (ParseRequestLine_ValidRequestString_POST_ParseSuccesful() != 0) {
        failedTests += 1;
        printf("!!TEST FAILED!!\n");
    }
    totalTests++;
    if (ParseRequestLine_ValidRequestString_DELETE_ParseSuccesful() != 0) {
        failedTests += 1;
        printf("!!TEST FAILED!!\n");
    }
    totalTests++;
    if (ParseRequestLine_ValidRequestString_v2_ParseSuccesful() != 0) {
        failedTests += 1;
        printf("!!TEST FAILED!!\n");
    }
    totalTests++;
    if (ParseRequestLine_ValidRequestString_v3_ParseSuccesful() != 0) {
        failedTests += 1;
        printf("!!TEST FAILED!!\n");
    }
    totalTests++;
    if (ParseRequestLine_InvalidRequestString_Method_ParseUnsuccesful() != 0) {
        failedTests += 1;
        printf("!!TEST FAILED!!\n");
    }
    totalTests++;
    if (ParseRequestLine_InvalidRequestString_Version_ParseUnsuccesful() != 0) {
        failedTests += 1;
        printf("!!TEST FAILED!!\n");
    }
    printf("------------------------------END OF TESTS--------------------------------\n");
    printf("Tests run: %zu,\nTests failed: %zu\n", totalTests, failedTests);


    return 0;
}