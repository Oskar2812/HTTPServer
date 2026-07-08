#include <stdio.h>
#include <stdint.h>

#include "HTTPRequestParsingTests.c"
#include "HTTPMessageHelperTests.c"
#include "HTTPResponseSerialisationTests.c"


#define RUN_TEST(test, total, failed) \
    do { \
        (total)++; \
        printf("Running %s\n", #test); \
        if ((test) != 0) { \
            (failed)++; \
            printf("!! TEST FAILED!!\n"); \
        } \
        else { \
            printf("Test Complete\n"); \
        } \
    } while (0)


int main() {
    size_t totalTests = 0;
    size_t failedTests = 0;

    printf("------------------------------RUNNING TESTS------------------------------\n\n\n");

    printf("------------------------------PARSING TESTS-------------------------------\n");
    
    RUN_TEST(ParseRequestLine_ValidRequestString_GET_ParseSuccesful(), totalTests, failedTests);
    RUN_TEST(ParseRequestLine_ValidRequestString_PUT_ParseSuccesful(), totalTests, failedTests);
    RUN_TEST(ParseRequestLine_ValidRequestString_POST_ParseSuccesful(), totalTests, failedTests);
    RUN_TEST(ParseRequestLine_ValidRequestString_DELETE_ParseSuccesful(), totalTests, failedTests);
    RUN_TEST(ParseRequestLine_ValidRequestString_v2_ParseSuccesful(), totalTests, failedTests);
    RUN_TEST(ParseRequestLine_ValidRequestString_v3_ParseSuccesful(), totalTests, failedTests);
    RUN_TEST(ParseRequestLine_QueryParameters_ParseSuccesful(), totalTests, failedTests);
    RUN_TEST(ParseRequestLine_InvalidRequestString_Method_ParseUnsuccesful(), totalTests, failedTests);
    RUN_TEST(ParseRequestLine_InvalidRequestString_Version_ParseUnsuccesful(), totalTests, failedTests);
    RUN_TEST(ParseFieldLine_ValidFieldLine_WithBlank_ParseSuccesful(), totalTests, failedTests);
    RUN_TEST(ParseFieldLine_ValidFieldLine_WithoutBlank_ParseSuccesful(), totalTests, failedTests);
    RUN_TEST(ParseFieldLine_ValidFieldLine_TrailingWhitespace_ParseSuccesful(), totalTests, failedTests);
    RUN_TEST(ParseFieldLine_ValidFieldLine_NoColon_ParseUnsuccesful(), totalTests, failedTests);
    RUN_TEST(ParseMessagePreamble_ValidPreamble_ParseSuccesful(), totalTests, failedTests);

    printf("------------------------------HELPER TESTS-------------------------------\n");
    RUN_TEST(GetHeaderValue_ValidSearch_ReturnsHeader(), totalTests, failedTests);
    RUN_TEST(GetHeaderValue_InvalidSearch_ReturnsHeader(), totalTests, failedTests);
    RUN_TEST(AddHeader_ValidHeader_Success(), totalTests, failedTests);

    printf("------------------------------Serialistation TESTS-------------------------------\n");
    RUN_TEST(SerialiseHTTPVersion_AllVersions_SerialisedCorrectly(), totalTests, failedTests);
    RUN_TEST(SerialiseHTTPVersion_InvalidVersion_SerialisationFails(), totalTests, failedTests);
    RUN_TEST(SerialiseHTTPStatus_AllStatuses_SerialisedCorrectly(), totalTests, failedTests);
    RUN_TEST(SerialiseStatusLine_ValidStatusLine_SerialisedCorrectly(), totalTests, failedTests);
    RUN_TEST(SerialiseFieldLine_ValidFieldLine_SerialisedCorrectly(), totalTests, failedTests);
    RUN_TEST(SerialiseHeaders_ValidHeaders_SerialisedCorrectly(), totalTests, failedTests);
    RUN_TEST(SerialiseBody_ValidBody_SerialisedCorrectly(), totalTests, failedTests);
    RUN_TEST(SerialiseResponse_ValidResponse_SerialisedCorrectly(), totalTests, failedTests);

    printf("------------------------------END OF TESTS--------------------------------\n");
    printf("Tests run: %zu,\nTests failed: %zu\n", totalTests, failedTests);


    return 0;
}