pkgname=gpio-switch-poller
pkgver=1
pkgrel=0
pkgdesc="pkg-config files"
arch=('armv7h')
depends=(libbcm2835)

source=(gpio_test.c gpio-switch-poller.service)
md5sums=(SKIP SKIP)

build() {
  gcc gpio_test.c -lbcm2835 -Wall -o gpio-switch-poller
}

package() {
  mkdir -p $pkgdir/usr/bin $pkgdir/etc/systemd/system
  cp gpio-switch-poller $pkgdir/usr/bin/
  cp gpio-switch-poller.service $pkgdir/etc/systemd/system/
}
