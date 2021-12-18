--TEST--
Load ini file
--SKIPIF--
<?php require 'skipif.inc'; ?>
--INI--
dotenv.file = {PWD}/sample.env
--FILE--
<?php
echo getenv('FOO'), PHP_EOL;
echo getenv('SOMETHING'), PHP_EOL;
echo getenv('SOMETHING_ELSE'), PHP_EOL;
echo getenv('VALUE'), PHP_EOL;
echo getenv('EMPTY'), PHP_EOL;
echo getenv('NULLVAL'), PHP_EOL;
--EXPECT--
bar
on
off
123

null
