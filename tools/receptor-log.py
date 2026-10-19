#!/usr/bin/env python3
# Receptor do registro no Mac da rede de casa (fork, 19/10).
#
# Na TV: Ajustes > Sobre e ajuda > Enviar registro para > "Mac da rede local",
# e depois "Enviar registro". O app faz POST /v1/registro com o JSON de sempre
# ({"versao", "plataforma", "quando", "texto"}) em NV_LOG_MAC (avisos.c, padrao
# http://192.168.1.33:8787). Cada envio vira um arquivo em <pasta>; o codigo de
# seis caracteres que a TV mostra e o mesmo do nome do arquivo.
#
# Uso: python3 -I tools/receptor-log.py [pasta] [porta]
#      (padrao: ~/Downloads/nuvio-logs e 8787)
import json, os, random, string, sys, time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

PASTA = os.path.expanduser(sys.argv[1] if len(sys.argv) > 1 else "~/Downloads/nuvio-logs")
PORTA = int(sys.argv[2]) if len(sys.argv) > 2 else 8787
os.makedirs(PASTA, exist_ok=True)


class H(BaseHTTPRequestHandler):
    def log_message(self, *a):
        pass

    def responder(self, st, obj):
        b = json.dumps(obj).encode()
        self.send_response(st)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(b)))
        self.end_headers()
        self.wfile.write(b)

    def do_POST(self):
        if not self.path.startswith("/v1/registro"):
            return self.responder(404, {"erro": "rota"})
        n = int(self.headers.get("Content-Length") or 0)
        try:
            d = json.loads(self.rfile.read(n).decode("utf-8", "replace"))
        except ValueError:
            return self.responder(400, {"erro": "json"})
        cod = "".join(random.choice(string.ascii_uppercase + string.digits) for _ in range(6))
        plat = "".join(c for c in str(d.get("plataforma", "tv")) if c.isalnum() or c == "-")[:20]
        nome = "%s-%s-%s.log" % (time.strftime("%Y%m%d-%H%M%S"), plat, cod)
        with open(os.path.join(PASTA, nome), "w", encoding="utf-8") as f:
            f.write("# versao %s · %s · %s · de %s\n" % (d.get("versao"), plat, d.get("quando"),
                                                      self.client_address[0]))
            f.write(str(d.get("texto", "")))
        print("recebido %s (%d KB) de %s" % (nome, n // 1024, self.client_address[0]), flush=True)
        self.responder(200, {"ok": True, "codigo": cod})


print("receptor em 0.0.0.0:%d -> %s" % (PORTA, PASTA), flush=True)
ThreadingHTTPServer(("0.0.0.0", PORTA), H).serve_forever()
