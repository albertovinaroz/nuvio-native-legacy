// Reuse the USB test's offline motor and network stubs.
#define main usbExistingTests
#include "p2pusb.c"
#undef main

#include <dirent.h>
#include <pthread.h>

static unsigned usbOpens, callerOpens;
static pthread_t caller;
static int scanOpenEnabled;
static char mounted[800];
static int mountStat(const char *path, struct stat *st) {
  char resolved[1024];
  if (scanOpenEnabled && strstr(path, "/regression")) {
    __atomic_add_fetch(&usbOpens, 1, __ATOMIC_SEQ_CST);
    if (pthread_equal(pthread_self(), caller))
      __atomic_add_fetch(&callerOpens, 1, __ATOMIC_SEQ_CST);
  }
  int rc = stat(path, st);
  size_t n = strlen(mounted);
  if (!rc && realpath(path, resolved) && n && !strncmp(resolved, mounted, n) &&
      (!resolved[n] || resolved[n] == '/')) st->st_dev++;
  return rc;
}
static unsigned scanEntries;
static int scanLstat(const char *path, struct stat *st) {
  scanEntries++;
  return lstat(path, st);
}
// Simulate a mount boundary without mounting a disk or requiring privileges.
#define lstat(path, st) scanLstat(path, st)
#define stat(path, st) mountStat(path, st)
#include "../src/p2pmotor.c"
#undef stat
#undef lstat

int main(int argc, char **argv) {
  char base[780], cache[820], det[64];
  assert(argc == 3);
  snprintf(dados, sizeof dados, "%s/dados", argv[1]);
  snprintf(base, sizeof base, "%s/regression", argv[1]);
  assert(mkdir(base, 0755) == 0);
  { char resolved[780]; assert(realpath(base, resolved)); strcpy(base, resolved); }
  snprintf(mounted, sizeof mounted, "%s/volume", base);
  snprintf(cache, sizeof cache, "%s/p2p", mounted);
  p2pmotor_teste_injetar(&OPS, NULL, NULL, NULL, 0);
  p2pmotor_teste_usb_base(base);
  p2pmotor_definir_usb(1);
  if (!strcmp(argv[2], "disabled") || !strcmp(argv[2], "boot-worker")) {
    caller = pthread_self();
    scanOpenEnabled = 1;
    p2pEnabled = strcmp(argv[2], "disabled") != 0;
    p2pmotor_limpar_sobra();
    pthread_mutex_lock(&trava);
    while (limpando || sobraFila) pthread_cond_wait(&sinal, &trava);
    pthread_mutex_unlock(&trava);
    if (!p2pEnabled) {
      assert(!p2pmotor_usb_presente());
      assert(p2pmotor_resumo(det, sizeof det) == P2P_OK);
      assert(usbOpens == 0);
    } else {
      assert(usbOpens > 0);
      assert(callerOpens == 0);
    }
  } else if (!strcmp(argv[2], "rejected-volume") || !strcmp(argv[2], "volume-budget")) {
    char good[900];
    assert(mkdir(mounted, 0755) == 0);
    assert(mkdir(cache, 0755) == 0);
    for (unsigned i = 0; i < 400; i++) {
      char path[900];
      snprintf(path, sizeof path, "%s/item-%u", mounted, i);
      assert(mkdir(path, 0755) == 0);
    }
    usbExtra[0] = 0;
    if (!strcmp(argv[2], "rejected-volume")) {
      scanEntries = 0;
      assert(!usbEscanearDir(mounted, cache, sizeof cache, 0, 1));
      assert(scanEntries == 2); // p2p/ and ownership marker only; no descent.
    } else {
      // Force a rejected mount before a usable sibling, independent of readdir order.
      snprintf(good, sizeof good, "%s/good", base);
      assert(mkdir(good, 0755) == 0);
      LimTrab lim = { .max = 256, .ate = agora() + 5 };
      assert(!usbEscanearDirL(mounted, cache, sizeof cache, 0, 1, &lim));
      strcpy(mounted, good);
      assert(usbEscanearDirL(good, cache, sizeof cache, 0, 1, &lim));
      assert(existe(cache));
    }
  } else if (!strcmp(argv[2], "drive-swap")) {
    char root[600], parent[600], marker[900], sentinel[900], old[900];
    assert(mkdir(mounted, 0755) == 0);
    assert(chmod(base, 0555) == 0);
    assert(pastas(root, sizeof root, parent, sizeof parent));
    assert(!strcmp(root, cache));
    snprintf(old, sizeof old, "%s/old-volume", argv[1]);
    assert(chmod(base, 0755) == 0);
    assert(rename(mounted, old) == 0);
    assert(mkdir(mounted, 0755) == 0);
    assert(mkdir(cache, 0755) == 0);
    snprintf(sentinel, sizeof sentinel, "%s/keep", cache);
    FILE *fp = fopen(sentinel, "w"); assert(fp);
    assert(fputs("user data\n", fp) >= 0); assert(fclose(fp) == 0);
    assert(chmod(base, 0555) == 0);
    // Even a marked replacement must fail cached mount-identity validation.
    usbMarcarDono(cache);
    assert(!usbCacheValido());
    snprintf(marker, sizeof marker, "%s/.nuvio", cache);
    assert(unlink(marker) == 0);
    // The same writable pathname now belongs to another, unmarked drive.
    assert(pastas(root, sizeof root, parent, sizeof parent));
    assert(strcmp(root, cache));
    assert(subir(&OPS, atomic_load(&geracao)) == P2P_ERR_MOTOR);
    assert(internalAttempt);
    assert(existe(sentinel));
    snprintf(marker, sizeof marker, "%s/.nuvio", cache);
    assert(!existe(marker));
    // A cleanup path saved before the swap must also preserve user files.
    percorrer(cache, 1, 0);
    assert(existe(sentinel));
  } else if (!strcmp(argv[2], "bounded-scan")) {
    // A rejected volume's contents must not monopolize synchronous discovery.
    assert(mkdir(mounted, 0755) == 0);
    assert(mkdir(cache, 0755) == 0); // Unowned p2p directory rejects allocation.
    for (unsigned i = 0; i < 400; i++) {
      char path[900];
      snprintf(path, sizeof path, "%s/item-%u", mounted, i);
      FILE *fp = fopen(path, "w");
      assert(fp);
      assert(fclose(fp) == 0);
    }
    usbExtra[0] = 0;
    scanEntries = 0;
    assert(!usbEscanearDir(base, cache, sizeof cache, 0, 1));
    assert(scanEntries <= 256);
  } else if (!strcmp(argv[2], "container")) {
    // A writable container is not a volume, even when empty.
    usbExtra[0] = 0;   // Exercise production mount checks, not fixture injection.
    assert(!usbEscanearDir(base, cache, sizeof cache, 0, 0));
    assert(mkdir(mounted, 0755) == 0);
    assert(usbEscanearDir(base, cache, sizeof cache, 0, 1));
    assert(existe(cache));
    snprintf(cache, sizeof cache, "%s/p2p", base);
    assert(!existe(cache));
  } else {
    // Failed allocation must expire for both UI probes and later allocation.
    // Make the container read-only so this test isolates negative caching.
    assert(chmod(base, 0555) == 0);
    assert(p2pmotor_resumo(det, sizeof det) == P2P_OK);
    assert(chmod(base, 0755) == 0);
    assert(mkdir(mounted, 0755) == 0);
    assert(chmod(base, 0555) == 0);
    usleep(2100000);
    if (!strcmp(argv[2], "retry-ui")) assert(p2pmotor_usb_presente());
    assert(p2pmotor_resumo(det, sizeof det) == P2P_OK);
    assert(existe(cache));
  }
  puts("regression ok");
  return 0;
}
