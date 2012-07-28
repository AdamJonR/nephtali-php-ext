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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_NEPHTALI_H
#define PHP_NEPHTALI_H

extern zend_module_entry nephtali_module_entry;
#define phpext_nephtali_ptr &nephtali_module_entry

#ifdef PHP_WIN32
#	define PHP_NEPHTALI_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_NEPHTALI_API __attribute__ ((visibility("default")))
#else
#	define PHP_NEPHTALI_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(nephtali);
PHP_MSHUTDOWN_FUNCTION(nephtali);
PHP_RINIT_FUNCTION(nephtali);
PHP_RSHUTDOWN_FUNCTION(nephtali);
PHP_MINFO_FUNCTION(nephtali);

PHP_FUNCTION(nephtali_markup_get_sections);
PHP_FUNCTION(nephtali_str_escape_html_callback);
PHP_FUNCTION(nephtali_str_escape_html);
PHP_FUNCTION(nephtali_row_databind);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:
*/

ZEND_BEGIN_MODULE_GLOBALS(nephtali)
	char* str_escape_html_charset;
ZEND_END_MODULE_GLOBALS(nephtali)

/* In every utility function you add that needs to use variables 
   in php_nephtali_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as NEPHTALI_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define NEPHTALI_G(v) TSRMG(nephtali_globals_id, zend_nephtali_globals *, v)
#else
#define NEPHTALI_G(v) (nephtali_globals.v)
#endif

#endif	/* PHP_NEPHTALI_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
