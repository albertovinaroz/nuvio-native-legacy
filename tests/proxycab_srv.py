# Servidor de teste do proxycab: so serve com "Referer: https://addon.invalid/",
# com Range. Uso: python3 -I tests/proxycab_srv.py <porta> <arquivo>
import sys, os
from http.server import BaseHTTPRequestHandler, HTTPServer
DADOS = open(sys.argv[2], "rb").read()
class H(BaseHTTPRequestHandler):
    def log_message(self, *a): pass
    def do_GET(self):
        if self.headers.get("Referer") != "https://addon.invalid/":
            self.send_response(403); self.send_header("Content-Length", "0"); self.end_headers(); return
        r = self.headers.get("Range")
        ini, fim = 0, len(DADOS) - 1
        if r and r.startswith("bytes="):
            a, _, b = r[6:].partition("-")
            ini = int(a); fim = int(b) if b else fim
            fim = min(fim, len(DADOS) - 1)
            self.send_response(206)
            self.send_header("Content-Range", "bytes %d-%d/%d" % (ini, fim, len(DADOS)))
        else:
            self.send_response(200)
        self.send_header("Content-Length", str(fim - ini + 1)); self.end_headers()
        self.wfile.write(DADOS[ini:fim + 1])
HTTPServer(("127.0.0.1", int(sys.argv[1])), H).serve_forever()
