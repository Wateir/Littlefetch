# Maintainer: wateir <contact@wateir.fr>

pkgname=LittleFetch-git
pkgver=r24.a1c8312
pkgrel=1
pkgdesc="A one liner fetch to display the maximun usefull information that the windows allow"
arch=("any")
url="https://github.com/Wateir/LittleFetch"
licence=('MIT')
provides=('LittleFetch')
depends=("git" "fzf" "pacman")
makedepends=("git")
source=("git+https://github.com/Wateir/LittleFetch")
sha256sums=('SKIP')

pkgver() {
    cd "$srcdir/LittleFetch"
    printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short=7 HEAD)"
}

package() {

  cd "$srcdir/LittleFetch"
  make
}
package() {
  cd "$srcdir/LittleFetch"
  make

  if [ -e "$pkgdir/usr/bin/LittleFetch" ]; then
        rm "$pkgdir/usr/bin/LittleFetch"
  fi
  
  install -Dm755 "prog" "$pkgdir/usr/bin/LittleFetch"
}
