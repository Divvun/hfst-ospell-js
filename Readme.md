# Node bindings for hfst-ospell

This is a simple work-in-progress library which aims to offer spell checking using [Hfst-ospell](https://github.com/hfst/hfst-ospell) in node.js.

[![Build Status](https://travis-ci.org/divvun/hfst-ospell-js.svg?branch=master)](https://travis-ci.org/divvun/hfst-ospell-js)

## Note on dictionary files

You can find several dictionaries on [divvun.no](http://www.divvun.no/korrektur/otherapps.html). Many are under the GPL, for some there is no license specified, though.

Assuming you accept the license terms, you can e.g. use

```sh
$ mkdir etc
$ curl http://divvun.no/static_files/zhfsts/se.zhfst > etc/se.zhfst
```

to download the dictionary for North Sámi.

## Install

To install this module, you first need to make sure you have

- a C++ compiler,
- Python (for node-gyp),
- and `libarchive` (using e.g. `apt-get install libarchive-dev` or `brew install libarchive && brew link libarchive --force`)

installed.

You can then use `npm install divvun/hfst-ospell-js` to install the node module directly from GitHub (it is not yet published to npm).

**Note:** NPM versions older than 3.7.0 do not resolve git submodules when installing, so you may need to clone this repository manually and use `npm install` with a local path.

## Usage

The API is pretty simple:

```js
var hfstospell = require("hfst-ospell-js");
var path_to_dictionary = "etc/se.zhfst";
var spellchecker = new hfstospell.SpellChecker(path_to_dictionary);

// .suggestions(string) returns a Promise
spellchecker.suggestions("akkusativa")
.then((suggestions) => console.log(suggestions))
.catch((error) => console.error(suggestions));
// => ['akkusatiivva', 'akkusatiiva', 'akkusatiivan']

// But you can also use it with a callback
spellchecker.suggestions("akkusativa", (error, suggestions) =>
  console.log(error, suggestions));
// => null, ['akkusatiivva', 'akkusatiiva', 'akkusatiivan']
```

## Development

After you cloned this repository, make sure to also fetch the hfst-ospell sources using `git submodule update --init --recursive`.

You can build the library using `node-gyp configure build`. (Feel free to ignore any warnings on `lib/*` files.)

Use `npm test` to verify the library works on the node side. Please note that this requires a dictionary file and tries to read `etc/se.zhfst` by default.

## TODO

- [x] Make it compile!
- [x] Async with a small JS wrapper for Promise support
- [ ] Investigate thread safety of hfst-ospell (we are currently using Mutexes to only ever search for one suggestion at a time)
- [x] Use a constructor for the wrapped C++ object so we can re-use the loaded spell checker
- [ ] Everything with TODO and FIXME in code!
- [x] Compile tinyxml2 ourselves (it's just one file)
- [ ] Compile libarchive ourselves (it's a truckload of stuff)
