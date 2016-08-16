--TEST--
Call setup with invalid argument expecting exception
--SKIPIF--
<?php if (!extension_loaded("wires")) print "skip"; ?>
--FILE--
<?php
use Carica\Gpio\WiringPi;
try {
  WiringPi\setup(42);
} catch (OutOfRangeException $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
Use Carica\Gpio\WiringPi::SETUP_* constant.
