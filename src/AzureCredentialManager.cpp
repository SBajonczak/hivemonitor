#include "AzureCredentialManager.h"

#include <base64.h>
#include <bearssl/bearssl.h>
#include <bearssl/bearssl_hmac.h>
#include <libb64/cdecode.h>
#include <az_core.h>
#include <az_iot.h>

#define DEVICE_ID "DEV-DEVICE-SBA"
#define MQTT_SERVER "sba-iot-hub.azure-devices.net"
#define MQTT_PORT 8883

#define sizeofarray(a) (sizeof(a) / sizeof(a[0]))
#define AZURE_SDK_CLIENT_USER_AGENT "c/" AZ_SDK_VERSION_STRING "(ard;esp8266)"
static az_iot_hub_client client;

AzureCredentialManager::AzureCredentialManager()
{
    az_iot_hub_client_options options = az_iot_hub_client_options_default();
    options.user_agent = AZ_SPAN_FROM_STR(AZURE_SDK_CLIENT_USER_AGENT);
    if (az_result_failed(az_iot_hub_client_init(
            &client,
            az_span_create((uint8_t *)MQTT_SERVER, strlen(MQTT_SERVER)),
            az_span_create((uint8_t *)DEVICE_ID, strlen(DEVICE_ID)),
            &options)))
    {
        Serial.println("Failed initializing Azure IoT Hub client");
        return;
    }
}
void AzureCredentialManager::setup()
{
}

String AzureCredentialManager::getClientID()
{

    size_t client_id_length;
    char mqtt_client_id[128];
    if (az_result_failed(az_iot_hub_client_get_client_id(
            &client, mqtt_client_id, sizeof(mqtt_client_id) - 1, &client_id_length)))
    {
        Serial.println("Failed getting client id");
        return "";
    }

    mqtt_client_id[client_id_length] = '\0';

    return String(mqtt_client_id);
}

static uint32_t getSecondsSinceEpoch()
{
    return (uint32_t)time(NULL);
}
String AzureCredentialManager::GenerateSasToken()
{

    char sas_token[400];
    size_t size = sizeof(sas_token);
    char base64_decoded_device_key[32];
    uint8_t signature[512];
    unsigned char encrypted_signature[32];
    char *device_key = "vZPo/5wkxOO4SBOzhib6nq2WZ3wOUP2jXSr9Du/Q1PA=";

    az_span signature_span = az_span_create((uint8_t *)signature, sizeof(signature));
    az_span out_signature_span;
    az_span encrypted_signature_span = az_span_create((uint8_t *)encrypted_signature, sizeof(encrypted_signature));

    uint32_t expiration = getSecondsSinceEpoch() + 60; // one hour in seconds

    // Get signature
    if (az_result_failed(az_iot_hub_client_sas_get_signature(&client, expiration, signature_span, &out_signature_span)))
    {
        Serial.println("Failed getting SAS signature");
        return "";
    }
    else
    {
        Serial.println("Succeed getting SAS signature");
    }

    // Base64-decode device key
    int base64_decoded_device_key_length = base64_decode_chars(device_key, strlen(device_key), base64_decoded_device_key);

    if (base64_decoded_device_key_length == 0)
    {
        Serial.println("Failed base64 decoding device key");
        return "";
    }
    else
    {
        Serial.println("Succeed base64 decoding device key");
    }

    // SHA-256 encrypt
    br_hmac_key_context kc;
    br_hmac_key_init(&kc, &br_sha256_vtable, base64_decoded_device_key, base64_decoded_device_key_length);

    br_hmac_context hmac_ctx;
    br_hmac_init(&hmac_ctx, &kc, 32);
    br_hmac_update(&hmac_ctx, az_span_ptr(out_signature_span), az_span_size(out_signature_span));
    br_hmac_out(&hmac_ctx, encrypted_signature);

    // Base64 encode encrypted signature
    String b64enc_hmacsha256_signature = base64::encode(encrypted_signature, br_hmac_size(&hmac_ctx));
    az_span b64enc_hmacsha256_signature_span = az_span_create((uint8_t *)b64enc_hmacsha256_signature.c_str(), b64enc_hmacsha256_signature.length());

    Serial.println("Try to generate a new password");

    // Generate SAS Token
    if (az_result_failed(az_iot_hub_client_sas_get_password(
            &client,
            expiration,
            b64enc_hmacsha256_signature_span,
            AZ_SPAN_EMPTY,
            sas_token,
            size,
            NULL)))
    {
        Serial.println("Failed getting SAS token");
        return "";
    }

    return String(sas_token);
}

String AzureCredentialManager::GetUserName()
{
    char mqtt_username[128];
    az_iot_hub_client_get_user_name(&client, mqtt_username, sizeof(mqtt_username), NULL);

    return String(mqtt_username);
}
String AzureCredentialManager::GetTopicName()
{
    static char telemetry_topic[128];

    if (az_result_failed(az_iot_hub_client_telemetry_get_publish_topic(
            &client, NULL, telemetry_topic, sizeof(telemetry_topic), NULL)))
    {
        Serial.println("Failed az_iot_hub_client_telemetry_get_publish_topic");
        return "";
    }
    return String(telemetry_topic);
}