
/* RawData_publisher.cxx

A publication of data of type RawData

This file is derived from code automatically generated by the rtiddsgen 
command:

rtiddsgen -language C++ -example <arch> RawData.idl

Example publication of type RawData automatically generated by 
'rtiddsgen'. To test them follow these steps:

(1) Compile this file and the example subscription.

(2) Start the subscription with the command
objs/<arch>/RawData_subscriber <domain_id> <sample_count>

(3) Start the publication with the command
objs/<arch>/RawData_publisher <domain_id> <sample_count>

(4) [Optional] Specify the list of discovery initial peers and 
multicast receive addresses via an environment variable or a file 
(in the current working directory) called NDDS_DISCOVERY_PEERS. 

You can run any number of publishers and subscribers programs, and can 
add and remove them dynamically from the domain.

Example:

To run the example application on domain <domain_id>:

On Unix: 

objs/<arch>/RawData_publisher <domain_id> o
objs/<arch>/RawData_subscriber <domain_id> 

On Windows:

objs\<arch>\RawData_publisher <domain_id>  
objs\<arch>\RawData_subscriber <domain_id>    

*/

#include <stdio.h>
#include <stdlib.h>

#include "RawData.h"
#include "RawDataSupport.h"
#include "ndds/ndds_cpp.h"

#include "DynamicArrayPrimitives_.h"
#include "DynamicArrayPrimitives_Plugin.h"
#include "DynamicArrayPrimitives_Support.h"

/* Delete all entities */
static int publisher_shutdown(
    DDSDomainParticipant *participant)
{
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = participant->delete_contained_entities();
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_contained_entities error %d\n", retcode);
            status = -1;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_participant error %d\n", retcode);
            status = -1;
        }
    }

    /* RTI Connext provides finalize_instance() method on
    domain participant factory for people who want to release memory used
    by the participant factory. Uncomment the following block of code for
    clean destruction of the singleton. */
    /*

    retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "finalize_instance error %d\n", retcode);
        status = -1;
    }
    */

    return status;
}

extern "C" int publisher_main(int domainId, int sample_count)
{
    DDSDomainParticipant *participant = NULL;
    DDSPublisher *publisher = NULL;
    DDSTopic *topic = NULL;
    DDSDataWriter *writer = NULL;
    RawDataDataWriter * RawData_writer = NULL;
    RawData *instance = NULL;
    DDS_ReturnCode_t retcode;
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    const char *type_name = NULL;
    struct DDS_TypeCode *type_code = NULL;
    int count = 0;  
    DDS_Duration_t send_period = {4,0};

    /* To customize participant QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    participant = DDSTheParticipantFactory->create_participant(
        domainId, DDS_PARTICIPANT_QOS_DEFAULT, 
        NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        fprintf(stderr, "create_participant error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* To customize publisher QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    publisher = participant->create_publisher(
        DDS_PUBLISHER_QOS_DEFAULT, NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        fprintf(stderr, "create_publisher error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Register type before creating topic */
    type_name = test_msgs_msg_dds__DynamicArrayPrimitives_TypeSupport::get_type_name();
    type_code = test_msgs_msg_dds__DynamicArrayPrimitives_TypeSupport::get_typecode();
    retcode = RawDataSupport_register_external_type(
        participant, type_name, type_code);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        publisher_shutdown(participant);
        return -1;
    }

    fprintf(stderr, "%s\n", type_name);

    /* To customize topic QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    topic = participant->create_topic(
        "my_generic_type",
        type_name,
        DDS_TOPIC_QOS_DEFAULT, NULL /* listener */,
        DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        fprintf(stderr, "create_topic error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* To customize data writer QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    writer = publisher->create_datawriter(
        topic, DDS_DATAWRITER_QOS_DEFAULT, NULL /* listener */,
        DDS_STATUS_MASK_NONE);
    if (writer == NULL) {
        fprintf(stderr, "create_datawriter error\n");
        publisher_shutdown(participant);
        return -1;
    }
    RawData_writer = RawDataDataWriter::narrow(writer);
    if (RawData_writer == NULL) {
        fprintf(stderr, "DataWriter narrow error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Create data sample for writing */
    instance = RawDataTypeSupport::create_data();
    instance->serialized_data.maximum(0);
    if (instance == NULL) {
        fprintf(stderr, "RawDataTypeSupport::create_data error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* For a data type that has a key, if the same instance is going to be
    written multiple times, initialize the key here
    and register the keyed instance prior to writing */
    instance_handle = RawData_writer->register_instance(*instance);


    test_msgs_msg_dds__DynamicArrayPrimitives_ * dynamic_array_instance =
      test_msgs_msg_dds__DynamicArrayPrimitives_TypeSupport::create_data();
    size_t length = 4;
    dynamic_array_instance->int32_values_.ensure_length(length, length);
    for (size_t i = 0; i < length; ++i) {
      dynamic_array_instance->int32_values_[i] = i;
    }
    strcpy(dynamic_array_instance->my_key, "abc");

    unsigned int serialization_buffer_size = test_msgs_msg_dds__DynamicArrayPrimitives_Plugin_get_serialized_sample_max_size(NULL, RTI_TRUE, 0, 0);
    fprintf(stderr, "serialization buffer size %u\n", serialization_buffer_size);
    if (serialization_buffer_size == RTI_CDR_MAX_SERIALIZED_SIZE) {
      fprintf(stderr, "max size ohje ohje\n");
      serialization_buffer_size = 1024;
    }
    DDS_Octet * serialization_buffer = (DDS_Octet *)malloc(serialization_buffer_size);
    for (unsigned int i = 0; i < serialization_buffer_size; ++i) {
      serialization_buffer[i] = 0;
    }

    if (!test_msgs_msg_dds__DynamicArrayPrimitives_Plugin_serialize_to_cdr_buffer(
          (char *)serialization_buffer, &serialization_buffer_size, dynamic_array_instance)) {
      fprintf(stderr, "Serialization of DyanmicArrayType failed\n");
      return false;
    }

    for (unsigned int i = 0; i < serialization_buffer_size; ++i) {
      fprintf(stderr, "%02x ", serialization_buffer[i]);
    }
    fprintf(stderr, "\n");
    instance->serialized_data.loan_contiguous(
        serialization_buffer, serialization_buffer_size, serialization_buffer_size);

    for (unsigned int i = 0; i < serialization_buffer_size; ++i) {
      fprintf(stderr, "%02x ", instance->serialized_data[i]);
    }
    fprintf(stderr, "\n");

    /* Main loop */
    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {

        printf("Writing RawData, count %d\n", count);

        /* Modify the data to be sent here */

        retcode = RawData_writer->write(*instance, instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "write error %d\n", retcode);
        }

        NDDSUtility::sleep(send_period);
    }

    instance->serialized_data.unloan();
    free(serialization_buffer);
    /*
    retcode = RawData_writer->unregister_instance(
        *instance, instance_handle);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "unregister instance error %d\n", retcode);
    }
    */

    /* Delete data sample */
    retcode = RawDataTypeSupport::delete_data(instance);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "RawDataTypeSupport::delete_data error %d\n", retcode);
    }

    /* Delete all entities */
    return publisher_shutdown(participant);
}

int main(int argc, char *argv[])
{
    int domainId = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    /* Uncomment this to turn on additional logging
    NDDSConfigLogger::get_instance()->
    set_verbosity_by_category(NDDS_CONFIG_LOG_CATEGORY_API, 
    NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
    */

    return publisher_main(domainId, sample_count);
}
