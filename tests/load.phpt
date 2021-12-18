--TEST--
Load ini file
--SKIPIF--
<?php require 'skipif.inc'; ?>
--INI--
dotenv.file = {PWD}/sample.env
--FILE--
<?php
var_export($_ENV['FOO']);
echo PHP_EOL;
var_export($_ENV['SOMETHING']);
echo PHP_EOL;
var_export($_ENV['SOMETHING_ELSE']);
echo PHP_EOL;
var_export($_ENV['VALUE']);
echo PHP_EOL;
var_export($_ENV['EMPTY']);
echo PHP_EOL;
var_export($_ENV['NULLVAL']);
echo PHP_EOL;
--EXPECT--
'bar'
'on'
'off'
'123'
''
'null'
