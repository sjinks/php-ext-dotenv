--TEST--
Check $_ENV and getenv() match (variables_order=EGPCS)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--INI--
dotenv.file = {PWD}/sample.env
variables_order = EGPCS
--FILE--
<?php
assert($_ENV['FOO'] === getenv('FOO'));
assert($_ENV['SOMETHING'] === getenv('SOMETHING'));
assert($_ENV['SOMETHING_ELSE'] === getenv('SOMETHING_ELSE'));
assert($_ENV['VALUE'] === getenv('VALUE'));
assert($_ENV['EMPTY'] === getenv('EMPTY'));
assert($_ENV['NULLVAL'] === getenv('NULLVAL'));
--EXPECT--
