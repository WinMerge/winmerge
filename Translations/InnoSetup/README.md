# InnoSetup Translations

The `.ISL` (ANSI) or `.ISLU` (Unicode) Inno Setup language files are in INI format, which means you have a line that looks like this to translate:

```ini
SomeLabelName=I'm a label's text translate me
```

If a particular line doesn't already contain a period in it please don't add one, that means Inno Setup will add a period automatically. If you add a period to that line, then there will be two of them.

The other type of text you should never modify are variables and they come in two forms:

 * `%1`, `%2`, `%3`, ...
 * `[name]`, `[mb]`, `[name/ver]`, ...

If you translate some text and forget what it looked like in English, just have a look at `English.isl` and search for the same constant. Where `XYZ` is the name of the constant:

```ini
XYZ=English Text
```
