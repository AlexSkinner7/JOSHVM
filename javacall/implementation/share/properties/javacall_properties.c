/*
 *
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "javacall_defs.h"
#include "javautil_string.h"
#include "javacall_memory.h"
#include "javacall_properties.h"
#include "javacall_config_db.h"
#include "javacall_logging.h"
#include "javautil_string.h"

extern unsigned short property_file_name[JAVACALL_MAX_FILE_NAME_LENGTH+1];

static javacall_handle handle = NULL;
static int property_was_updated = 0;

static const char application_prefix[] = "application:";
static const char internal_prefix[] = "internal:";
static const char jsr75_prefix[] = "jsr75:";
static const char ams_prefix[] = "ams:";
static const char device_prefix[] = "device:";

/**
 * Initializes the configuration sub-system.
 *
 * @return <tt>JAVACALL_OK</tt> for success, JAVACALL_FAIL otherwise
 */
javacall_result javacall_initialize_configurations(void) {
    int file_name_len = javautil_wcslen(property_file_name);
    property_was_updated = 0;

    handle = javacall_configdb_load(property_file_name, file_name_len);
    if (handle == NULL) {
    	javacall_logging_printf(JAVACALL_LOGGING_ERROR, JC_NONE, "configdb load fail\n");
        return JAVACALL_FAIL;
    }

    return JAVACALL_OK;
}

/**
 * Finalize the configuration subsystem.
 */
void javacall_finalize_configurations(void) {
    if (property_was_updated != 0) {
#ifdef USE_PROPERTIES_FROM_FS
		int file_name_len = javautil_wcslen(property_file_name);
        javacall_configdb_dump_ini(handle, property_file_name, file_name_len);
#endif //USE_PROPERTIES_FROM_FS
    }
    javacall_configdb_free(handle);
    handle = NULL; 
}

/**
 * Gets the value of the specified property in the specified
 * property set.
 *
 * @param key The key to search for
 * @param type The property type 
 * @param result Where to put the result
 *
 * @return If found: <tt>JAVACALL_OK</tt>, otherwise
 *         <tt>JAVACALL_FAIL</tt>
 */
javacall_result javacall_get_property(const char* key, 
                                      javacall_property_type type,
                                      char** result){
    char* joined_key = NULL;

    if (JAVACALL_APPLICATION_PROPERTY == type) {
        #ifdef ENABLE_DYNAMIC_PROP
        {
            extern javacall_result javacall_get_dynamic_property(const char* key, char** result);
            if (JAVACALL_OK == javacall_get_dynamic_property(key, result)) {
                return JAVACALL_OK;
            }
        }
        #endif
        joined_key = javautil_string_strcat(application_prefix, key);
    } else if (JAVACALL_INTERNAL_PROPERTY == type) {
        joined_key = javautil_string_strcat(internal_prefix, key);
    } else if (JAVACALL_JSR75_PROPERTY == type) {
        joined_key = javautil_string_strcat(jsr75_prefix, key);
    } else if (JAVACALL_AMS_PROPERTY == type) {
        joined_key = javautil_string_strcat(ams_prefix, key);
    } else if (JAVACALL_DEVICE_PROPERTY == type) {
        joined_key = javautil_string_strcat(device_prefix, key);
    }

    if (joined_key == NULL) {
        *result = NULL;
        return JAVACALL_FAIL;       
    }

    if (JAVACALL_OK == javacall_configdb_getstring(handle, joined_key, NULL, result)) {
        javacall_free(joined_key);
        return JAVACALL_OK; 
    } else {
        javacall_free(joined_key);
        *result = NULL;
        return JAVACALL_FAIL;
    }
}

/**
 * Gets the value of the specified property in the specified
 * property set and parse it into integer.  If the property has not been found, 
 * assign NULL to result.
 *
 * @param key The key to search for
 * @param type The property type
 * @param int_value integer value parsed from property_value
 *
 * @return If found and parsed into int: <tt>JAVACALL_OK</tt>, otherwise
 *         <tt>JAVACALL_FAIL</tt>
 */
javacall_result javacall_get_property_int(const char* key, 
                                          javacall_property_type type,
                                          int *int_value) {
    char *str = NULL;

    if (JAVACALL_OK != javacall_get_property(key, type, &str)) {
        return JAVACALL_FAIL;
    }

    if (NULL == str) {
		return JAVACALL_FAIL;
	} else {
		*int_value = atoi(str);   
		return JAVACALL_OK;
	}
}

/**
 * Sets a property value matching the key in the specified
 * property set.
 *
 * @param key The key to set
 * @param value The value to set <tt>key</tt> to
 * @param replace_if_exist The value to decide if it's needed to replace
 * the existing value corresponding to the key if already defined
 * @param type The property type 
 * 
 * @return Upon success <tt>JAVACALL_OK</tt>, otherwise
 *         <tt>JAVACALL_FAIL</tt>
 */
javacall_result javacall_set_property(const char* key, 
                                      const char* value, 
                                      int replace_if_exist,
                                      javacall_property_type type) {
    char* joined_key = NULL;

    if (JAVACALL_APPLICATION_PROPERTY == type) {
        joined_key = javautil_string_strcat(application_prefix, key);
    } else if (JAVACALL_INTERNAL_PROPERTY == type) {
        joined_key = javautil_string_strcat(internal_prefix, key);
    } else if (JAVACALL_JSR75_PROPERTY == type) {
        joined_key = javautil_string_strcat(jsr75_prefix, key);
    } else if (JAVACALL_AMS_PROPERTY == type) {
        joined_key = javautil_string_strcat(ams_prefix, key);
    } else if (JAVACALL_DEVICE_PROPERTY == type) {
        joined_key = javautil_string_strcat(device_prefix, key);
    }

	javacall_logging_printf(JAVACALL_LOGGING_INFORMATION, JC_NONE, 
		"javacall_set_property: key=%s, value=%s, type=%d, replace=%d\n", key, value, type, replace_if_exist);

    if (joined_key == NULL) {
        return JAVACALL_FAIL;       
    }

    if (replace_if_exist == 0) { /* don't replace existing value */
        if (JAVACALL_OK == javacall_configdb_find_key(handle,joined_key)) {
            /* key exist, don't set */
            javacall_free(joined_key);
        } else {/* key doesn't exist, set it */
            javacall_configdb_setstr(handle, joined_key, (char *)value);
            property_was_updated=1;
            javacall_free(joined_key);
        }
    } else { /* replace existing value */
        javacall_configdb_setstr(handle, joined_key, (char *)value);
        javacall_free(joined_key);
        property_was_updated=1;
    }
    return JAVACALL_OK;
}



#ifdef __cplusplus
}
#endif

