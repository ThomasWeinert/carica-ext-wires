--TEST--
Check for wires presence
--SKIPIF--
<?php if (!extension_loaded("wires")) print "skip"; ?>
--FILE--
<?php
echo "wires extension is available";
?>
--EXPECT--
wires extension is available