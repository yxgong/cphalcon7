
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

#include "http/cookie.h"
#include "http/cookie/exception.h"
#include "cryptinterface.h"
#include "diinterface.h"
#include "di/injectable.h"
#include "filterinterface.h"
#include "session/adapterinterface.h"

#include <ext/standard/head.h>

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/exception.h"
#include "kernel/object.h"
#include "kernel/fcall.h"
#include "kernel/operators.h"
#include "kernel/array.h"
#include "kernel/file.h"
#include "kernel/concat.h"

#include "interned-strings.h"

/**
 * Phalcon\Http\Cookie
 *
 * Provide OO wrappers to manage a HTTP cookie
 */
zend_class_entry *phalcon_http_cookie_ce;

PHP_METHOD(Phalcon_Http_Cookie, __construct);
PHP_METHOD(Phalcon_Http_Cookie, setValue);
PHP_METHOD(Phalcon_Http_Cookie, getValue);
PHP_METHOD(Phalcon_Http_Cookie, send);
PHP_METHOD(Phalcon_Http_Cookie, restore);
PHP_METHOD(Phalcon_Http_Cookie, delete);
PHP_METHOD(Phalcon_Http_Cookie, useEncryption);
PHP_METHOD(Phalcon_Http_Cookie, isUsingEncryption);
PHP_METHOD(Phalcon_Http_Cookie, setExpiration);
PHP_METHOD(Phalcon_Http_Cookie, getExpiration);
PHP_METHOD(Phalcon_Http_Cookie, setPath);
PHP_METHOD(Phalcon_Http_Cookie, getPath);
PHP_METHOD(Phalcon_Http_Cookie, setDomain);
PHP_METHOD(Phalcon_Http_Cookie, getDomain);
PHP_METHOD(Phalcon_Http_Cookie, setSecure);
PHP_METHOD(Phalcon_Http_Cookie, getSecure);
PHP_METHOD(Phalcon_Http_Cookie, setHttpOnly);
PHP_METHOD(Phalcon_Http_Cookie, getHttpOnly);
PHP_METHOD(Phalcon_Http_Cookie, __toString);

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_http_cookie___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, expire)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, secure)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, httpOnly)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_http_cookie_setvalue, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_http_cookie_getvalue, 0, 0, 0)
	ZEND_ARG_INFO(0, filters)
	ZEND_ARG_INFO(0, defaultValue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_http_cookie_useencryption, 0, 0, 1)
	ZEND_ARG_INFO(0, useEncryption)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_http_cookie_setexpiration, 0, 0, 1)
	ZEND_ARG_INFO(0, expire)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_http_cookie_setpath, 0, 0, 1)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_http_cookie_setdomain, 0, 0, 1)
	ZEND_ARG_INFO(0, domain)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_http_cookie_setsecure, 0, 0, 1)
	ZEND_ARG_INFO(0, secure)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_http_cookie_sethttponly, 0, 0, 1)
	ZEND_ARG_INFO(0, httpOnly)
ZEND_END_ARG_INFO()

static const zend_function_entry phalcon_http_cookie_method_entry[] = {
	PHP_ME(Phalcon_Http_Cookie, __construct, arginfo_phalcon_http_cookie___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Phalcon_Http_Cookie, setValue, arginfo_phalcon_http_cookie_setvalue, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, getValue, arginfo_phalcon_http_cookie_getvalue, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, send, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, restore, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, delete, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, useEncryption, arginfo_phalcon_http_cookie_useencryption, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, isUsingEncryption, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, setExpiration, arginfo_phalcon_http_cookie_setexpiration, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, getExpiration, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, setPath, arginfo_phalcon_http_cookie_setpath, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, getPath, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, setDomain, arginfo_phalcon_http_cookie_setdomain, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, getDomain, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, setSecure, arginfo_phalcon_http_cookie_setsecure, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, getSecure, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, setHttpOnly, arginfo_phalcon_http_cookie_sethttponly, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, getHttpOnly, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Cookie, __toString, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Phalcon\Http\Cookie initializer
 */
PHALCON_INIT_CLASS(Phalcon_Http_Cookie){

	PHALCON_REGISTER_CLASS_EX(Phalcon\\Http, Cookie, http_cookie, phalcon_di_injectable_ce, phalcon_http_cookie_method_entry, 0);

	zend_declare_property_bool(phalcon_http_cookie_ce, SL("_readed"), 0, ZEND_ACC_PROTECTED);
	zend_declare_property_bool(phalcon_http_cookie_ce, SL("_restored"), 0, ZEND_ACC_PROTECTED);
	zend_declare_property_bool(phalcon_http_cookie_ce, SL("_useEncryption"), 0, ZEND_ACC_PROTECTED);
	zend_declare_property_null(phalcon_http_cookie_ce, SL("_filter"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(phalcon_http_cookie_ce, SL("_name"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(phalcon_http_cookie_ce, SL("_value"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(phalcon_http_cookie_ce, SL("_expire"), ZEND_ACC_PROTECTED);
	zend_declare_property_string(phalcon_http_cookie_ce, SL("_path"), "/", ZEND_ACC_PROTECTED);
	zend_declare_property_null(phalcon_http_cookie_ce, SL("_domain"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(phalcon_http_cookie_ce, SL("_secure"), ZEND_ACC_PROTECTED);
	zend_declare_property_bool(phalcon_http_cookie_ce, SL("_httpOnly"), 1, ZEND_ACC_PROTECTED);

	return SUCCESS;
}

/**
 * Phalcon\Http\Cookie constructor
 *
 * @param string $name
 * @param mixed $value
 * @param int $expire
 * @param string $path
 * @param boolean $secure
 * @param string $domain
 * @param boolean $httpOnly
 */
PHP_METHOD(Phalcon_Http_Cookie, __construct){

	zval *name, *value = NULL, *expire = NULL, *path = NULL, *secure = NULL, *domain = NULL;
	zval *http_only = NULL;

	phalcon_fetch_params(0, 1, 6, &name, &value, &expire, &path, &secure, &domain, &http_only);
	PHALCON_ENSURE_IS_STRING(name);

	if (!expire) {
		expire = &PHALCON_GLOBAL(z_zero);
	}

	phalcon_update_property_this(getThis(), SL("_name"), name);

	if (value && Z_TYPE_P(value) != IS_NULL) {
		phalcon_update_property_this(getThis(), SL("_value"), value);
		phalcon_update_property_bool(getThis(), SL("_readed"), 1);
	}

	phalcon_update_property_this(getThis(), SL("_expire"), expire);

	if (path && Z_TYPE_P(path) != IS_NULL) {
		phalcon_update_property_this(getThis(), SL("_path"), path);
	} else {
		zval tmp;
		ZVAL_STRINGL(&tmp, "/", 1);
		phalcon_update_property_this(getThis(), SL("_path"), &tmp);
	}

	if (secure && Z_TYPE_P(secure) != IS_NULL) {
		phalcon_update_property_this(getThis(), SL("_secure"), secure);
	}

	if (domain && Z_TYPE_P(domain) != IS_NULL) {
		phalcon_update_property_this(getThis(), SL("_domain"), domain);
	}

	if (http_only && Z_TYPE_P(http_only) != IS_NULL) {
		phalcon_update_property_this(getThis(), SL("_httpOnly"), http_only);
	}
}

/**
 * Sets the cookie's value
 *
 * @param string $value
 * @return Phalcon\Http\CookieInterface
 */
PHP_METHOD(Phalcon_Http_Cookie, setValue){

	zval *value;

	phalcon_fetch_params(0, 1, 0, &value);

	phalcon_update_property_this(getThis(), SL("_value"), value);
	phalcon_update_property_bool(getThis(), SL("_readed"), 1);
	RETURN_THISW();
}

/**
 * Returns the cookie's value
 *
 * @param string|array $filters
 * @param string $defaultValue
 * @return mixed
 */
PHP_METHOD(Phalcon_Http_Cookie, getValue){

	zval *filters = NULL, *default_value = NULL, *restored, *dependency_injector = NULL;
	zval *readed, *name, *_COOKIE, *value = NULL, *encryption;
	zval *service = NULL, *crypt = NULL, *decrypted_value = NULL, *filter = NULL;

	PHALCON_MM_GROW();

	phalcon_fetch_params(0, 0, 2, &filters, &default_value);

	if (!filters) {
		filters = &PHALCON_GLOBAL(z_null);
	}

	if (!default_value) {
		default_value = &PHALCON_GLOBAL(z_null);
	}

	restored = phalcon_read_property(getThis(), SL("_restored"), PH_NOISY);
	if (!zend_is_true(restored)) {
		PHALCON_CALL_METHOD(NULL, getThis(), "restore");
	}

	PHALCON_INIT_VAR(dependency_injector);

	readed = phalcon_read_property(getThis(), SL("_readed"), PH_NOISY);
	if (PHALCON_IS_FALSE(readed)) {
		name = phalcon_read_property(getThis(), SL("_name"), PH_NOISY);

		_COOKIE = phalcon_get_global(SL("_COOKIE"));
		if (phalcon_array_isset_fetch(&value, _COOKIE, name)) {
			encryption = phalcon_read_property(getThis(), SL("_useEncryption"), PH_NOISY);
			if (zend_is_true(encryption)) {
				dependency_injector = phalcon_read_property(getThis(), SL("_dependencyInjector"), PH_NOISY);
				if (Z_TYPE_P(dependency_injector) != IS_OBJECT) {
					PHALCON_THROW_EXCEPTION_STR(phalcon_http_cookie_exception_ce, "A dependency injection object is required to access the 'filter' service");
					return;
				}

				PHALCON_INIT_VAR(service);
				ZVAL_STRING(service, "crypt");

				PHALCON_CALL_METHOD(&crypt, dependency_injector, "getshared", service);
				PHALCON_VERIFY_INTERFACE(crypt, phalcon_cryptinterface_ce);

				/** 
				 * Decrypt the value also decoding it with base64
				 */
				PHALCON_CALL_METHOD(&decrypted_value, crypt, "decryptbase64", value);
			} else {
				PHALCON_CPY_WRT(decrypted_value, value);
			}

			/** 
			 * Update the decrypted value
			 */
			phalcon_update_property_this(getThis(), SL("_value"), decrypted_value);
			if (Z_TYPE_P(filters) != IS_NULL) {
				filter = phalcon_read_property(getThis(), SL("_filter"), PH_NOISY);
				if (Z_TYPE_P(filter) != IS_OBJECT) {
					if (Z_TYPE_P(dependency_injector) == IS_NULL) {
						dependency_injector = phalcon_read_property(getThis(), SL("_dependencyInjector"), PH_NOISY);
						PHALCON_VERIFY_INTERFACE_EX(dependency_injector, phalcon_diinterface_ce, phalcon_http_cookie_exception_ce, 1);
					}

					PHALCON_INIT_NVAR(service);
					ZVAL_STR(service, IS(filter));

					PHALCON_CALL_METHOD(&filter, dependency_injector, "getshared", service);
					PHALCON_VERIFY_INTERFACE(filter, phalcon_filterinterface_ce);
					phalcon_update_property_this(getThis(), SL("_filter"), filter);
				}

				PHALCON_RETURN_CALL_METHOD(filter, "sanitize", decrypted_value, filters);
				RETURN_MM();
			}

			/** 
			 * Return the value without filtering
			 */

			RETURN_CTOR(decrypted_value);
		}

		RETURN_CTOR(default_value);
	}

	value = phalcon_read_property(getThis(), SL("_value"), PH_NOISY);

	RETURN_CTOR(value);
}

/**
 * Sends the cookie to the HTTP client
 * Stores the cookie definition in session
 *
 * @return Phalcon\Http\Cookie
 */
PHP_METHOD(Phalcon_Http_Cookie, send){

	zval *name, *value, *expire, *domain, *path, *secure;
	zval *http_only, *dependency_injector, *definition;
	zval *service = NULL, *session = NULL, *key, *encryption, *crypt = NULL;
	zval *encrypt_value = NULL, *has_session = NULL;

	PHALCON_MM_GROW();

	name = phalcon_read_property(getThis(), SL("_name"), PH_NOISY);
	value = phalcon_read_property(getThis(), SL("_value"), PH_NOISY);
	expire = phalcon_read_property(getThis(), SL("_expire"), PH_NOISY);
	domain = phalcon_read_property(getThis(), SL("_domain"), PH_NOISY);
	path = phalcon_read_property(getThis(), SL("_path"), PH_NOISY);
	secure = phalcon_read_property(getThis(), SL("_secure"), PH_NOISY);
	http_only = phalcon_read_property(getThis(), SL("_httpOnly"), PH_NOISY);
	dependency_injector = phalcon_read_property(getThis(), SL("_dependencyInjector"), PH_NOISY);
	if (Z_TYPE_P(dependency_injector) == IS_OBJECT) {
		PHALCON_INIT_VAR(service);
		ZVAL_STR(service, IS(session));

		PHALCON_CALL_METHOD(&has_session, dependency_injector, "has", service);
		if (zend_is_true(has_session)) {
			PHALCON_INIT_VAR(definition);
			array_init(definition);
			if (!PHALCON_IS_LONG(expire, 0)) {
				phalcon_array_update_string(definition, SL("expire"), expire, PH_COPY);
			}

			if (PHALCON_IS_NOT_EMPTY(path)) {
				phalcon_array_update_string(definition, SL("path"), path, PH_COPY);
			}

			if (PHALCON_IS_NOT_EMPTY(domain)) {
				phalcon_array_update_str(definition, IS(domain), domain, PH_COPY);
			}

			if (PHALCON_IS_NOT_EMPTY(secure)) {
				phalcon_array_update_string(definition, SL("secure"), secure, PH_COPY);
			}

			if (PHALCON_IS_NOT_EMPTY(http_only)) {
				phalcon_array_update_string(definition, SL("httpOnly"), http_only, PH_COPY);
			}

			/**
			 * The definition is stored in session
			 */
			if (phalcon_fast_count_ev(definition)) {
				PHALCON_CALL_METHOD(&session, dependency_injector, "getshared", service);

				if (Z_TYPE_P(session) != IS_NULL) {
					PHALCON_VERIFY_INTERFACE(session, phalcon_session_adapterinterface_ce);

					PHALCON_INIT_VAR(key);
					PHALCON_CONCAT_SV(key, "_PHCOOKIE_", name);
					PHALCON_CALL_METHOD(NULL, session, "set", key, definition);
				}
			}
		}
	}

	encryption = phalcon_read_property(getThis(), SL("_useEncryption"), PH_NOISY);
	if (zend_is_true(encryption) && PHALCON_IS_NOT_EMPTY(value)) {
		if (Z_TYPE_P(dependency_injector) != IS_OBJECT) {
			PHALCON_THROW_EXCEPTION_STR(phalcon_http_cookie_exception_ce, "A dependency injection object is required to access the 'filter' service");
			return;
		}

		PHALCON_INIT_NVAR(service);
		ZVAL_STRING(service, "crypt");

		PHALCON_CALL_METHOD(&crypt, dependency_injector, "getshared", service);
		PHALCON_VERIFY_INTERFACE(crypt, phalcon_cryptinterface_ce);

		/**
		 * Encrypt the value also coding it with base64
		 */
		PHALCON_CALL_METHOD(&encrypt_value, crypt, "encryptbase64", value);
	} else {
		PHALCON_CPY_WRT(encrypt_value, value);
	}

	/** 
	 * Sets the cookie using the standard 'setcookie' function
	 */
	convert_to_string_ex(name);
	convert_to_string_ex(encrypt_value);
	convert_to_long_ex(expire);
	convert_to_string_ex(path);
	convert_to_string_ex(domain);
	convert_to_long_ex(secure);
	convert_to_long_ex(http_only);

	php_setcookie(Z_STR_P(name), Z_STR_P(encrypt_value), Z_LVAL_P(expire), Z_STR_P(path), Z_STR_P(domain), Z_LVAL_P(secure), 1, Z_LVAL_P(http_only));

	RETURN_THIS();
}

/**
 * Reads the cookie-related info from the SESSION to restore the cookie as it was set
 * This method is automatically called internally so normally you don't need to call it
 *
 * @return Phalcon\Http\Cookie
 */
PHP_METHOD(Phalcon_Http_Cookie, restore){

	zval *restored, *dependency_injector, *service;
	zval *session = NULL, *name, *key, *definition = NULL, *expire, *domain;
	zval *path, *secure, *http_only;

	PHALCON_MM_GROW();

	restored = phalcon_read_property(getThis(), SL("_restored"), PH_NOISY);
	if (!zend_is_true(restored)) {
		dependency_injector = phalcon_read_property(getThis(), SL("_dependencyInjector"), PH_NOISY);
		if (Z_TYPE_P(dependency_injector) == IS_OBJECT) {

			PHALCON_INIT_VAR(service);
			ZVAL_STR(service, IS(session));

			PHALCON_CALL_METHOD(&session, dependency_injector, "getshared", service);
			PHALCON_VERIFY_INTERFACE(session, phalcon_session_adapterinterface_ce);

			name = phalcon_read_property(getThis(), SL("_name"), PH_NOISY);

			PHALCON_INIT_VAR(key);
			PHALCON_CONCAT_SV(key, "_PHCOOKIE_", name);

			PHALCON_CALL_METHOD(&definition, session, "get", key);
			if (Z_TYPE_P(definition) == IS_ARRAY) { 
				if (phalcon_array_isset_str(definition, SL("expire"))) {
					PHALCON_OBS_VAR(expire);
					phalcon_array_fetch_str(&expire, definition, SL("expire"), PH_NOISY);
					phalcon_update_property_this(getThis(), SL("_expire"), expire);
				}
				if (phalcon_array_isset_str(definition, SL("domain"))) {
					PHALCON_OBS_VAR(domain);
					phalcon_array_fetch_str(&domain, definition, SL("domain"), PH_NOISY);
					phalcon_update_property_this(getThis(), SL("_domain"), domain);
				}

				if (phalcon_array_isset_str(definition, SL("path"))) {
					PHALCON_OBS_VAR(path);
					phalcon_array_fetch_str(&path, definition, SL("path"), PH_NOISY);
					phalcon_update_property_this(getThis(), SL("_path"), path);
				}

				if (phalcon_array_isset_str(definition, SL("secure"))) {
					PHALCON_OBS_VAR(secure);
					phalcon_array_fetch_str(&secure, definition, SL("secure"), PH_NOISY);
					phalcon_update_property_this(getThis(), SL("_secure"), secure);
				}

				if (phalcon_array_isset_str(definition, SL("httpOnly"))) {
					PHALCON_OBS_VAR(http_only);
					phalcon_array_fetch_str(&http_only, definition, SL("httpOnly"), PH_NOISY);
					phalcon_update_property_this(getThis(), SL("_httpOnly"), http_only);
				}
			}
		}

		phalcon_update_property_bool(getThis(), SL("_restored"), 1);
	}

	RETURN_THIS();
}

/**
 * Deletes the cookie by setting an expire time in the past
 *
 */
PHP_METHOD(Phalcon_Http_Cookie, delete){

	zval *name, *domain, *path, *secure, *http_only, *dependency_injector;
	zval *service, *session = NULL, *key;

	PHALCON_MM_GROW();

	name = phalcon_read_property(getThis(), SL("_name"), PH_NOISY);
	domain = phalcon_read_property(getThis(), SL("_domain"), PH_NOISY);
	path = phalcon_read_property(getThis(), SL("_path"), PH_NOISY);
	secure = phalcon_read_property(getThis(), SL("_secure"), PH_NOISY);
	http_only = phalcon_read_property(getThis(), SL("_httpOnly"), PH_NOISY);

	dependency_injector = phalcon_read_property(getThis(), SL("_dependencyInjector"), PH_NOISY);
	if (Z_TYPE_P(dependency_injector) == IS_OBJECT) {
		PHALCON_INIT_VAR(service);
		ZVAL_STR(service, IS(session));

		PHALCON_CALL_METHOD(&session, dependency_injector, "getshared", service);
		PHALCON_VERIFY_INTERFACE(session, phalcon_session_adapterinterface_ce);

		PHALCON_INIT_VAR(key);
		PHALCON_CONCAT_SV(key, "_PHCOOKIE_", name);
		PHALCON_CALL_METHOD(NULL, session, "remove", key);
	}

	phalcon_update_property_null(getThis(), SL("_value"));

	convert_to_string_ex(name);
	convert_to_string_ex(path);
	convert_to_string_ex(domain);
	convert_to_long_ex(secure);
	convert_to_long_ex(http_only);

	php_setcookie(Z_STR_P(name), NULL, time(NULL) - 691200, Z_STR_P(path), Z_STR_P(domain), Z_LVAL_P(secure), 1, Z_LVAL_P(http_only));

	PHALCON_MM_RESTORE();
}

/**
 * Sets if the cookie must be encrypted/decrypted automatically
 *
 * @param boolean $useEncryption
 * @return Phalcon\Http\Cookie
 */
PHP_METHOD(Phalcon_Http_Cookie, useEncryption){

	zval *use_encryption;

	phalcon_fetch_params(0, 1, 0, &use_encryption);

	phalcon_update_property_this(getThis(), SL("_useEncryption"), use_encryption);
	RETURN_THISW();
}

/**
 * Check if the cookie is using implicit encryption
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Http_Cookie, isUsingEncryption){


	RETURN_MEMBER(getThis(), "_useEncryption");
}

/**
 * Sets the cookie's expiration time
 *
 * @param int $expire
 * @return Phalcon\Http\Cookie
 */
PHP_METHOD(Phalcon_Http_Cookie, setExpiration){

	zval *expire, *restored;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 0, &expire);

	restored = phalcon_read_property(getThis(), SL("_restored"), PH_NOISY);
	if (!zend_is_true(restored)) {
		PHALCON_CALL_METHOD(NULL, getThis(), "restore");
	}

	phalcon_update_property_this(getThis(), SL("_expire"), expire);

	RETURN_THIS();
}

/**
 * Returns the current expiration time
 *
 * @return string
 */
PHP_METHOD(Phalcon_Http_Cookie, getExpiration){

	zval *restored, *expire;

	PHALCON_MM_GROW();

	restored = phalcon_read_property(getThis(), SL("_restored"), PH_NOISY);
	if (!zend_is_true(restored)) {
		PHALCON_CALL_METHOD(NULL, getThis(), "restore");
	}

	expire = phalcon_read_property(getThis(), SL("_expire"), PH_NOISY);

	RETURN_CTOR(expire);
}

/**
 * Sets the cookie's expiration time
 *
 * @param string $path
 * @return Phalcon\Http\Cookie
 */
PHP_METHOD(Phalcon_Http_Cookie, setPath){

	zval *path, *restored;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 0, &path);

	restored = phalcon_read_property(getThis(), SL("_restored"), PH_NOISY);
	if (!zend_is_true(restored)) {
		PHALCON_CALL_METHOD(NULL, getThis(), "restore");
	}

	phalcon_update_property_this(getThis(), SL("_path"), path);

	RETURN_THIS();
}

/**
 * Returns the current cookie's path
 *
 * @return string
 */
PHP_METHOD(Phalcon_Http_Cookie, getPath){

	zval *restored, *path;

	PHALCON_MM_GROW();

	restored = phalcon_read_property(getThis(), SL("_restored"), PH_NOISY);
	if (!zend_is_true(restored)) {
		PHALCON_CALL_METHOD(NULL, getThis(), "restore");
	}

	path = phalcon_read_property(getThis(), SL("_path"), PH_NOISY);

	RETURN_CTOR(path);
}

/**
 * Sets the domain that the cookie is available to
 *
 * @param string $domain
 * @return Phalcon\Http\Cookie
 */
PHP_METHOD(Phalcon_Http_Cookie, setDomain){

	zval *domain, *restored;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 0, &domain);

	restored = phalcon_read_property(getThis(), SL("_restored"), PH_NOISY);
	if (!zend_is_true(restored)) {
		PHALCON_CALL_METHOD(NULL, getThis(), "restore");
	}

	phalcon_update_property_this(getThis(), SL("_domain"), domain);

	RETURN_THIS();
}

/**
 * Returns the domain that the cookie is available to
 *
 * @return string
 */
PHP_METHOD(Phalcon_Http_Cookie, getDomain){

	zval *restored, *domain;

	PHALCON_MM_GROW();

	restored = phalcon_read_property(getThis(), SL("_restored"), PH_NOISY);
	if (!zend_is_true(restored)) {
		PHALCON_CALL_METHOD(NULL, getThis(), "restore");
	}

	domain = phalcon_read_property(getThis(), SL("_domain"), PH_NOISY);

	RETURN_CTOR(domain);
}

/**
 * Sets if the cookie must only be sent when the connection is secure (HTTPS)
 *
 * @param boolean $secure
 * @return Phalcon\Http\Cookie
 */
PHP_METHOD(Phalcon_Http_Cookie, setSecure){

	zval *secure, *restored;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 0, &secure);

	restored = phalcon_read_property(getThis(), SL("_restored"), PH_NOISY);
	if (!zend_is_true(restored)) {
		PHALCON_CALL_METHOD(NULL, getThis(), "restore");
	}

	phalcon_update_property_this(getThis(), SL("_secure"), secure);

	RETURN_THIS();
}

/**
 * Returns whether the cookie must only be sent when the connection is secure (HTTPS)
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Http_Cookie, getSecure){

	zval *restored, *secure;

	PHALCON_MM_GROW();

	restored = phalcon_read_property(getThis(), SL("_restored"), PH_NOISY);
	if (!zend_is_true(restored)) {
		PHALCON_CALL_METHOD(NULL, getThis(), "restore");
	}

	secure = phalcon_read_property(getThis(), SL("_secure"), PH_NOISY);

	RETURN_CTOR(secure);
}

/**
 * Sets if the cookie is accessible only through the HTTP protocol
 *
 * @param boolean $httpOnly
 * @return Phalcon\Http\Cookie
 */
PHP_METHOD(Phalcon_Http_Cookie, setHttpOnly){

	zval *http_only, *restored;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 0, &http_only);

	restored = phalcon_read_property(getThis(), SL("_restored"), PH_NOISY);
	if (!zend_is_true(restored)) {
		PHALCON_CALL_METHOD(NULL, getThis(), "restore");
	}

	phalcon_update_property_this(getThis(), SL("_httpOnly"), http_only);

	RETURN_THIS();
}

/**
 * Returns if the cookie is accessible only through the HTTP protocol
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Http_Cookie, getHttpOnly){

	zval *restored, *http_only;

	PHALCON_MM_GROW();

	restored = phalcon_read_property(getThis(), SL("_restored"), PH_NOISY);
	if (!zend_is_true(restored)) {
		PHALCON_CALL_METHOD(NULL, getThis(), "restore");
	}

	http_only = phalcon_read_property(getThis(), SL("_httpOnly"), PH_NOISY);

	RETURN_CTOR(http_only);
}

/**
 * Magic __toString method converts the cookie's value to string
 *
 * @return mixed
 */
PHP_METHOD(Phalcon_Http_Cookie, __toString){

	zval *value;

	value = phalcon_read_property(getThis(), SL("_value"), PH_NOISY);
	if (Z_TYPE_P(value) == IS_NULL) {
		if (FAILURE == phalcon_call_class_method_aparams(&return_value, getThis(), Z_OBJCE_P(getThis()), phalcon_fcall_method, "getvalue", 8, 0, NULL)) {
			if (EG(exception)) {
				zval e;
				ZVAL_OBJ(&e, EG(exception));
				zval *m = zend_read_property(Z_OBJCE(e), &e, SL("message"), 1, NULL);

				Z_TRY_ADDREF_P(m);
				if (Z_TYPE_P(m) != IS_STRING) {
					convert_to_string_ex(m);
				}

				zend_clear_exception();
				zend_error(E_ERROR, "%s", Z_STRVAL_P(m));
				zval_ptr_dtor(m);
			}
		}

		convert_to_string(return_value);
		return;
	}

	RETURN_ZVAL(value, 1, 0);
}
