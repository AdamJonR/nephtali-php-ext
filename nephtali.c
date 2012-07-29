/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Adam Jon Richardson                                          |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include "php.h"
#include "php_ini.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include "php_nephtali.h"
#include "ext/standard/php_string.h"
#include "ext/standard/html.h"
#include "ext/pcre/php_pcre.h"

ZEND_DECLARE_MODULE_GLOBALS(nephtali)

/* {{{ nephtali_functions[]
 *
 * Every user visible function must have an entry in nephtali_functions[].
 */
const zend_function_entry nephtali_functions[] = {
	PHP_FE(nephtali_markup_get_sections,	NULL)
	PHP_FE(nephtali_str_escape_html_callback,	NULL)
	PHP_FE(nephtali_str_escape_html,	NULL)
	PHP_FE(nephtali_row_databind,	NULL)
	PHP_FE_END	/* Must be the last line in nephtali_functions[] */
};
/* }}} */

/* {{{ nephtali_module_entry
 */
zend_module_entry nephtali_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"nephtali",
	nephtali_functions,
	PHP_MINIT(nephtali),
	PHP_MSHUTDOWN(nephtali),
	PHP_RINIT(nephtali),     /* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(nephtali), /* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(nephtali),
#if ZEND_MODULE_API_NO >= 20010901
	"1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_NEPHTALI
ZEND_GET_MODULE(nephtali)
#endif

static void php_nephtali_init_globals(zend_nephtali_globals *nephtali_globals TSRMLS_DC)
{
	nephtali_globals->str_escape_html_charset = NULL;
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(nephtali)
{
	ZEND_INIT_MODULE_GLOBALS(nephtali, php_nephtali_init_globals, NULL);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(nephtali)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(nephtali)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "nephtali support", "enabled");
	php_info_print_table_end();

}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(nephtali)
{
	NEPHTALI_G(str_escape_html_charset) = estrdup("UTF-8");
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_SHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(nephtali)
{
	efree(NEPHTALI_G(str_escape_html_charset));
	return SUCCESS;
}
/* }}} */

/* {{{ proto array nephtali_markup_get_sections(string markup, string type [, string name [, bool is_optional]])
   Returns array containing markup sections */
PHP_FUNCTION(nephtali_markup_get_sections)
{
	zend_bool is_optional = 0;
	char *markup = NULL;
	char *type = NULL;
	char *name = NULL;
	char *comment_open = "<!--";
	char *comment_close = "-->";
	char *delimiter_temp[100];
	int argc = ZEND_NUM_ARGS();
	int markup_len = 0;
	int type_len = 0;
	int name_len = 0;
	int comment_open_len = (sizeof comment_open) - 1;
	int comment_close_len = (sizeof comment_close) - 1;
	int return_val_count = 0;
	int delimiter_temp_len = 0;
	zval *delimiter, *zmarkup;
	HashTable *return_val_hash;
	// parse params
	if (zend_parse_parameters(argc TSRMLS_CC, "ss|sb", &markup, &markup_len, &type, &type_len, &name, &name_len, &is_optional) == FAILURE) 
		return;
	// initialize return array
	array_init(return_value);
	// return if the string is empty
	if (!markup_len) {
		// add array index of empty string, just as was sent
		add_next_index_string(return_value, "", 1);
		return;
	}
	// return if type is longer than 40 characters
	if (type_len > 40) {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), "The \"name\" argument to nephtali_markup_get_sections() has a maximum length of 40 characters.", 0 TSRMLS_CC);
		return;
	}
	// return if name is not null and longer than 40 characters
	if (name != NULL && name_len > 40) {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), "The \"type\" argument to nephtali_markup_get_sections() has a maximum length of 40 characters.", 0 TSRMLS_CC);
		return;
	}
	// define zval version of markup
	MAKE_STD_ZVAL(zmarkup);
	ZVAL_STRINGL(zmarkup, markup, markup_len, 1);
	// define delimiter_temp and because of usage context being limited to html comments, don't worry about binary safety and just use strcat
	strcpy(delimiter_temp, comment_open);
	if (name_len == 0) {
		delimiter_temp_len = comment_open_len + type_len + comment_close_len;
		strcat(delimiter_temp, type);
		strcat(delimiter_temp, comment_close);
	} else {
		delimiter_temp_len = comment_open_len + type_len + 1 + name_len + comment_close_len;
		strcat(delimiter_temp, type);
		strcat(delimiter_temp, ":");
		strcat(delimiter_temp, name);
		strcat(delimiter_temp, comment_close);
	}
	// define delimiter
	MAKE_STD_ZVAL(delimiter);
	ZVAL_STRING(delimiter, delimiter_temp, 1);
	// break up markup into sections
	php_explode(delimiter, zmarkup, return_value, LONG_MAX);
	return_val_hash = Z_ARRVAL_P(return_value);
	return_val_count = zend_hash_num_elements(return_val_hash);
	// free resources
	zval_ptr_dtor(&delimiter);
	zval_ptr_dtor(&zmarkup);
	// check if correct number of regions present
	if (return_val_count == 3 || (is_optional && return_val_count == 1)) {
		// return valid array
		return;
	} else {
		char *err_message[200];
		// throw an exception because the number of sections is not valid
		if (name == NULL) {
			sprintf(err_message, "The number of sections found for the %s comment region were invalid.", type);
			zend_throw_exception(zend_exception_get_default(TSRMLS_C), err_message, 0 TSRMLS_CC);
			return;
		} else {
			sprintf(err_message, "The number of sections found for the %s comment region named %s were invalid.", type, name);
			zend_throw_exception(zend_exception_get_default(TSRMLS_C), err_message, 0 TSRMLS_CC);
			return;
		}
	}
}
/* }}} */

/* {{{ proto string nephtali_str_escape_html_callback(array matches)
   Returns the first match after unescaping any html entities */
PHP_FUNCTION(nephtali_str_escape_html_callback)
{
	int argc = ZEND_NUM_ARGS();
	int matches_count = 0, output_len = 0;
	char *output = NULL;
	zval *matches = NULL, **data;
	HashTable *matches_hash;
	HashPosition pointer;
	if (zend_parse_parameters(argc TSRMLS_CC, "a", &matches) == FAILURE)
		return;

	// define matches vars
	matches_hash = Z_ARRVAL_P(matches);
	matches_count = zend_hash_num_elements(matches_hash);
	// return if the matches array is empty
	if (!matches_count) {
		RETURN_EMPTY_STRING();
	}
	// get first array item matches[0]
	zend_hash_internal_pointer_reset_ex(matches_hash, &pointer);
	if ((zend_hash_get_current_data_ex(matches_hash, (void**) &data, &pointer) == SUCCESS) && (Z_TYPE_PP(data) == IS_STRING)) {
		output = php_unescape_html_entities(Z_STRVAL_PP(data), Z_STRLEN_PP(data), &output_len, 0, ENT_QUOTES, NEPHTALI_G(str_escape_html_charset) TSRMLS_CC);
		RETURN_STRING(output, 0);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto string nephtali_str_escape_html(string str [, array whitelist [, string charset]])
   Returns html-escaped output whilst preserving whitelist items, including those identified by regex */
PHP_FUNCTION(nephtali_str_escape_html)
{
	int argc = ZEND_NUM_ARGS();
	char *str, *charset, *new_str, *escaped_data, *temp_str, *temp_data_str;
	int str_len = 0, new_str_len = 0, charset_len = 0, replacements = 0, whitelist_count = 0, i = 0, escaped_data_len = 0, _new_new_str_len = 0, temp_data_str_len = 0, regex_replace_count = 0;
	zval *whitelist, **data, *pcre_callback;
	HashTable *whitelist_hash;
	HashPosition pointer;
	// set defaults
	whitelist = NULL;
	charset = "UTF-8";
	charset_len = sizeof(charset) - 1;
	// parse and cast params
	if (zend_parse_parameters(argc TSRMLS_CC, "s|as", &str, &str_len, &whitelist, &charset, &charset_len) == FAILURE) {
		return;
	}
	// test for characters to be escaped
	for (i = 0; i < str_len; i++) {
		if (str[i] == '<' || str[i] == '>' || str[i] == '&' || str[i] == '"' || str[i] == '\'') {
			replacements++;
			// break out of loop after finding a char needing escaping
			break;
		}
	}
	// return original string if there are no characters to be escaped
	if (!replacements) {
		RETURN_STRING(str, 1);
	}
	// escape <,>,',", and & characters
	new_str = php_escape_html_entities(str, str_len, &new_str_len, 0, ENT_QUOTES, charset TSRMLS_CC);
	// return new_str if there is no whitelist
	if (!whitelist) {
		RETURN_STRING(new_str, 0);
	}
	// define up whitelist vars
	whitelist_hash = Z_ARRVAL_P(whitelist);
	whitelist_count = zend_hash_num_elements(whitelist_hash);
	// return new_str if whitelist is empty
	if (!whitelist_count) {
		RETURN_STRING(new_str, 0);
	}
	// define calback vars
	MAKE_STD_ZVAL(pcre_callback);
	ZVAL_STRING(pcre_callback, "nephtali_str_escape_html_callback", 1);
	// update global charset for use in pcre callback
	if (strcmp(charset, NEPHTALI_G(str_escape_html_charset))) {
		efree(NEPHTALI_G(str_escape_html_charset));
		NEPHTALI_G(str_escape_html_charset) = estrdup(charset);
	}
	// loop through whitelists and perform string replacements
	for(
		zend_hash_internal_pointer_reset_ex(whitelist_hash, &pointer);
		zend_hash_get_current_data_ex(whitelist_hash, (void**) &data, &pointer) == SUCCESS;
		zend_hash_move_forward_ex(whitelist_hash, &pointer)
	) {
		if (Z_TYPE_PP(data) == IS_STRING) {
			temp_data_str = Z_STRVAL_PP(data);
			temp_data_str_len = Z_STRLEN_PP(data);
			// only finds and allows regexes without pattern modifiers www.php.net/manual/en/reference.pcre.pattern.modifiers.php
			if (temp_data_str[0] == '/' && temp_data_str[temp_data_str_len - 1] == '/') {
				// regex replacement
				// can escape regex because <,>,",', and & are not meta characters
				escaped_data = php_escape_html_entities(temp_data_str, temp_data_str_len, &escaped_data_len, 0, ENT_QUOTES, charset TSRMLS_CC);
				temp_str = php_pcre_replace(escaped_data, escaped_data_len, new_str, new_str_len, pcre_callback, 1, &_new_new_str_len, -1, &regex_replace_count TSRMLS_CC);
			} else {
				// standard string replacement
				escaped_data = php_escape_html_entities(temp_data_str, temp_data_str_len, &escaped_data_len, 0, ENT_QUOTES, charset TSRMLS_CC); 
				temp_str = php_str_to_str_ex(new_str, new_str_len, escaped_data, escaped_data_len, temp_data_str, temp_data_str_len, &_new_new_str_len, 0, NULL);
			}
			// clean up escaped_data
			efree(escaped_data);
			// save temp_str to new_str after freeing when non-null value returned (invalid regex could cause)
			if (temp_str != NULL) {
				efree(new_str);
				new_str = temp_str;
			}
		}
	}
	// clean up
	zval_ptr_dtor(&pcre_callback);
	// return string
	RETURN_STRING(new_str, 0);
}
/* }}} */

/* {{{ proto string nephtali_row_databind(string markup [, array row [, array opts]])
   Databinds associative array row to markup by mappings */
PHP_FUNCTION(nephtali_row_databind)
{
	char *markup = NULL;
	int argc = ZEND_NUM_ARGS();
	int markup_len;
	zval *row = NULL;
	zval *opts = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "s|aa", &markup, &markup_len, &row, &opts) == FAILURE) 
		return;

	php_error(E_WARNING, "nephtali_row_databind: not yet implemented");
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
