/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: krakjoe                                                      |
  +----------------------------------------------------------------------+
*/

/* $Id$ */
#ifndef HAVE_INSPECTOR
#define HAVE_INSPECTOR

#include "php.h"
#include "php_ini.h"
#include "zend_closures.h"

#include "ext/standard/info.h"
#include "ext/spl/spl_exceptions.h"
#include "php_inspector.h"

#include "inspector.h"

/* {{{ */
zend_object* php_inspector_create(zend_class_entry *ce) {
	php_inspector_t *inspector = ecalloc(1, sizeof(php_inspector_t) + zend_object_properties_size(ce));

	zend_object_std_init(&inspector->std, ce);	
	object_properties_init(&inspector->std, ce);

	inspector->std.handlers = &php_inspector_handlers;	

	return &inspector->std;
} /* }}} */

/* {{{ */
zend_function* php_inspector_find(zend_class_entry *scope, zend_string *name) {
	zend_string *lower = zend_string_tolower(name);
	zend_function *found = zend_hash_find_ptr(
		scope ? 
			&scope->function_table : 
			CG(function_table), 
		lower);
	zend_string_release(lower);
	return found;
} /* }}} */

/* {{{ */
void php_inspector_construct(zval *object, zend_function *function) {
	php_inspector_t *inspector = php_inspector_fetch(object);

	if (function->type != ZEND_USER_FUNCTION) {
		if (function->common.scope) {
			zend_throw_exception_ex(
				spl_ce_RuntimeException, 0, 
				"can not inspect internal function %s::%s", 
				ZSTR_VAL(function->common.scope->name), ZSTR_VAL(function->common.function_name));
		} else {
			zend_throw_exception_ex(
				spl_ce_RuntimeException, 0, 
				"can not inspect internal function %s", 
				ZSTR_VAL(function->common.function_name));
		}
		return;
	}

	inspector->ops = emalloc(sizeof(zend_op_array));
	memcpy(
		inspector->ops, function, sizeof(zend_op_array));
	function_add_ref((zend_function*)inspector->ops);
} /* }}} */

/* {{{ */
void php_inspector_destroy(zend_object *object) {
	php_inspector_t *inspector = 
		php_inspector_fetch_from(object);

	zend_object_std_dtor(object);

	if (inspector->ops) {
		destroy_op_array(inspector->ops);
		efree(inspector->ops);
	}
}
/* }}} */
#endif
