# Node bindings for hfst-ospell

This is a simple work-in-progress library which aims to offer spell checking using [Hfst-ospell](https://github.com/hfst/hfst-ospell) in node.js.

## Note on dictionary files

You can find several dictionaries on [divvun.no](http://www.divvun.no/korrektur/otherapps.html). Many are under the GPL, for some there is no license specified, though.

Assuming you accept the license terms, you can e.g. use

```sh
$ mkdir etc
$ curl http://divvun.no/static_files/zhfsts/se.zhfst > etc/se.zhfst
```

to download the dictionary for North Sámi.

## Install

This library is currently only known to work on my machine. If you are _really_ lucky – and you happen to have `tinyxml2` and `libarchive` installed – you might be able to get it working on your machine, too.

## Development

After you cloned this repository, make sure to also fetch the hfst-ospell sources using `git submodule update --init --recursive`.

You can build the library using `node-gyp configure build`. (Feel free to ignore any warnings on `lib/*` files.)

Use `npm test` to verify the library works on the node side. Please note that this requires a dictionary file and tries to read `etc/se.zhfst` by default.

## TODO

- [x] Make it compile!
- [ ] Everything with TODO and FIXME in code!
- [ ] Compile tinyxml2 ourselves (it's just one file)
- [ ] Compile libarchive ourselves (it's a truckload of stuff)