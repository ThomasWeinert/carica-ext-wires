Carica Wires
============

License: [The MIT License](https://opensource.org/licenses/mit-license.php)

Copyright: 2016 Thomas Weinert <thomas@weinert.info>

Carica Wires is a PHP 7 wrapper extension for the WiringPi library.

To install the extension clone or download the repository. After that phpize, configure and make it.

```
phpize
./configure
make
sudo make install
```

You need to load the extension in your `php.ini`.

```
extension=wires.so
```

To validate that the extension is installed execute:

```
php -ri wires
```

Basic usage
-----------

The extension defines the function inside the namespace `Carica\Gpio\WiringPi`. You will need to call the
`setup` function before any other.

```php
<?php
use Carica\Gpio\WiringPi;

WiringPi\setup();

$pin = "26";
WiringPi\pinMode($pin, WiringPi\OUTPUT);

$on = false;
while (TRUE) {
    echo ($on = !$on) ? "on\n" : "off\n";
    WiringPi\digitalWrite($pin, $on ? WiringPi\HIGH : WiringPi\LOW);
    sleep(1);
}
```



