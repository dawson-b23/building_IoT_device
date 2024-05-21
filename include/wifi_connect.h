/**
 * @file wifi_connect.h
 * @author Dawson Burgess (burg1648@vandals.uidaho.edu)
 * @brief Used for keeping defines for local SSID/PASS
 * 
 * @version 0.1
 * @date 2024-05-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//#define DAWSON_MOBILE 1
#define DAWSON_NETWORK

// WIFI connectivity via mobile hotspot for Dawson 
#ifdef DAWSON_MOBILE
    // Add your mobile hotspot information here
    #define SECRET_SSID "Dawsons_Phone"
    #define SECRET_PASS "quentintanner"
#endif

// WIFI connectivity via home network for Dawson 
#ifdef DAWSON_NETWORK
    // Add your mobile hotspot information here
    #define SECRET_SSID "Gritman State University"
    #define SECRET_PASS "YUMYUMCRANS"
#endif
