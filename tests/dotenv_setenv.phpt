--TEST--
dotenv_setenv()
--SKIPIF--
<?php include('skipif.inc'); ?>
--ENV--
VARIABLE_TO_BE_OVERWRITTEN=1
VARIABLE_TO_BE_KEPT=2
--FILE--
<?php
dotenv_setenv(['VARIABLE_TO_BE_OVERWRITTEN' => '10'], true);
echo getenv('VARIABLE_TO_BE_OVERWRITTEN'), PHP_EOL;
dotenv_setenv(['VARIABLE_TO_BE_KEPT' => '20'], false);
echo getenv('VARIABLE_TO_BE_KEPT'), PHP_EOL;

$before = getenv();
dotenv_setenv([
        0       => '1',
        'KEY_1' => null,
        'KEY_2' => false,
        'KEY_3' => true,
        'KEY_4' => 1,
        'KEY_5' => 1.1,
        'KEY_6' => [],
    ], true
);
$after = getenv();
var_export($before == $after);
--EXPECT--
10
2
true