--TEST--
dotenv_find_file_upward()
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
var_export(dotenv_find_file_upward(__DIR__, basename(__FILE__)));
echo PHP_EOL;
var_export(dotenv_find_file_upward(__DIR__, 'config.m4'));
echo PHP_EOL;
var_export(dotenv_find_file_upward(__DIR__, 'this-file-does-not-exist-really-really'));
echo PHP_EOL;
var_export(dotenv_find_file_upward('.', 'config.m4'));
echo PHP_EOL;
var_export(dotenv_find_file_upward('.', 'this-file-does-not-exist-really-really'));
echo PHP_EOL;
--EXPECTF--
'%s%etests%edotenv_find_file_upward.php'
'%s%econfig.m4'
NULL
'%s%econfig.m4'
NULL
