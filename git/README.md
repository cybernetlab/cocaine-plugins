## Git cocaine plugin

This plugin wraps [libgit2](https://libgit2.github.com/) library with cocaine service.

### Libgit2 install

Unfortunately the default `libgit2-dev` package contains non thread safe version of library. So to use this library with cocaine you should make and install it manually with `THREADSAFE` option:

```sh
git clone git://github.com/libgit2/libgit2.git libgit2
cd libgit2 && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr -DTHREADSAFE=ON ..
make
sudo make install
```

