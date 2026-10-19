// PROXY DE CABECALHOS PARA O VOD DA SAMSUNG (fork, 19/10).
//
// O player do Tizen (Tizen.Multimedia.Player) so aceita User-Agent e Cookie
// (tizen-tpk/Video.cs): um addon que exige Referer ou Origin
// (behaviorHints.proxyHeaders.request) tocava no iPhone e falhava na TV. Este
// proxy e um servidor HTTP em 127.0.0.1 (porta livre) dentro do app: o player
// pede http://127.0.0.1:P/v?s=N com o Range dele, e o proxy repete o pedido na
// fonte COM os cabecalhos do addon (rede_fluxo64) e devolve o corpo, com o
// Content-Range certo para o player poder buscar.
//
// So entra quando ha cabecalho que o player nao manda (proxycab_precisa): a
// fonte comum continua indo direto. Uma sessao por vez; conexoes da mesma
// sessao (sonda + leitura + busca) sao independentes. Logs [proxy-cab] sem URL.
#ifndef NV_PROXYCAB_H
#define NV_PROXYCAB_H
#include <stddef.h>

// 1 se `cabs` ("Nome: valor" por linha) traz algum cabecalho que o player da
// Samsung nao envia sozinho (tudo menos User-Agent e Cookie). Puro, testavel.
int proxycab_precisa(const char *cabs);
// Abre uma sessao para `fonte` com `cabs` e escreve em `saida` a URL local.
// 0 = o proxy nao subiu (tocar a fonte direto).
int proxycab_url(const char *fonte, const char *cabs, char *saida, size_t n);
// Encerra a sessao em curso (troca de fonte, saida do player).
void proxycab_parar(void);
// 1 quando `url` e uma URL deste proxy.
int proxycab_e_url(const char *url);

#endif
