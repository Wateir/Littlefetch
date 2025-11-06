# Maintainer: wateir <contact@wateir.fr>

pkgname=LittleFetch-git
pkgver=r21.64969de
pkgrel=1
pkgdesc="A one-liner fetch to display the maximum useful information that the terminal allows"
arch=("any")
url="https://github.com/Wateir/LittleFetch"
license=('MIT')
provides=('LittleFetch')
depends=("git" "fzf" "pacman")
makedepends=("git")
source=("git+https://github.com/Wateir/LittleFetch.git")
packager="wateir <contact@wateir.fr>"
sha256sums=('SKIP')

pkgver() {
  cd "$srcdir/LittleFetch"
  printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short=7 HEAD)"
}

build() {
  cd "$srcdir/LittleFetch"
  make
}

package() {
  cd "$srcdir/LittleFetch"

  if [ -e "$pkgdir/usr/bin/LittleFetch" ]; then
    rm "$pkgdir/usr/bin/LittleFetch"
  fi

  install -Dm755 "prog" "$pkgdir/usr/bin/LittleFetch"
}
