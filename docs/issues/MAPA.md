# Mapa vivo das issues

Base: `5544c3e4c` (v2.0.5 já publicada; pendências pós-release em TRIAGEM-2.0.5.md). Atualizado em 2026-10-16T01:55:20-03:00. 464 issues (abertas e fechadas) de iqui27/nuvio-native-legacy.

## Como atualizar

1. Chegou issue nova, ou um conserto entrou numa branch/tag: edite **uma** entrada em `docs/issues/mapa.json` (procure por `"numero": N`) e rode `python3 docs/issues/mapa.py`, que valida e reescreve este arquivo. Sem o script, edite a linha equivalente aqui.
2. Campos: `numero`, `titulo`, `plataforma` (LG, Samsung .tpk, Samsung .wgt, Android, all, `?`), `tipo` (bug, feature, question, meta), `status`, `release` (2.0.2 ou tag antiga, 2.0.3/2.0.4/2.0.5 = versões publicadas; `-` = nenhum conserto publicado; planejamento usa `alvo`), `conserto` (hashes curtos com a ref entre parênteses), `ultima_resposta` (`nos` = o último comentário é nosso, `data`, `ultimo_comentario_por`, `ultima_nossa`), `proximo_passo`, `notas`, `alvo` (só issues ABERTAS, obrigatório: `2.0.3`, `2.0.4`, `2.0.5`, `2.0.6`, `2.1`, `2.2`, `futuro`, `nao vamos fazer`, ou `ja-lancada` quando já saiu e só falta fechar). `python3 docs/issues/mapa.py --check` valida e confere se o MAPA.md está em dia, sem escrever; o gerador sai com erro se uma issue aberta não tiver alvo. O roadmap e as decisões ficam em `roadmap` e `decisoes` no json; decisão tem `id` (OBRIGATÓRIO e estável, `dec-<assunto>` em minúsculas/dígitos/hifens: é o que liga a resposta do dono no painel à decisão; nunca mude nem reutilize, e o id que sai de vez vai para `decisoes_ids_retirados`), `estado` (`pendente` ou `decidida`) e, decidida, `decisao`, `data` e `quem`. Campo opcional `release` diz a versão afetada quando ela não dá para deduzir do alvo das issues ligadas. `aplicada_em` (AAAA-MM-DD) é preenchido pela coordenação depois de aplicar a resposta; a decisão sai da fila de pendentes do painel.
3. Vocabulário de `status`: `aberta`, `respondida`, `consertada-nao-lancada`, `integrada`, `em-andamento`, `lancada`, `por-desenho`, `fora-do-escopo`, `precisa-log`, `duplicada`. Extra: `fechada-sem-resposta` (issue fechada sem nenhum comentário).
4. Regra de honestidade: só vale `consertada-nao-lancada`/`lancada` com commit na ref. "Lançado" = conserto contido em tag estável ou explicitado em notas de release. `integrada` = conserto na integração ainda sem release; `em-andamento` = conserto em branch com atividade nos últimos 14 dias. Sem commit, escreva "suspeita" ou "sem commit" em `conserto`/`notas`. Comentário que anuncia uma versão, sozinho, não prova conserto; ausência de evidência mantém `aberta`.
5. Atenção: mensagens de commit com `(#203)` / `(#204)` falam da VERSÃO 2.0.3 / 2.0.4 (o plano que hoje é a 2.0.5; a linha 2.0.3.1 é que saiu como 2.0.4), não das issues #203/#204. Associações de versão foram excluídas; commits que tratam dos issues reais foram conferidos separadamente.
6. Auditoria: TODA validação (teste FAIL->PASS, revisão de Codex/OpenCode/ultrareview, teste na TV, leitura de log) entra em `validacoes` no json, com `data` (AAAA-MM-DD [HH:MM]), `quem` (pessoa ou agente e modelo), `o_que`, `issues` (lista, pode ser vazia), `ref` (commit, branch ou build testado), `resultado` (`passou`, `falhou`, `inconclusivo`, `nao-reproduziu`, `pendente`) e `evidencia` (arquivo, log, PR ou id do registro). Uma validação que falhou fica registrada mesmo depois do conserto: a seguinte aponta o conserto. O gerador recusa entrada sem esses campos.
7. Próximo passo "postar correção": há rascunhos em `/Volumes/ExternalSSD/tmp/203-respostas-correcao.md` (#302 #350 #286 #312 #368 #360); o dono decide. Correção conhecida: "Dolby Vision in MKV" é DESLIGADO por padrão (a resposta do #312 disse ligado).

## Roadmap

Alvo de cada issue aberta e os recursos por versão. Alvos são decisão de planejamento, não promessa pública; os pontos marcados SUSPEITA não têm confirmação.

### 2.0.3 - lançada 09/10 (v2.0.3, 8ed4517c)

Publicada na tag v2.0.3 (commit 8ed4517c, 09/10/2026). Lista completa em docs/releases/2.0.3/NOTAS.md. Em 09/10 os consertos com todos os commits contidos na tag passaram a status lancada; o que ficou como estava tem commits fora da tag ou conserto só suspeito (ver notas de cada issue). Inclui #361 (URL de cartaz longa).


### 2.0.4 - lancada 10/10 (v2.0.4, db6ae4ee)

Publicada na tag v2.0.4 (commit db6ae4ee, 10/10/2026). Correções confirmadas na tag estão marcadas como lançadas.

- **Temporadas: abas da lista publicada (#372 parte 1)** (#372): INTEGRADO em integracao/2.0.3.1 (merge 21077ab5; 038a7dc7, c2112b61, a120b67c, 167310e6, 8eb5ce90), sem tag v*. CAUSA PROVADA: regressão 9677065b (v2.0.1, catálogo do Nuvio primeiro para a lista de episódios; o TMDB junta anime longo, Bleach {1:366,2:50} contra S1-16 no Cinemeta) e defeito antigo das abas de temporada vindas do corpo do Nuvio mesmo quando a lista do add-on vence (descoberta.c ~7069-7087, detail.c ~742-765). Log BZB857 (2.0.3, webOS). A parte 2 (numeração) foi para a 2.0.5. #328 saiu daqui: vai para a 2.0.5. Origem: logs dos autores, 09/10; decisão do dono 09/10. Branches: `agente/2031-temporadas (integrada)`.
- **Queda ao pesquisar na TV ao vivo (#344), primeira rodada** (#344): INTEGRADO em integracao/2.0.3.1 (merge c9af64b1; teste 9d78c75a, conserto 8dd273f3), sem tag v*. CAUSA PROVADA por tombstone simbolizado (log AX1R49): leitura fora do limite em buscaFazer (src/guia.c ~3965, ps[24] com epg_faixa/xtepg_faixa devolvendo o total), desde a v1.7.0, todas as plataformas com o guia. Origem: log do autor, 09/10. Branches: `agente/2031-guiabusca (integrada)`.
- **Ordem da Home ao trocar de perfil (#392, #294)** (#392, #294): INTEGRADO em integracao/2.0.3.1 (merge bdd49eb6). CAUSA PROVADA pelo log VV8JG2 (.tpk Tizen 6.0, v2.0.3): o build da Home registra os catálogos de add-ons do perfil anterior no arquivo de ordem do perfil novo e expulsa fileiras. Teste bb583b60, conserto 9023ebdb. Origem: log do autor, 09/10. Branches: `agente/2031-ordemperfil`.
- **Queda do webOS 3 na sonda do adaptador DTS**: INTEGRADO em integracao/2.0.3.1 (merge 09ce19a0): a sonda do adaptador DTS responde uma vez por processo (00b4a937) e a libplayerAPIs fica na memória (RTLD_NODELETE, 65491e7b); a explicação do mecanismo é SUSPEITA, não certeza (1d7264c1). Sem issue no mapa. Origem: investigação 09/10. Branches: `agente/2031-webos3`.
- **Fontes com FHD sem numero caem em Outras (#402)** (#402): INTEGRADO em integracao/2.0.3.1 (merge ca8f29b4; teste tests/fhd402.sh, revisado por Codex e OpenCode). CAUSA lida no codigo: stream_parse.c:281-283 e badges.c:40-42 nao reconhecem FHD/FULLHD sem numero, entao altura=0 e o grupo vira Outras (streams.c:2105). Conserto de 2 linhas mais teste; candidato a entrar na 2.0.4 se o dono quiser. Nao e a ordenacao nem uma configuracao. Origem: agente 09/10.
- **Legendas "Da conta" e modelo de URL do poster (#378, #390)**: INTEGRADO em integracao/2.0.3.1 (merges 28aa5c4b e beab28ec), cada um com teste que falhava antes do conserto.
- **Dolby Vision perfil 5 na LG: decisão pendente (HDR10 ou MP4 primeiro)**: Pendente: duas branches de alternativas; a decisão espera o teste do dono na C9. Entra no hotfix só se for escolhida. Origem: dono, 09/10; decisão pendente. Branches: `agente/2031-dvp5-hdr10`, `agente/2031-dvp5-mp4`.

### 2.0.5 - lançada 12/10 (v2.0.5, b7c2c1f9)

Publicada em 12/10/2026 às 21:17 UTC. Evidência: .triagem/releases.json e tag v2.0.5. Pendências por issue foram reavaliadas em TRIAGEM-2.0.5.md; itens abaixo são histórico do plano, não promessas pendentes.

- **Próximo episódio automático após 5 s** (#256, #331, #326, #329): Estende a contagem estilo Netflix do #331 (agente/203-331) e o pedido #256; é uma opção no mesmo card, então vai junto. Sem branch próprio para os 5 s: SUSPEITA de que baste um valor padrão.
- **Estatísticas do player e tamanho do buffer** (#338): Código pronto em agente/203-338; a resposta já prometeu "next build".
- **Biblioteca: indicadores de visto, esconder assistidos, vitrine de coleção, detalhes da fonte** (#352, #366, #362, #355): Pequenos, cada um em seu branch agente/204-*; não dependem do redesign.
- **Catálogos da conta, sync de add-ons, fonte recusada, P2P por formato** (#358, #365, #360, #364, #349): Branches agente/204-conta-catalogos, 204-sync-addons, 204-fonte-recusada prontos ou quase.
- **Samsung: DTS/TrueHD escondido (fase 1)** (#313): Plano e spike em agente/204-samsung-dts (docs/plans); fase 1 já planejada para a 2.0.5.
- **Android: fileiras ilimitadas na Home, armazenamento rotativo do P2P** (#334): agente/204-fileiras-android e agente/203-334-janela (janela de streaming do P2P) existem e ficaram fora da 2.0.3 por risco.
- **Triagem de bugs sem conserto e dois pedidos pequenos (#306, #337)** (#288, #315, #316, #344, #345, #346, #353, #357, #367, #372, #373, #378, #379, #306, #337, #385, #308): Todos aguardam log do autor; entram na 2.0.5 se o log aparecer a tempo, senão escorregam.
- **Binge group: lembrar também a fonte que o automático tocou (#310)** (#310): Pedido (a) do #310. A revisão de código mostrou que a escolha manual de outro episódio já aplica o binge group; a lacuna era só a fonte escolhida à mão ser lembrada. O dono aprovou em 09/10 ("sim vamos adicionar na 2.0.4"; a 2.0.4 planejada foi renumerada para 2.0.5 em 09/10) lembrar também o binge group da fonte que o automático tocou. Código em agente/204-binge (29d2c248, 02753401, 6345e9ff), não integrado. Origem: dono 09/10. Branches: `agente/204-binge`.
- **Trailer vazando para a sessão do episódio no .tpk (#385)** (#385): SUSPEITA, sem conserto. No log do #385 (.tpk, 2.0.2), "tpk evento 1 (129036)" vem sempre depois de "[trailer] ... no fundo com som" e aparece até sem episódio aberto; no log 1 o evento de duração do trailer chegou depois de "abrir: url ao pipeline" e o posplay o leu como duração do episódio ("duracao suspeita 129s"). Conferir se os eventos do player do trailer (duração, fim, erro) são descartados ao abrir o episódio. Separado da recusa de conexão do TorBox, que foi bloqueio do nó/IP (conserto da pausa na 2.0.3). Origem: logs do #385, 09/10.
- **Numeração de temporadas Cinemeta + camada de tradução (#372 parte 2)** (#372, #328): Opção (b) escolhida pelo dono: lista de episódios na numeração Cinemeta/TVDB mais uma camada de tradução por número absoluto do episódio para o progresso/scrobble do Trakt, a conta Nuvio, o Simkl, os ids de "a seguir"/Continuar assistindo e as marcas de visto (vistonao/vistoep). A parte 1 (abas de temporada) sai na 2.0.4. #328 acompanha: mesma raiz suspeita. Origem: dono 09/10.
- **CI com ASan/TSan nos testes de host (+ ASan do app inteiro por candidata a release, helpers de buffer limitado)**: Alternativa sem Rust do estudo: SANITIZE=1 e SANITIZE=thread nos 152 + 29 testes que já aceitam, falhando o PR; roteiro fixo com ASan no app inteiro por release candidate; helpers de buffer limitado (nv_cpy, nv_slice). Cobre os bugs de memória do ranking a custo de horas, sem tocar em alvo de TV. Referência: docs/plans/rust-piloto.md ("alternativas sem Rust", seção 5). Origem: dono 09/10. Branches: `docs/plans/rust-piloto.md`.
- **Biblioteca: aba Salvos seguir o destino Simkl (sugestao)** (#393): #393; só se o autor confirmar que o que falta é isso (Listas › Simkl e "Onde o + salva" já existem). Sugestão do agente, não aprovado pelo dono. Origem: agente 09/10.
- **Binario do LG sem simbolos de depuracao: ipk menor (#401)** (#401): MEDIDO no ipk 2.0.4: 58,3 MB, binario 55,3 MB com debug_info/.symtab (30,8 MB de secoes de depuracao, 10,5 MB comprimidas), arte 39 MB ja comprimida. Strip no link do tools/arm.sh deve baixar o ipk para ~48 MB (estimativa, nao medida) e guardar o binario com simbolos como anexo da release para simbolizar queda. Nao prova que seja a causa dos 15 min do #401 (SUSPEITA: download da TV ou instalacao de 1190 arquivos); pedir ao autor o tempo por fase. Fontes opcionais (Roboto, Montserrat, Inter, ate ~4 MB descomprimidos) poderiam baixar sob demanda como a Naskh (assrender.c ~870), sem promessa. Origem: agente 09/10.
- **Botao Proximo episodio na fileira de controles do player (#403)** (#403): Hoje so o cartao na janela de creditos/fim (dispensa gruda) e a lista pelo botao Episodios. Sugestao: acao de proximo episodio na fileira (ou atalho) para serie, reaproveitando player_proximo_episodio e o pedido de app.c:4424. Vai junto com o proximo episodio automatico. Sugestao do agente, nao aprovado pelo dono. Origem: agente 09/10.
- **Dados do app fora da pasta do app (LG) e limpeza de sobras** (#401): Medido na C9 (09/10): .nuvio dentro da pasta do app com 359 MB / 3980 arquivos (cache de imagens 111 MB, cache.suspeito-1853 195 MB nunca apagado, trailers 28 MB, fontes de legenda ASS repetidas por faixa ~7,5 MB cada). SUSPEITA de ser o que deixa a atualização pelo Homebrew lenta (#401): instalação nova é rápida, atualização por cima leva 15 min. Fazer: (1) cache de imagens, trailers e fontes da legenda fora da pasta do app; (2) apagar cache.suspeito-* antigos; (3) não guardar cópia das fontes por faixa. Aprovado pelo dono 09/10.
- **Novos de 09/10: ordem do add-on, busca rapida, notas em bloco, menu do topo (#400, #405, #406, #407)**: Triagem 09/10 sem conserto: #400 pedido (opcao de manter a ordem do add-on), #405 bug com log ABB125, #406 pedido, #407 bug sem log, todos do Samsung menos o #400.
- **Perfil/Home (#392)** (#392): Correção integrada; hash 811ba74a. Origem: origin/master..HEAD.
- **Fallback TMDB (#416, #419)** (#416, #419): Correção integrada; hash 8b5ed6c0. Origem: origin/master..HEAD.
- **Cartão de trailer (#417)** (#417): Correção integrada; hash f219bc9d. Origem: origin/master..HEAD.
- **Screensaver durante trailer (#422)** (#422): Correção integrada; hash a408e51a. Origem: origin/master..HEAD.
- **Legendas embutidas de texto (#421)** (#421): Correção integrada; hash c8e8d983. Origem: origin/master..HEAD.
- **DisplayRoi e aspecto (#426, #341)** (#426, #341): Correção integrada; hash 3ba2987a. Origem: origin/master..HEAD.
- **Seleção de legendas hbs (#429)** (#429): Correção integrada; hash 2d42c8e0. Origem: origin/master..HEAD.

### 2.0.6 - proposta de próximo patch; sem data

Bugs com impacto no usuário ainda sem conserto publicado, por plataforma. Proposta de triagem; priorização e capacidade dependem do dono.

- **Bugs pendentes — Android** (#345, #346, #357, #399, #424): Reproduzir na 2.0.5, corrigir e validar na plataforma indicada; não é promessa de release. Origem: triagem local 12/10/2026.
- **Bugs pendentes — LG** (#288, #353, #367, #382, #401, #415, #423, #428, #468): Reproduzir na 2.0.5, corrigir e validar na plataforma indicada; não é promessa de release. Origem: triagem local 12/10/2026.
- **Bugs pendentes — Samsung .tpk** (#315, #316, #365, #379, #386, #388, #391, #394, #398, #404, #405, #407, #413, #414, #418, #451): Reproduzir na 2.0.5, corrigir e validar na plataforma indicada; não é promessa de release. Origem: triagem local 12/10/2026.

### 2.1 - aprovada pelo dono em 06/10/2026 (e 07/10 para Ajustes); sem data

Os seis recursos aprovados pelo dono, mais perfis com o pacote de UX dos Ajustes. Os itens marcados "suspeita" foram colocados aqui pelo agente anterior e NÃO estão aprovados.

- **Watch Together (Assistir Juntos)**: Aprovado pelo dono para a 2.1. Sala compartilhada entre aparelhos; o branch já carrega a marca de perfil infantil enviada para a sala. Origem: dono 06/10. Branches: `feat/watch-together`.
- **Diário + sono (Diário + sono)**: Aprovado pelo dono para a 2.1. Origem: dono 06/10. Branches: `feat/sp-diario`.
- **Trava (Lock)**: Aprovado pelo dono para a 2.1. Origem: dono 06/10. Branches: `feat/sp-trava`.
- **Caça / Conquistas (Hunt/Achievements)**: Aprovado pelo dono para a 2.1. Origem: dono 06/10. Branches: `feat/sp-conquistas`.
- **Redesign da Biblioteca (Biblioteca)**: Branch agente/202-biblioteca (procedência por título, aba Listas); o plano de dados está em docs/plans/biblioteca-ilimitada.md (branch agente/biblioteca-ilimitada-plano) e diz "não na 2.0.3". Mexe em memória e navegação; não cabe num ponto de versão. Origem: dono 06/10. Branches: `agente/202-biblioteca`, `agente/biblioteca-ilimitada-plano (docs/plans/biblioteca-ilimitada.md)`.
- **Coleções + fileiras**: Aprovado pelo dono para a 2.1. Os pequenos ajustes de vitrine (#362) continuam na 2.0.5 em agente/204-colecao-vitrine e não dependem disto. Origem: dono 06/10. Branches: `feat/colecoes-fileiras`, `agente/201-colecoes`.
- **Perfis + tour, "Começar", "Resolver um problema" e busca com i18n nos Ajustes**: Decisão de UX dos Ajustes de 07/10: esses quatro itens saem na 2.1 junto com perfis. Branch de cada um: não identificado (suspeita: parte do trabalho está em feat/ajustes-ux e agente/204-perfil, não confirmado). Origem: dono 07/10.
- **Limites maiores de Trakt/Biblioteca (sem teto)**: Parte do plano biblioteca-ilimitada, que o dono aprovou só como redesign; tirar os tetos do Trakt como item próprio não foi citado por ele. A 2.0.3 só pagina a watchlist do Trakt até 400 itens e mantém os tetos de hoje (96ccdab1). Tirar o teto exige o índice leve + janela de CatItem do plano biblioteca-ilimitada, que é parte do redesign. Origem: suspeita (sugestão do agente, não aprovado).
- **Remoção de barras pretas embutidas no quadro (blackbar/crop)** (#341): Recurso novo, sem issue nem branch (relacionados: #341 aspecto/zoom e #241 barras do trailer, esse já tratado). Precisa detectar a barra no quadro em cada pipeline (LG, .tpk, Android). SUSPEITA de 2.1; pode virar 2.0.5 se for só um recorte manual por título. Origem: suspeita (sugestão do agente, não aprovado).
- **Dolby Vision em MKV na Samsung**: A 2.0.3 só traz DV em MKV na LG (opcional, desligado) e a Samsung nunca escolhe DV. Na Samsung falta prova de que o decodificador aceita; precisa de spike por modelo antes de prometer. SUSPEITA de 2.1. Origem: suspeita (sugestão do agente, não aprovado).
- **Menus em árabe e auto sync de legenda por linha** (#250, #333, #374): SUSPEITA do agente, fora do roadmap do dono: o árabe nos menus foi prometido na resposta do #325 e o auto sync tem pesquisa em legenda-sync-lg-samsung-pesquisa.md, mas nenhum dos dois consta da decisão de 06/10. Pode sair da 2.1 para futuro se o dono preferir. Origem: suspeita (sugestão do agente, não aprovado).
- **Plano de refatoração geral**: docs/plans/refatoracao-geral.md (branch agente/refatoracao-plano, 0dc06334): 23 mudanças pequenas, cada uma com porteiro de teste. Fazer depois da 2.0.3 sair e fora da 2.0.5, para não misturar refatoração com os recursos acima. Itens 01-04 (apagar morto, deduplicar Biblioteca) vão melhor junto do redesign da Biblioteca. Ordem com o Rust (dono 09/10): o porte Rust de fonteparalela+streams vem depois do piloto e deve ser coordenado com os itens do plano que tocam streams.c. Origem: suspeita (sugestão do agente, não aprovado). Branches: `agente/refatoracao-plano`.
- **Auto-play: reusar último link com validade e regex separada por tipo (filmes / séries / anime)** (#310): Pedidos (b) e (c) do #310. (b) não há cache de link hoje (fontepref guarda só a identidade, 180 dias; fontecache é prefetch de 30 s): guardar URL de debrid pede arquivo, validade nos Ajustes e queda para a busca. (c) hoje há uma regex e um modo só (fonteregra.c:15, ajustes.c:1301, uma chave na conta): por tipo pede três regex, três modos, blob novo e uma definição de anime. Médio e grande, por isso 2.1 e não 2.0.5. Sem aprovação do dono ainda. Origem: suspeita (sugestão do agente, não aprovado).
- **Piloto Rust 'mkvcore' (no_std, atrás de -DNV_RUST_MKVCORE, com fallback C) — piloto de toolchain**: Experimento de toolchain nos 5 alvos com flag por alvo e fallback C (reversível trocando uma variável de build); flag desligada por padrão no primeiro candidato. Estimativa de 3 a 4 semanas, SUSPEITA. Referência: docs/plans/rust-piloto.md (seção 4). Origem: dono 09/10. Branches: `docs/plans/rust-piloto.md`.
- **Portar fonteparalela+streams e o publicador do catálogo (se o piloto passar)**: Só depois do piloto mkvcore passar os guards nos 5 alvos; é onde está o ganho real de Rust segundo o estudo, mas o custo é de meses (SUSPEITA). Coordenar com o plano de refatoração geral, que também mexe em streams.c. Referência: docs/plans/rust-piloto.md (seção 6). Origem: dono 09/10. Branches: `docs/plans/rust-piloto.md`.
- **Atraso do áudio no player (sugestao)** (#397): Pedido no #397; não existe hoje (só o atraso de legenda). Sugestão do agente, não aprovado pelo dono: começar onde o backend deixa (Android/Media3); LG e Samsung podem não ter API. Origem: agente 09/10.

### 2.2 - aprovada pelo dono em 06/10/2026; sem data

Servidores locais, guia de TV e música.

- **Servidores locais (Jellyfin / Plex / Emby)**: Aprovado pelo dono para a 2.2; ele tem um servidor real para testar. Origem: dono 06/10. Branches: `f11-jellyfin`, `feat/emby-plex`.
- **Guia de TV (guia pro)**: Aprovado pelo dono para a 2.2. Existem várias linhas de trabalho: guia-tv, guia-pro, guia-pro-2, lembrete e a UI w18; qual é a mais recente fica a conferir. Origem: dono 06/10. Branches: `feat/guia-tv`, `feat/guia-pro`, `feat/guia-pro-2`, `feat/guia-lembrete`, `ui/w18-guia-tv`.
- **Música (álbum da trilha sonora)**: Aprovado pelo dono para a 2.2: álbum da trilha, só link do Spotify, prévia para testar na TV e no celular, crowdsourcing no Android permitido, Apple Insight via uts-api timed-metadata. Branch: nenhum encontrado. Origem: dono 06/10.

### futuro - sem data nem dono

Pedidos que fazem sentido mas não têm plano.

- **Porte VIDAA, layout RTL, anime-skip.com, busca por microfone, pontos do carrossel estilo Apple TV, serviços na barra lateral** (#135, #260, #343, #347, #348, #304, #342): Cada um é grande ou depende de terceiros; nenhum tem branch.
- **Passo de quadro no player (sugestao)** (#397): Pedido no #397 ("frame skipping", ambíguo); não existe, só saltos de 10 a 120 s. Sugestão do agente, não aprovado pelo dono; depende de a TV aceitar avanço por quadro. Origem: agente 09/10.

### nao vamos fazer - fora do escopo do produto

PC, iOS, e itens que não são trabalho de código.

- **App para PC (#354), .ipa para iOS (#307), relatório automático de logs (#324), aviso de desempenho em 4K (#292)** (#354, #307, #324, #292): Plataformas que o projeto não atende ou issues que não pedem mudança.

Plano de refatoração: `docs/plans/refatoracao-geral.md` (branch `agente/refatoracao-plano`, ainda não integrado).

### Issues abertas por alvo

| Alvo | Qtd | Issues |
|---|---|---|
| 2.0.3 | 0 |  |
| 2.0.4 | 0 |  |
| 2.0.5 | 2 | #495, #512 |
| 2.0.6 | 33 | #407, #412, #413, #414, #415, #418, #423, #424, #451, #459, #461, #462, #468, #469, #471, #479, #483, #492, #494, #496, #497, #500, #501, #502, #503, #504, #505, #506, #507, #508, #528, #530, #545 |
| 2.1 | 5 | #420, #434, #457, #458, #539 |
| 2.2 | 0 |  |
| futuro | 11 | #437, #438, #450, #466, #467, #509, #510, #514, #521, #524, #542 |
| nao vamos fazer | 0 |  |
| ja-lancada | 14 | #410, #416, #417, #419, #421, #422, #425, #426, #429, #430, #433, #439, #440, #486 |

## Fechar com a 2.0.3

Issues ABERTAS no GitHub cujo conserto saiu na v2.0.3 (commits contidos na tag). Cada uma tem uma resposta curta em inglês para colar; "autor confirmou" diz se quem abriu já testou. O dono decide quando fechar.

| # | Título | Autor confirmou? | Resposta curta (EN) |
|---|---|---|---|

### Não fechar ainda

| # | Título | Motivo |
|---|---|---|

## Decisões para o dono

### Pendentes


### Decididas

- 2026-10-11 (dono (painel)), `dec-256-proximo-episodio` [respondida, a aplicar]: Próximo episódio automático após 5 s: entra na 2.0.5 junto da contagem do #331, ligado por padrão ou desligado? **Sim: 2.0.5, desligado por padrao, valor configuravel (junto da contagem do #331).** **Resposta do dono: SIM** (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-360-botao-sincronizar` [respondida, a aplicar]: #360 (botão Sincronizar addons) fica em 2.0.5 mesmo com a mescla do sync já na 2.0.3? **Sim: mescla ja na 2.0.3; botao Sincronizar addons (agente/204-sync-addons) na 2.0.5; avisado na issue (11/10).** **Resposta do dono: SIM** (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-334-p2p-limite` [respondida, a aplicar]: #334 sai como 2.0.3 só com o limite de P2P e a limpeza, e a janela de streaming (armazenamento rotativo, agente/203-334-janela) fica para a 2.0.5? **Sim: limite e limpeza ja na 2.0.3; janela de streaming (agente/203-334-janela) na 2.0.5; avisado na issue (11/10).** **Resposta do dono: SIM** (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-326-promessas-next-build` [respondida, a aplicar]: #326 e #338 foram prometidos para "próxima atualização/next build" e o código só está em branches de 2.0.5. Responder já avisando 2.0.5? **Sim: #326 e #338 corrigidos na issue para 2.0.5 (11/10).** **Resposta do dono: SIM** (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-bugs-sem-log` [respondida, a aplicar]: Bugs sem log (#288, #315, #316, #344-346, #353, #357, #367, #372, #373, #378, #379) ficam todos em 2.0.5? **Sim: ficam na 2.0.5 (seguir a recomendacao).** **Resposta do dono: SIM** (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-354-nao-vamos-fazer` [respondida, a aplicar]: "nao vamos fazer": #354 (PC), #307 (.ipa iOS), #324 (relatório de logs), #292 (aviso 4K Android). Confirma? **Ainda nao vamos fazer, mas pode ser que sim no futuro (nao fechar como recusado definitivo).** **Resposta do dono: SIM** Nota: ainda nao vamos fazer, mas pode ser que sim. (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-304-pedidos-interface` [respondida, a aplicar]: Pedidos de interface (#304 serviços na barra lateral, #347 microfone, #348 pontos do carrossel/botão Reproduzir, #342 botão Pular cena) ficam em "futuro" ou "nao vamos fazer"? **Futuro (sem release marcada).** **Resposta do dono: SIM** Nota: futuro. (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-135-vidaa-rtl` [respondida, a aplicar]: #135 (porte VIDAA) e #260 (RTL) em "futuro" ou "nao vamos fazer"? **Futuro (sem release marcada).** **Resposta do dono: SIM** Nota: futuro. (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-144-aguardando-fechamento` [respondida, a aplicar]: Issues abertas só aguardando fechamento (#144, #252, #287, #290, #293, #296, #298, #300, #303): fechar? **Sim: fechadas 144 252 290 296 298 300 303 com comentario (11/10); 293 fica aberta ate a 2.0.5 (codec vem com Stats for nerds).** **Resposta do dono: SIM** (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-341-blackbar-crop` [respondida, a aplicar]: Blackbar/crop (remover barras pretas embutidas) é 2.1, 2.2 ou 2.0.5? Não consta do roadmap aprovado. **Vai para a 2.1.** **Resposta do dono: SIM** Nota: 2.1. (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-dv-mkv-samsung` [respondida, a aplicar]: Dolby Vision em MKV na Samsung: manter na 2.1, passar para 2.2 ou futuro? Não consta do roadmap aprovado. **Vai para a 2.1.** **Resposta do dono: SIM** Nota: 2.1. (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-352-limites-trakt` [respondida, a aplicar]: Limites maiores do Trakt/Biblioteca: entram dentro do redesign aprovado da 2.1 ou ficam fora? **Sim: limites maiores do Trakt/Biblioteca ficam dentro do redesign da 2.1.** **Resposta do dono: SIM** Nota: ficam dentro da 2.1. (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-refatoracao-geral` [respondida, a aplicar]: Plano de refatoração geral (docs/plans/refatoracao-geral.md): quando? Não consta do roadmap aprovado. **Sim, mas so o que melhorar de verdade; usar Rust onde ele resolver problema nosso.** **Resposta do dono: SIM** Nota: apenas o que for melhorar e tb usar rust no que for resolver nossos problemas tb. (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-250-arabe-autosync` [respondida, a aplicar]: Menus em árabe (#250, #333) e auto sync de legenda (#374) seguem na 2.1? Não constam do roadmap aprovado; o árabe foi prometido na resposta do #325. **Sim: menus em arabe (#250/#333) e auto sync de legenda (#374) na 2.1.** **Resposta do dono: SIM** Nota: sim. 2.1. (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-fechar-203-sem-confirmacao` [respondida, a aplicar]: Fechar com a 2.0.3 (bloco acima): fechar já as issues sem confirmação do autor ou esperar o retorno? **Sim: fechar sem esperar confirmacao (todas ja fechadas em 10/10).** **Resposta do dono: SIM** (2026-10-11).
- 2026-10-11 (dono (painel)), `dec-perfil-sem-trakt` [respondida, a aplicar]: Perfil sem Trakt (usuários com Simkl veem "Trakt desconectado"): entra em qual versão? **Sim: 2.0.5 — perfil com dados do Simkl e mensagem certa quando so o Simkl esta ligado.** **Resposta do dono: SIM** Nota: 2.0.5. (2026-10-11).
- 2026-10-09 (dono (Henrique)), `dec-400-ordem-fontes-addon` [decidida]: Ordem das fontes do add-on (#400, AIOStreams): entra na 2.0.4 ou fica para a 2.0.5? **Entra na 2.0.4. Implementação delegada ao Codex em agente/204-ordem400.**
- 2026-10-09 (dono (Henrique)), `dec-408-atualizar-pr` [decidida]: Atualizar o PR #408 (review only) com KM7, consertos do Copilot/ultrareview, OpenSubtitles e mapa? **Sim. Push feito (063f381b).**
- 2026-10-09 (dono (Henrique)), `dec-204-card-novidades` [decidida]: Card de novidades da 2.0.4 (hotfix): fazer ou não? **Fazer. Delegado ao Codex em agente/204-novidades (até 3 cenas, só o que NOTAS.md prova).**
- 2026-10-09 (dono (Henrique)), `dec-painel-sempre-atualizado` [decidida]: Painel de issues: sempre atualizado? **Sim, faz parte do fluxo: hook post-commit/post-merge republica o painel (tools/painel-issues).**
- 2026-10-09 (dono (Henrique)), `dec-km7-se-hdr` [decidida]: Erros de KM7 SE (Reddit): consertar na 2.0.4? **Sim, 2.0.4 (98929593, ccd37565).**
- 2026-10-09 (dono (Henrique)), `dec-dvp5-lg-hdr10-ou-mp4` [decidida]: Dolby Vision perfil 5 na LG: variante HDR10 ou MP4 primeiro (agente/2031-dvp5-hdr10 x agente/2031-dvp5-mp4)? **MP4 (agente/2031-dvp5-mp4) juntado na 2.0.4; C9 rodou o build, sem título DV perfil 5 em MP4 para exercitar.**
- 2026-10-09 (dono (Henrique)), `dec-canal-comunidade` [decidida]: Canal de comunidade (Telegram ou Discord) pedido por usuário? **Discord. Servidor "Nuvio Legacy" criado em 09/10 com modo Comunidade e estrutura no estilo do oficial (Info & Updates, Discussion por plataforma, Feedback em fóruns, Testing, Off-Topic). Convite permanente: https://discord.gg/9NWr6SHyzJ (vai no cartão de novidades com QR, notas e README).**
- 2026-10-09 (dono (Henrique)), `dec-410-vidro-profundidade-tpk` [decidida]: #410 (vidro, Profundidade e Reflexo no .tpk) entra em qual versão? **2.0.4: é regressão da 2.0.3.**
- 2026-10-09 (dono (Henrique)), `dec-addons-principal-off` [decidida]: "Usar os addons do perfil principal" ligado ou desligado por padrão? **Desligado por padrão (a6ae4f64); quem já escolheu mantém.**
- 2026-10-09 (dono (Henrique)), `dec-central-pi` [decidida]: Central de comando: base dos executores? **Sim: pi como harness dos executores; Sol estudando o RPC/sessões/fork.**

## Resumo

Por status:

| Status | Qtd |
|---|---|
| lancada | 304 |
| integrada | 56 |
| aberta | 47 |
| respondida | 22 |
| por-desenho | 15 |
| precisa-log | 7 |
| fechada-sem-resposta | 6 |
| fora-do-escopo | 4 |
| duplicada | 2 |
| em_revisao | 1 |

Por release (grupo de planejamento):

| Grupo | Qtd |
|---|---|
| lançadas em tag v* (qualquer versão) | 304 |
| sem release | 131 |
| 2.0.5 (integração, sem release do conserto) | 22 |
| futuro (2.1/2.2) | 5 |
| 2.0.3 lançada, com pendência | 2 |

Lançadas por versão: 1.0.7: 10, 1.0.8: 1, 1.0.10: 3, 1.0.11: 3, 1.0.15: 1, 1.0.22: 1, 1.0.24: 1, 1.0.29: 1, 1.0.30: 2, 1.0.31: 2, 1.0.32: 2, 1.0.34: 1, 1.0.35: 1, 1.0.37: 1, 1.0.38: 2, 1.0.39: 1, 1.0.42: 1, 1.0.43: 4, 1.0.44: 4, 1.0.45: 1, 1.0.51: 3, 1.0.52: 1, 1.0.53: 1, 1.0.55: 3, 1.2.1: 4, 1.3.0: 1, 1.3.2: 5, 1.3.4: 5, 1.3.5: 2, 1.3.8: 1, 1.3.9: 1, 1.3.10: 1, 1.3.11: 1, 1.3.12: 5, 1.4: 7, 1.4.1: 2, 1.4.2: 9, 1.4.3: 9, 1.4.4: 3, 1.4.5: 1, 1.4.6: 7, 1.4.7: 1, 1.5.0: 2, 1.5.1: 4, 1.5.2: 9, 1.5.3: 2, 1.5.4: 9, 1.6.0: 8, 1.6.1: 4, 1.6.2: 3, 1.6.3: 2, 1.6.4: 2, 1.6.5: 4, 1.7.0: 10, 1.7.1: 4, 1.7.2: 5, 1.7.4: 5, 2.0.0: 22, 2.0.1: 12, 2.0.2: 20, 2.0.3: 29, 2.0.4: 9, 2.0.5: 23.

Abertas no GitHub: 65. Fechadas: 399.
Abertas sem nenhum comentário nosso: 48.

## 2.0.3 lançada com pendência (precisa-log, respondida ou conserto parcial)

2 issues.

| # | Título | Plat. | Tipo | Status | Release | Alvo | Conserto | Última resposta | Próximo passo |
|---|---|---|---|---|---|---|---|---|---|
| [#246](https://github.com/iqui27/nuvio-native-legacy/issues/246) | HEVC anime videos can’t be “seeked in player” | ? | bug | integrada | 2.0.3 | - | a8048e0de, a7f23592, a8048e0d | nós 10-06 | lancar com 2.0.5 se TV ok (testar no TCL/C9) |
| [#280](https://github.com/iqui27/nuvio-native-legacy/issues/280) | Home Row does not update properly (Bingecat Addon) and Some  | LG | bug | precisa-log | 2.0.3 | - | 3c51d5b7 | nós 10-11 | pedir log (autor mandou só captura "See logs", sem código) |

Notas:

- **#246**: NOTAS 2.0.3: "LG: resume no longer drops the source on a failed seek (#246)". Não confirmado que cobre o relato HEVC/anime; plataforma do relato desconhecida. ALVO 2.0.3 = SUSPEITA: o conserto está nas NOTAS mas não está confirmado que cobre o relato; falta log do autor. COMMITS vs v2.0.3 (09/10): a7f23592, a8048e0d, 023497dd estão na tag; status mantido porque o conserto é suspeita/parcial e sem confirmação do autor. Revisão 2026-10-12: Seek manual/HEVC não é corrigido pela proteção de retomada contra troca de fonte. [alvo antes: 2.0.6; fechada no GH 10-15] | Escrivao 2026-10-15 11:48: commit em integracao/2.0.5 (a8048e0de); status -> integrada
- **#280**: SUSPEITA: único commit que cita #280 é 3c51d5b7 (remontagens da descoberta, 2.0.3); a correção é genérica de fileiras. ALVO 2.0.3 = SUSPEITA: o conserto está nas NOTAS mas não está confirmado que cobre o relato; falta log do autor. COMMITS vs v2.0.3 (09/10): 3c51d5b7 estão na tag; status mantido porque o conserto é suspeita/parcial e sem confirmação do autor. Revisão 2026-10-12: Remontagem espaçada não comprova correção da contagem/configuração; mantenedor ainda pede log em 11/10. [alvo antes: 2.0.6; fechada no GH 10-15]

## 2.0.4 (hotfix publicado)

9 issues.

| # | Título | Plat. | Tipo | Status | Release | Alvo | Conserto | Última resposta | Próximo passo |
|---|---|---|---|---|---|---|---|---|---|
| [#344](https://github.com/iqui27/nuvio-native-legacy/issues/344) | Live TV search crash | Android | bug | lancada | 2.0.4 | - | 35a742ee, 3e3044eb, 8dd273f3 | nós 10-10 | sem ação; conserto publicado |
| [#372](https://github.com/iqui27/nuvio-native-legacy/issues/372) | [Bug] Anime episodes not displaying further than season 1 | Samsung .tpk | bug | lancada | 2.0.4 | - | 21077ab5, 8eb5ce90, c2112b61 | nós 10-10 | sem ação; conserto publicado |
| [#378](https://github.com/iqui27/nuvio-native-legacy/issues/378) | “From Account” subtitle setting defaults to “NONE” on playba | Samsung .tpk | bug | lancada | 2.0.4 | - | 04844fb9, 28aa5c4b, 59733d54 | nós 10-10 | sem ação; conserto publicado |
| [#390](https://github.com/iqui27/nuvio-native-legacy/issues/390) | Custom poster source not setting | Samsung .tpk | bug | lancada | 2.0.4 | - | a7bb794c, a8fcd146, adfba576 | nós 10-12 | dono: avaliar fechamento com v2.0.4 |
| [#392](https://github.com/iqui27/nuvio-native-legacy/issues/392) | Profile switching still rearrange my home screen | Samsung .tpk | bug | lancada | 2.0.4 | - | 1d4b5e97, 2d3e472e, 9023ebdb | nós 10-11 | dono: avaliar fechamento com v2.0.4 |
| [#400](https://github.com/iqui27/nuvio-native-legacy/issues/400) | feature-request: allow using default addon sources sort orde | all | feature | lancada | 2.0.4 | - | 3d9db2d8, a46e1a4c, f60a4ba5 | nós 10-10 | sem ação; conserto publicado |
| [#409](https://github.com/iqui27/nuvio-native-legacy/issues/409) | The source didn't answer in time error even though the serve | Android | bug | lancada | 2.0.4 | - | 2051869e, 5cd60adf, 6d46e336 | nós 10-10 | sem ação; conserto publicado |
| [#410](https://github.com/iqui27/nuvio-native-legacy/issues/410) | UI appearence issues | Samsung .tpk | bug | lancada | 2.0.4 | ja-lancada | 52dbde71, b32c9410, b8935ecf | autor 10-12 | dono: avaliar fechamento com v2.0.4 |
| [#411](https://github.com/iqui27/nuvio-native-legacy/issues/411) | [Tizen 4.0 / v2.0.3] Play/Pause works but shows Samsung "Not | Samsung .tpk | bug | lancada | 2.0.4 | - | release | nós 10-10 | sem ação; conserto publicado |

Notas:

- **#344**: Triagem 2026-10-12: 35a742ee em v2.0.4; 3e3044eb em v2.0.4; 8dd273f3 em v2.0.4.
- **#372**: Triagem 2026-10-12: 21077ab5 em v2.0.4; 8eb5ce90 em v2.0.4; c2112b61 em v2.0.4.
- **#378**: Triagem 2026-10-12: 04844fb9 em v2.0.4; 28aa5c4b em v2.0.4; 59733d54 em v2.0.4.
- **#390**: Triagem 2026-10-12: a7bb794c em v2.0.4; a8fcd146 em v2.0.4; adfba576 em v2.0.4. Conserto inicial na 2.0.4; aliases e aviso imediato também na 2.0.5. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#392**: Triagem 2026-10-12: 1d4b5e97 em v2.0.4; 2d3e472e em v2.0.4; 9023ebdb em v2.0.4. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#400**: Triagem 2026-10-12: 3d9db2d8 em v2.0.4; a46e1a4c em v2.0.4; f60a4ba5 em v2.0.4.
- **#409**: Triagem 2026-10-12: 2051869e em v2.0.4; 5cd60adf em v2.0.4; 6d46e336 em v2.0.4. Escrivao 10-15: reaberta no GH (OPEN no snapshot 10-15).
- **#410**: Triagem 2026-10-12: 52dbde71 em v2.0.4; b32c9410 em v2.0.4; b8935ecf em v2.0.4. Parcial: efeitos têm conserto; nitidez/4K precisa confirmação na TV.
- **#411**: Triagem 2026-10-12: release-notes.md v2.0.4: - **Samsung Tizen 4 / 5: no more "Not Available" toast** on Play/Pause and other media keys (#411).. Escrivao 10-15: reaberta no GH (OPEN no snapshot 10-15).

## Conserto da integração 2.0.5 ainda sem release

22 issues.

| # | Título | Plat. | Tipo | Status | Release | Alvo | Conserto | Última resposta | Próximo passo |
|---|---|---|---|---|---|---|---|---|---|
| [#302](https://github.com/iqui27/nuvio-native-legacy/issues/302) | Live TV issue persists with 2.1 tpk65 | Samsung .tpk | bug | integrada | 2.0.5 | - | 5cfc61f10, 6c4d3863, 8fea5018 | autor 10-10 | lancar com 2.0.5 se TV ok (testar no TCL/C9) |
| [#402](https://github.com/iqui27/nuvio-native-legacy/issues/402) | Wrong sorting in sources | Samsung .tpk | bug | integrada | 2.0.5 | - | ca8f29b4, 219d3bf4 | autor 10-10 | lancar com 2.0.5 (testar no TCL antes do release) |
| [#412](https://github.com/iqui27/nuvio-native-legacy/issues/412) | Playback makes UI extremely laggy | Samsung .tpk | bug | integrada | 2.0.5 | 2.0.6 | 224063f2, 224063f22 | nós 10-11 | merged em integracao/2.0.5 (7a6126407 Keep TPK recovery seek targets until host confirma) |
| [#415](https://github.com/iqui27/nuvio-native-legacy/issues/415) | constant crashing, no matter if I play or not | LG | bug | integrada | 2.0.5 | 2.0.6 | 5c6b8093d, 5c266499 | nós 10-10 | lancar com 2.0.5 se TV ok (testar no TCL/C9) |
| [#419](https://github.com/iqui27/nuvio-native-legacy/issues/419) | [Samsung Tizen 6 / v2.0.4] Clicking a Recommendations card d | Samsung .tpk | bug | integrada | 2.0.5 | ja-lancada | 7f338254, 8b5ed6c0, d6b41e1 | nós 10-11 | merged em integracao/2.0.5 (d6b41e161 test(posplay): cover TMDB recommendation open at e) |
| [#428](https://github.com/iqui27/nuvio-native-legacy/issues/428) | Constant freezes | LG | bug | precisa-log | 2.0.5 | - | - | nós 10-11 | triar |
| [#429](https://github.com/iqui27/nuvio-native-legacy/issues/429) | Subtitles auto select | Android | bug | integrada | 2.0.5 | ja-lancada | agente/205-i429hbs, e8d9086a4 | nós 10-11 | merged em integracao/2.0.5 (f59554839 fix(#429): move hr/sr/bs to end of language list, ) |
| [#469](https://github.com/iqui27/nuvio-native-legacy/issues/469) | all movies shows not correct aspect ratio , original, stretc | Samsung .tpk | bug | precisa-log | 2.0.5 | 2.0.6 | - | sem comentários | Pedir log (codigo) com troca de modo de aspecto; ver qual pacote .tpk e qual caminho (Disp |
| [#470](https://github.com/iqui27/nuvio-native-legacy/issues/470) | Next Episode Countdown Only Starts After Credits Finish | Samsung .tpk | bug | integrada | 2.0.5 | - | 7839b119, d1087e75c | sem comentários | merged em integracao/2.0.5 (96a56ae2c fix: start next episode countdown when the card ap) |
| [#471](https://github.com/iqui27/nuvio-native-legacy/issues/471) | Trailer Playback Does Not Fill the Cards Properly | Samsung .tpk | bug | integrada | 2.0.5 | 2.0.6 | 38369f89, 38369f890 | sem comentários | merged em integracao/2.0.5 (60254073f fix(tpk): confine card trailer holes to the poster) |
| [#475](https://github.com/iqui27/nuvio-native-legacy/issues/475) | Subtitle outline color setting | Samsung .tpk | melhoria | integrada | 2.0.5 | - | 6a260e9b | autor 10-13 | lancar com 2.0.5 (testar no TCL antes do release) |
| [#477](https://github.com/iqui27/nuvio-native-legacy/issues/477) | Bigger synopsis font | Samsung .tpk | melhoria | integrada | 2.0.5 | - | 1366561a, 6a260e9b, 6a260e9b0 | autor 10-13 | merged em integracao/2.0.5 (dd56de2e6 Merge integracao/2.0.5 preserving synopsis and epi) |
| [#479](https://github.com/iqui27/nuvio-native-legacy/issues/479) | Formatter mode source list stutters on webOS | Samsung .tpk | bug | integrada | 2.0.5 | 2.0.6 | b3c38e71e | autor 10-13 | merged em integracao/2.0.5 (2a2356c29 fix: tint cached text RGB at draw time to restore ) |
| [#481](https://github.com/iqui27/nuvio-native-legacy/issues/481) | Show audio and subtitle metadata in the stream source list | ? | feature | em_revisao | 2.0.5 | - | e4fb8c08/c9932635 | sem comentários | merge agente/205-pr487v2 (3 commits, review OK ac1ba9725) ou descartar pelo dono |
| [#484](https://github.com/iqui27/nuvio-native-legacy/issues/484) | External subtitles disappear after a few minutes without inp | Samsung .tpk | bug | integrada | 2.0.5 | - | 027fd3c7, 027fd3c7c | autor 10-13 | merged em integracao/2.0.5 (df7db179c test(subtitles): make external-subtitle guards fas) |
| [#486](https://github.com/iqui27/nuvio-native-legacy/issues/486) | Sometimes wont load P2P addons | ? | bug | integrada | 2.0.5 | ja-lancada | edff609f/c670c37e/20bb2f3a, 16351755c, cdb45cb92 | sem comentários | merged em integracao/2.0.5 (8f4fb5587 fix(p2p): revalidate cached USB ownership and volu) |
| [#488](https://github.com/iqui27/nuvio-native-legacy/issues/488) | Profiles switching | ? | bug | integrada | 2.0.5 | - | 067c0c35, agente/205-i488perfil, f51aa3d76 | sem comentários | merged em integracao/2.0.5 (d16de03ad fix: profile switch pass waits for the account cyc) |
| [#492](https://github.com/iqui27/nuvio-native-legacy/issues/492) | Some Features suggestions | ? | feature | integrada | 2.0.5 | 2.0.6 | c624b273, c624b2739 | sem comentários | merged em integracao/2.0.5 (34a8d4819 Merge branch 'integracao/2.0.5' into agente/205-re) |
| [#499](https://github.com/iqui27/nuvio-native-legacy/issues/499) | Cached / not cached indicator on Debrid sources (torbox, rea | ? | feature | integrada | 2.0.5 | - | aab8e18d, 57aee24fd | sem comentários | merged em integracao/2.0.5 (aab8e18d9 Mark debrid-cached sources with a lightning icon i) |
| [#502](https://github.com/iqui27/nuvio-native-legacy/issues/502) | Detail page keeps the low-res thumbnail instead of loading f | ? | bug | integrada | 2.0.5 | 2.0.6 | d3fc8e5e, 20f1104f, 20f1104fc | sem comentários | merged em integracao/2.0.5 (d3fc8e5ee fix(detail): stop seeding list poster into backdro) |
| [#503](https://github.com/iqui27/nuvio-native-legacy/issues/503) | Low-resolution backdrop when opening a recommended title's d | ? | bug | integrada | 2.0.5 | 2.0.6 | d3fc8e5e, 20f1104f | sem comentários | merged em integracao/2.0.5 (d3fc8e5ee fix(detail): stop seeding list poster into backdro) |
| [#504](https://github.com/iqui27/nuvio-native-legacy/issues/504) | Catalog rows from removed add-ons remain in the “Not on home | ? | bug | integrada | 2.0.5 | 2.0.6 | be7c0dee, 3c58d0ce, 3c58d0ce6 | sem comentários | merged em integracao/2.0.5 (be7c0deeb fix(fileiras): drop untouched rows of removed add-) |

Notas:

- **#302**: Rascunho de correção: o conserto do live TV NÃO está na 2.0.2 (resposta anterior dizia que sim). 0021b573 (2.0.2) é parcial. LANÇADA na v2.0.3 (09/10): todos os commits do conserto estão contidos na tag. Log BKPH09 ainda não foi lido; sem conserto confirmado para este relato. Revisão 2026-10-12: Reconexão publicada, mas autor relata persistência em 2.0.4 em 10/10. [alvo antes: 2.0.6; fechada no GH 10-15] | Escrivao 2026-10-15 11:48: commit em integracao/2.0.5 (5cfc61f10); status -> integrada
- **#402**: Namer03, S95C, .tpk 2.0.3. A imagem mostra, na aba NMR, o cabecalho do grupo "Other SDR" com fileiras cujo nome (formatter do addon) diz "FHD | REMUX | SDR" e "FHD | SDR"; so a ultima diz "N/A | SDR" (essa realmente nao declara resolucao). CAUSA (lida no codigo, sem teste ainda): o grupo vem de grupoRes (src/streams.c:2105) que usa s->altura ou os selos r-4k/r-1080/r-720; a altura sai de stream_parse.c:281-283, que so procura os numeros 2160/1440/1080/720/480 e 4k/uhd, e badges_detectar (badges.c:40-42) so tem 1080p/1080i/1080. A palavra FHD sozinha (comum em formatter de AIOStreams) nao casa, entao altura=0 e o grupo e Outras (GRUPO_NOME, streams.c:2072). O app ja conhece FHD para canal ao vivo (nv_res_do_texto, livetv_regras.h:38) mas nao usa no filme/serie. NAO e ordenacao (nivelHdr, MP4 primeiro da LG e perfil 5 nao entram: e a Samsung .tpk e o problema e o grupo, nao a ordem dentro dele). Nao e configuracao (fonteregra.h trata auto-play, nao grupos). Cuidado no conserto: nv_res_do_texto aceita "hd" solto como 720, o que casaria DTS-HD; copiar so fhd/fullhd/full hd. Candidato a 2.0.4 (muda so o parser, 2 linhas + teste), decisao do dono. SUSPEITA: o JSON bruto do autor pode ter a resolucao em outro campo; sem o log/JSON da fonte nao foi provado. Diagnóstico atualizado: torrents sem cache; sem conserto identificado. Revisão 2026-10-12: FHD corrigido parcialmente; autor reabriu com ordenação ainda errada em 2.0.4. Escrivão 14/10 14:47: integrada via tpkui. [alvo antes: 2.0.6; fechada no GH 10-15]
- **#412**: Log QHQRVE (S95C, .tpk): relógio do player congelado, UI com upd=120 ms por quadro, parar retorna 0 ms mas o player nativo segue tocando. Conserto em agente/204-tpkpreso412 (71c7d615): ASS deixa de segurar o quadro, recupera relógio congelado, parada confirmada antes de abrir outro player. Revisão Codex achou P1 (Voltar durante PrepareAsync encerraria o processo numa TV sã): rodada 2 em andamento. Aguarda log; sem conserto confirmado para este relato. Revisão 2026-10-12: Correção 2.0.4 não cobre a persistência após reinstalação; comentário de 11/10 descreve novo fix em progresso. Escrivão 14/10 14:47: integrada via tpk412b. [sync 2026-10-15: agente/205-tpk412b integrada]
- **#415**: Diagnóstico via memlog; sem conserto confirmado. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho. | Escrivao 2026-10-15 11:48: commit em integracao/2.0.5 (5c6b8093d); status -> integrada
- **#419**: Job i419recs done 20:05: commit d6b41e1 so teste posplay. Sem device Tizen. Ja corrigido em 2.0.5 pelos commits v2.0.5. [sync 2026-10-15: agente/205-i419recs integrada] Escrivao 10-15 22:49: merged via agente/205-i419recs (f9ccde55c).
- **#428**: Aguarda código/log para diagnóstico. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho. | Escrivão 2026-10-14 16:11: GitHub CLOSED no snapshot de issues.json (2026-10-13). Escrivao 10-15: reaberta no GH (OPEN no snapshot 10-15).
- **#429**: Job i429hbs done 20:00. Sem Makefile no worktree; testes linguas.sh/legauto.sh rc 0. Mapeamento locale->hr nao feito. | Escrivao 10-15 02:44: review y OK; z BLOQUEIA antes do fix. Escrivao 10-15 02:49: review y OK (j-revg-i429hbs-y 23:11) apos fixz 22:28; merge pendente, branch tip f59554839. Escrivao 10-15 07:10: review y OK (23:11) apos fixz 22:28; branch ainda 2 ahead, merge pendente. | Escrivao 2026-10-15 07:50: sem merge ainda; review OK y, branch nao mergeado | Escrivao 15/10 09:48: review OK ainda sem merge. | Escrivão 10/15 12:48: branch i429hbs ainda nao integrada (2 commits fora) | Escrivao 10-15 13:48: review y OK (j-revg-i429hbs-y 23:11), branch f59554839 ainda NAO mergeado. [sync 2026-10-15: agente/205-i429hbs integrada] Escrivao 10-15 22:49: merged via agente/205-i429hbs (e8d9086a4).
- **#469**: Q800T (GQ65Q800TGT, 2020, Tizen 5.5), 2.0.5. Nenhum modo de aspecto muda a imagem. 2.0.5 ja tem c4f344e8 (DisplayRoi + fallback 2.0.4, #426/#341) e 81d2152b (tpk 6+). Suspeita: Tizen 5.5 cai no caminho sem DisplayRoi. Pedir codigo do log e foto.
- **#470**: rawldon, AU7000 Tizen 6.0, .tpk 2.0.5, log B9RJZ8 (registro 73397): [posplay] creditos aceito (sobram 265s, janela 300s); cartao aparece nos creditos mas contagem so comeca no fim. Escrivão 14/10 14:47: integrada via i18nordem, i470. | Escrivão 2026-10-14 16:11: GitHub CLOSED no snapshot de issues.json (2026-10-13). Escrivao 10-15: reaberta no GH (OPEN no snapshot 10-15). [sync 2026-10-15: agente/205-i470 integrada]
- **#471**: rawldon, AU7000 Tizen 6.0, .tpk 2.0.5, log QNHOK2 (registro 73399): "[trailer] tpk: plano visivel (sem recorte)" — trailer no cartao sem crop/fill no tpk. Relacionado a #469 (DisplayRoi/aspecto no tpk). Escrivão 14/10 14:47: integrada via i471. [sync 2026-10-15: agente/205-i471 integrada]
- **#475**: Pedido: cor do contorno da legenda. Escrivão 14/10 14:47: integrada via sinopse477.
- **#477**: Snapshot 2026-10-14 10:15: 1366561a "Restore the original hero synopsis size and add a Synopsis size setting" em agente/205-sinopse; j-205-sinopse done e j-revg-sinopse veredito OK. Escrivao 14/10 19:2x: 1366561a ancestral de integracao/2.0.5 (6a260e9b). [sync 2026-10-15: agente/205-sinopse477 integrada]
- **#479**: LG C5 webOS 26, 2.0.5, log M8C717 (registro 73696): descer na lista de fontes no modo Formatter engasga; modo padrao liso. | Escrivao 10-15 08:50: job formatter479 review OK; ver fora_do_github dono-205-formatter479. [sync 2026-10-15: agente/205-formatter479 integrada]
- **#481**: Snapshot 2026-10-14 10:15: "Sources: show declared audio and subtitle languages" (e4fb8c08) e o merge c9932635 "Merge PR #487 v2" existem so em agente/205-pr487v2, nao ancestral de integracao/2.0.5. OPEN no GitHub. | fechada no GH 2026-10-14 (CLOSED); alvo antigo 2.0.6 removido por regra do mapa.py; decidir com dono Escrivao 10-15: reaberta no GH (OPEN no snapshot 10-15). Escrivao 10-15: agente/205-pr487v2 (ac1ba9725) review OK j-rev-pr487-v2; nao mergeado; dono decide 2.0.6. | Escrivao 2026-10-15 11:48: GH fechada; branch 3 ahead, review OK, nao mergeado | Escrivão 10/15 12:48: GH #481 fechada; branch pr/487-v2 e agente/205-pr487v2 nao integrados
- **#484**: LG OLED C1 webOS, 2.0.5: legenda de addon (OpenSubtitlesV3/AIOMetadata) some apos minutos sem tecla; embutida ok. Sem log. | Escrivão 2026-10-14 16:11: GitHub CLOSED no snapshot de issues.json (2026-10-14). | Escrivão 2026-10-14 16:11: conserto integrado via agente/205-leg484 (merge 027fd3c7); fechada no GitHub. Escrivao 10-15: reaberta no GH (OPEN no snapshot 10-15). [sync 2026-10-15: agente/205-leg484 integrada]
- **#486**: Snapshot 2026-10-14 10:15: tres commits (#486) de agente/205-p2p486 ja sao ancestrais de integracao/2.0.5. #505 (sources P2P somem no .tpk 2.0.5) pode ser o mesmo relato; job agente/205-i505 RUNNING investiga. OPEN no GitHub. | Escrivao 10-15 02:44: integrado mas review BLOQUEIA; status em_revisao. Escrivao 10-15 02:49: git: agente/205-p2p486 e ancestral de integracao/2.0.5; BLOQUEIA 01:17 -> fix 01:22 -> OK 01:42. Escrivao 10-15 07:10: 205-p2pusb review v BLOQUEIA 06:49 (j-revg-p2pusb-v, mount container como USB + rescan suprimido) apos fix w 05:51; branch 9 ahead. | Escrivao 10-15 08:50: branch agente/205-p2pusb (dono P2P USB) BLOQUEIA v 06:49, fix v 07:52 aguarda review. Review OK rev3 de 01:42 era do p2p486 antigo. | Escrivao 15/10 09:48: fix j-fix-p2pusb-w em curso. | Escrivao 2026-10-15 11:48: prompt p-205-p2p486 em /Volumes/ExternalSSD/tmp/rev; job terminado, sem job vivo | Escrivão 10/15 12:48: p2p486 merged, mas p2pusb (#486/#505) segue aberto | Escrivao 10-15 13:48: v4 BLOQUEIA (12:48); fix y em curso 13:47 sem review. [sync 2026-10-15: agente/205-p2p486 integrada] Escrivao 10-15 22:49: merged via agente/205-p2pusb (cdb45cb92).
- **#488**: pr493 (#488 extra) lost: 12 commits, ultimo BLOQUEIA, sem job. Testes desc_iniciar_corrida/montagem_cedo ja quebrados na branch. Escrivao 10-15 02:49: git: agente/205-i488perfil integrado; review z OK 21:46 apos fix 20:03; falta teste de troca de perfil no TCL. Escrivao 10-15 07:10: pr/493-fix review OK fix2; autor publicou head nova 5694694cc; fixes portados 65613c562 (pr/493-v3) e review OK 06:50 (j-revpr493-v3). Mergear pr/493-v3 para carregar #488 no PR. | Escrivao 10-15 08:50: pr/493-v3 (3 ahead, review OK 06:50, nao mergeado) substitui o pr/493 velho; #488 segue nele. | Escrivao 15/10 09:48: GitHub fechada (issues.json 10-15). Escrivao 10-15: reaberta no GH (OPEN no snapshot 10-15). Escrivao 10-15 10:50: reaberta no GH; pr/493-v3 review OK sem merge. | Escrivao 2026-10-15 11:48: prompt p-205-perfil488 em /Volumes/ExternalSSD/tmp/rev; job terminado, sem job vivo [sync 2026-10-15: agente/205-i488perfil integrada] [sync 2026-10-15: agente/205-perfil488 integrada] Escrivao 10-15 22:49: merged via agente/205-i488perfil (f51aa3d76).
- **#492**: Review j-revg-retomamorna OK 17:06. Merge agente/205-retomamorna em integracao/2.0.5 (c624b273, 18:48); j-merge-205-retomamorna done 18:45. Sem acao remota. Escrivao 14/10 19:1x: merge 18:48. [sync 2026-10-15: agente/205-retomamorna integrada]
- **#499**: Snapshot 2026-10-14 10:15: aab8e18d "Mark debrid-cached sources with a lightning icon in the source list (#499)" e ancestral de integracao/2.0.5 (merge 57aee24f agente/205-499cache). Nenhum job j-revg-499cache: conserto integrado sem review registrado. OPEN no GitHub. [sync 2026-10-15: agente/205-499cache integrada]
- **#502**: Snapshot 2026-10-14 10:15: d3fc8e5e "fix(detail): stop seeding list poster into backdrop for recommendations" cobre #502 e #503, so em agente/205-i502arte. Job RUNNING, sem validacao. OPEN no GitHub. Escrivão 14/10 14:47: integrada via i502arte. [sync 2026-10-15: agente/205-i502arte integrada]
- **#503**: Snapshot 2026-10-14 10:15: mesmo conserto d3fc8e5e de #502 (agente/205-i502arte, RUNNING). OPEN no GitHub. Escrivão 14/10 14:47: integrada via i502arte. | Escrivão 2026-10-14 16:11: merge 20f1104f consta em log.txt da integracao.
- **#504**: Snapshot 2026-10-14 10:15: conserto be7c0dee "fix(fileiras): drop untouched rows of removed add-ons when the sheet opens (#504)" so em agente/205-i504 (nao e ancestral de integracao/2.0.5). Job j-205-i504 RUNNING, sem log de validacao ainda. Issue ainda OPEN no GitHub. Escrivão 14/10 14:47: integrada via i504. | Escrivao 2026-10-15 11:48: prompt p-205-i504 em /Volumes/ExternalSSD/tmp/rev; job terminado, sem job vivo [sync 2026-10-15: agente/205-i504 integrada]

## Próximo patch proposto (2.0.6)

24 issues.

| # | Título | Plat. | Tipo | Status | Release | Alvo | Conserto | Última resposta | Próximo passo |
|---|---|---|---|---|---|---|---|---|---|
| [#407](https://github.com/iqui27/nuvio-native-legacy/issues/407) | Top menu open when scrolling up on the title page | Samsung .tpk | bug | integrada | - | 2.0.6 | 219d3bf4 | sem comentários | lancar com 2.0.5 (testar no TCL antes do release) |
| [#413](https://github.com/iqui27/nuvio-native-legacy/issues/413) | [Samsung Tizen 4 / v2.0.3] Finished movie plays again when s | Samsung .tpk | bug | integrada | - | 2.0.6 | d8cc7fa2 | autor 10-10 | lancar com 2.0.5 (testar no TCL antes do release) |
| [#414](https://github.com/iqui27/nuvio-native-legacy/issues/414) | https://nuvio.tvflix.co.uk | Samsung .tpk | bug | integrada | - | 2.0.6 | 168be34d9 | sem comentários | merged em integracao/2.0.5 (90f09070a fix(collections): keep Menu catalog selector open ) |
| [#418](https://github.com/iqui27/nuvio-native-legacy/issues/418) | App fails to connect: "The Nuvio account server didn't respo | Samsung .tpk | bug | integrada | - | 2.0.6 | deb68c4b9 | sem comentários | merged em integracao/2.0.5 (01567c6b0 Classify account login failures and retry transien) |
| [#423](https://github.com/iqui27/nuvio-native-legacy/issues/423) | Not update LG | LG | bug | aberta | - | 2.0.6 | - | autor 10-11 | triar/reproduzir; falta conserto comprovado |
| [#424](https://github.com/iqui27/nuvio-native-legacy/issues/424) | Live tv not loading categories | Android | bug | aberta | - | 2.0.6 | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#451](https://github.com/iqui27/nuvio-native-legacy/issues/451) | Continue watching subtitles not syncing | Samsung .tpk | bug | aberta | - | 2.0.6 | - | autor 10-11 | Pedir versão exata, perfil usado nos dois clientes, uma série/episódio ausente e sequência |
| [#459](https://github.com/iqui27/nuvio-native-legacy/issues/459) | Send partial playback progress to Simkl | LG | feature | integrada | - | 2.0.6 | fe1366e5 | sem comentários | lancar com 2.0.5 (testar no TCL antes do release) |
| [#461](https://github.com/iqui27/nuvio-native-legacy/issues/461) | Top toolbar | Android | feature | integrada | - | 2.0.6 | f3de11e7 | nós 10-12 | lancar com 2.0.5 (testar no TCL antes do release) |
| [#462](https://github.com/iqui27/nuvio-native-legacy/issues/462) | Samsung .tpk: test build for automatic subtitle sync (Tizen  | Samsung .tpk | meta | integrada | - | 2.0.6 | 84095af, 3247fcd22, da9e686a7 | autor 10-12 | merged em integracao/2.0.5 (1d946dad0 i18n: fill subtitle-sync entries in all 28 languag) |
| [#468](https://github.com/iqui27/nuvio-native-legacy/issues/468) | All Jellyfin catalogs do not show in catalogs list | LG | bug | aberta | - | 2.0.6 | - | sem comentários | Pedir log e nomes/quantidade dos catálogos esperados sem endereço/credenciais; obter fixtu |
| [#483](https://github.com/iqui27/nuvio-native-legacy/issues/483) | Anime subtitles not rendering | ? | bug | integrada | - | 2.0.6 | 251632be, 251632beb | sem comentários | merged em integracao/2.0.5 (fd6d795d0 fix(mkv): fetch Tracks via SeekHead when probe mis) |
| [#494](https://github.com/iqui27/nuvio-native-legacy/issues/494) | Add contribution path for Aniskip and TheIntroDB skip marker | ? | feature | aberta | - | 2.0.6 | - | sem comentários | sem job nem branch: triar e criar job |
| [#496](https://github.com/iqui27/nuvio-native-legacy/issues/496) | No next-episode suggestion / no quick next episode shortcut  | ? | feature | aberta | - | 2.0.6 | - | sem comentários | sem job nem branch: triar e criar job |
| [#497](https://github.com/iqui27/nuvio-native-legacy/issues/497) | Prioritize continue watch sync on startup | ? | feature | aberta | - | 2.0.6 | - | sem comentários | sem job nem branch: triar e criar job |
| [#500](https://github.com/iqui27/nuvio-native-legacy/issues/500) | [Samsung Tizen 9 / V2.0.5] Trailers not playing on Hero | ? | bug | integrada | - | 2.0.6 | b07ef570a, bf9edb493, agente/205-herotrailer | sem comentários | merged em integracao/2.0.5 (4359b10f5, perffinal); review OK (j-revg-herotrailer, j-revg-p |
| [#501](https://github.com/iqui27/nuvio-native-legacy/issues/501) | [Android TV] Add External Player Selection and Custom Subtit | ? | feature | aberta | - | 2.0.6 | - | sem comentários | sem job nem branch: triar e criar job |
| [#505](https://github.com/iqui27/nuvio-native-legacy/issues/505) | My P2p add on sources don’t load with version2.05 | ? | bug | integrada | - | 2.0.6 | b576c7a9, b576c7a9a, cdb45cb92 | sem comentários | merged em integracao/2.0.5 (8f4fb5587 fix(p2p): revalidate cached USB ownership and volu) |
| [#506](https://github.com/iqui27/nuvio-native-legacy/issues/506) | App fails to connect: "The Nuvio account server didn't respo | ? | bug | integrada | - | 2.0.6 | 0e567d1a, agente/205-dicascontrole | sem comentários | merged em integracao/2.0.5 (a5c857c51 Merge integracao/2.0.5 into agente/205-dicascontro) |
| [#507](https://github.com/iqui27/nuvio-native-legacy/issues/507) | Library on home | ? | feature | aberta | - | 2.0.6 | - | sem comentários | sem job nem branch: triar e criar job |
| [#508](https://github.com/iqui27/nuvio-native-legacy/issues/508) | Stream loading backdrop | ? | feature | integrada | - | 2.0.6 | d57b79e9, d57b79e9b | sem comentários | merged em integracao/2.0.5 (95d225dc2 Merge branch 'integracao/2.0.5' into agente/205-i5) |
| [#528](https://github.com/iqui27/nuvio-native-legacy/issues/528) | [Bug/FR] Cast Images Fail to Load — Request for Metadata Add | ? | ? | integrada |  | 2.0.6 | f5c785d4f | sem comentários | merged em integracao/2.0.5 (10501d047 fix(metadata): preserve addon cast portraits when ) |
| [#530](https://github.com/iqui27/nuvio-native-legacy/issues/530) | Subtitle issues | ? | ? | integrada |  | 2.0.6 | cbd542148, 23cb26843, agente/205-legatraso | sem comentários | merged em integracao/2.0.5 (23cb26843, sinal do atraso manual invertido); revisao BLOQUEIA |
| [#545](https://github.com/iqui27/nuvio-native-legacy/issues/545) | Catalog Now Showing | ? | bug | aberta | - | 2.0.6 | - | sem comentários | sem job; triar (sem prompt de job no snapshot) |

Notas:

- **#407**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. Escrivão 14/10 14:47: integrada via tpkui.
- **#413**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. Escrivão 14/10 14:47: integrada via tpkfim.
- **#414**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [sync 2026-10-15: agente/205-col414 integrada]
- **#418**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [sync 2026-10-15: agente/205-login418 integrada]
- **#423**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais.
- **#424**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais.
- **#451**: QN74F Tizen 9, versão apenas Latest: Continue Watching tem uma série contra quatro em outros clientes; autor corrige título e confirma Nuvio sync, sem Trakt/Simkl. src/syncprog.c:syncprog_puxar já importa sync_pull_watch_progress do perfil ativo, normaliza milissegundos e descarta duração inválida; syncprog_aplicar mescla progresso e src/descoberta.c monta a união da conta/local. SUSPEITA: perfil, importação/normalização ou filtros de CW; não ausência geral de sync nem problema de legendas. Sem log nem conserto comprovado deste caso.
- **#459**: LG 2.0.4, Simkl conectado diretamente: pede enviar progresso parcial ao pausar/sair para retomar em outros apps. src/simkl.c:simkl_ler_playback e simkl_continuar leem pausados; simkl_episodios_marcar/simkl_titulo_marcar enviam histórico de visto. src/player.c chama trakt_scrobble ao tocar/pausar; não há envio Simkl equivalente neste caminho. Leitura de playback e histórico existem, envio incremental Simkl não comprovado. Limite demonstrado do cliente, sem afirmar capacidade atual da API Simkl offline. Escrivão 14/10 14:47: integrada via simkl-scrobble.
- **#461**: Pede opção de toolbar no topo; mantenedor responde que hoje há menu lateral e relaciona #407. src/menu.c:desenhaBarra/geoEm implementam rail/painel lateral; src/detail.c roteia CIMA para área social e ESQUERDA para menu. Não há opção top toolbar comprovada; barra social no topo não é substituto da navegação principal. Sem bug ou causa a diagnosticar. Escrivão 14/10 14:47: integrada via menutopo.
- **#462**: Job i462sync done 20:07: 84095af auto sync rejection translated tracks; tests autosync.sh/legsync.sh rc 0. [sync 2026-10-15: agente/205-i18n462 integrada] [sync 2026-10-15: agente/205-i462sync integrada] Escrivao 10-15 22:49: merged via agente/205-i462sync (da9e686a7).
- **#468**: C5 webOS 26, 2.0.5: AIOMetadata via Jellyfin mostra cerca de quatro catálogos. src/jellyfin.c:jf_bibliotecas consulta UserViews; jf_ler_bibliotecas só aceita IDs hex limpos e CollectionType movies/tvshows. tarefaBibliotecas usa JF_FIL_MAX=6 (src/jellyfin.h) e pula biblioteca sem itens. Estes limites/filtros são comprovados no código, mas quatro visíveis não prova qual disparou. SUSPEITA: teto de seis, tipo/ID não convencional do servidor compatível ou fileira vazia. Nenhum conserto lançado para este relato.
- **#483**: Impl fd6d795d; j-revg-i483remux OK 16:52. Merge 251632be (18:01). fetch via SeekHead quando a sonda erra o layout REMUX. Escrivao 14/10 19:1x: merge 18:01. | Escrivao 2026-10-15 11:48: prompt p-205-i483remux em /Volumes/ExternalSSD/tmp/rev; job terminado, sem job vivo [sync 2026-10-15: agente/205-i483remux integrada]
- **#494**: Snapshot 2026-10-14 10:15: criada 2026-10-14, labels sem label. Sem triagem local.
- **#496**: Snapshot 2026-10-14 10:15: criada 2026-10-14, labels sem label. Sem triagem local.
- **#497**: Snapshot 2026-10-14 10:15: criada 2026-10-14, labels enhancement. Sem triagem local.
- **#500**: Snapshot 2026-10-14 10:15: criada 2026-10-14, labels bug. Sem triagem local. [sync 2026-10-15: agente/205-i500q integrada] Escrivao 10-15 22:49: merged via agente/205-i500q (bf9edb493). Escrivao 10-16 00:28: merge agente/205-herotrailer (85221cec4 ajustes/hero trailer veto row); review j-revg-herotrailer OK. Escrivao 10-16 01:50: merge agente/205-perffinal 4359b10f5; review j-revg-perffinal OK.
- **#501**: Snapshot 2026-10-14 10:15: criada 2026-10-14, labels enhancement. Sem triagem local.
- **#505**: Snapshot 2026-10-14 10:15: 3fc691d4 "fix(sources): count notWebReady P2P drops for empty-sheet hint (#505)" em agente/205-i505, nao ancestral de integracao/2.0.5. OPEN no GitHub. Escrivão 14/10 14:47: integrada via i505. | Escrivão 2026-10-14 16:11: merge b576c7a9 consta em log.txt da integracao. Escrivao 10-15 07:10: mesmo relato do p2pusb; review v BLOQUEIA 06:49; branch agente/205-p2pusb 9 ahead. | Escrivao 10-15 08:50: liga ao dono P2P USB (agente/205-p2pusb); fix 07:52 sem review. | Escrivao 2026-10-15 11:48: prompt p-205-i505* em /Volumes/ExternalSSD/tmp/rev; job terminado, sem job vivo | Escrivão 10/15 12:48: escopo em agente/205-p2pusb | Escrivao 10-15 13:48: v4 BLOQUEIA (12:48); fix y em curso 13:47 sem review. [sync 2026-10-15: agente/205-i505 integrada] [sync 2026-10-15: agente/205-p2pusb integrada] Escrivao 10-15 22:49: merged via agente/205-p2pusb (cdb45cb92).
- **#506**: Snapshot 2026-10-14 10:15: job j-205-i506 RUNNING (branch agente/205-i506, 0 commits fora de integracao/2.0.5). Suspeita de causa comum com #505. OPEN no GitHub. Escrivão 14/10 14:47: integrada via i506. | Escrivão 2026-10-14 16:11: merge 0e567d1a (ancestral) consta em log.txt da integracao. | Escrivao 2026-10-15 11:48: prompt p-205-i506 em /Volumes/ExternalSSD/tmp/rev; job terminado, sem job vivo [sync 2026-10-15: agente/205-i506 integrada]
- **#507**: Snapshot 2026-10-14 10:15: criada 2026-10-14, labels enhancement. Sem triagem local.
- **#508**: Snapshot 2026-10-14 10:15: criada 2026-10-14, labels enhancement. Sem triagem local. Escrivão 14/10 14:47: branch agente/205-i508fundo: review OK, merge pendente. | Escrivão 2026-10-14 16:11: i508fundo mergeada (d57b79e9); review j-revg-i508fundo OK. | Escrivao 2026-10-15 11:48: prompt p-205-i508fundo em /Volumes/ExternalSSD/tmp/rev; job terminado, sem job vivo [sync 2026-10-15: agente/205-i508fundo integrada]
- **#528**: Escrivao 2026-10-15 11:48: item novo (issues.json), alvo 2.0.6 provisório [sync 2026-10-15: agente/205-i528q integrada] Escrivao 10-15 22:49: merged via agente/205-i528q (f5c785d4f).
- **#530**: Escrivao 10/15 13:48: novo no GitHub 2026-10-15 (Subtitle issues), sem prompt de job. [sync 2026-10-15: agente/205-i530q integrada] Escrivao 10-15 22:49: merged via agente/205-i530q (cbd542148). Escrivao 10-16 00:06: merge agente/205-legatraso (05c24b89c/ad03fedc5/8d14dd55e, migracao do valor salvo). BLOQUEIA (j-revg-legatraso: audsync) -> OK (j-revg2-legatraso). Escrivao 10-16 01:50: HEAD 4359b10f5; ainda nao instalado na TV.
- **#545**: Nova em 2026-10-16 (issues.json, labels ['bug']). Sem job no snapshot.

## Futuro (2.1, 2.2, depois)

19 issues.

| # | Título | Plat. | Tipo | Status | Release | Alvo | Conserto | Última resposta | Próximo passo |
|---|---|---|---|---|---|---|---|---|---|
| [#250](https://github.com/iqui27/nuvio-native-legacy/issues/250) | Add Arabic language support | all | feature | respondida | 2.1 | - | - | nós 10-06 | nada |
| [#260](https://github.com/iqui27/nuvio-native-legacy/issues/260) | Add an optional layout switch to change UI direction from Ri | all | feature | respondida | futuro | - | - | autor 10-06 | nada |
| [#343](https://github.com/iqui27/nuvio-native-legacy/issues/343) | [suggestion] anime-skip.com | all | feature | respondida | futuro (anime-skip.com) | - | - | autor 10-12 | nada |
| [#420](https://github.com/iqui27/nuvio-native-legacy/issues/420) | Feature request: Android TV title deep links (ACTION_VIEW) f | Android | feature | aberta | - | 2.1 | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#434](https://github.com/iqui27/nuvio-native-legacy/issues/434) | Ability to remove genre tags in title page | all | feature | aberta | - | 2.1 | - | nós 10-11 | triar/reproduzir; falta conserto comprovado |
| [#437](https://github.com/iqui27/nuvio-native-legacy/issues/437) | Cloudstream Repository Support | all | feature | fora-do-escopo | - | futuro | - | nós 10-11 | Dono postar resposta de fechamento (rascunho em respostas-triagem-2.0.5.md). |
| [#438](https://github.com/iqui27/nuvio-native-legacy/issues/438) | MdbList | LG | feature | aberta | - | futuro | - | nós 10-11 | triar/reproduzir; falta conserto comprovado |
| [#450](https://github.com/iqui27/nuvio-native-legacy/issues/450) | PS5 Port | all | feature | aberta | - | futuro | - | autor 10-11 | Avaliar SDK/toolchain, vídeo, entrada e distribuição PS5 antes de qualquer plano; manter p |
| [#457](https://github.com/iqui27/nuvio-native-legacy/issues/457) | Add an option to show info for preresolved streams in detail | Android | feature | aberta | - | 2.1 | - | sem comentários | Definir quais campos confiáveis do Stream mostrar na ficha e estado sem fonte/sonda penden |
| [#458](https://github.com/iqui27/nuvio-native-legacy/issues/458) | Check for duplicate sources | all | feature | aberta | - | 2.1 | - | autor 10-12 | Criar caso local com dois provedores retornando mesmo URL/hash+fileIdx; definir identidade |
| [#466](https://github.com/iqui27/nuvio-native-legacy/issues/466) | Incremental watch tracking with watch_state resource | all | feature | integrada | - | futuro | 90e43bd1 | sem comentários | lancar com 2.0.5 (testar no TCL antes do release) |
| [#467](https://github.com/iqui27/nuvio-native-legacy/issues/467) | Watch tracking with Anilist | all | feature | aberta | - | futuro | - | sem comentários | Definir autenticação e mapeamento de anime/episódios AniList; comparar implementação diret |
| [#509](https://github.com/iqui27/nuvio-native-legacy/issues/509) | Expanded poster trailer | ? | enhancement | integrada | - | futuro | 13e13b92, 13e13b92a | sem comentários | merged em integracao/2.0.5 (707769017 trailer (#509): migrate trailer sound after the sa) |
| [#510](https://github.com/iqui27/nuvio-native-legacy/issues/510) | Subtitles list not the same as nuvio in phone applications l | ? | bug | integrada |  | futuro | abff3d67b | sem comentários | merged em integracao/2.0.5 (8bd95ed4d fix: preserve subtitle selection after MKV track r) |
| [#514](https://github.com/iqui27/nuvio-native-legacy/issues/514) | Ability to rate a movie or an episode | ? | ? | aberta |  | futuro | - | sem comentários | triagem; sem job ainda |
| [#521](https://github.com/iqui27/nuvio-native-legacy/issues/521) | Layout | ? | bug | integrada |  | futuro | 5184d0a1b | sem comentários | merged em integracao/2.0.5 (c670a5315 fix(i18n): translate the two Mural titles keys in ) |
| [#524](https://github.com/iqui27/nuvio-native-legacy/issues/524) | Anime series showing up as an altogether different movie | ? | bug | integrada | - | futuro | e2fe84954 | sem comentários | merged em integracao/2.0.5 (ac3d60575 fix: preserve catalog identity when parsing anime ) |
| [#539](https://github.com/iqui27/nuvio-native-legacy/issues/539) | Feature Request: Add Deep Link Support to Open Specific Movi | ? | feature | aberta |  | 2.1 | - | sem comentários | triagem: pedido de deep link; fora da 2.0.5 (sem prompt) |
| [#542](https://github.com/iqui27/nuvio-native-legacy/issues/542) | Torbox cloud library | ? | feature | aberta | - | futuro | - | sem comentários | triar; sem job |

Notas:

- **#250**: Metadados em árabe na 2.0.1; menus em árabe planejados para 2.1 (segundo a resposta em #325). Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho. [alvo antes: 2.1; fechada no GH 10-15]
- **#260**: Layout RTL "planejado para depois"; sem branch. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho. [alvo antes: futuro; fechada no GH 10-15]
- **#343**: AniSkip/TheIntroDB já na 2.0.1 (3a02817d); anime-skip.com "na atualização seguinte". Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#420**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. Dono 12/10: esquema nuviolegacy:// (nao nuvio://), mesmas formas do oficial.
- **#434**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais.
- **#437**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais.
- **#438**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais.
- **#450**: Pedido de porte para PS5 com jailbreak; autor já usa webOS e aceita que é trabalho grande. src/menu.c:menu_desenhar depende de SDL/gfx; android/app/src/main/java/space/nuvio/nativelegacy/NvPlayer.kt é backend Media3 específico, e tizen-tpk contém empacotamento próprio. Reuso da UI não fornece backend/SDK/instalação PS5. Não há implementação PS5 comprovada nem bug atual.
- **#457**: Pede selos DV/HDR10, tamanho, áudio e release group da fonte pré-resolvida na ficha. src/app.c usa stream_antecipada para iniciar a fonte; src/streams.c:montar e src/streams.h já mantêm metadados e exibem fontes/selos na folha. src/detail.c:detail_desenhar desenha ficha do título, não uma opção de resumo de fonte pré-resolvida com todos esses campos. Funcionalidade relacionada existe na folha, não prova entrega do pedido na ficha.
- **#458**: Relata fontes iguais entre add-ons, repetição no autoplay e aba All; pede deduplicação por prioridade e agregação por qualidade (corpo disponível é trecho). src/streams.c:mesmaFonte inclui provedor, URL, hash, arquivo e rótulo para manter foco entre atualizações, não dedup global; montar já organiza por qualidade ou preserva ordem do add-on. src/fontesmortas.c registra fontes mortas por provedor/identidade. SUSPEITA: tentativas repetidas entre provedores; não reproduzida. Agrupamento existe parcialmente, filtro transversal por prioridade não comprovado.
- **#466**: Pede resource watch_state para addons receberem eventos incrementais e delegarem tracking a vários provedores. src/addons.c interpreta resources de metadados/fontes/legendas; src/player.c emite eventos Trakt e progresso local/Nuvio via src/syncprog.c, sem despachante watch_state. Referências externas não consultadas (offline). Infraestrutura de tracking existe, contrato/autorização/entrega de eventos ao addon não estão implementados nesse caminho. Escrivão 14/10 14:47: integrada via ws1.
- **#467**: Pede tracking AniList, sem plataforma. src/artereserva.c:anilist consulta arte GraphQL e src/descoberta.c reconhece IDs anilist; isso é metadado/arte, não autenticação ou envio de histórico. src/visto.c:visto_titulo/visto_episodios distribuem visto para provedores existentes, sem destino AniList. Tracking não implementado no caminho lido; sem bug a reproduzir.
- **#509**: Criada 2026-10-14 (snapshot 14:46). Escrivão 14/10 14:47: integrada via i509trailer. | Escrivão 2026-10-14 16:11: conserto 13e13b92 ja em integracao/2.0.5; sem release ainda. | Escrivao 2026-10-15 11:48: prompt p-205-i509trailer* em /Volumes/ExternalSSD/tmp/rev; job terminado, sem job vivo [sync 2026-10-15: agente/205-i509trailer integrada]
- **#510**: Criada 2026-10-14 (snapshot 14:46). [sync 2026-10-15: agente/205-i510q integrada] Escrivao 10-15 22:49: merged via agente/205-i510q (abff3d67b).
- **#514**: Issue nova 14/10 20:52 (rating de filme/episodio). Feature, nao 2.0.5.
- **#521**: Issue nova 2026-10-15 06:48 (issues.json). Sem descrição de contexto; triar. [sync 2026-10-15: agente/205-i521q em_job] [sync 2026-10-15 19:50: agente/205-i521q merged em integracao/2.0.5 (5184d0a1b); fix do BLOQUEIA incluso] Escrivao 10-15 22:49: merged via agente/205-i521q (5184d0a1b).
- **#524**: Issue nova 2026-10-15 11:33 (issues.json, label bug). Sem triagem local. [sync 2026-10-15: agente/205-i524q integrada] Escrivao 10-15 22:49: merged via agente/205-i524q (e2fe84954).
- **#539**: Escrivao 2026-10-15 20:48: novo no GitHub 2026-10-15, sem job.
- **#542**: Entered mapa 2026-10-15 from issues.json; feature request, not in 2.0.5 scope.

## Outras pendências

2 issues.

| # | Título | Plat. | Tipo | Status | Release | Alvo | Conserto | Última resposta | Próximo passo |
|---|---|---|---|---|---|---|---|---|---|
| [#495](https://github.com/iqui27/nuvio-native-legacy/issues/495) | How to return to the island card (movie resume) from the hom | ? | feature | integrada | - | 2.0.5 | 9734ecc2b, d590e0f9 | sem comentários | merged em integracao/2.0.5 (d590e0f9e feat(home): Up on the top hero card reaches the is) |
| [#512](https://github.com/iqui27/nuvio-native-legacy/issues/512) | Tracking and next episode issues with Kaiju no 8 season 2 | ? | bug | integrada |  | 2.0.5 | 1b47d82f8, 8c7adb07, a0e14d7c | sem comentários | merged em integracao/2.0.5 (8c7adb078 fix(trakt): read numeric episode ids and keep them) |

Notas:

- **#495**: Review OK j-revg-i495ilha 19:55. Merged 9734ecc2b. Escrivao antigo dizia sem review: errado. [sync 2026-10-15: agente/205-i495ilha integrada] Escrivao 10-15 22:49: merged via agente/205-i495ilha (9734ecc2b).
- **#512**: Review OK j-revg-i512temporada-r2 19:57. Merged 1b47d82f8. P2 aberto: episodio sem releaseDate nao marca CWO_SEM_DATA (trakt.c:616). | Escrivao 2026-10-15 11:48: prompt p-205-i512temporada* em /Volumes/ExternalSSD/tmp/rev; job terminado, sem job vivo [sync 2026-10-15: agente/205-i512temporada integrada] Escrivao 10-15 22:49: merged via agente/205-i512temporada (1b47d82f8).

## Já lançado

295 issues.

### Lançadas e ainda abertas no GitHub (10)

| # | Título | Plat. | Tipo | Status | Release | Alvo | Conserto | Última resposta | Próximo passo |
|---|---|---|---|---|---|---|---|---|---|
| [#416](https://github.com/iqui27/nuvio-native-legacy/issues/416) | [Samsung Tizen 6 / v2.0.4] Filmography page empty - every ap | Samsung .tpk | bug | lancada | 2.0.5 | ja-lancada | 4b43829a, 7f338254, 8b5ed6c0 | nós 10-11 | dono: avaliar fechamento com v2.0.5 |
| [#417](https://github.com/iqui27/nuvio-native-legacy/issues/417) | [Samsung Tizen 6 / v2.0.4] Every card in the Trailers row pl | Samsung .tpk | bug | lancada | 2.0.5 | ja-lancada | e0bb1020, f219bc9d | nós 10-11 | dono: avaliar fechamento com v2.0.5 |
| [#421](https://github.com/iqui27/nuvio-native-legacy/issues/421) | Bold and style in Arabic subtitles still not working | Samsung .tpk | bug | lancada | 2.0.5 | ja-lancada | aced3443, c8e8d983 | nós 10-11 | dono: avaliar fechamento com v2.0.5 |
| [#422](https://github.com/iqui27/nuvio-native-legacy/issues/422) | In-app screensaver comes on during an autoplaying trailer if | Android | bug | lancada | 2.0.5 | ja-lancada | 65bb6e2c, a408e51a | nós 10-11 | dono: avaliar fechamento com v2.0.5 |
| [#425](https://github.com/iqui27/nuvio-native-legacy/issues/425) | Unnable to remove subtitle languages | all | feature | lancada | 2.0.5 | ja-lancada | 2b4119d5, 2c571ab6, 8b7468b0 | sem comentários | dono: avaliar fechamento com v2.0.5 |
| [#426](https://github.com/iqui27/nuvio-native-legacy/issues/426) | Aspect ratio bug | Samsung .tpk | bug | lancada | 2.0.5 | ja-lancada | 3ba2987a, c4f344e8 | autor 10-12 | dono: avaliar fechamento com v2.0.5 |
| [#430](https://github.com/iqui27/nuvio-native-legacy/issues/430) | Multiple intro skip sources and priority order | all | feature | lancada | 2.0.5 | ja-lancada | 551edb43, a9c97b26 | nós 10-11 | dono: avaliar fechamento com v2.0.5 |
| [#433](https://github.com/iqui27/nuvio-native-legacy/issues/433) | All posters are displayed horizontally, not coming from my a | LG | bug | lancada | 2.0.5 | ja-lancada | 7777e129, ea0c322a | autor 10-12 | dono: avaliar fechamento com v2.0.5 |
| [#439](https://github.com/iqui27/nuvio-native-legacy/issues/439) | Increase maximum subtitle size from 250 to 300 | all | feature | lancada | 2.0.5 | ja-lancada | 2f1e2de1, 6ad103f3 | nós 10-11 | Dono revisar rascunho de fechamento citando 2.0.5; pedir reabertura se 300% não estiver di |
| [#440](https://github.com/iqui27/nuvio-native-legacy/issues/440) | Request: Subtitle size decreases when Bold is enabled | all | bug | lancada | 2.0.5 | ja-lancada | 2f1e2de1, 6ad103f3 | nós 10-11 | Dono revisar fechamento com 2.0.5; reabrir com idioma, fonte e exemplo se o tamanho ainda  |

Notas:

- **#416**: Triagem 2026-10-12: 4b43829a em v2.0.5; 7f338254 em v2.0.5; 8b5ed6c0 em v2.0.5.
- **#417**: Triagem 2026-10-12: e0bb1020 em v2.0.5; f219bc9d em v2.0.5; release-notes.md v2.0.5: - The title page shows one trailer card again (#417), and the screensaver waits while a trailer plays (#422)..
- **#421**: Triagem 2026-10-12: aced3443 em v2.0.5; c8e8d983 em v2.0.5; release-notes.md v2.0.5: - Embedded text subtitles are drawn by the app on LG and Samsung (#421); on LG their size matches the preview..
- **#422**: Triagem 2026-10-12: 65bb6e2c em v2.0.5; a408e51a em v2.0.5; release-notes.md v2.0.5: - The title page shows one trailer card again (#417), and the screensaver waits while a trailer plays (#422)..
- **#425**: Triagem 2026-10-12: 2b4119d5 em v2.0.5; 2c571ab6 em v2.0.5; 8b7468b0 em v2.0.5.
- **#426**: Triagem 2026-10-12: 3ba2987a em v2.0.5; c4f344e8 em v2.0.5; release-notes.md v2.0.5: - Samsung aspect ratio: the picture stays where it belongs on Tizen 4 (#426, #341), an off-screen crop is refused instead of pushing the image away, and a refused or queued change gives the sound back and stays in its own video.. | 13/10 KeijoMika: zoom ok no Q80R Tizen 5 em 2.0.5, mas pausado so aplica ao retomar. Job agente/205-aspecto426.
- **#430**: Triagem 2026-10-12: 551edb43 em v2.0.5; a9c97b26 em v2.0.5; release-notes.md v2.0.5: - Auto-skip of credits only fires with a real start and end marker (#329, #331) and rewinding never skips them again; intro markers also cover anime opened from IMDb/TMDB, in any season (#430)..
- **#433**: Triagem 2026-10-12: 7777e129 em v2.0.5; ea0c322a em v2.0.5; release-notes.md v2.0.5: - Metadata loads again when the main TMDB address is blocked or times out (#416, #419), and posters with very long addresses load again (#433).. Parcial: URL longa tem conserto; conferir formato do poster e Continue Watching.
- **#439**: Pede limite de legenda de 250% para 300%, sem plataforma indicada. Já lançado: src/ajustes.c:ajustes_leg2_tamanho chega a 300; src/player.c:desenharLegendaPrincipal limita pelo TXT_LEG_PCT_MAX (src/text.h), compartilhado com legenda secundária/preview. 6ad103f3 e merge 2f1e2de1 contidos em v2.0.5; .triagem/release-notes.md explicita #439/#440. Não depende só do comentário de intenção.
- **#440**: Relata texto menor ao ligar Bold; mantenedor identifica face árabe Noto Naskh Bold. src/text.c:corpoNegrito compara altura do glifo/ascensão, arabeBoldFace ajusta a face Bold à regular e fonteNegritoReal faz ajuste das famílias. Causa e compensação explícitas no código, não mera SUSPEITA. 6ad103f3 e 2f1e2de1 em v2.0.5 e nota de release #440 comprovam lançamento; não há prova física nesta triagem.

### Lançadas e fechadas (285)

| # | Título | Plat. | Tipo | Status | Release | Alvo | Conserto | Última resposta | Próximo passo |
|---|---|---|---|---|---|---|---|---|---|
| [#1](https://github.com/iqui27/nuvio-native-legacy/issues/1) | poster dont load on tiles | Samsung (tpk/wgt?) | ? | lancada | 1.0.7 | - | 04c37f87 | nós 09-08 | sem ação; conserto publicado |
| [#2](https://github.com/iqui27/nuvio-native-legacy/issues/2) | Main Nuvio icon not showing in apps list | LG | bug | lancada | 1.0.7 | - | 04c37f87 | nós 09-07 | sem ação; conserto publicado |
| [#3](https://github.com/iqui27/nuvio-native-legacy/issues/3) | Language still showing spanish | ? | ? | lancada | 1.0.7 | - | 04c37f87 | nós 09-07 | sem ação; conserto publicado |
| [#4](https://github.com/iqui27/nuvio-native-legacy/issues/4) | Nuvio Keyboard Help | ? | ? | lancada | 1.0.7 | - | 04c37f87 | autor 09-07 | sem ação; conserto publicado |
| [#5](https://github.com/iqui27/nuvio-native-legacy/issues/5) | Nuvio Sync Issue | ? | bug | lancada | 1.0.7 | - | 04c37f87 | nós 09-07 | sem ação; conserto publicado |
| [#6](https://github.com/iqui27/nuvio-native-legacy/issues/6) | Video Black Screen Issue | ? | bug | lancada | 1.0.7 | - | 04c37f87 | autor 09-22 | sem ação; conserto publicado |
| [#7](https://github.com/iqui27/nuvio-native-legacy/issues/7) | UI Resolution & Torrent Menu Lag | ? | bug | lancada | 1.0.7 | - | 04c37f87 | autor 09-07 | sem ação; conserto publicado |
| [#8](https://github.com/iqui27/nuvio-native-legacy/issues/8) | Profile Switching & Metadata Language Bug | ? | bug | lancada | 1.0.7 | - | 04c37f87 | autor 09-07 | sem ação; conserto publicado |
| [#9](https://github.com/iqui27/nuvio-native-legacy/issues/9) | Missing Subtitle Languages Bug | ? | bug | lancada | 1.0.7 | - | 04c37f87 | autor 09-07 | sem ação; conserto publicado |
| [#10](https://github.com/iqui27/nuvio-native-legacy/issues/10) | Bug: Collection is completely empty on Tizen | Samsung (tpk/wgt?) | bug | lancada | 1.0.7 | - | 04c37f87 | autor 09-07 | sem ação; conserto publicado |
| [#11](https://github.com/iqui27/nuvio-native-legacy/issues/11) | Feedback for v1.0.7: Profile Management Bug, Syncing & Setti | ? | bug | lancada | 1.0.8 | - | 90743102 | nós 09-07 | sem ação; conserto publicado |
| [#12](https://github.com/iqui27/nuvio-native-legacy/issues/12) | Bug: Some UI text is not translated to English on Tizen | Samsung (tpk/wgt?) | bug | lancada | 1.0.10 | - | 7716f148 | autor 09-08 | sem ação; conserto publicado |
| [#13](https://github.com/iqui27/nuvio-native-legacy/issues/13) | Collections Installed from Nuvio Account Do Not Appear on Ti | Samsung (tpk/wgt?) | bug | lancada | 1.0.10 | - | 7716f148 | nós 09-09 | sem ação; conserto publicado |
| [#14](https://github.com/iqui27/nuvio-native-legacy/issues/14) | Bug: Next Episode Does Not Play After Confirming “Play Next” | ? | bug | lancada | 1.0.10 | - | 7716f148 | autor 09-08 | sem ação; conserto publicado |
| [#16](https://github.com/iqui27/nuvio-native-legacy/issues/16) | Bug: Detail Page Flickers and Randomly Opens a Different Tit | ? | bug | lancada | 1.0.11 | - | b28db878 | autor 09-08 | sem ação; conserto publicado |
| [#17](https://github.com/iqui27/nuvio-native-legacy/issues/17) | Bug: Posters Do Not Load When Using Better Posters URL | ? | bug | lancada | 1.0.15 | - | 4723f22f | autor 09-21 | sem ação; conserto publicado |
| [#18](https://github.com/iqui27/nuvio-native-legacy/issues/18) | Bug: Xperience Collections Are Split Into Multiple Home Rows | ? | bug | lancada | 1.0.11 | - | b28db878 | nós 09-14 | sem ação; conserto publicado |
| [#19](https://github.com/iqui27/nuvio-native-legacy/issues/19) | Bug: Random Profile Data Appears Briefly Before My Trakt Pro | ? | bug | lancada | 1.0.11 | - | b28db878 | autor 09-09 | sem ação; conserto publicado |
| [#21](https://github.com/iqui27/nuvio-native-legacy/issues/21) | Backdrop/Hero image takes too long to load after moving focu | ? | ? | lancada | 1.0.24 | - | 2de641c9 | autor 09-09 | sem ação; conserto publicado |
| [#22](https://github.com/iqui27/nuvio-native-legacy/issues/22) | Unable to remove items from Continue Watching | ? | bug | lancada | 1.0.22 | - | 545dd122 | autor 09-09 | sem ação; conserto publicado |
| [#23](https://github.com/iqui27/nuvio-native-legacy/issues/23) | Seeing Portuguese strings in settings and Home Screen | LG | ? | lancada | 1.0.32 | - | 7ef20234 | nós 09-14 | sem ação; conserto publicado |
| [#24](https://github.com/iqui27/nuvio-native-legacy/issues/24) | 🐛 BingeCat Catalogs Not Showing in the Fork | ? | bug | lancada | 1.0.29 | - | 8f1f7a97 | autor 09-09 | sem ação; conserto publicado |
| [#25](https://github.com/iqui27/nuvio-native-legacy/issues/25) | Bug: Continue Watching Does Not Update Until App Restart | ? | bug | lancada | 1.0.30 | - | sem commit | autor 09-09 | nada (fechada) |
| [#26](https://github.com/iqui27/nuvio-native-legacy/issues/26) | Support question WebOS 26 (LG B4 2024) | LG | question | lancada | 1.0.30 | - | de71b7f9 | nós 09-09 | nada (fechada) |
| [#27](https://github.com/iqui27/nuvio-native-legacy/issues/27) | Log debug | ? | bug | lancada | 1.6.5 | - | 46b1d1b7 | nós 09-15 | nada (fechada) |
| [#28](https://github.com/iqui27/nuvio-native-legacy/issues/28) | Viewport resolution | ? | ? | lancada | 1.0.32 | - | 2de0c2ec | autor 09-10 | sem ação; conserto publicado |
| [#29](https://github.com/iqui27/nuvio-native-legacy/issues/29) | Collection Focus GIFs Do Not Play on Tizen | Samsung (tpk/wgt?) | bug | lancada | 1.0.34 | - | f3d40b21 | nós 09-14 | sem ação; conserto publicado |
| [#30](https://github.com/iqui27/nuvio-native-legacy/issues/30) | Connecting Trakt Causes Collections to Disappear From Home | ? | ? | lancada | 1.0.31 | - | bbfc4bd1 | nós 09-10 | sem ação; conserto publicado |
| [#31](https://github.com/iqui27/nuvio-native-legacy/issues/31) | Content Advisory Badges Not Showing Consistently | ? | bug | lancada | 1.0.31 | - | bbfc4bd1 | autor 09-14 | sem ação; conserto publicado |
| [#32](https://github.com/iqui27/nuvio-native-legacy/issues/32) | Bug: “More Like This” menu appears when a movie starts inste | ? | bug | lancada | 1.0.35 | - | sem commit | autor 09-11 | nada (fechada) |
| [#33](https://github.com/iqui27/nuvio-native-legacy/issues/33) | Bug Report: Severe UI stutters / freezes during navigation o | Samsung (tpk/wgt?) | bug | lancada | 1.0.39 | - | 1444a771 | autor 09-11 | sem ação; conserto publicado |
| [#34](https://github.com/iqui27/nuvio-native-legacy/issues/34) | Play next  triggers too soon | ? | ? | lancada | 1.0.37 | - | 0e43c9ad | nós 09-14 | sem ação; conserto publicado |
| [#35](https://github.com/iqui27/nuvio-native-legacy/issues/35) | Episodes listed when clicking a show with multiple seasons. | ? | ? | lancada | 1.0.38 | - | 2d995944 | nós 09-14 | sem ação; conserto publicado |
| [#36](https://github.com/iqui27/nuvio-native-legacy/issues/36) | Remove from continue watching not highlighting | ? | bug | lancada | 1.0.38 | - | 87d986be | nós 09-14 | sem ação; conserto publicado |
| [#37](https://github.com/iqui27/nuvio-native-legacy/issues/37) | Support for New Addon Types | ? | feature | lancada | 1.0.42 | - | d7c0aefa | nós 09-15 | sem ação; conserto publicado |
| [#38](https://github.com/iqui27/nuvio-native-legacy/issues/38) | Continue Watching row does not update or takes a long time t | ? | bug | lancada | 1.0.44 | - | 24f8e451 | nós 09-14 | sem ação; conserto publicado |
| [#39](https://github.com/iqui27/nuvio-native-legacy/issues/39) | Hero/backdrop is delayed | ? | ? | lancada | 1.0.44 | - | 24f8e451 | nós 09-14 | sem ação; conserto publicado |
| [#40](https://github.com/iqui27/nuvio-native-legacy/issues/40) | Play/Pause button not working | Samsung (tpk/wgt?) | bug | lancada | 1.0.43 | - | a3c48b28 | nós 09-15 | sem ação; conserto publicado |
| [#41](https://github.com/iqui27/nuvio-native-legacy/issues/41) | Portuguese text in subtitle menu | ? | ? | lancada | 1.0.43 | - | 1142ee84 | nós 09-15 | sem ação; conserto publicado |
| [#42](https://github.com/iqui27/nuvio-native-legacy/issues/42) | Issues with version 1.0.42 | ? | bug | lancada | 1.0.43 | - | 1142ee84, 28f468ef | nós 09-15 | sem ação; conserto publicado |
| [#43](https://github.com/iqui27/nuvio-native-legacy/issues/43) | Continue watching row. Always sets you to the first episode  | ? | ? | lancada | 1.0.43 | - | 31ad46da | nós 09-15 | sem ação; conserto publicado |
| [#44](https://github.com/iqui27/nuvio-native-legacy/issues/44) | Collections Installed from Nuvio Website Do Not Appear in Ap | Samsung (tpk/wgt?) | bug | lancada | 1.0.44 | - | 24f8e451 | autor 09-14 | sem ação; conserto publicado |
| [#45](https://github.com/iqui27/nuvio-native-legacy/issues/45) | Support for gifs as profile picture | ? | feature | lancada | 1.0.44 | - | 24f8e451 | nós 09-14 | sem ação; conserto publicado |
| [#46](https://github.com/iqui27/nuvio-native-legacy/issues/46) | Start from the beginning | ? | ? | lancada | 1.0.45 | - | 3267613a | nós 09-15 | sem ação; conserto publicado |
| [#48](https://github.com/iqui27/nuvio-native-legacy/issues/48) | Startup log screen on startup | ? | ? | lancada | 1.7.4 | - | e0596c71 | autor 09-15 | sem ação; conserto publicado |
| [#49](https://github.com/iqui27/nuvio-native-legacy/issues/49) | GIF plays briefly then flickers and reverts to a static imag | ? | bug | lancada | 1.0.52 | - | 6efca7e5 | autor 09-16 | sem ação; conserto publicado |
| [#50](https://github.com/iqui27/nuvio-native-legacy/issues/50) | Player loading screen briefly shows the wrong title when swi | ? | bug | lancada | 1.0.51 | - | sem commit | autor 09-15 | nada (fechada) |
| [#51](https://github.com/iqui27/nuvio-native-legacy/issues/51) | Active catalogs/collections appear in the middle or bottom o | ? | ? | lancada | 1.0.51 | - | sem commit | autor 09-15 | nada (fechada) |
| [#52](https://github.com/iqui27/nuvio-native-legacy/issues/52) | Portugues text in integration tab and studio tab | ? | ? | lancada | 1.0.51 | - | sem commit | nós 09-15 | nada (fechada) |
| [#54](https://github.com/iqui27/nuvio-native-legacy/issues/54) | Feature request | ? | feature | lancada | 1.0.53 | - | 78793586 | autor 09-16 | sem ação; conserto publicado |
| [#55](https://github.com/iqui27/nuvio-native-legacy/issues/55) | Bug | ? | bug | lancada | 1.0.55 | - | 758aa5e3 | nós 09-19 | sem ação; conserto publicado |
| [#56](https://github.com/iqui27/nuvio-native-legacy/issues/56) | Feature Request – Remember the selected source between episo | ? | feature | lancada | 1.0.55 | - | 1e6a770e | autor 09-16 | sem ação; conserto publicado |
| [#57](https://github.com/iqui27/nuvio-native-legacy/issues/57) | Resume opens the source panel instead of the previously used | ? | ? | lancada | 1.0.55 | - | 1e6a770e | nós 09-18 | sem ação; conserto publicado |
| [#60](https://github.com/iqui27/nuvio-native-legacy/issues/60) | Trailers of movie under a different one | ? | ? | lancada | 1.2.1 | - | 699e244e | nós 09-19 | sem ação; conserto publicado |
| [#61](https://github.com/iqui27/nuvio-native-legacy/issues/61) | bug | ? | bug | lancada | 1.2.1 | - | 699e244e | nós 09-19 | sem ação; conserto publicado |
| [#62](https://github.com/iqui27/nuvio-native-legacy/issues/62) | bug | ? | bug | lancada | 1.2.1 | - | 699e244e | nós 09-19 | sem ação; conserto publicado |
| [#65](https://github.com/iqui27/nuvio-native-legacy/issues/65) | Application crashes after reaching end of row | ? | bug | lancada | 1.2.1 | - | 699e244e | autor 09-19 | sem ação; conserto publicado |
| [#66](https://github.com/iqui27/nuvio-native-legacy/issues/66) | 'Continue Watching' missing shows from Trakt | ? | bug | lancada | 1.3.0 | - | 8e64134a | nós 09-21 | sem ação; conserto publicado |
| [#67](https://github.com/iqui27/nuvio-native-legacy/issues/67) | Posters not loading | ? | bug | lancada | 1.3.2 | - | 8732c235 | autor 09-21 | sem ação; conserto publicado |
| [#68](https://github.com/iqui27/nuvio-native-legacy/issues/68) | Crashes | ? | bug | lancada | 1.3.2 | - | a8970d44 | autor 09-20 | sem ação; conserto publicado |
| [#69](https://github.com/iqui27/nuvio-native-legacy/issues/69) | Hero/backdrop | ? | bug | lancada | 1.3.2 | - | a8970d44 | nós 09-19 | sem ação; conserto publicado |
| [#70](https://github.com/iqui27/nuvio-native-legacy/issues/70) | Mark as watched issues | ? | bug | lancada | 1.3.2 | - | 701f3db1 | nós 09-19 | sem ação; conserto publicado |
| [#71](https://github.com/iqui27/nuvio-native-legacy/issues/71) | Unable to change value on memory used by images | ? | bug | lancada | 1.3.2 | - | 097ef52f | nós 09-19 | sem ação; conserto publicado |
| [#72](https://github.com/iqui27/nuvio-native-legacy/issues/72) | Navigating now strutting | Samsung (tpk/wgt?) | bug | lancada | 1.3.4 | - | 2e60f9b8, 47c7b45b, 60bcfc02 | nós 09-20 | sem ação; conserto publicado |
| [#73](https://github.com/iqui27/nuvio-native-legacy/issues/73) | 'Up next' appearing before the credits again | ? | bug | lancada | 1.4 | - | 876742e2 | nós 09-22 | sem ação; conserto publicado |
| [#74](https://github.com/iqui27/nuvio-native-legacy/issues/74) | Very low resolution ticks | ? | ? | lancada | 1.3.4 | - | f4e96196 | nós 09-20 | sem ação; conserto publicado |
| [#76](https://github.com/iqui27/nuvio-native-legacy/issues/76) | Build not reading the catalogue correctly | ? | bug | lancada | 1.3.4 | - | 91450a5f | nós 09-20 | sem ação; conserto publicado |
| [#77](https://github.com/iqui27/nuvio-native-legacy/issues/77) | Tyzen os v 1.3.3 - TV got struck | ? | bug | lancada | 1.3.5 | - | 2764aa05 | autor 09-21 | sem ação; conserto publicado |
| [#78](https://github.com/iqui27/nuvio-native-legacy/issues/78) | Unable to scroll through trakt ratings for TV Show | ? | bug | lancada | 1.3.4 | - | 9e388586 | autor 09-20 | sem ação; conserto publicado |
| [#79](https://github.com/iqui27/nuvio-native-legacy/issues/79) | Issue with TV Show seasons | ? | bug | lancada | 1.3.4 | - | 9e388586 | nós 09-20 | sem ação; conserto publicado |
| [#80](https://github.com/iqui27/nuvio-native-legacy/issues/80) | Samsung: Resume opens source list, slow first stream load, a | Samsung (tpk/wgt?) | bug | lancada | 1.3.5 | - | b43ffef0 | autor 09-20 | sem ação; conserto publicado |
| [#82](https://github.com/iqui27/nuvio-native-legacy/issues/82) | Trailer | Samsung (tpk/wgt?) | ? | lancada | 1.3.9 | - | b3b635d4 | nós 09-21 | sem ação; conserto publicado |
| [#83](https://github.com/iqui27/nuvio-native-legacy/issues/83) | Certain addons not showing | ? | bug | lancada | 1.4 | - | 876742e2 | autor 09-22 | sem ação; conserto publicado |
| [#84](https://github.com/iqui27/nuvio-native-legacy/issues/84) | GIF not smooth | ? | bug | lancada | 1.3.8 | - | 2883053e | autor 09-25 | sem ação; conserto publicado |
| [#85](https://github.com/iqui27/nuvio-native-legacy/issues/85) | Minor visual bugs + settings not saved | Samsung (tpk/wgt?) | bug | lancada | 1.3.10 | - | 6f2eb5f7 | autor 09-21 | sem ação; conserto publicado |
| [#86](https://github.com/iqui27/nuvio-native-legacy/issues/86) | Trailer don't play | ? | bug | lancada | 1.3.11 | - | 58d491a7 | autor 09-21 | sem ação; conserto publicado |
| [#87](https://github.com/iqui27/nuvio-native-legacy/issues/87) | IMDB rating not shown on continue watching and on episodes | ? | bug | lancada | 1.3.12 | - | 6542d85e | nós 09-21 | sem ação; conserto publicado |
| [#88](https://github.com/iqui27/nuvio-native-legacy/issues/88) | App working fine now | ? | ? | lancada | 1.4.1 | - | d683d997 | nós 09-23 | sem ação; conserto publicado |
| [#89](https://github.com/iqui27/nuvio-native-legacy/issues/89) | Posters from xperience are cropped (not sized correctly  | ? | bug | lancada | 1.3.12 | - | 9cc8d044 | nós 09-21 | sem ação; conserto publicado |
| [#92](https://github.com/iqui27/nuvio-native-legacy/issues/92) | Anime Embedded subtitles broken | LG | bug | lancada | 1.4.2 | - | 6a61e67e | autor 09-25 | sem ação; conserto publicado |
| [#93](https://github.com/iqui27/nuvio-native-legacy/issues/93) | Autoplay doesnt work on continue watching | ? | bug | lancada | 1.3.12 | - | 8882ce89 | nós 09-21 | sem ação; conserto publicado |
| [#94](https://github.com/iqui27/nuvio-native-legacy/issues/94) | Cast list only shows three cast members | ? | ? | lancada | 1.3.12 | - | ebdf2cd3 | nós 09-22 | sem ação; conserto publicado |
| [#95](https://github.com/iqui27/nuvio-native-legacy/issues/95) | Home catalog rows remember previous tile focus after restart | ? | bug | lancada | 1.4 | - | 876742e2 | nós 09-24 | sem ação; conserto publicado |
| [#97](https://github.com/iqui27/nuvio-native-legacy/issues/97) | All english is in portugues | ? | ? | lancada | 1.3.12 | - | 2b94b895 | autor 09-21 | sem ação; conserto publicado |
| [#99](https://github.com/iqui27/nuvio-native-legacy/issues/99) | Possibility of having LG magic mouse cursor support? | LG | question | lancada | 1.4.2 | - | 5c4e7135, c8eb88dc, cc990b4e | autor 09-23 | sem ação; conserto publicado |
| [#100](https://github.com/iqui27/nuvio-native-legacy/issues/100) | Episodes not being marked as watched when completed | LG | bug | lancada | 1.4 | - | 876742e2 | autor 09-23 | sem ação; conserto publicado |
| [#101](https://github.com/iqui27/nuvio-native-legacy/issues/101) | Playing an episode sometimes loads sources of previous watch | LG | bug | lancada | 1.4 | - | 876742e2 | autor 09-23 | sem ação; conserto publicado |
| [#102](https://github.com/iqui27/nuvio-native-legacy/issues/102) | Episode carousel auto scrolls to first episode of the season | LG | bug | lancada | 1.4 | - | 876742e2 | autor 09-23 | sem ação; conserto publicado |
| [#103](https://github.com/iqui27/nuvio-native-legacy/issues/103) | 🐛 Catalog row: Last focused tile gets clipped at the right e | ? | ? | lancada | 1.4 | - | 876742e2 | autor 09-22 | sem ação; conserto publicado |
| [#104](https://github.com/iqui27/nuvio-native-legacy/issues/104) | Theme colors | ? | feature | lancada | 1.4.2 | - | 6a61e67e, e7453e91 | autor 09-23 | sem ação; conserto publicado |
| [#105](https://github.com/iqui27/nuvio-native-legacy/issues/105) | Bug: Auto-selected source can get stuck loading indefinitely | ? | bug | lancada | 1.4.2 | - | 6a61e67e | autor 09-23 | sem ação; conserto publicado |
| [#106](https://github.com/iqui27/nuvio-native-legacy/issues/106) | Catalog order doesn't persist between app updates | ? | bug | lancada | 1.4.2 | - | 6a3f3a05, 6a61e67e | nós 09-23 | sem ação; conserto publicado |
| [#108](https://github.com/iqui27/nuvio-native-legacy/issues/108) | Request. Pressing 'Season' brings up option to mark all as w | ? | feature | lancada | 1.4.2 | - | sem commit | nós 09-23 | nada (fechada) |
| [#109](https://github.com/iqui27/nuvio-native-legacy/issues/109) | Media player focus | ? | ? | lancada | 1.4.1 | - | d683d997 | autor 09-23 | sem ação; conserto publicado |
| [#110](https://github.com/iqui27/nuvio-native-legacy/issues/110) | SIMKL no Option for continue list | ? | feature | lancada | 1.4.2 | - | a00f2e23, ae51d9b0 | nós 09-23 | sem ação; conserto publicado |
| [#111](https://github.com/iqui27/nuvio-native-legacy/issues/111) | Movie starts with black screen | LG | bug | lancada | 1.4.2 | - | 6a3f3a05 | autor 09-24 | sem ação; conserto publicado |
| [#112](https://github.com/iqui27/nuvio-native-legacy/issues/112) | iptv channel don't work in samsung | Samsung (tpk/wgt?) | bug | lancada | 1.4.2 | - | 1c9f7c59 | nós 09-23 | sem ação; conserto publicado |
| [#113](https://github.com/iqui27/nuvio-native-legacy/issues/113) | Diagnostic | Samsung (tpk/wgt?) | ? | lancada | 1.4.3 | - | 10e4c3c0, 9cacb9c5 | autor 09-23 | sem ação; conserto publicado |
| [#114](https://github.com/iqui27/nuvio-native-legacy/issues/114) | Artwork / Backdrop Loading Regression in v1.4.2 | Samsung (tpk/wgt?) | ? | lancada | 1.4.3 | - | eedf7c81 | autor 09-23 | sem ação; conserto publicado |
| [#115](https://github.com/iqui27/nuvio-native-legacy/issues/115) | More like this appears too early  | ? | bug | lancada | 1.4.6 | - | 89eab64c, 8d36a1f2 | nós 09-24 | sem ação; conserto publicado |
| [#116](https://github.com/iqui27/nuvio-native-legacy/issues/116) | icons appear to be glitched or incorrect | ? | ? | lancada | 1.4.3 | - | 0fde2217 | nós 09-23 | sem ação; conserto publicado |
| [#117](https://github.com/iqui27/nuvio-native-legacy/issues/117) | App appears to be slower in newer version  | ? | bug | lancada | 1.4.3 | - | sem commit | nós 09-23 | nada (fechada) |
| [#118](https://github.com/iqui27/nuvio-native-legacy/issues/118) | incorrect hero artwork in continue watching | ? | bug | lancada | 1.4.3 | - | 10e4c3c0, a994a0fc | nós 09-23 | sem ação; conserto publicado |
| [#119](https://github.com/iqui27/nuvio-native-legacy/issues/119) | Update screen: scroll problem and no update button | Samsung (tpk/wgt?) | bug | lancada | 1.4.6 | - | sem commit | autor 09-25 | nada (fechada) |
| [#120](https://github.com/iqui27/nuvio-native-legacy/issues/120) | Send log screen popup on every start | Samsung (tpk/wgt?) | bug | lancada | 1.4.3 | - | a8eb6296, d1427b55 | autor 09-24 | sem ação; conserto publicado |
| [#121](https://github.com/iqui27/nuvio-native-legacy/issues/121) | Rewind, forward focus problem | Samsung (tpk/wgt?) | bug | lancada | 1.4.3 | - | 9cc535e8, d1427b55 | autor 09-24 | sem ação; conserto publicado |
| [#122](https://github.com/iqui27/nuvio-native-legacy/issues/122) | Embedded subtitles isn't displayed | Samsung (tpk/wgt?) | bug | lancada | 1.4.3 | - | d1427b55, f4f05699 | autor 09-24 | sem ação; conserto publicado |
| [#123](https://github.com/iqui27/nuvio-native-legacy/issues/123) | TV Shows / Series — Trailer Support | ? | feature | lancada | 1.4.3 | - | 9293bf5b, bd405a92 | nós 09-24 | sem ação; conserto publicado |
| [#124](https://github.com/iqui27/nuvio-native-legacy/issues/124) | Trailer Playback in Hero / Backdrop | Samsung (tpk/wgt?) | feature | lancada | 1.5.2 | - | 72e3378f | nós 09-27 | sem ação; conserto publicado |
| [#125](https://github.com/iqui27/nuvio-native-legacy/issues/125) | Metadata sync delay | ? | ? | lancada | 1.4.4 | - | sem commit | nós 09-24 | nada (fechada) |
| [#126](https://github.com/iqui27/nuvio-native-legacy/issues/126) | Missing catalogs | ? | bug | lancada | 1.4.6 | - | 32a83118, 8874e60f | autor 09-24 | sem ação; conserto publicado |
| [#127](https://github.com/iqui27/nuvio-native-legacy/issues/127) | No separate upcoming row | ? | bug | lancada | 1.4.7 | - | 563ccc81 | nós 09-27 | sem ação; conserto publicado |
| [#128](https://github.com/iqui27/nuvio-native-legacy/issues/128) | (QOL) Hide ui except for progress bar while seeking | ? | ? | lancada | 1.4.5 | - | 6cce1253 | nós 09-24 | sem ação; conserto publicado |
| [#129](https://github.com/iqui27/nuvio-native-legacy/issues/129) | Subtitle/audio language doesnt save | ? | bug | lancada | 1.4.4 | - | 0e6f0da6, 725a0cbc, 94c59a3e | nós 09-24 | sem ação; conserto publicado |
| [#130](https://github.com/iqui27/nuvio-native-legacy/issues/130) | Source selection loading multiple files before playing | ? | ? | lancada | 1.4.4 | - | 7f66bfd3, 89c77368 | nós 09-24 | sem ação; conserto publicado |
| [#131](https://github.com/iqui27/nuvio-native-legacy/issues/131) | Number of Titles limited to 205 | ? | bug | lancada | 1.4.6 | - | sem commit | nós 09-24 | nada (fechada) |
| [#132](https://github.com/iqui27/nuvio-native-legacy/issues/132) | Only 1 source is listed | Samsung (tpk/wgt?) | bug | lancada | 1.4.6 | - | 52316a09, cd440320 | autor 09-27 | nada (fechada) |
| [#133](https://github.com/iqui27/nuvio-native-legacy/issues/133) | Poster unwatched blur effect doesn't work | Samsung (tpk/wgt?) | bug | lancada | 1.4.6 | - | 91ceeeea, f5a0017c | autor 09-25 | sem ação; conserto publicado |
| [#134](https://github.com/iqui27/nuvio-native-legacy/issues/134) | Please support plugin | ? | feature | lancada | 2.0.0 | - | release | nós 10-06 | sem ação; conserto publicado |
| [#136](https://github.com/iqui27/nuvio-native-legacy/issues/136) | Trailer playback issues by source on Samsung AU7000 | Samsung (tpk/wgt?) | bug | lancada | 1.4.6 | - | ea872c3b | autor 09-25 | sem ação; conserto publicado |
| [#137](https://github.com/iqui27/nuvio-native-legacy/issues/137) | Samsung: help test a native (.tpk) Nuvio — 2 minutes, TVs fr | Samsung (tpk/wgt?) | ? | lancada | 1.5.4 | - | 266dce13, 2f444a50, 4b2d3956 | nós 09-30 | sem ação; conserto publicado |
| [#138](https://github.com/iqui27/nuvio-native-legacy/issues/138) | Samsung 2018/2019 (Tizen 4/5): experimental build available, | ? | ? | lancada | 1.6.0 | - | release | nós 09-30 | sem ação; conserto publicado |
| [#141](https://github.com/iqui27/nuvio-native-legacy/issues/141) | Some catalog GIFs not playing | ? | bug | lancada | 1.5.0 | - | 0a315f7f, 68521ca4 | autor 09-28 | sem ação; conserto publicado |
| [#142](https://github.com/iqui27/nuvio-native-legacy/issues/142) | Possibility of manually selecting hero artwork | ? | feature | lancada | 1.5.0 | - | 86c5a1c9 | autor 09-25 | sem ação; conserto publicado |
| [#144](https://github.com/iqui27/nuvio-native-legacy/issues/144) | Align UI with nuvioTV (android) | Android | feature | lancada | 1.5.2 | - | 876ddc79, 98160e65 | nós 10-11 | sem ação; conserto publicado |
| [#145](https://github.com/iqui27/nuvio-native-legacy/issues/145) | The image only uses a small part of the window on the upper  | LG | bug | lancada | 1.6.0 | - | c79995b6 | nós 09-30 | sem ação; conserto publicado |
| [#146](https://github.com/iqui27/nuvio-native-legacy/issues/146) | Add more languages | Android | feature | lancada | 1.6.0 | - | release | nós 09-30 | sem ação; conserto publicado |
| [#147](https://github.com/iqui27/nuvio-native-legacy/issues/147) | Remote Button Input Delay — Volume and Home Buttons | Samsung (tpk/wgt?) | bug | lancada | 1.5.2 | - | 378127d9, dc18b5e1 | nós 09-30 | sem ação; conserto publicado |
| [#149](https://github.com/iqui27/nuvio-native-legacy/issues/149) | Send logs automatically doesn't stay activated | ? | bug | lancada | 1.5.1 | - | 400dd41d, ea0a1373 | nós 09-27 | sem ação; conserto publicado |
| [#150](https://github.com/iqui27/nuvio-native-legacy/issues/150) | Episode Descriptions Remain in English Despite Portuguese La | ? | ? | lancada | 1.5.1 | - | 035c538a | nós 09-27 | sem ação; conserto publicado |
| [#151](https://github.com/iqui27/nuvio-native-legacy/issues/151) | The next episode doesn't play automatically / disappears fro | Samsung (tpk/wgt?) | bug | lancada | 1.5.3 | - | b274752b | autor 09-28 | sem ação; conserto publicado |
| [#153](https://github.com/iqui27/nuvio-native-legacy/issues/153) | Cast artwork does not match names of actors  | ? | bug | lancada | 1.5.1 | - | e3ae1533 | nós 09-27 | sem ação; conserto publicado |
| [#156](https://github.com/iqui27/nuvio-native-legacy/issues/156) | Long single line subtitles overflow as ellipsis instead of w | LG | ? | lancada | 1.5.1 | - | 96f5abe4 | nós 09-27 | sem ação; conserto publicado |
| [#158](https://github.com/iqui27/nuvio-native-legacy/issues/158) | Live TV (Xtream): channels don't play and guide is empty (LG | LG | bug | lancada | 1.5.2 | - | 9a50dd55 | nós 10-06 | sem ação; conserto publicado |
| [#159](https://github.com/iqui27/nuvio-native-legacy/issues/159) | Source panel lags while textures are loading | ? | bug | lancada | 1.5.2 | - | 821b39f0 | autor 09-27 | sem ação; conserto publicado |
| [#160](https://github.com/iqui27/nuvio-native-legacy/issues/160) | Hero catalogs | ? | ? | lancada | 1.5.2 | - | ebf1a825 | nós 09-27 | sem ação; conserto publicado |
| [#162](https://github.com/iqui27/nuvio-native-legacy/issues/162) | Allow removal of certain options from sidebar | ? | feature | lancada | 1.5.2 | - | d4037a42 | nós 09-27 | sem ação; conserto publicado |
| [#163](https://github.com/iqui27/nuvio-native-legacy/issues/163) | Number of catalog options shown | ? | feature | lancada | 1.5.2 | - | 426b0296, 6b05521d | nós 09-27 | sem ação; conserto publicado |
| [#164](https://github.com/iqui27/nuvio-native-legacy/issues/164) | Hero "Loading artwork…" placeholder makes focus transitions  | ? | ? | lancada | 1.5.2 | - | cda85061 | nós 09-27 | sem ação; conserto publicado |
| [#165](https://github.com/iqui27/nuvio-native-legacy/issues/165) | native app works prefectly | Samsung (tpk/wgt?) | ? | lancada | 1.5.4 | - | c4e4d922 | autor 09-29 | sem ação; conserto publicado |
| [#169](https://github.com/iqui27/nuvio-native-legacy/issues/169) | Can P2P content play on LG? No option to enable it | LG | question | lancada | 1.6.0 | - | release | autor 10-05 | sem ação; conserto publicado |
| [#170](https://github.com/iqui27/nuvio-native-legacy/issues/170) | The Tyzen os Native app (tpk - experimental 1.5.2) not openi | Samsung (tpk/wgt?) | bug | lancada | 1.5.4 | - | release | autor 09-30 | sem ação; conserto publicado |
| [#171](https://github.com/iqui27/nuvio-native-legacy/issues/171) | P2P addons (Torrentio) missing from sources but shown in the | Samsung (tpk/wgt?) | question | lancada | 2.0.0 | - | 775f9db9 | nós 09-30 | sem ação; conserto publicado |
| [#172](https://github.com/iqui27/nuvio-native-legacy/issues/172) | UI: port ideas from the Corby7 fork | ? | feature | lancada | 1.6.0 | - | 414cd61e, 4c2a6951, 65e3c8db | nós 09-29 | sem ação; conserto publicado |
| [#173](https://github.com/iqui27/nuvio-native-legacy/issues/173) | "Update now" not appearing on version 1.5.2 | ? | bug | lancada | 1.5.3 | - | sem commit | autor 09-30 | nada (fechada) |
| [#174](https://github.com/iqui27/nuvio-native-legacy/issues/174) | Episodes missing of some Tv series | Samsung (tpk/wgt?) | bug | lancada | 1.6.0 | - | 582bc2e6 | autor 09-30 | sem ação; conserto publicado |
| [#175](https://github.com/iqui27/nuvio-native-legacy/issues/175) | Allow users to choose different Search sources | ? | feature | lancada | 1.6.0 | - | 582bc2e6 | nós 09-30 | sem ação; conserto publicado |
| [#176](https://github.com/iqui27/nuvio-native-legacy/issues/176) | LG G5 (webOS 10) feedback: languages, half-size video with 4 | LG | bug | lancada | 1.6.0 | - | 3e90ae7f, 6def852c, a6c318ef | autor 09-30 | sem ação; conserto publicado |
| [#177](https://github.com/iqui27/nuvio-native-legacy/issues/177) | Next episode thumbnail isn't blurred | ? | bug | lancada | 1.5.4 | - | 340e5791, 3f834522 | autor 10-03 | sem ação; conserto publicado |
| [#178](https://github.com/iqui27/nuvio-native-legacy/issues/178) | Trailer zoom options + fullscreen audio issues | Samsung (tpk/wgt?) | bug | lancada | 1.5.4 | - | 1c2de6b7, b904ea23 | autor 09-30 | sem ação; conserto publicado |
| [#179](https://github.com/iqui27/nuvio-native-legacy/issues/179) | Trakt playback scrobbling not working (Now Watching, progres | LG | bug | lancada | 1.5.4 | - | 3ae82b79, 3f834522 | nós 09-30 | sem ação; conserto publicado |
| [#180](https://github.com/iqui27/nuvio-native-legacy/issues/180) | Tizen 4/5 native: app loads but exits during sign-in | Samsung (tpk/wgt?) | bug | lancada | 1.5.4 | - | 6f581853, c121f7d1, dde1abd2 | nós 09-30 | sem ação; conserto publicado |
| [#181](https://github.com/iqui27/nuvio-native-legacy/issues/181) | Native self-update via memfd (download new libnuvio.so, no r | Samsung (tpk/wgt?) | feature | lancada | 1.5.4 | - | ae0ba290 | nós 09-30 | sem ação; conserto publicado |
| [#182](https://github.com/iqui27/nuvio-native-legacy/issues/182) | Addons not loading sometimes in LG web os 26 | LG | bug | lancada | 1.5.4 | - | 3f834522, abc5408e | autor 09-30 | sem ação; conserto publicado |
| [#184](https://github.com/iqui27/nuvio-native-legacy/issues/184) | Auto-update on native samsung tizen not working | Samsung .tpk | bug | lancada | 1.6.1 | - | 755a6dad | nós 10-01 | sem ação; conserto publicado |
| [#185](https://github.com/iqui27/nuvio-native-legacy/issues/185) | colored line bug | Samsung .tpk | bug | lancada | 1.6.1 | - | release | autor 09-30 | sem ação; conserto publicado |
| [#186](https://github.com/iqui27/nuvio-native-legacy/issues/186) | Stream Badges are B&W | LG | bug | lancada | 1.6.1 | - | 53d01871 | sem comentários | sem ação; conserto publicado |
| [#187](https://github.com/iqui27/nuvio-native-legacy/issues/187) | The Guide feature does not open a title | LG | bug | lancada | 1.6.1 | - | 3951e747, 5aebadf3 | nós 10-01 | sem ação; conserto publicado |
| [#188](https://github.com/iqui27/nuvio-native-legacy/issues/188) | Video playback bug | Samsung .tpk | bug | lancada | 1.6.3 | - | 5b0e4957 | nós 09-30 | sem ação; conserto publicado |
| [#190](https://github.com/iqui27/nuvio-native-legacy/issues/190) | Incorrect Title and Epsiode List | LG | bug | lancada | 1.6.2 | - | 000015d7 | autor 10-02 | sem ação; conserto publicado |
| [#191](https://github.com/iqui27/nuvio-native-legacy/issues/191) | 1.6.1 and 1.6.0 (.wgt) is extremely lagging | Samsung .wgt | bug | lancada | 1.6.2 | - | 2b99d09b | nós 10-01 | sem ação; conserto publicado |
| [#193](https://github.com/iqui27/nuvio-native-legacy/issues/193) | Black screen when exit from app | Samsung .tpk | bug | lancada | 1.7.0 | - | release | autor 10-02 | sem ação; conserto publicado |
| [#194](https://github.com/iqui27/nuvio-native-legacy/issues/194) | TMDB Collections Not Working | LG | bug | lancada | 1.6.2 | - | 34db0d1a | autor 09-30 | sem ação; conserto publicado |
| [#195](https://github.com/iqui27/nuvio-native-legacy/issues/195) | Tpk 1.6.1 - trailer autoplay bug | Samsung (tpk/wgt?) | bug | lancada | 1.6.3 | - | 5b0e4957, 8316a07f | autor 10-02 | sem ação; conserto publicado |
| [#196](https://github.com/iqui27/nuvio-native-legacy/issues/196) | Not Available toast message upon pressing Play/Pause button | Samsung .tpk | bug | lancada | 1.6.4 | - | release | autor 10-01 | sem ação; conserto publicado |
| [#197](https://github.com/iqui27/nuvio-native-legacy/issues/197) | Catalogue Not showing | Samsung .tpk | bug | lancada | 1.6.4 | - | b98ae904, f5813122 | nós 10-01 | sem ação; conserto publicado |
| [#198](https://github.com/iqui27/nuvio-native-legacy/issues/198) | Stream Badge | LG | feature | lancada | 1.6.5 | - | 46b1d1b7, f26efc21 | autor 10-01 | sem ação; conserto publicado |
| [#199](https://github.com/iqui27/nuvio-native-legacy/issues/199) | Continue watching is not showing unaired up next | LG | bug | lancada | 1.6.5 | - | 149f1235 | autor 10-08 | sem ação; conserto publicado |
| [#200](https://github.com/iqui27/nuvio-native-legacy/issues/200) | Custom posters not working correctly | LG | bug | lancada | 1.6.5 | - | f0735c7b | autor 10-02 | sem ação; conserto publicado |
| [#201](https://github.com/iqui27/nuvio-native-legacy/issues/201) |  Top 10 view inconsistent with automatic view | ? | bug | lancada | 1.7.0 | - | aa43af06, b3991145, d93ea5a8 | autor 10-02 | sem ação; conserto publicado |
| [#202](https://github.com/iqui27/nuvio-native-legacy/issues/202) | Matching accent colour in settings obscures text | LG | bug | lancada | 1.7.0 | - | c522e1f2 | nós 10-02 | sem ação; conserto publicado |
| [#203](https://github.com/iqui27/nuvio-native-legacy/issues/203) | Menu bar remain visible during play | Samsung .tpk | bug | lancada | 1.7.0 | - | 29342682 | autor 10-02 | sem ação; conserto publicado |
| [#204](https://github.com/iqui27/nuvio-native-legacy/issues/204) | Automatic Trailer and LG Smart Magic Not Working | LG | bug | lancada | 1.7.0 | - | 1865cfa2, dcf0d8d7 | nós 10-02 | sem ação; conserto publicado |
| [#205](https://github.com/iqui27/nuvio-native-legacy/issues/205) | Disappearing "Continue Watching" items and appearing "Resume | Samsung .tpk | bug | lancada | 1.7.0 | - | dc9ca3bf | autor 10-02 | sem ação; conserto publicado |
| [#206](https://github.com/iqui27/nuvio-native-legacy/issues/206) | Embedded subtitle and audio language list display during pla | Samsung .tpk | bug | lancada | 1.7.0 | - | 94690ba3 | autor 10-02 | sem ação; conserto publicado |
| [#208](https://github.com/iqui27/nuvio-native-legacy/issues/208) | Couldn't resume a movie from the tile | Samsung .tpk | bug | lancada | 1.7.0 | - | 39e4c794 | autor 10-02 | sem ação; conserto publicado |
| [#209](https://github.com/iqui27/nuvio-native-legacy/issues/209) | Series Titles and Descriptions Displayed in English Despite  | LG | bug | lancada | 1.7.0 | - | 0da72c2a | nós 10-02 | sem ação; conserto publicado |
| [#210](https://github.com/iqui27/nuvio-native-legacy/issues/210) | Username stucks on collapsed 'Modern sidebar' | Samsung .tpk | bug | lancada | 1.7.0 | - | 3e203fcf | autor 10-02 | sem ação; conserto publicado |
| [#211](https://github.com/iqui27/nuvio-native-legacy/issues/211) | LG UK6540PSB Crashes on startup. | LG | bug | lancada | 2.0.2 | - | 937f8e6b | nós 10-02 | sem ação; conserto publicado |
| [#212](https://github.com/iqui27/nuvio-native-legacy/issues/212) | Not being able to tell if I watched something or not. | LG | bug | lancada | 1.7.1 | - | 24a14ed0, 5f5174ad, 7a5896fa | autor 10-03 | sem ação; conserto publicado |
| [#213](https://github.com/iqui27/nuvio-native-legacy/issues/213) | Continue watching shows incorrect titles | LG | bug | lancada | 1.7.1 | - | 28064cae, 5658d0b0, 7a5896fa | autor 10-03 | sem ação; conserto publicado |
| [#214](https://github.com/iqui27/nuvio-native-legacy/issues/214) | no carga el QR para iniciar sesión | LG | bug | lancada | 1.7.1 | - | release | nós 10-02 | sem ação; conserto publicado |
| [#215](https://github.com/iqui27/nuvio-native-legacy/issues/215) | 1.7 introduced bugs on LG | LG | bug | lancada | 1.7.1 | - | 0ad0b807, 98ba4ac6, ce575c14 | nós 10-02 | sem ação; conserto publicado |
| [#216](https://github.com/iqui27/nuvio-native-legacy/issues/216) | Addition of touch controls | all | feature | lancada | 1.7.2 | - | 7924f0eb, 80bb3dd2, 941ac3f0 | autor 10-03 | sem ação; conserto publicado |
| [#221](https://github.com/iqui27/nuvio-native-legacy/issues/221) | Sources take too long to fetch links | Samsung .tpk | bug | lancada | 1.7.2 | - | 1e4cfa8e, 98607591, e012f141 | autor 10-03 | sem ação; conserto publicado |
| [#222](https://github.com/iqui27/nuvio-native-legacy/issues/222) | Discord Rich Presence Integration | all | feature | lancada | 1.7.4 | - | 1ea1510b | autor 10-03 | sem ação; conserto publicado |
| [#223](https://github.com/iqui27/nuvio-native-legacy/issues/223) | [Android TV] Blank / black screen on launch after "Preparing | ? | bug | lancada | 1.7.2 | - | 105e759b | nós 10-06 | sem ação; conserto publicado |
| [#224](https://github.com/iqui27/nuvio-native-legacy/issues/224) | WebOS crashing | LG | bug | lancada | 1.7.2 | - | 67401c73 | nós 10-03 | sem ação; conserto publicado |
| [#225](https://github.com/iqui27/nuvio-native-legacy/issues/225) | App does not launch after updating | LG | bug | lancada | 1.7.2 | - | 67401c73 | nós 10-03 | sem ação; conserto publicado |
| [#226](https://github.com/iqui27/nuvio-native-legacy/issues/226) | alt/old icon | ? | feature | lancada | 2.0.1 | - | release | autor 10-07 | sem ação; conserto publicado |
| [#227](https://github.com/iqui27/nuvio-native-legacy/issues/227) | Adding localized movie/series title somewhere | LG | feature | lancada | 1.7.4 | - | release | nós 10-03 | sem ação; conserto publicado |
| [#228](https://github.com/iqui27/nuvio-native-legacy/issues/228) | Auto trailer doesn't work | Samsung .tpk | bug | lancada | 2.0.0 | - | 1e03fc30 | autor 10-06 | sem ação; conserto publicado |
| [#229](https://github.com/iqui27/nuvio-native-legacy/issues/229) | Different IMDB score | Samsung .tpk | bug | lancada | 1.7.4 | - | release | autor 10-03 | sem ação; conserto publicado |
| [#231](https://github.com/iqui27/nuvio-native-legacy/issues/231) | Option to disable Cinemata | Samsung (tpk/wgt?) | feature | lancada | 2.0.0 | - | d6495798 | nós 10-06 | sem ação; conserto publicado |
| [#232](https://github.com/iqui27/nuvio-native-legacy/issues/232) | Next episode thumbnail isn’t blurred | ? | ? | lancada | 1.7.4 | - | 2495e39e | nós 10-06 | sem ação; conserto publicado |
| [#233](https://github.com/iqui27/nuvio-native-legacy/issues/233) | Nuvio account isn't synced by app | Samsung .tpk | bug | lancada | 2.0.0 | - | 3b625f58 | nós 10-06 | sem ação; conserto publicado |
| [#234](https://github.com/iqui27/nuvio-native-legacy/issues/234) | Customization options for app icon/splash screen, trailer UI | LG | feature | lancada | 2.0.0 | - | 652b759d, b8a900f5 | nós 10-06 | sem ação; conserto publicado |
| [#235](https://github.com/iqui27/nuvio-native-legacy/issues/235) | Seeking | ? | ? | lancada | 2.0.0 | - | 652b759d, b8a900f5 | nós 10-06 | sem ação; conserto publicado |
| [#237](https://github.com/iqui27/nuvio-native-legacy/issues/237) | Missing forward slash ('/') in TV channel source input (Stal | ? | bug | lancada | 2.0.0 | - | 8f8be06a, e5a848c3 | nós 10-06 | sem ação; conserto publicado |
| [#238](https://github.com/iqui27/nuvio-native-legacy/issues/238) | “Wait for add-ons” in playback is blocked by “depth effect” | ? | ? | lancada | 2.0.0 | - | 73f93686, d6495798, ebaef8ad | nós 10-06 | sem ação; conserto publicado |
| [#239](https://github.com/iqui27/nuvio-native-legacy/issues/239) | Arabic Subtitle Letters Appear Disconnected | LG | bug | lancada | 2.0.0 | - | 6bb186ac, d00870f0 | nós 10-06 | sem ação; conserto publicado |
| [#241](https://github.com/iqui27/nuvio-native-legacy/issues/241) | Trailer black bar are back | Samsung .tpk | bug | lancada | 2.0.0 | - | 50ba9279, 50df3f42, 9a0cce0f | autor 10-06 | sem ação; conserto publicado |
| [#243](https://github.com/iqui27/nuvio-native-legacy/issues/243) | IMDb rating missing on Continue Watching and a placeholder " | Samsung .tpk | bug | lancada | 2.0.0 | - | 8e7f4098, ccfe01a8 | nós 10-06 | sem ação; conserto publicado |
| [#244](https://github.com/iqui27/nuvio-native-legacy/issues/244) | Continue Watching: the same episode repeats 5 times and mark | Samsung .tpk | bug | lancada | 2.0.0 | - | 8e7f4098, d6495798 | nós 10-06 | sem ação; conserto publicado |
| [#245](https://github.com/iqui27/nuvio-native-legacy/issues/245) | Arabic subtitles not shaped/RTL — letters appear disconnecte | LG | bug | lancada | 2.0.0 | - | 6bb186ac, d00870f0 | nós 10-06 | sem ação; conserto publicado |
| [#247](https://github.com/iqui27/nuvio-native-legacy/issues/247) | Arabic subtitle | LG | bug | lancada | 2.0.0 | - | 6bb186ac, a1cf2e46, d00870f0 | nós 10-06 | sem ação; conserto publicado |
| [#249](https://github.com/iqui27/nuvio-native-legacy/issues/249) | [Feature]: Keep the current video full-size when the “Up Nex | ? | feature | lancada | 2.0.0 | - | release | autor 10-06 | sem ação; conserto publicado |
| [#253](https://github.com/iqui27/nuvio-native-legacy/issues/253) | Arabic subtitles/UI rendering as squares on Samsung Smart TV | Samsung .tpk | bug | lancada | 2.0.0 | - | a1cf2e46 | nós 10-06 | sem ação; conserto publicado |
| [#255](https://github.com/iqui27/nuvio-native-legacy/issues/255) | Home screen only displays 5 collections | Samsung .tpk | bug | lancada | 2.0.1 | - | 40bc9180 | nós 10-06 | sem ação; conserto publicado |
| [#256](https://github.com/iqui27/nuvio-native-legacy/issues/256) | Add Autoplay for Next Episode & Allow Customizable "Up Next" | ? | feature | lancada | 2.0.5 | - | 11791eda | autor 10-08 | sem ação; conserto publicado |
| [#258](https://github.com/iqui27/nuvio-native-legacy/issues/258) | Arabic subtitles not working | Samsung .tpk | bug | lancada | 2.0.0 | - | a1cf2e46 | nós 10-06 | sem ação; conserto publicado |
| [#261](https://github.com/iqui27/nuvio-native-legacy/issues/261) | Arabic Subtitles doesnt workining. | LG | bug | lancada | 2.0.0 | - | a1cf2e46 | nós 10-06 | sem ação; conserto publicado |
| [#262](https://github.com/iqui27/nuvio-native-legacy/issues/262) | Subtitle delay slider | ? | feature | lancada | 2.0.0 | - | release | nós 10-06 | sem ação; conserto publicado |
| [#263](https://github.com/iqui27/nuvio-native-legacy/issues/263) | Add an option to toggle between 12-hour and 24-hour clock fo | Samsung (tpk/wgt?) | feature | lancada | 2.0.0 | - | 08a904d1 | nós 10-06 | sem ação; conserto publicado |
| [#265](https://github.com/iqui27/nuvio-native-legacy/issues/265) | LG Smart TV – Freezing, Stuttering | LG | bug | lancada | 2.0.1 | - | release | autor 10-07 | sem ação; conserto publicado |
| [#266](https://github.com/iqui27/nuvio-native-legacy/issues/266) | Bug: Doesn't open on ATv | Android | bug | lancada | 2.0.2 | - | a1d5e039 | autor 10-08 | dono: avaliar fechamento com v2.0.2 |
| [#269](https://github.com/iqui27/nuvio-native-legacy/issues/269) | Embedded subtitles are not being detected or not being displ | Samsung .tpk | bug | lancada | 2.0.1 | - | 34a1eae6, 47a3d55d, ce1552cd | nós 10-10 | sem ação; conserto publicado |
| [#271](https://github.com/iqui27/nuvio-native-legacy/issues/271) | Intermittent App Crash When Navigating Settings | LG | bug | lancada | 2.0.1 | - | release | autor 10-07 | sem ação; conserto publicado |
| [#272](https://github.com/iqui27/nuvio-native-legacy/issues/272) | Alternate App Icon Selection Does Not Change App Icon | LG | bug | lancada | 2.0.1 | - | sem commit | autor 10-06 | nada (fechada) |
| [#274](https://github.com/iqui27/nuvio-native-legacy/issues/274) | Arabic Language, TMDb Metadata, and Logos/Backdrops Issues | Samsung .tpk | bug | lancada | 2.0.1 | - | release | autor 10-07 | sem ação; conserto publicado |
| [#275](https://github.com/iqui27/nuvio-native-legacy/issues/275) | Data Saving Option (Max/Min File Size Limit for Playback) | Samsung (tpk/wgt?) | feature | lancada | 2.0.1 | - | release | nós 10-06 | sem ação; conserto publicado |
| [#276](https://github.com/iqui27/nuvio-native-legacy/issues/276) | Integrating QuickJS Engine into TPK Build to Enable External | Samsung (tpk/wgt?) | feature | lancada | 2.0.0 | - | release | nós 10-06 | sem ação; conserto publicado |
| [#277](https://github.com/iqui27/nuvio-native-legacy/issues/277) | Addon not showing | LG | bug | lancada | 2.0.1 | - | release | nós 10-06 | sem ação; conserto publicado |
| [#278](https://github.com/iqui27/nuvio-native-legacy/issues/278) | Arabic subtitle symbols not shown | LG | bug | lancada | 2.0.1 | - | sem commit | autor 10-07 | nada (fechada) |
| [#281](https://github.com/iqui27/nuvio-native-legacy/issues/281) | Auto-trailer doesn’t have sound on Tizen 9 | Samsung .tpk | bug | lancada | 2.0.1 | - | 3076c3a5 | nós 10-06 | sem ação; conserto publicado |
| [#282](https://github.com/iqui27/nuvio-native-legacy/issues/282) | Collection images/AIO regex patterns | Samsung .tpk | bug | lancada | 2.0.1 | - | sem commit | autor 10-06 | nada (fechada) |
| [#283](https://github.com/iqui27/nuvio-native-legacy/issues/283) | Minha tv lg nao consegue abrir nenhum canal dos meus addons | LG | bug | lancada | 2.0.2 | - | b68bad96 | nós 10-11 | dono: avaliar fechamento com v2.0.2 |
| [#284](https://github.com/iqui27/nuvio-native-legacy/issues/284) | Source result ("Best for this tv") missing resolution | ? | feature | lancada | 2.0.3 | - | a63ec982, df492356 | autor 10-10 | sem ação; conserto publicado |
| [#286](https://github.com/iqui27/nuvio-native-legacy/issues/286) | Performance on older Samsung UA40N5300 | Samsung .tpk | bug | lancada | 2.0.2 | - | e03080dd | nós 10-08 | dono: avaliar fechamento com v2.0.2 |
| [#287](https://github.com/iqui27/nuvio-native-legacy/issues/287) | Auto select forced embedded sub | ? | feature | lancada | 2.0.2 | - | 0702a143 | autor 10-07 | sem ação; conserto publicado |
| [#289](https://github.com/iqui27/nuvio-native-legacy/issues/289) | Eliminate “OK” button on profile pin entry | all | feature | lancada | 2.0.2 | - | 3eb5aef5 | nós 10-07 | sem ação; conserto publicado |
| [#290](https://github.com/iqui27/nuvio-native-legacy/issues/290) | Few minor issues on 2.1 | ? | bug | lancada | 2.0.2 | - | 4843e5b0, 66f19578, 75cee76f | nós 10-11 | sem ação; conserto publicado |
| [#293](https://github.com/iqui27/nuvio-native-legacy/issues/293) | Audio codec details on player | all | feature | lancada | 2.0.2 | - | db1f5891 | nós 10-11 | dono: avaliar fechamento com v2.0.2 |
| [#294](https://github.com/iqui27/nuvio-native-legacy/issues/294) | 🐛 Home collection order resets after switching profiles | Samsung .tpk | bug | lancada | 2.0.2 | - | b4186acd | nós 10-10 | sem ação; conserto publicado |
| [#295](https://github.com/iqui27/nuvio-native-legacy/issues/295) | Profile picker background — “Profile art” option not working | Samsung .tpk | bug | lancada | 2.0.2 | - | 881f7cae | nós 10-07 | sem ação; conserto publicado |
| [#296](https://github.com/iqui27/nuvio-native-legacy/issues/296) | Si podrías agregar estas visitas seria grandioso | Samsung .wgt | feature | lancada | 2.0.2 | - | release | nós 10-11 | sem ação; conserto publicado |
| [#297](https://github.com/iqui27/nuvio-native-legacy/issues/297) |  Issue: P2P stream stops due to full TV storage. | Samsung .tpk | bug | lancada | 2.0.2 | - | 1b6a5439, 325f4e2b | nós 10-07 | sem ação; conserto publicado |
| [#298](https://github.com/iqui27/nuvio-native-legacy/issues/298) | The Arabic subtitles | Samsung (Tizen 6) | feature | lancada | 2.0.2 | - | release | nós 10-11 | sem ação; conserto publicado |
| [#300](https://github.com/iqui27/nuvio-native-legacy/issues/300) | Default Aspect Ratio Option | all | feature | lancada | 2.0.2 | - | release | nós 10-11 | sem ação; conserto publicado |
| [#303](https://github.com/iqui27/nuvio-native-legacy/issues/303) | how to remove continue watching from opening screen | ? | feature | lancada | 2.0.2 | - | 1300a834 | nós 10-11 | sem ação; conserto publicado |
| [#305](https://github.com/iqui27/nuvio-native-legacy/issues/305) | Hide player ui when pressing up | LG | feature | lancada | 2.0.3 | - | 98bee79f | nós 10-10 | sem ação; conserto publicado |
| [#308](https://github.com/iqui27/nuvio-native-legacy/issues/308) | [bug] subtitles block source | Samsung .tpk | bug | lancada | 2.0.3 | - | 19ef3af8, 4151683c, 6269f306 | nós 10-09 | sem ação; conserto publicado |
| [#310](https://github.com/iqui27/nuvio-native-legacy/issues/310) | [port] regex/options for autoplay | ? | feature | lancada | 2.0.2 | - | release | nós 10-11 | dono: avaliar fechamento com v2.0.2 |
| [#311](https://github.com/iqui27/nuvio-native-legacy/issues/311) | [suggestion] option to disable "from nuvio search" | ? | feature | lancada | 2.0.2 | - | release | autor 10-10 | sem ação; conserto publicado |
| [#312](https://github.com/iqui27/nuvio-native-legacy/issues/312) | few minor bugs, none affect use | LG | bug | lancada | 2.0.3 | - | 317cb179 | nós 10-10 | sem ação; conserto publicado |
| [#317](https://github.com/iqui27/nuvio-native-legacy/issues/317) | App doesn't  open on my LG webOS Tv UK6550PSB | LG | bug | lancada | 2.0.2 | - | 937f8e6b | nós 10-09 | sem ação; conserto publicado |
| [#318](https://github.com/iqui27/nuvio-native-legacy/issues/318) | screen flickers and stops responding when pressing OK (andro | Android | bug | lancada | 2.0.2 | - | 7e96e7e7 | autor 10-09 | sem ação; conserto publicado |
| [#319](https://github.com/iqui27/nuvio-native-legacy/issues/319) | Home row still not updating properly | Samsung .tpk | bug | lancada | 2.0.2 | - | c796ca7d | nós 10-10 | sem ação; conserto publicado |
| [#320](https://github.com/iqui27/nuvio-native-legacy/issues/320) | Arabic language in Subtitle shows no glyph font | Samsung .tpk | bug | lancada | 2.0.3 | - | 22ecac46, 8ab3b78f | nós 10-10 | sem ação; conserto publicado |
| [#321](https://github.com/iqui27/nuvio-native-legacy/issues/321) | The "not started" label does not shift downwards. | Samsung .tpk | bug | lancada | 2.0.3 | - | b347004c | autor 10-09 | sem ação; conserto publicado |
| [#322](https://github.com/iqui27/nuvio-native-legacy/issues/322) | Can't delete from Continue watching on the home screen | Samsung .tpk | bug | lancada | 2.0.3 | - | b8d110de | autor 10-09 | sem ação; conserto publicado |
| [#323](https://github.com/iqui27/nuvio-native-legacy/issues/323) | App crashes on playback | Android | bug | lancada | 2.0.3 | - | 1b6c9a89 | nós 10-10 | sem ação; conserto publicado |
| [#326](https://github.com/iqui27/nuvio-native-legacy/issues/326) | Add continue watching options | Android | feature | lancada | 2.0.5 | - | 1cc512dd, 56868756, 95fc93c5 | nós 10-11 | dono: avaliar fechamento com v2.0.5 |
| [#327](https://github.com/iqui27/nuvio-native-legacy/issues/327) | Hero doesn’t working / Catalog bug | Samsung .wgt | bug | lancada | 2.0.3 | - | 8ab1b19b, d268b069 | autor 10-07 | sem ação; conserto publicado |
| [#328](https://github.com/iqui27/nuvio-native-legacy/issues/328) | 🐛 Bug Report: Episodes Are Being Duplicated | Samsung .tpk | bug | lancada | 2.0.3 | - | 7edab4b0, 9c5d0027 | nós 10-11 | dono: avaliar fechamento com v2.0.3 |
| [#329](https://github.com/iqui27/nuvio-native-legacy/issues/329) | Add skip intro, recaps and credits. | Android | feature | lancada | 2.0.5 | - | 4193781f, 95fc93c5, a0a392fb | sem comentários | dono: avaliar fechamento com v2.0.5 |
| [#330](https://github.com/iqui27/nuvio-native-legacy/issues/330) | [Bug]extremely laggy after repeated playback | Samsung .tpk | bug | lancada | 2.0.3 | - | 2f047898 | sem comentários | sem ação; conserto publicado |
| [#331](https://github.com/iqui27/nuvio-native-legacy/issues/331) | 💡 Feature Request: Netflix-Style Next Episode Countdown | ? | feature | lancada | 2.0.5 | - | 11791eda, 4193781f, 4c17e680 | sem comentários | dono: avaliar fechamento com v2.0.5 |
| [#332](https://github.com/iqui27/nuvio-native-legacy/issues/332) | Nuvio didn't finish opening | Android | bug | lancada | 2.0.3 | - | 35421d1a, eabe3afd | nós 10-10 | sem ação; conserto publicado |
| [#334](https://github.com/iqui27/nuvio-native-legacy/issues/334) | P2P filled the TV's free space" error when playing large/4K  | Samsung .tpk | bug | lancada | 2.0.3 | - | c3e19d00, d3ca720b | nós 10-11 | dono: avaliar fechamento com v2.0.3 |
| [#335](https://github.com/iqui27/nuvio-native-legacy/issues/335) | Extend Arabic subtitle size scale (200%-250%), ASS support,  | Samsung (tpk/wgt?) | feature | lancada | 2.0.3 | - | 01016036, 3b93dfad, 503ed5cc | nós 10-10 | sem ação; conserto publicado |
| [#338](https://github.com/iqui27/nuvio-native-legacy/issues/338) | Add Stats for Nerds and Custom Buffer Size | all | feature | lancada | 2.0.5 | - | 74f781f3, 8c856535, 95fc93c5 | nós 10-11 | dono: avaliar fechamento com v2.0.5 |
| [#339](https://github.com/iqui27/nuvio-native-legacy/issues/339) | Pantalla de ajustes | ? | feature | lancada | 2.0.3 | - | 30e13fa4, 7dc8cd71, b1b53000 | nós 10-10 | sem ação; conserto publicado |
| [#340](https://github.com/iqui27/nuvio-native-legacy/issues/340) | Quicker Seek/skipping | all | feature | lancada | 2.0.3 | - | bc63d8ca | nós 10-10 | sem ação; conserto publicado |
| [#341](https://github.com/iqui27/nuvio-native-legacy/issues/341) | Aspect Ratio / Crop feature to fill screen does not work on  | Samsung .tpk | bug | lancada | 2.0.3 | - | release | sem comentários | dono: avaliar fechamento com v2.0.3 |
| [#350](https://github.com/iqui27/nuvio-native-legacy/issues/350) | Major bug | Samsung .tpk | bug | lancada | 2.0.3 | - | 4b8ba24b | autor 10-08 | dono: avaliar fechamento com v2.0.3 |
| [#352](https://github.com/iqui27/nuvio-native-legacy/issues/352) | [Feature Request] Library Watched Indicators, Watched/Unwatc | ? | feature | lancada | 2.0.5 | - | 402e2438, d4d72639, fab062b3 | nós 10-08 | dono: avaliar fechamento com v2.0.5 |
| [#355](https://github.com/iqui27/nuvio-native-legacy/issues/355) | Option to view full/untruncated text and metadata for addon  | ? | feature | lancada | 2.0.5 | - | ac055d54, b176eff5, c8dbfda5 | autor 10-08 | dono: avaliar fechamento com v2.0.5 |
| [#356](https://github.com/iqui27/nuvio-native-legacy/issues/356) | Upcoming shows not appearing in continue watching | ? | bug | lancada | 2.0.3 | - | d47a78c8 | autor 10-10 | dono: avaliar fechamento com v2.0.3 |
| [#358](https://github.com/iqui27/nuvio-native-legacy/issues/358) | 🐛 Bug Report: Some Add-on Catalogs Are Incorrectly Moved to  | Samsung .tpk | bug | lancada | 2.0.5 | - | 29c640b7, c4edb7c6 | autor 10-08 | dono: avaliar fechamento com v2.0.5 |
| [#359](https://github.com/iqui27/nuvio-native-legacy/issues/359) | 🐛 Bug Report: Floating Sidebar Overlaps/Interferes With Cont | Samsung .tpk | bug | lancada | 2.0.3 | - | 1860216b | autor 10-09 | sem ação; conserto publicado |
| [#360](https://github.com/iqui27/nuvio-native-legacy/issues/360) | Sync addons | ? | feature | lancada | 2.0.3 | - | 46507ea5 | autor 10-11 | sem ação; conserto publicado |
| [#361](https://github.com/iqui27/nuvio-native-legacy/issues/361) | Poster URL Max Character Length Too Short | all | feature | lancada | 2.0.3 | - | 2220b019, 9ac3531d | nós 10-10 | sem ação; conserto publicado |
| [#362](https://github.com/iqui27/nuvio-native-legacy/issues/362) | Collection View Options | ? | feature | lancada | 2.0.5 | - | 22320582, 491fcee3 | autor 10-09 | dono: avaliar fechamento com v2.0.5 |
| [#363](https://github.com/iqui27/nuvio-native-legacy/issues/363) | Unable to Send Recommendation to a Friend | LG | bug | lancada | 2.0.3 | - | 54cf2fab, 9bb93887 | nós 10-10 | sem ação; conserto publicado |
| [#364](https://github.com/iqui27/nuvio-native-legacy/issues/364) | Sources unable to open | Samsung .tpk | bug | lancada | 2.0.5 | - | 8f2c1701, bf666aa7, d78963ab | sem comentários | dono: avaliar fechamento com v2.0.5 |
| [#368](https://github.com/iqui27/nuvio-native-legacy/issues/368) | Tab enhancement | ? | feature | lancada | 2.0.3 | - | dbec4f44 | nós 10-10 | sem ação; conserto publicado |
| [#369](https://github.com/iqui27/nuvio-native-legacy/issues/369) | [Bug] Multiple issues/Missing Features on Android TV version | Android | bug | lancada | 2.0.3 | - | 5f04c36b, 712557ca | nós 10-11 | dono: avaliar fechamento com v2.0.3 |
| [#370](https://github.com/iqui27/nuvio-native-legacy/issues/370) | Subtitles sync issues | Samsung .wgt | bug | lancada | 2.0.3 | - | 2aee231b, 56c9832e | nós 10-10 | sem ação; conserto publicado |
| [#371](https://github.com/iqui27/nuvio-native-legacy/issues/371) | [bug] Ui issue viewing "profile & stats" | Samsung .tpk | bug | lancada | 2.0.3 | - | 2fa6e51a, b39f4149 | autor 10-08 | sem ação; conserto publicado |
| [#383](https://github.com/iqui27/nuvio-native-legacy/issues/383) | [port] subtitle memory same as nuvio | all | feature | lancada | 2.0.5 | - | 3654b88e | autor 10-12 | dono: avaliar fechamento com v2.0.5 |
| [#384](https://github.com/iqui27/nuvio-native-legacy/issues/384) | Embedded ASS subtitles stopped rendering after anime intro | LG | bug | lancada | 2.0.3 | - | 3e7bbb62, 4151683c, 9ee093ac | autor 10-09 | sem ação; conserto publicado |
| [#385](https://github.com/iqui27/nuvio-native-legacy/issues/385) | Internet Connection Drop and Reconnecting Bug to no end | Samsung .wgt | bug | lancada | 2.0.3 | - | 4151683c, 9ee093ac | autor 10-09 | dono: avaliar fechamento com v2.0.3 |
| [#387](https://github.com/iqui27/nuvio-native-legacy/issues/387) | [bug] instant long press in library | Samsung .wgt | bug | lancada | 2.0.5 | - | 03213598, 0f98379f, c82fa422 | autor 10-12 | dono: avaliar fechamento com v2.0.5 |
| [#427](https://github.com/iqui27/nuvio-native-legacy/issues/427) | Memory for images capped at 160 MB | LG | bug | lancada | 2.0.5 | - | 92ee6141 | nós 10-12 | dono: avaliar fechamento com v2.0.5 |

Notas:

- **#1**: Triagem 2026-10-12: 04c37f87 em v1.0.7.
- **#2**: Triagem 2026-10-12: 04c37f87 em v1.0.7.
- **#3**: Triagem 2026-10-12: 04c37f87 em v1.0.7.
- **#4**: Triagem 2026-10-12: 04c37f87 em v1.0.7.
- **#5**: Triagem 2026-10-12: 04c37f87 em v1.0.7.
- **#6**: Triagem 2026-10-12: 04c37f87 em v1.0.7.
- **#7**: Triagem 2026-10-12: 04c37f87 em v1.0.7.
- **#8**: Triagem 2026-10-12: 04c37f87 em v1.0.7.
- **#9**: Triagem 2026-10-12: 04c37f87 em v1.0.7.
- **#10**: Triagem 2026-10-12: 04c37f87 em v1.0.7.
- **#11**: Triagem 2026-10-12: 90743102 em v1.0.8.
- **#12**: Triagem 2026-10-12: 7716f148 em v1.0.10.
- **#13**: Triagem 2026-10-12: 7716f148 em v1.0.10.
- **#14**: Triagem 2026-10-12: 7716f148 em v1.0.10.
- **#16**: Triagem 2026-10-12: b28db878 em v1.0.11.
- **#17**: Triagem 2026-10-12: 4723f22f em v1.0.15.
- **#18**: Triagem 2026-10-12: b28db878 em v1.0.11.
- **#19**: Triagem 2026-10-12: b28db878 em v1.0.11.
- **#21**: Triagem 2026-10-12: 2de641c9 em v1.0.24.
- **#22**: Triagem 2026-10-12: 545dd122 em v1.0.22.
- **#23**: Triagem 2026-10-12: 7ef20234 em v1.0.32.
- **#24**: Triagem 2026-10-12: 8f1f7a97 em v1.0.29.
- **#25**: Sem commit #25; versão 1.0.30 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#26**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#27**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#28**: Triagem 2026-10-12: 2de0c2ec em v1.0.32.
- **#29**: Triagem 2026-10-12: f3d40b21 em v1.0.34.
- **#30**: Triagem 2026-10-12: bbfc4bd1 em v1.0.31.
- **#31**: Triagem 2026-10-12: bbfc4bd1 em v1.0.31.
- **#32**: Sem commit #32; versão 1.0.35 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#33**: Triagem 2026-10-12: 1444a771 em v1.0.39.
- **#34**: Triagem 2026-10-12: 0e43c9ad em v1.0.37.
- **#35**: Triagem 2026-10-12: 2d995944 em v1.0.38.
- **#36**: Triagem 2026-10-12: 87d986be em v1.0.38.
- **#37**: Triagem 2026-10-12: d7c0aefa em v1.0.42.
- **#38**: Triagem 2026-10-12: 24f8e451 em v1.0.44.
- **#39**: Triagem 2026-10-12: 24f8e451 em v1.0.44.
- **#40**: Triagem 2026-10-12: a3c48b28 em v1.0.43.
- **#41**: Triagem 2026-10-12: 1142ee84 em v1.0.43.
- **#42**: Triagem 2026-10-12: 1142ee84 em v1.0.43; 28f468ef em v1.0.43.
- **#43**: Triagem 2026-10-12: 31ad46da em v1.0.43.
- **#44**: Triagem 2026-10-12: 24f8e451 em v1.0.44.
- **#45**: Triagem 2026-10-12: 24f8e451 em v1.0.44.
- **#46**: Triagem 2026-10-12: 3267613a em v1.0.45.
- **#48**: Triagem 2026-10-12: e0596c71 em v1.7.4.
- **#49**: Triagem 2026-10-12: 6efca7e5 em v1.0.52.
- **#50**: Sem commit #50; versão 1.0.51 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#51**: Sem commit #51; versão 1.0.51 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#52**: Sem commit #52; versão 1.0.51 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#54**: Triagem 2026-10-12: 78793586 em v1.0.53.
- **#55**: Triagem 2026-10-12: 758aa5e3 em v1.0.55.
- **#56**: Triagem 2026-10-12: 1e6a770e em v1.0.55.
- **#57**: Triagem 2026-10-12: 1e6a770e em v1.0.55.
- **#60**: Triagem 2026-10-12: 699e244e em v1.2.1.
- **#61**: Triagem 2026-10-12: 699e244e em v1.2.1.
- **#62**: Triagem 2026-10-12: 699e244e em v1.2.1.
- **#65**: Triagem 2026-10-12: 699e244e em v1.2.1.
- **#66**: Triagem 2026-10-12: 8e64134a em v1.3.0.
- **#67**: Triagem 2026-10-12: 8732c235 em v1.3.2.
- **#68**: Triagem 2026-10-12: a8970d44 em v1.3.2.
- **#69**: Triagem 2026-10-12: a8970d44 em v1.3.2.
- **#70**: Triagem 2026-10-12: 701f3db1 em v1.3.2.
- **#71**: Triagem 2026-10-12: 097ef52f em v1.3.2.
- **#72**: Triagem 2026-10-12: 2e60f9b8 em v1.3.4; 47c7b45b em v1.3.4; 60bcfc02 em v1.3.4.
- **#73**: Triagem 2026-10-12: 876742e2 em v1.4.
- **#74**: Triagem 2026-10-12: f4e96196 em v1.3.4.
- **#76**: Triagem 2026-10-12: 91450a5f em v1.3.4.
- **#77**: Triagem 2026-10-12: 2764aa05 em v1.3.5.
- **#78**: Triagem 2026-10-12: 9e388586 em v1.3.4.
- **#79**: Triagem 2026-10-12: 9e388586 em v1.3.4.
- **#80**: Triagem 2026-10-12: b43ffef0 em v1.3.5.
- **#82**: Triagem 2026-10-12: b3b635d4 em v1.3.9.
- **#83**: Triagem 2026-10-12: 876742e2 em v1.4.
- **#84**: Triagem 2026-10-12: 2883053e em v1.3.8.
- **#85**: Triagem 2026-10-12: 6f2eb5f7 em v1.3.10.
- **#86**: Triagem 2026-10-12: 58d491a7 em v1.3.11.
- **#87**: Triagem 2026-10-12: 6542d85e em v1.3.12.
- **#88**: Triagem 2026-10-12: d683d997 em v1.4.1.
- **#89**: Triagem 2026-10-12: 9cc8d044 em v1.3.12.
- **#92**: Triagem 2026-10-12: 6a61e67e em v1.4.2.
- **#93**: Triagem 2026-10-12: 8882ce89 em v1.3.12.
- **#94**: Triagem 2026-10-12: ebdf2cd3 em v1.3.12.
- **#95**: Triagem 2026-10-12: 876742e2 em v1.4.
- **#97**: Triagem 2026-10-12: 2b94b895 em v1.3.12.
- **#99**: Triagem 2026-10-12: 5c4e7135 em v1.4.2; c8eb88dc em v1.4.2; cc990b4e em v1.4.2.
- **#100**: Triagem 2026-10-12: 876742e2 em v1.4.
- **#101**: Triagem 2026-10-12: 876742e2 em v1.4.
- **#102**: Triagem 2026-10-12: 876742e2 em v1.4.
- **#103**: Triagem 2026-10-12: 876742e2 em v1.4.
- **#104**: Triagem 2026-10-12: 6a61e67e em v1.4.2; e7453e91 em v1.4.2.
- **#105**: Triagem 2026-10-12: 6a61e67e em v1.4.2.
- **#106**: Triagem 2026-10-12: 6a3f3a05 em v1.4.2; 6a61e67e em v1.4.2.
- **#108**: Sem commit #108; versão 1.4.2 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#109**: Triagem 2026-10-12: d683d997 em v1.4.1.
- **#110**: Triagem 2026-10-12: a00f2e23 em v1.4.2; ae51d9b0 em v1.4.2.
- **#111**: Triagem 2026-10-12: 6a3f3a05 em v1.4.2.
- **#112**: Triagem 2026-10-12: 1c9f7c59 em v1.4.2.
- **#113**: Triagem 2026-10-12: 10e4c3c0 em v1.4.3; 9cacb9c5 em v1.4.3.
- **#114**: Triagem 2026-10-12: eedf7c81 em v1.4.3.
- **#115**: Triagem 2026-10-12: 89eab64c em v1.4.6; 8d36a1f2 em v1.4.6.
- **#116**: Triagem 2026-10-12: 0fde2217 em v1.4.3.
- **#117**: Sem commit #117; versão 1.4.3 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#118**: Triagem 2026-10-12: 10e4c3c0 em v1.4.3; a994a0fc em v1.4.3.
- **#119**: Sem commit #119; versão 1.4.6 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#120**: Triagem 2026-10-12: a8eb6296 em v1.4.3; d1427b55 em v1.4.3.
- **#121**: Triagem 2026-10-12: 9cc535e8 em v1.4.3; d1427b55 em v1.4.3.
- **#122**: Triagem 2026-10-12: d1427b55 em v1.4.3; f4f05699 em v1.4.3.
- **#123**: Triagem 2026-10-12: 9293bf5b em v1.4.3; bd405a92 em v1.4.3.
- **#124**: Triagem 2026-10-12: 72e3378f em v1.5.2.
- **#125**: Sem commit #125; versão 1.4.4 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#126**: Triagem 2026-10-12: 32a83118 em v1.4.6; 8874e60f em v1.4.6.
- **#127**: Triagem 2026-10-12: 563ccc81 em v1.4.7.
- **#128**: Triagem 2026-10-12: 6cce1253 em v1.4.5.
- **#129**: Triagem 2026-10-12: 0e6f0da6 em v1.4.4; 725a0cbc em v1.4.4; 94c59a3e em v1.4.4.
- **#130**: Triagem 2026-10-12: 7f66bfd3 em v1.4.4; 89c77368 em v1.4.4.
- **#131**: Sem commit #131; versão 1.4.6 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#132**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#133**: Triagem 2026-10-12: 91ceeeea em v1.4.6; f5a0017c em v1.4.6.
- **#134**: Triagem 2026-10-12: release-notes.md v2.0.0: - **Plugins and P2P (experimental, off by default).** Nuvio scraper repositories as an extra source, and a built-in engine for torrents you pick by hand..
- **#136**: Triagem 2026-10-12: ea872c3b em v1.4.6.
- **#137**: Triagem 2026-10-12: 266dce13 em v1.5.4; 2f444a50 em v1.5.4; 4b2d3956 em v1.5.4.
- **#138**: Triagem 2026-10-12: release-notes.md v1.6.0: | Samsung 2018–2020 | Tizen 4.0 / 5.0 / 5.5 | `Nuvio-1.6.0-NuvioTpk40.tpk` (native) |.
- **#141**: Triagem 2026-10-12: 0a315f7f em v1.5.0; 68521ca4 em v1.5.0.
- **#142**: Triagem 2026-10-12: 86c5a1c9 em v1.5.0.
- **#144**: Triagem 2026-10-12: 876ddc79 em v1.5.2; 98160e65 em v1.5.2.
- **#145**: Triagem 2026-10-12: c79995b6 em v1.6.0.
- **#146**: Triagem 2026-10-12: release-notes.md v1.6.0: - **28 new languages**, and the interface language can follow your account, then the TV. Settings › Appearance..
- **#147**: Triagem 2026-10-12: 378127d9 em v1.5.2; dc18b5e1 em v1.5.2.
- **#149**: Triagem 2026-10-12: 400dd41d em v1.5.1; ea0a1373 em v1.5.1.
- **#150**: Triagem 2026-10-12: 035c538a em v1.5.1.
- **#151**: Triagem 2026-10-12: b274752b em v1.5.3.
- **#153**: Triagem 2026-10-12: e3ae1533 em v1.5.1.
- **#156**: Triagem 2026-10-12: 96f5abe4 em v1.5.1.
- **#158**: Triagem 2026-10-12: 9a50dd55 em v1.5.2.
- **#159**: Triagem 2026-10-12: 821b39f0 em v1.5.2.
- **#160**: Triagem 2026-10-12: ebf1a825 em v1.5.2.
- **#162**: Triagem 2026-10-12: d4037a42 em v1.5.2.
- **#163**: Triagem 2026-10-12: 426b0296 em v1.5.2; 6b05521d em v1.5.2.
- **#164**: Triagem 2026-10-12: cda85061 em v1.5.2.
- **#165**: Triagem 2026-10-12: c4e4d922 em v1.5.4.
- **#169**: Triagem 2026-10-12: release-notes.md v1.6.0: - **Sources**: AllDebrid, experimental P2P through a Stremio streaming server on your network, and a per-add-on speed test..
- **#170**: Triagem 2026-10-12: release-notes.md v1.5.4: - **Tizen 8/9**: the native app now opens from the TV's app menu, not only from Apps2Samsung (#170)..
- **#171**: Triagem 2026-10-12: 775f9db9 em v2.0.0.
- **#172**: Triagem 2026-10-12: 414cd61e em v1.6.0; 4c2a6951 em v1.6.0; 65e3c8db em v1.6.0.
- **#173**: Sem commit #173; versão 1.5.3 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#174**: Triagem 2026-10-12: 582bc2e6 em v1.6.0.
- **#175**: Triagem 2026-10-12: 582bc2e6 em v1.6.0.
- **#176**: Triagem 2026-10-12: 3e90ae7f em v1.6.0; 6def852c em v1.6.0; a6c318ef em v1.6.0.
- **#177**: Triagem 2026-10-12: 340e5791 em v1.5.4; 3f834522 em v1.5.4; release-notes.md v1.5.4: - **Next-episode card** (#177): the thumbnail respects "blur unwatched episodes"..
- **#178**: Triagem 2026-10-12: 1c2de6b7 em v1.5.4; b904ea23 em v1.5.4; release-notes.md v1.5.4: - **Trailers with sound (Samsung native)**: fullscreen trailers try IMDb first, so Apple TV and Netflix movie trailers have audio (#178)..
- **#179**: Triagem 2026-10-12: 3ae82b79 em v1.5.4; 3f834522 em v1.5.4; release-notes.md v1.5.4: - **Trakt "Now Watching"** (#179): Nuvio now tells Trakt when you start and pause, so progress and watched history sync like the official app..
- **#180**: Triagem 2026-10-12: 6f581853 em v1.5.4; c121f7d1 em v1.5.4; dde1abd2 em v1.5.4.
- **#181**: Triagem 2026-10-12: ae0ba290 em v1.5.4.
- **#182**: Triagem 2026-10-12: 3f834522 em v1.5.4; abc5408e em v1.5.4; release-notes.md v1.5.4: - **Addons not showing until reload** (#182): a slow addon (e.g. AIOStreams) is asked once more before the list appears..
- **#184**: Triagem 2026-10-12: 755a6dad em v1.6.1; release-notes.md v1.6.1: - **Samsung native** (#184): after an update downloads, the card offers "Restart now"..
- **#185**: Triagem 2026-10-12: release-notes.md v1.6.1: - **Video edge** (#185): the video area lines up with the picture to the pixel..
- **#186**: Triagem 2026-10-12: 53d01871 em v1.6.1; release-notes.md v1.6.1: - **Symbols as boxes** (#186): the check mark in the audio/subtitle panel and symbols in source names no longer show as empty squares with LG Display or Droid Sans..
- **#187**: Triagem 2026-10-12: 3951e747 em v1.6.1; 5aebadf3 em v1.6.1; release-notes.md v1.6.1: - **Explore** (#187): stars open again when the TMDB integration is off, and holding OK no longer loops. The map follows your metadata language..
- **#188**: Triagem 2026-10-12: 5b0e4957 em v1.6.3; release-notes.md v1.6.3: - **Samsung native: home screen instead of video** (#188, #195): trailers and movies could show the Samsung home screen behind Nuvio, with only the sound playing. The trailer zoom sent the TV a picture area larger than the screen; it's off on the native app for now, and the picture only opens once the video is really playing. The native app for 2021 and newer also keeps its window opaque, so this is a new .tpk worth installing (the self-update brings only the first part)..
- **#190**: Triagem 2026-10-12: 000015d7 em v1.6.2; release-notes.md v1.6.2: - **Wrong show in the player** (#190): after Continue watching refreshed, the pause screen and episode list could show another title..
- **#191**: Triagem 2026-10-12: 2b99d09b em v1.6.2; release-notes.md v1.6.2: - **Samsung .wgt lag** (#191): text was being measured again every frame since 1.6.0; the web version is back to 1.5.3 speed..
- **#193**: Triagem 2026-10-12: release-notes.md v1.7.0: - **Samsung native: video behind the TV menu or a black screen** (#195, #203, #193)..
- **#194**: Triagem 2026-10-12: 34db0d1a em v1.6.2; release-notes.md v1.6.2: - **Movie collections** (#194): films that belong to a saga now show a Collection section with the other parts..
- **#195**: Triagem 2026-10-12: 5b0e4957 em v1.6.3; 8316a07f em v1.6.3; release-notes.md v1.6.3: - **Samsung native: home screen instead of video** (#188, #195): trailers and movies could show the Samsung home screen behind Nuvio, with only the sound playing. The trailer zoom sent the TV a picture area larger than the screen; it's off on the native app for now, and the picture only opens once the video is really playing. The native app for 2021 and newer also keeps its window opaque, so this is a new .tpk worth installing (the self-update brings only the first part)..
- **#196**: Triagem 2026-10-12: release-notes.md v1.6.4: - **Samsung native, "Not Available" on media keys** (#196)..
- **#197**: Triagem 2026-10-12: b98ae904 em v1.6.4; f5813122 em v1.6.4; release-notes.md v1.6.4: - **Home rows** (#197): pressing an arrow repeatedly at the row limit no longer hides rows, and rows damaged by it are repaired once on start..
- **#198**: Triagem 2026-10-12: 46b1d1b7 em v1.6.5; f26efc21 em v1.6.5; release-notes.md v1.6.5: - **Colored stream badges** (#198): resolution, source, HDR/DV, audio, codec and services each get their own color, like the Nuvio badge pack. Settings › Playback › Colored badges..
- **#199**: Triagem 2026-10-12: 149f1235 em v1.6.5; release-notes.md v1.6.5: - **Up next from your account** (#199): without Trakt or Simkl, Continue watching now lists the next episode of the shows you watch, including ones not aired yet with their date (follows "Show unaired episodes"). The full watch history is synced, not just the first 900 entries..
- **#200**: Triagem 2026-10-12: f0735c7b em v1.6.5; release-notes.md v1.6.5: - **Custom posters from AIOMetadata** (#200): BetterPosters/PostersPlus show on every title, not just some..
- **#201**: Triagem 2026-10-12: aa43af06 em v1.7.0; b3991145 em v1.7.0; d93ea5a8 em v1.7.0.
- **#202**: Triagem 2026-10-12: c522e1f2 em v1.7.0; release-notes.md v1.7.0: - **Smaller fixes**: first row style under the Spotlight (#201), accent text in Settings (#202), MKV track names (#206), custom poster language (#200), LG trailers and pointer (#204), collapsed sidebar name (#210)..
- **#203**: Triagem 2026-10-12: 29342682 em v1.7.0.
- **#204**: Triagem 2026-10-12: 1865cfa2 em v1.7.0; dcf0d8d7 em v1.7.0.
- **#205**: Triagem 2026-10-12: dc9ca3bf em v1.7.0; release-notes.md v1.7.0: - **Resume from a row tile** (#208) and **disappearing Continue watching items** (#205)..
- **#206**: Triagem 2026-10-12: 94690ba3 em v1.7.0; release-notes.md v1.7.0: - **Smaller fixes**: first row style under the Spotlight (#201), accent text in Settings (#202), MKV track names (#206), custom poster language (#200), LG trailers and pointer (#204), collapsed sidebar name (#210)..
- **#208**: Triagem 2026-10-12: 39e4c794 em v1.7.0; release-notes.md v1.7.0: - **Resume from a row tile** (#208) and **disappearing Continue watching items** (#205)..
- **#209**: Triagem 2026-10-12: 0da72c2a em v1.7.0; release-notes.md v1.7.0: - **Series in English with another language set** (#209)..
- **#210**: Triagem 2026-10-12: 3e203fcf em v1.7.0; release-notes.md v1.7.0: - **Smaller fixes**: first row style under the Spotlight (#201), accent text in Settings (#202), MKV track names (#206), custom poster language (#200), LG trailers and pointer (#204), collapsed sidebar name (#210)..
- **#211**: Triagem 2026-10-12: 937f8e6b em v2.0.2.
- **#212**: Triagem 2026-10-12: 24a14ed0 em v1.7.1; 5f5174ad em v1.7.1; 7a5896fa em v1.7.1.
- **#213**: Triagem 2026-10-12: 28064cae em v1.7.1; 5658d0b0 em v1.7.1; 7a5896fa em v1.7.1.
- **#214**: Triagem 2026-10-12: release-notes.md v1.7.1: - **Account outages** (#214, #215): temporary server errors preserve your session and the last successful account data for the current profile. Incomplete replies cannot replace a complete saved library..
- **#215**: Triagem 2026-10-12: 0ad0b807 em v1.7.1; 98ba4ac6 em v1.7.1; ce575c14 em v1.7.1.
- **#216**: Triagem 2026-10-12: 7924f0eb em v1.7.2; 80bb3dd2 em v1.7.2; 941ac3f0 em v1.7.2.
- **#221**: Triagem 2026-10-12: 1e4cfa8e em v1.7.2; 98607591 em v1.7.2; e012f141 em v1.7.2.
- **#222**: Triagem 2026-10-12: 1ea1510b em v1.7.4.
- **#223**: Triagem 2026-10-12: 105e759b em v1.7.2; release-notes.md v1.7.2: - **Android**: the app no longer restarts when the TV changes display settings, and the black screen before the first frame is gone. This may help the black screen in #223..
- **#224**: Triagem 2026-10-12: 67401c73 em v1.7.2.
- **#225**: Triagem 2026-10-12: 67401c73 em v1.7.2.
- **#226**: Triagem 2026-10-12: release-notes.md v2.0.1: - New app icon. A new What's New, with optional ways to support the project..
- **#227**: Triagem 2026-10-12: release-notes.md v1.7.4: - **Localized titles:** the written title appears when the logo is missing or confirmed to be in another language, without duplicating it during loading..
- **#228**: Triagem 2026-10-12: 1e03fc30 em v2.0.0.
- **#229**: Triagem 2026-10-12: release-notes.md v1.7.4: - **Subtitles and ratings:** Romanian `ron` support, more languages in All subtitles, and shared verified IMDb ratings across screens..
- **#231**: Triagem 2026-10-12: d6495798 em v2.0.0; release-notes.md v2.0.0: - New option to leave Cinemeta out of search (#231)..
- **#232**: Triagem 2026-10-12: 2495e39e em v1.7.4.
- **#233**: Triagem 2026-10-12: 3b625f58 em v2.0.0; release-notes.md v2.0.0: - Samsung: account catalogs and their order now follow the account (#233)..
- **#234**: Triagem 2026-10-12: 652b759d em v2.0.0; b8a900f5 em v2.0.0.
- **#235**: Triagem 2026-10-12: 652b759d em v2.0.0; b8a900f5 em v2.0.0; release-notes.md v2.0.0: - Seek waits a moment before applying when Seekr thumbnails are on (#235)..
- **#237**: Triagem 2026-10-12: 8f8be06a em v2.0.0; e5a848c3 em v2.0.0; release-notes.md v2.0.0: - Stalker/Xtream portal entry accepts "/" and "_" (#237)..
- **#238**: Triagem 2026-10-12: 73f93686 em v2.0.0; d6495798 em v2.0.0; ebaef8ad em v2.0.0.
- **#239**: Triagem 2026-10-12: 6bb186ac em v2.0.0; d00870f0 em v2.0.0.
- **#241**: Triagem 2026-10-12: 50ba9279 em v2.0.0; 50df3f42 em v2.0.0; 9a0cce0f em v2.0.0.
- **#243**: Triagem 2026-10-12: 8e7f4098 em v2.0.0; ccfe01a8 em v2.0.0; release-notes.md v2.0.0: - Continue watching: the same episode repeated, and "watched" not removing it (#244). The made-up "14" rating is gone (#243)..
- **#244**: Triagem 2026-10-12: 8e7f4098 em v2.0.0; d6495798 em v2.0.0; release-notes.md v2.0.0: - Continue watching: the same episode repeated, and "watched" not removing it (#244). The made-up "14" rating is gone (#243)..
- **#245**: Triagem 2026-10-12: 6bb186ac em v2.0.0; d00870f0 em v2.0.0.
- **#247**: Triagem 2026-10-12: 6bb186ac em v2.0.0; a1cf2e46 em v2.0.0; d00870f0 em v2.0.0.
- **#249**: Triagem 2026-10-12: release-notes.md v2.0.0: - Next episode card stays over the full-screen video in series (#249)..
- **#253**: Triagem 2026-10-12: a1cf2e46 em v2.0.0.
- **#255**: Triagem 2026-10-12: 40bc9180 em v2.0.1; release-notes.md v2.0.1: - **Collections.** Accounts with more than 256 folders lost their last collections (#255)..
- **#256**: Triagem 2026-10-12: 11791eda em v2.0.5.
- **#258**: Triagem 2026-10-12: a1cf2e46 em v2.0.0.
- **#261**: Triagem 2026-10-12: a1cf2e46 em v2.0.0.
- **#262**: Triagem 2026-10-12: release-notes.md v2.0.0: - **Player.** Audio and Subtitles open from the corner island. Second subtitle track, subtitle AutoSync, a delay slider, and seeking that speeds up while you hold..
- **#263**: Triagem 2026-10-12: 08a904d1 em v2.0.0.
- **#265**: Triagem 2026-10-12: release-notes.md v2.0.1: - **LG:** visual effects lower themselves on slower TVs..
- **#266**: Triagem 2026-10-12: a1d5e039 em v2.0.2; release-notes.md v2.0.2: - **Android 11:** a black screen at launch now shows a rescue screen, and the log is sent even without login (#266).. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#269**: Triagem 2026-10-12: 34a1eae6 em v2.0.1; 47a3d55d em v2.0.1; ce1552cd em v2.0.1.
- **#271**: Triagem 2026-10-12: release-notes.md v2.0.1: - **Crashes on LG and Android.** The app could close by itself when Home refreshed, most often while changing Settings..
- **#272**: Sem commit #272; versão 2.0.1 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#274**: Triagem 2026-10-12: release-notes.md v2.0.1: - **Arabic** text reads right to left with joined letters..
- **#275**: Triagem 2026-10-12: release-notes.md v2.0.1: - **File size range** for automatic playback: Settings › Sources and add-ons › Maximum size and Minimum size..
- **#276**: Triagem 2026-10-12: release-notes.md v2.0.0: - **Plugins and P2P (experimental, off by default).** Nuvio scraper repositories as an extra source, and a built-in engine for torrents you pick by hand..
- **#277**: Triagem 2026-10-12: release-notes.md v2.0.1: - **Add-ons behave like official Nuvio.** Slow add-ons get more time, setup kept in the link is no longer dropped, and subtitle add-ons like Subtitle Sync have time to answer..
- **#278**: Sem commit #278; versão 2.0.1 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#281**: Triagem 2026-10-12: 3076c3a5 em v2.0.1; release-notes.md v2.0.1: - **Trailers on the hero and focused posters** now start on accounts with a single profile (#228). Samsung .tpk trailers re-apply their volume after starting (#281)..
- **#282**: Sem commit #282; versão 2.0.1 citada na nossa resposta. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#283**: Triagem 2026-10-12: b68bad96 em v2.0.2; release-notes.md v2.0.2: - **LG:** add-on live channels work (#283). CH+ opens Salvos, like on the other platforms.. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#284**: Triagem 2026-10-12: a63ec982 em v2.0.3; df492356 em v2.0.3; release-notes.md v2.0.3: - **Best for this TV** shows the resolution in the add-on's own order too, and an unknown resolution shows as "?" instead of 0p (#284). On LG, Dolby Vision in MKV only counts toward this ranking when that option is on..
- **#286**: Triagem 2026-10-12: e03080dd em v2.0.2; release-notes.md v2.0.2: - **Samsung Tizen 4 with Mali-400:** the "Light" effects level now drops to minimal (#286).. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#287**: Triagem 2026-10-12: 0702a143 em v2.0.2; release-notes.md v2.0.2: - **Player:** audio codec shown (#293), forced subtitles (#287), resolution in the sources list, Arabic subtitles (#273)..
- **#289**: Triagem 2026-10-12: 3eb5aef5 em v2.0.2; release-notes.md v2.0.2: - Home trailer fade matches the still art (#290), plus fixes for #294, #295 and #289..
- **#290**: Triagem 2026-10-12: 4843e5b0 em v2.0.2; 66f19578 em v2.0.2; 75cee76f em v2.0.2.
- **#293**: Triagem 2026-10-12: db1f5891 em v2.0.2; release-notes.md v2.0.2: - **Player:** audio codec shown (#293), forced subtitles (#287), resolution in the sources list, Arabic subtitles (#273).. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#294**: Triagem 2026-10-12: b4186acd em v2.0.2; release-notes.md v2.0.2: - Home trailer fade matches the still art (#290), plus fixes for #294, #295 and #289..
- **#295**: Triagem 2026-10-12: 881f7cae em v2.0.2; release-notes.md v2.0.2: - Home trailer fade matches the still art (#290), plus fixes for #294, #295 and #289..
- **#296**: Triagem 2026-10-12: release-notes.md v2.0.2: - **Auto-play rules like official Nuvio:** allowed add-ons and plugins, regex filter, and an instant-wait option..
- **#297**: Triagem 2026-10-12: 1b6a5439 em v2.0.2; 325f4e2b em v2.0.2; release-notes.md v2.0.2: - **P2P files over 2 GB** on Samsung and LG (#297)..
- **#298**: Triagem 2026-10-12: release-notes.md v2.0.2: - **Player:** audio codec shown (#293), forced subtitles (#287), resolution in the sources list, Arabic subtitles (#273)..
- **#300**: Triagem 2026-10-12: release-notes.md v2.0.2: - **Default aspect ratio** applied when playback starts (#300)..
- **#303**: Triagem 2026-10-12: 1300a834 em v2.0.2; release-notes.md v2.0.2: - **Continue on profile picker** (#303)..
- **#305**: Triagem 2026-10-12: 98bee79f em v2.0.3; release-notes.md v2.0.3: - **Player:** UP at the top of the controls hides them (#305); the Seekr time and preview follow the hold..
- **#308**: Triagem 2026-10-12: 19ef3af8 em v2.0.3; 4151683c em v2.0.3; 6269f306 em v2.0.3.
- **#310**: Triagem 2026-10-12: release-notes.md v2.0.2: - **Auto-play rules like official Nuvio:** allowed add-ons and plugins, regex filter, and an instant-wait option.. Parcial: cache de link e regex por tipo continuam propostos para 2.1. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#311**: Triagem 2026-10-12: release-notes.md v2.0.2: - **Search:** a switch to turn Cinemeta results off (#311)..
- **#312**: Triagem 2026-10-12: 317cb179 em v2.0.3; release-notes.md v2.0.3: - **Sync** summary is translated on the spot, and the Continue Watching source "Both" is now "All sources" (#312). With your own Seekr key there is no 50-lookup cap and no counter..
- **#317**: Triagem 2026-10-12: 937f8e6b em v2.0.2; release-notes.md v2.0.2: - **LG:** startup breadcrumbs and a crash report even before the first frame (#317)..
- **#318**: Triagem 2026-10-12: 7e96e7e7 em v2.0.2; release-notes.md v2.0.2: - **MStar Android boxes:** no more flicker from the video surface switching, and the remote stays bound to the app (#318)..
- **#319**: Triagem 2026-10-12: c796ca7d em v2.0.2; release-notes.md v2.0.2: - **Home** drops rows of removed or disabled add-ons right away, and they no longer take Home slots (#319)..
- **#320**: Triagem 2026-10-12: 22ecac46 em v2.0.3; 8ab3b78f em v2.0.3.
- **#321**: Triagem 2026-10-12: b347004c em v2.0.3.
- **#322**: Triagem 2026-10-12: b8d110de em v2.0.3.
- **#323**: Triagem 2026-10-12: 1b6c9a89 em v2.0.3; release-notes.md v2.0.3: - **Crashes from threads racing each other** (watched episodes, parallel source check, catalog episodes, sign-in token, collections) (#203, #323)..
- **#326**: Triagem 2026-10-12: 1cc512dd em v2.0.5; 56868756 em v2.0.5; 95fc93c5 em v2.0.5. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#327**: Triagem 2026-10-12: 8ab1b19b em v2.0.3; d268b069 em v2.0.3; release-notes.md v2.0.3: - **Home:** switching profile no longer shows the previous profile's rows (#294); the hero no longer depends on a drawn row, and a catalog added from Not on Home is no longer swallowed by a collection (#327); rebuilds are spaced out so Home settles with many collections (#319, #280)..
- **#328**: Triagem 2026-10-12: 7edab4b0 em v2.0.3; 9c5d0027 em v2.0.3; release-notes.md v2.0.3: - **Duplicate episodes** in the episode list (#328).. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#329**: Triagem 2026-10-12: 4193781f em v2.0.5; 95fc93c5 em v2.0.5; a0a392fb em v2.0.5. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#330**: Triagem 2026-10-12: 2f047898 em v2.0.3; release-notes.md v2.0.3: - **Embedded subtitles:** large MKV track headers, gentler reads, no stale downloads (#308, #330); more add-on subtitles (#268); Arabic subtitles with real bold and sharp ASS in 4K (#335).. confirmada pelo autor de #412: substitui #330 com reprodução melhor; acompanhar #412.
- **#331**: Triagem 2026-10-12: 11791eda em v2.0.5; 4193781f em v2.0.5; 4c17e680 em v2.0.5. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#332**: Triagem 2026-10-12: 35421d1a em v2.0.3; eabe3afd em v2.0.3; release-notes.md v2.0.3: - **Android:** start hang on Shield and other boxes, sign-in through the system network stack when libcurl fails (#266, #332); MStar flicker fixes and a crash report on Android 11 and older (#318)..
- **#334**: Triagem 2026-10-12: c3e19d00 em v2.0.3; d3ca720b em v2.0.3; release-notes.md v2.0.3: - **P2P space limit** in the P2P settings (#334). Leftovers from a session that crashed are deleted at start.. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#335**: Triagem 2026-10-12: 01016036 em v2.0.3; 3b93dfad em v2.0.3; 503ed5cc em v2.0.3.
- **#338**: Triagem 2026-10-12: 74f781f3 em v2.0.5; 8c856535 em v2.0.5; 95fc93c5 em v2.0.5. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#339**: Triagem 2026-10-12: 30e13fa4 em v2.0.3; 7dc8cd71 em v2.0.3; b1b53000 em v2.0.3.
- **#340**: Triagem 2026-10-12: bc63d8ca em v2.0.3; release-notes.md v2.0.3: - **Holding fast-forward** speeds up and crosses the whole file in about 10 s. A single press is still 10 s (#340)..
- **#341**: Triagem 2026-10-12: release-notes.md v2.0.3: - **Samsung:** Pause and subtitle list errors (#269); aspect/zoom button on Tizen 4/5 (#341, probable fix, not yet confirmed on a TV); Arabic subtitles when an older install left old fonts. When the TV hides an audio track it can't play (DTS, TrueHD), the other tracks keep the names from the file instead of a generic one.. Aspecto tem conserto publicado; crop de barras embutidas é pedido separado aprovado para 2.1. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#350**: Triagem 2026-10-12: 4b8ba24b em v2.0.3; release-notes.md v2.0.3: - **"View details"** in the Continue Watching hold menu, when OK on the card plays (#350).; release-notes.md v2.0.3: - **Live TV:** the reconnect watcher took the provider's end of stream for a manual pause and never reopened the channel. Manual pause is now tracked on its own (#302, #350).. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#352**: Triagem 2026-10-12: 402e2438 em v2.0.5; d4d72639 em v2.0.5; fab062b3 em v2.0.5. Parcial: indicadores/ordenação lançados; menus árabes e limites maiores continuam na 2.1. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#355**: Triagem 2026-10-12: ac055d54 em v2.0.5; b176eff5 em v2.0.5; c8dbfda5 em v2.0.5. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#356**: Triagem 2026-10-12: d47a78c8 em v2.0.3; release-notes.md v2.0.3: - **Up next** no longer disappears when the Nuvio catalog does not list the episode; Cinemeta is checked first (#356).. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#358**: Triagem 2026-10-12: 29c640b7 em v2.0.5; c4edb7c6 em v2.0.5; release-notes.md v2.0.5: - A new add-on catalog that no longer fits on Home goes to the queue instead of being switched off (#358), and a row switch now takes effect instead of flipping back.. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#359**: Triagem 2026-10-12: 1860216b em v2.0.3; release-notes.md v2.0.3: - **Fixed sidebar:** the row editor (Reorder and enable rows) and the View all and collection grids start after it instead of under it (#359)..
- **#360**: Triagem 2026-10-12: 46507ea5 em v2.0.3; release-notes.md v2.0.3: - **Add-on sync** merges your TV edits with the account instead of replacing it: an add-on installed on the phone stays, one removed on the phone does not come back. The merged list is never cut. After 3 refusals (4xx) the TV edit is dropped and the account list is applied (#360)..
- **#361**: Triagem 2026-10-12: 2220b019 em v2.0.3; 9ac3531d em v2.0.3; release-notes.md v2.0.3: - **Posters with long image URLs** (over ~500 characters, such as rating-poster services) are no longer cut on the backdrop, the title page, Saved and the account library, so they load; a background or logo URL too long to keep is dropped with a log line instead of loading as a broken link (#361)..
- **#362**: Triagem 2026-10-12: 22320582 em v2.0.5; 491fcee3 em v2.0.5. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#363**: Triagem 2026-10-12: 54cf2fab em v2.0.3; 9bb93887 em v2.0.3; release-notes.md v2.0.3: - **Recommending a series** from a Continue Watching card works again (#363)..
- **#364**: Triagem 2026-10-12: 8f2c1701 em v2.0.5; bf666aa7 em v2.0.5; d78963ab em v2.0.5. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#368**: Triagem 2026-10-12: dbec4f44 em v2.0.3; release-notes.md v2.0.3: - **Search** waits 300 ms after the last key before asking the add-ons, and keeps the previous rows until the first reply (#368). A search with no results clears the previous ones..
- **#369**: Triagem 2026-10-12: 5f04c36b em v2.0.3; 712557ca em v2.0.3; release-notes.md v2.0.3: - **Thai** is drawn with a bundled Noto Sans Thai font, in the interface and in subtitles (#369).. Parcial: tailandês e ocultar não lançados têm conserto; o relatório contém outros pedidos sem prova. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#370**: Triagem 2026-10-12: 2aee231b em v2.0.3; 56c9832e em v2.0.3; release-notes.md v2.0.3: - **Samsung .wgt:** Arabic subtitles use Noto Naskh (#370). Holding OK in the subtitle sheet makes one choice instead of reloading on every repeat (#370)..
- **#371**: Triagem 2026-10-12: 2fa6e51a em v2.0.3; b39f4149 em v2.0.3; release-notes.md v2.0.3: - **Profile page:** Left at the edge opens the sidebar without closing the page, also on a new profile or one with only friends (#371)..
- **#383**: Triagem 2026-10-12: 3654b88e em v2.0.5; release-notes.md v2.0.5: - A subtitle you picked by hand survives into the next episode (#383); forced subtitles and unknown types keep their behaviour, and picking the active subtitle from outside the recommendations no longer lands on "Off".. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#384**: Triagem 2026-10-12: 3e7bbb62 em v2.0.3; 4151683c em v2.0.3; 9ee093ac em v2.0.3.
- **#385**: Triagem 2026-10-12: 4151683c em v2.0.3; 9ee093ac em v2.0.3; release-notes.md v2.0.3: - **Video server refusing connections** (#385). When the server of the video refuses a connection (seen on Samsung .tpk with TorBox links), the embedded subtitle reader, the track probe and the chapter reader stop and wait 10 to 60 s before trying that server again, with one connection at a time, instead of retrying several times while the video opens. It does not make a refused source play: if the server also refuses the TV player, the source still fails.. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#387**: Triagem 2026-10-12: 03213598 em v2.0.5; 0f98379f em v2.0.5; c82fa422 em v2.0.5. [alvo antes: ja-lancada; fechada no GH 10-15]
- **#427**: Triagem 2026-10-12: 92ee6141 em v2.0.5. | Escrivão 2026-10-14 16:11: GitHub CLOSED no snapshot de issues.json (2026-10-13). Escrivao 10-15: reaberta no GH (OPEN no snapshot 10-15).

## Fechado sem conserto (por-desenho, fora-do-escopo, duplicada, respondida, sem resposta)

91 issues.

| # | Título | Plat. | Tipo | Status | Release | Alvo | Conserto | Última resposta | Próximo passo |
|---|---|---|---|---|---|---|---|---|---|
| [#15](https://github.com/iqui27/nuvio-native-legacy/issues/15) | Bug: Trailer Does Not Play When Selected on Detail Page | ? | bug | respondida | - | - | - | autor 09-16 | nada (fechada) |
| [#20](https://github.com/iqui27/nuvio-native-legacy/issues/20) | Library does not populate from account | LG | bug | respondida | - | - | - | nós 09-08 | nada (fechada) |
| [#47](https://github.com/iqui27/nuvio-native-legacy/issues/47) | Feature request | ? | feature | respondida | - | - | - | nós 09-15 | nada (fechada) |
| [#53](https://github.com/iqui27/nuvio-native-legacy/issues/53) | Sources not showing up | ? | bug | respondida | - | - | - | autor 09-16 | nada (fechada) |
| [#58](https://github.com/iqui27/nuvio-native-legacy/issues/58) | Trakt Library titles are not appearing | Samsung (tpk/wgt?) | bug | por-desenho | - | - | - | nós 09-19 | nada (fechada) |
| [#59](https://github.com/iqui27/nuvio-native-legacy/issues/59) | "Meu Futebol" add-on not working | Samsung (tpk/wgt?) | bug | por-desenho | 1.2.1 (LG); Samsung sem solução | - | - | nós 09-19 | nada (fechada) |
| [#63](https://github.com/iqui27/nuvio-native-legacy/issues/63) | Support for Tizen 5 | Samsung (tpk/wgt?) | feature | por-desenho | - | - | - | nós 09-19 | nada (fechada) |
| [#64](https://github.com/iqui27/nuvio-native-legacy/issues/64) | no highcache version for 1.1.2? | ? | question | respondida | - | - | - | nós 09-19 | nada (fechada) |
| [#75](https://github.com/iqui27/nuvio-native-legacy/issues/75) | Request: settings menu navigation | ? | feature | fechada-sem-resposta | - | - | - | sem comentários | nada (fechada) |
| [#81](https://github.com/iqui27/nuvio-native-legacy/issues/81) | Seek not functioning properly  | ? | bug | respondida | 1.3.7 | - | - | nós 09-20 | nada (fechada) |
| [#90](https://github.com/iqui27/nuvio-native-legacy/issues/90) | feature request | ? | feature | respondida | - | - | - | nós 09-23 | nada (fechada) |
| [#91](https://github.com/iqui27/nuvio-native-legacy/issues/91) | MP4 container toggle | ? | feature | fechada-sem-resposta | - | - | - | sem comentários | nada (fechada) |
| [#96](https://github.com/iqui27/nuvio-native-legacy/issues/96) | Request | Samsung (tpk/wgt?) | feature | respondida | - | - | - | autor 09-27 | nada (fechada) |
| [#98](https://github.com/iqui27/nuvio-native-legacy/issues/98) | Dunno how github does its uploads but the there's no release | ? | ? | fechada-sem-resposta | - | - | - | sem comentários | nada (fechada) |
| [#107](https://github.com/iqui27/nuvio-native-legacy/issues/107) | Not searching files from addon | ? | bug | precisa-log | - | - | - | autor 09-23 | nada (fechada) |
| [#135](https://github.com/iqui27/nuvio-native-legacy/issues/135) | Looking for testers: experimental Hisense VIDAA port (Hisens | VIDAA | feature | respondida | - | - | faacf493, 08d21c20, a81639d2 | autor 10-06 | responder (feedback do gabo748 em 06/10 sem retorno) |
| [#140](https://github.com/iqui27/nuvio-native-legacy/issues/140) | trailer no sound | ? | ? | fechada-sem-resposta | - | - | - | sem comentários | nada (fechada) |
| [#148](https://github.com/iqui27/nuvio-native-legacy/issues/148) | Sudden crash by moving through the app | ? | bug | precisa-log | - | - | - | autor 09-27 | nada (fechada) |
| [#154](https://github.com/iqui27/nuvio-native-legacy/issues/154) | Relatório de Logs | ? | meta | por-desenho | - | - | - | nós 09-27 | nada (relatório automático de logs) |
| [#155](https://github.com/iqui27/nuvio-native-legacy/issues/155) | Thanks for building this! | LG | meta | respondida | - | - | - | autor 10-06 | nada (fechada) |
| [#161](https://github.com/iqui27/nuvio-native-legacy/issues/161) | Relatório de logs | ? | meta | por-desenho | - | - | - | nós 09-30 | nada (relatório automático de logs) |
| [#167](https://github.com/iqui27/nuvio-native-legacy/issues/167) | Tizen 4.0 | Samsung (tpk/wgt?) | ? | fechada-sem-resposta | - | - | - | sem comentários | nada (fechada) |
| [#168](https://github.com/iqui27/nuvio-native-legacy/issues/168) | Trailer plays without sound | ? | bug | fechada-sem-resposta | - | - | - | sem comentários | nada (fechada) |
| [#183](https://github.com/iqui27/nuvio-native-legacy/issues/183) | trailer resolution | ? | ? | respondida | - | - | - | nós 09-30 | nada (fechada) |
| [#189](https://github.com/iqui27/nuvio-native-legacy/issues/189) | Actors UI | ? | feature | respondida | - | - | - | autor 09-30 | nada (fechada) |
| [#192](https://github.com/iqui27/nuvio-native-legacy/issues/192) | Relatório de logs | ? | meta | por-desenho | - | - | 77227ac1, b61d74a6 | nós 10-07 | nada (relatório automático de logs) |
| [#217](https://github.com/iqui27/nuvio-native-legacy/issues/217) | Building on a Linux host: three small blockers (and one targ | ? | bug | respondida | - | - | - | nós 10-02 | nada (fechada) |
| [#240](https://github.com/iqui27/nuvio-native-legacy/issues/240) | tardan demasiado en cargar una serie o pelicula | LG | bug | precisa-log | - | - | - | autor 10-06 | nada (fechada) |
| [#242](https://github.com/iqui27/nuvio-native-legacy/issues/242) | API QUESTION | ? | question | respondida | - | - | - | nós 10-06 | nada (fechada) |
| [#248](https://github.com/iqui27/nuvio-native-legacy/issues/248) | Catalog sequencing mismatched | Samsung .tpk | bug | por-desenho | - | - | - | autor 10-04 | nada (fechada) |
| [#252](https://github.com/iqui27/nuvio-native-legacy/issues/252) | Add default audio language setting for anime | LG | feature | por-desenho | - | - | - | nós 10-11 | nada (fechada 11/10 por decisao do dono) |
| [#254](https://github.com/iqui27/nuvio-native-legacy/issues/254) | Home row does not update properly after installing/removing  | Samsung .tpk | bug | respondida | 2.0.0 | - | - | nós 10-06 | nada (fechada) |
| [#257](https://github.com/iqui27/nuvio-native-legacy/issues/257) | FYI: included in Tizen Community Packages | Samsung (tpk/wgt?) | meta | respondida | - | - | - | nós 10-06 | nada (fechada) |
| [#267](https://github.com/iqui27/nuvio-native-legacy/issues/267) | No blue button on newer Samsung remote | Samsung (tpk/wgt?) | feature | por-desenho | - | - | - | autor 10-06 | nada (fechada) |
| [#270](https://github.com/iqui27/nuvio-native-legacy/issues/270) | 📊 Relatório de logs | ? | meta | por-desenho | - | - | d560db8c, 666428cc | nós 10-07 | nada (relatório automático de logs) |
| [#279](https://github.com/iqui27/nuvio-native-legacy/issues/279) | Match framerate | Samsung (tpk/wgt?) | feature | fora-do-escopo | - | - | - | autor 10-06 | nada (fechada) |
| [#288](https://github.com/iqui27/nuvio-native-legacy/issues/288) | Profile selection background is grainy dark video | LG | bug | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#292](https://github.com/iqui27/nuvio-native-legacy/issues/292) | GoogleTV performance UPDATE | Android | question | por-desenho | - | - | - | sem comentários | nada |
| [#304](https://github.com/iqui27/nuvio-native-legacy/issues/304) | Apple tv dynamic homeacreen | ? | feature | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#306](https://github.com/iqui27/nuvio-native-legacy/issues/306) | Refresh Live TV | LG | feature | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#307](https://github.com/iqui27/nuvio-native-legacy/issues/307) | Request | iOS | feature | fora-do-escopo | - | - | - | autor 10-07 | responder (sem resposta; pedido de .ipa) |
| [#309](https://github.com/iqui27/nuvio-native-legacy/issues/309) | [suggestion] sources formatter | ? | feature | por-desenho | - | - | - | autor 10-07 | nada (fechada) |
| [#313](https://github.com/iqui27/nuvio-native-legacy/issues/313) | [suggestion] downmix DTS to AC3 5.1 | Samsung | feature | integrada | - | - | bf3e71e4a | autor 10-07 | lancar com 2.0.5 se TV ok (testar no TCL/C9) |
| [#315](https://github.com/iqui27/nuvio-native-legacy/issues/315) | .avi media files fail to play ("Could not open the source") | Samsung .tpk | bug | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#316](https://github.com/iqui27/nuvio-native-legacy/issues/316) | Live tv schedule | Samsung .tpk | bug | integrada | - | - | 7f3177ef | sem comentários | lancar com 2.0.5 (testar no TCL antes do release) |
| [#324](https://github.com/iqui27/nuvio-native-legacy/issues/324) | Relatório de logs | all | meta | por-desenho | - | - | 83c2cedd | nós 10-11 | nada |
| [#325](https://github.com/iqui27/nuvio-native-legacy/issues/325) | Add Arabic Language Support | all | feature | duplicada | 2.1 | - | - | autor 10-07 | nada (fechada) |
| [#333](https://github.com/iqui27/nuvio-native-legacy/issues/333) | Arabic language | all | feature | duplicada | 2.1 | - | - | nós 10-10 | responder (apontar #250/#260; sem resposta nossa) |
| [#336](https://github.com/iqui27/nuvio-native-legacy/issues/336) | App wont launch | Samsung .tpk | meta | por-desenho | - | - | - | autor 10-08 | nada (relatório automático de logs) |
| [#337](https://github.com/iqui27/nuvio-native-legacy/issues/337) | Be able to hide the "skip" intro button | ? | feature | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#342](https://github.com/iqui27/nuvio-native-legacy/issues/342) | Add an optional "Skip Scene" floating button for explicit co | all | feature | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#345](https://github.com/iqui27/nuvio-native-legacy/issues/345) | Audio non-existent on startup | Android | bug | integrada | - | - | c03201c0c | sem comentários | merged em integracao/2.0.5 (fc2a32c76 fix(android): preserve safe numeric fields in fata) |
| [#346](https://github.com/iqui27/nuvio-native-legacy/issues/346) | Continue Watching unselectable | Android | bug | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#347](https://github.com/iqui27/nuvio-native-legacy/issues/347) | [suggestion] search by microphone | ? | feature | aberta | - | - | - | autor 10-12 | triar/reproduzir; falta conserto comprovado |
| [#348](https://github.com/iqui27/nuvio-native-legacy/issues/348) | Dots y imdb | ? | feature | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#349](https://github.com/iqui27/nuvio-native-legacy/issues/349) | Source Preference Option for MP4 and P2P | ? | feature | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#353](https://github.com/iqui27/nuvio-native-legacy/issues/353) | Hero page trailer only plays once and has no sound | LG | bug | aberta | - | - | - | nós 10-08 | triar/reproduzir; falta conserto comprovado |
| [#354](https://github.com/iqui27/nuvio-native-legacy/issues/354) | Bring the Full TV Experience to PC - Same App, No Compromise | PC | feature | fora-do-escopo | - | - | - | autor 10-11 | responder (sem resposta) |
| [#357](https://github.com/iqui27/nuvio-native-legacy/issues/357) | Nuvio v2.0.1/2.0.2 – Crashes on Zidoo Z9X 8K and Ugoos AM9 P | Android | bug | precisa-log | - | - | - | nós 10-11 | responder: como tirar log sem abrir o app (adb logcat) |
| [#365](https://github.com/iqui27/nuvio-native-legacy/issues/365) | 🐛 Bug Report: Ghost Catalogs Still Remain in “Not on Home Sc | Samsung .tpk | bug | integrada | - | - | 3c58d0ce | sem comentários | lancar com 2.0.5 (testar no TCL antes do release) |
| [#366](https://github.com/iqui27/nuvio-native-legacy/issues/366) | [Feature request] Optional setting to hide watched movies fr | LG | feature | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#367](https://github.com/iqui27/nuvio-native-legacy/issues/367) | [Bug][webOS] Audio control sometimes opens Subtitles on firs | LG | bug | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#373](https://github.com/iqui27/nuvio-native-legacy/issues/373) | [bug] missing info after continue watching | Samsung .tpk | bug | por-desenho | - | - | - | nós 10-11 | nada (fechada; comportamento explicado) |
| [#374](https://github.com/iqui27/nuvio-native-legacy/issues/374) | [port] sync subtitles by line "auto sync" | ? | feature | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#379](https://github.com/iqui27/nuvio-native-legacy/issues/379) | Movie or TV show at the end never return to homescreen. | Samsung .tpk | bug | integrada | - | - | d8cc7fa2 | sem comentários | lancar com 2.0.5 (testar no TCL antes do release) |
| [#382](https://github.com/iqui27/nuvio-native-legacy/issues/382) | No internet error despite having internet | LG | bug | integrada | - | - | c0368dd4, c0368dd40, 6e29915c0 | nós 10-11 | merged em integracao/2.0.5 (b7740b8dc Define the POST retry helper in the shared branch ) |
| [#386](https://github.com/iqui27/nuvio-native-legacy/issues/386) | [bug] glitchy info in playback at the top left when pressing | Samsung .tpk | bug | integrada | - | - | d8cc7fa2 | nós 10-11 | lancar com 2.0.5 (testar no TCL antes do release) |
| [#388](https://github.com/iqui27/nuvio-native-legacy/issues/388) | Profile picture on side bar is squashed | Samsung .tpk | bug | integrada | - | - | 219d3bf4 | autor 10-12 | lancar com 2.0.5 (testar no TCL antes do release) |
| [#389](https://github.com/iqui27/nuvio-native-legacy/issues/389) | Card depth effect | Samsung (tpk/wgt?) | feature | aberta | - | - | - | autor 10-09 | triar/reproduzir; falta conserto comprovado |
| [#391](https://github.com/iqui27/nuvio-native-legacy/issues/391) | Episodes and Seasons fail | Samsung .tpk | bug | integrada | - | - | 219d3bf4 | sem comentários | lancar com 2.0.5 (testar no TCL antes do release) |
| [#393](https://github.com/iqui27/nuvio-native-legacy/issues/393) | [Feature Request] Option to change the library source from T | ? | feature | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#394](https://github.com/iqui27/nuvio-native-legacy/issues/394) | ASS subtitles: lag on large tracks, shadow/color shift, occa | Samsung .tpk | bug | aberta | - | - | - | nós 10-09 | triar/reproduzir; falta conserto comprovado |
| [#396](https://github.com/iqui27/nuvio-native-legacy/issues/396) | Why C and not Rust, and the plan for the next releases and D | all | meta | respondida | - | - | - | autor 10-10 | Dono revisar resposta informativa e estado atual do Discord/roadmap; fechar como informati |
| [#397](https://github.com/iqui27/nuvio-native-legacy/issues/397) | [Feature Request] Playback Engine Robustness: Native Codec O | LG | feature | respondida | - | - | - | autor 10-10 | postar o rascunho (EN, nas notas): o que já existe e o caminho; o que falta (atraso do áud |
| [#398](https://github.com/iqui27/nuvio-native-legacy/issues/398) | Subtitle outline transparrency | Samsung .tpk | bug | aberta | - | - | - | sem comentários | Pedir exemplo visual local, tipo de legenda (embutida/add-on, SRT/ASS), ajustes e código d |
| [#399](https://github.com/iqui27/nuvio-native-legacy/issues/399) | Feature request: Audio decoder selection and Dolby TrueHD 7. | Android | bug | aberta | - | - | - | sem comentários | Pedir versão exata, Android, configuração de saída HDMI e log após congelar; comparar AC3/ |
| [#401](https://github.com/iqui27/nuvio-native-legacy/issues/401) | Long update time on Homebrew channel (15 mins +) | LG | bug | respondida | - | - | - | autor 10-10 | conserto na 2.0.5 (dados fora da pasta do app + limpeza de sobras; strip do binário) |
| [#403](https://github.com/iqui27/nuvio-native-legacy/issues/403) | feature-request: Add a next button to go to next episode | all | feature | aberta | - | - | - | nós 10-09 | triar/reproduzir; falta conserto comprovado |
| [#404](https://github.com/iqui27/nuvio-native-legacy/issues/404) | Continue watching mixes up | Samsung .tpk | bug | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#405](https://github.com/iqui27/nuvio-native-legacy/issues/405) | Quick search doesn't list all results | Samsung .tpk | bug | integrada | - | - | 219d3bf4 | sem comentários | lancar com 2.0.5 (testar no TCL antes do release) |
| [#406](https://github.com/iqui27/nuvio-native-legacy/issues/406) | Passing through ratings on title page | Samsung .tpk | feature | aberta | - | - | - | sem comentários | triar/reproduzir; falta conserto comprovado |
| [#436](https://github.com/iqui27/nuvio-native-legacy/issues/436) | Feature request: Option to view submitted logs | all | feature | aberta | - | - | - | nós 10-11 | triar/reproduzir; falta conserto comprovado |
| [#460](https://github.com/iqui27/nuvio-native-legacy/issues/460) | Playback buttons for reloading stream and next episode | Android | feature | aberta | - | - | - | sem comentários | Conferir #403 e definir botões VOD com episódio disponível; reusar encaminhamento de próxi |
| [#473](https://github.com/iqui27/nuvio-native-legacy/issues/473) | Sound does not start | ? | bug | aberta | - | - | - | sem comentários | triar: reaberta no GH 10-15; sem commit de conserto identificado |
| [#474](https://github.com/iqui27/nuvio-native-legacy/issues/474) | Top right bar is unreachable | ? | bug | integrada | - | - | agente/205-menutopo | sem comentários | merged em integracao/2.0.5 (dbe8a3b8d Merge integracao/2.0.5 into agente/205-menutopo) |
| [#476](https://github.com/iqui27/nuvio-native-legacy/issues/476) | No internet | ? | bug | aberta | - | - | - | sem comentários | triar: reaberta no GH 10-15; sem commit de conserto identificado |
| [#480](https://github.com/iqui27/nuvio-native-legacy/issues/480) | Remember Preferred Audio Language Across Sources | ? | feature | aberta | - | - | - | sem comentários | triar: reaberta no GH 10-15; sem commit de conserto identificado |
| [#482](https://github.com/iqui27/nuvio-native-legacy/issues/482) | Continue Watching content leaks between profiles | ? | bug | integrada | - | - | 067c0c35, f51aa3d76 | sem comentários | merged em integracao/2.0.5 (d16de03ad fix: profile switch pass waits for the account cyc) |
| [#511](https://github.com/iqui27/nuvio-native-legacy/issues/511) | Show subtitle track name instead of only showing language | ? | enhancement | integrada |  | - | f0ab7c91, f0ab7c918 | sem comentários | merged em integracao/2.0.5 (2e05dd94d subtitles: show track name as the row title, langu) |
| [#513](https://github.com/iqui27/nuvio-native-legacy/issues/513) | Any series episode autoplay even option is disabled | ? | bug | integrada |  | - | 84add4581 | sem comentários | merged em integracao/2.0.5 (adbaf0f9f fix: initialize episode outputs before storing sou) |
| [#529](https://github.com/iqui27/nuvio-native-legacy/issues/529) | Dubbing filter bug | ? | ? | integrada |  | - | f8fbd3bd3 | sem comentários | merged em integracao/2.0.5 (5f486b7a1 fix: respect preferred audio language in source fi) |

Notas:

- **#15**: Resposta: plano de passar o trailer ao app de YouTube da TV; rawldon confirmou depois que toca (com atraso). Sem commit #15. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#20**: "Fixed on master; next release" (08/09); nenhum commit cita #20. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#47**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#53**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#58**: Salvos = união da lista Nuvio e Trakt watchlist, de propósito. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#59**: LG manda Referer desde 1.2.1; AVPlay da Samsung não permite header. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#63**: Tizen 5.0 sem WebAssembly threads; depois o .tpk 4/5 (1.6.0) resolveu por outro caminho. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#64**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#75**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#81**: v1.3.7 apenas registra o tempo de seek na Samsung (diagnóstico). Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#90**: "Not in 1.4.2 yet — still on the list". Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#91**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#96**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#98**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#107**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#135**: Porte experimental VIDAA; issue de chamada de testadores. Commits faacf493 (2.0.2) e feat/vidaa. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho. [alvo antes: futuro; fechada no GH 10-15]
- **#140**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#148**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#154**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#155**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#161**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#167**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#168**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#183**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#189**: "Vou tentar ajustes em breve", sem commit #189. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#192**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#217**: Contribuidor oferece PRs de build em Linux; resposta aceita. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#240**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#242**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#248**: Resposta diz que é comportamento da plataforma Tizen. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#252**: Opção já existe: Ajustes > Idiomas e legendas > Idioma do áudio. Alvo ja-lancada: a opção já existe. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#254**: Resposta: busca atualiza ao mudar add-ons na 2.0; a parte da Home não confirmada. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#257**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#267**: Resposta: CH+/CH- já atribuídos. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#270**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#279**: Tizen não tem API pública para trocar a taxa de atualização. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#288**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: 2.0.6; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#292**: Aviso nosso (virou issue a partir da discussão #291): resolução da interface em 4K deixa o Android lento. Alvo nao vamos fazer: é aviso/pergunta, não há mudança de código planejada (ver decisões). Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho. [alvo antes: nao vamos fazer; fechada no GH 10-15]
- **#304**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: futuro; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#306**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: 2.1; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#307**: SUSPEITA de fora-do-escopo: pedido de .ipa, app é C/SDL para TVs. O dono decide. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho. [alvo antes: nao vamos fazer; fechada no GH 10-15]
- **#309**: Já existe: Fontes e add-ons > Texto da fonte. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#313**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. bf3e71e4 implementa AC3 no webOS; f45ddc14 só avisa na Samsung. Nenhum atende ao downmix Samsung pedido. [alvo antes: 2.1; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar. | Escrivao 2026-10-15 11:48: commit em integracao/2.0.5 (bf3e71e4a); status -> integrada
- **#315**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: 2.0.6; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#316**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. Escrivão 14/10 14:47: integrada via livetv. [alvo antes: 2.0.6; fechada no GH 10-15]
- **#324**: Relatório automático de triagem de logs (comentários nossos); não é bug. Alvo nao vamos fazer: não é bug nem pedido. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho. [alvo antes: nao vamos fazer; fechada no GH 10-15]
- **#325**: Duplicata de #250/#260. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#333**: Duplicata de #250 (Arabic). Comentário no commit c824675c (v1.0.1) é falso positivo. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#336**:  Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho.
- **#337**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: 2.1; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#342**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: futuro; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#345**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. | 13/10 ghbarker: logs 6HMRKA (73567) e Cf4whk (73436), Android. Job agente/205-audio345. [alvo antes: 2.0.6; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar. [sync 2026-10-15: agente/205-audio345 integrada]
- **#346**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: 2.0.6; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#347**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: futuro; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#348**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: futuro; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#349**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: 2.1; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#353**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: 2.0.6; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#354**: SUSPEITA: versão desktop está fora do escopo (app é para TVs). Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho. [alvo antes: nao vamos fazer; fechada no GH 10-15]
- **#357**: Zidoo Z9X 8K (Android 11) fecha ao abrir; Ugoos AM9 Pro. Autor comentou que não achou o aparelho no relatório #324 e não consegue enviar log. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho. [alvo antes: 2.0.6; fechada no GH 10-15]
- **#365**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. suspeita: catálogos removidos persistem; #51 também relata isso, exigir reprodução atual; acompanhar #254. Escrivão 14/10 14:47: integrada via i504, tpkui. [alvo antes: 2.0.6; fechada no GH 10-15]
- **#366**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: 2.1; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#367**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: 2.0.6; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#373**: Revisão 2026-10-12: autor confirma UI resolvida em 2.0.4; mantenedor explica em 11/10 que Opening source é esperado por renovação dos links. Fechada no snapshot; sem commit de fix comprovado.
- **#374**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. Pedido é sync pela linha clicada e limite maior de atraso; AutoSync existente não atende ao recurso solicitado. [alvo antes: 2.1; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#379**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. Escrivão 14/10 14:47: integrada via tpkfim. [alvo antes: 2.0.6; fechada no GH 10-15]
- **#382**: LG 50UT8050PSB, Nuvio 2.0.2 (.ipk). Relato: depois de ligar a TV a frio aparece "sem internet" mesmo com internet, some, volta várias vezes até parar de vez. O campo de log ficou sem código. SUSPEITA, sem prova: o aviso nasce do teste de rede do app (redesaude) logo após o boot, quando a rede da TV ainda sobe; não lido no código nem em log. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho. | 13/10 Mizikashi1: voltou ao abrir serie, log RSM044 (registro 73601). Job agente/205-net382. Escrivão 14/10 14:47: integrada via 382dns. [alvo antes: 2.0.6; fechada no GH 10-15] | Escrivao 2026-10-15 11:48: prompt p-205-382dns em /Volumes/ExternalSSD/tmp/rev; job terminado, sem job vivo [sync 2026-10-15: agente/205-382dns integrada] [sync 2026-10-15: agente/205-net382 integrada]
- **#386**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. Escrivão 14/10 14:47: integrada via tpkfim. [alvo antes: 2.0.6; fechada no GH 10-15]
- **#388**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. Escrivão 14/10 14:47: integrada via tpkui. [alvo antes: 2.0.6; fechada no GH 10-15]
- **#389**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: futuro; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#391**: QN55Q60RAG, 2.0.3: só primeira temporada e episódios incompletos; log Z4HDY2 já fornecido. src/descoberta.c:parsearEpisodios, episodiosDoCatalogo e temporadasDosEps leem videos[], deduplicam (temporada, episódio), escolhem a lista de metadados e derivam as abas da lista publicada. A seleção descarta temporadas <= 0 e pode preferir outra lista quando a base parece lista de arquivos. SUSPEITA: cobertura/numeração ou escolha da lista de metadados; não prova duplicata de #372 nem resolução de #391. Há tratamento implementado, mas nenhum conserto estável especificamente comprovado para este relato. Escrivão 14/10 14:47: integrada via tpkui. [alvo antes: 2.0.6; fechada no GH 10-15]
- **#393**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: 2.1; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#394**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: 2.0.6; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#396**: Publicação informativa do mantenedor sobre C, Rust e roadmap; comentários pedem Discord. src/menu.c usa SDL/OpenGL e android/app/src/main/java/space/nuvio/nativelegacy/NvPlayer.kt contém o backend Media3: implementação C compartilhada com pontes de plataforma, não migração Rust. Não há bug reproduzível nem conserto a atribuir; o trecho do corpo é incompleto e não comprova entrega do piloto Rust. .triagem/release-notes.md v2.0.5 já anuncia o servidor comunitário Discord; o pedido de Discord tem evidência de atendimento, não de migração Rust. [alvo antes: futuro; fechada no GH 10-15]
- **#397**: Pedido em quatro partes (formulário diz LG webOS / All; o texto fala de Android/Fire TV). Caminhos conferidos NO CÓDIGO (09/10), sem prova em TV. (a) HDR/Dolby Vision e passthrough: JÁ HÁ o que o app controla. Preferência de fonte em Ajustes › Reprodução › "Dolby Vision e HDR" (AJ_FONTE_HDR) e "Preferir som Dolby Atmos" (AJ_ATMOS, ajustes.c:4452-4453); "Dolby Vision em MKV (experimental)" só LG webOS 4+, desligado por padrão (AJ_DV_MKV, ajustes.c:5399): na 2.0.3 troca TrueHD por E-AC-3/AC-3 no idioma ouvido para manter o Dolby Vision (docs/releases/2.0.3/NOTAS.md:17). "Converter DTS para a TV ouvir" (AJ_DTS_AC3, ajustes.c:4464): só LG, converte o DTS em estéreo (AAC) ou 5.1 (Dolby Digital) via src/dts/; na Samsung o app só AVISA quando a TV recusa DTS/TrueHD (video_tpk.c:130-141, #313; esconder as faixas é 2.0.5, fase 1). LIMITADO PELA TV/PLATAFORMA, não dá para prometer: na LG e na Samsung quem decodifica HDR/DV e faz o passthrough é a TV (uMS / AVPlay / player .tpk), o app só escolhe a fonte e o caminho; no Android o Media3 usa o passthrough por AudioCapabilities (NvPlayer.kt:374-386; video_android.c:27) e com bitstream o app não mexe no áudio (sem reforço de volume, sem velocidade: video_android.c:260, 836-841). Não há "robustez garantida" a implementar sem um caso de TV com log. (b) Estilo da legenda na hora: JÁ EXISTE no player. Botão Legendas (PLR_CC, player.c:189) › aba "Estilo" (legendasui.c:824; faixas.c:1489, corpo faixas.c:533-600), com Tamanho (50% a 250%, de 10 em 10, padrão 120%, faixas.c:590), Fonte OpenSubtitles, Cor, Opacidade (100/75/50/25%), Fundo (Nenhum, Escuro 25/50/75/100%), Posição (1 a 8), Borda (Nenhuma/Contorno/Sombra), Atraso, Negrito (Ligado/Desligado) e Restaurar padrão; OK cicla o valor e aplica na hora. Ressalvas: com ASS desenhado pelo app, fonte/cor/fundo/posição/borda/negrito ficam "Preservado pelo ASS" (faixas.c:536-541) e o tamanho vira escala; o negrito vale só para legenda externa/OpenSubtitles, a faixa que a TV desenha segue o peso da TV (faixas.c:571-572). Os valores são cíclicos (sem slider). (c) Atraso: legenda JÁ EXISTE: Legendas › linha "Atraso" (LR_ATRASO, legendasui.c:671-676), régua de -10 s a +10 s, 0,1 s por toque e 0,5 s segurando a seta (legendasui.c:444-451); a legenda secundária tem o seu; e "Sincronização automática" (AutoSync, linha LR_SYNC) que acerta pelo áudio (Ajustes "Sincronizar pelo áudio", AJ_LEG_SYNC_AUDIO, só onde o player entrega PCM, hoje Android: audsync.h:1-6). ATRASO DO ÁUDIO NÃO EXISTE (sem backend nem linha: grep audio_atraso/audioDelay vazio). (d) Proporção JÁ EXISTE: botão Proporção (PLR_ASPECTO) ou tecla 0, 8 modos: Original, Recortar, Esticar, Zoom leve, Zoom cinema, Zoom ultra, Ajustar altura, Ajustar largura (player.c:877-881, player.h:218-226); padrão em Ajustes › Reprodução › "Proporção padrão" (AJ_PROPORCAO_PADRAO). Em TVs que não recortam a imagem os modos de zoom caem para Original (player.c:1173); o .wgt Samsung (AVPlay/WebAssembly) não tem o zoom que corta a barra preta. Velocidade JÁ EXISTE: folha de Áudio, linha "Velocidade" (faixas.c:1448; "Playback speed" no i18n), 0,75x 1x 1,25x 1,5x 1,75x 2x (velocidade.c:7), por vídeo; disponível na LG, no .tpk e no Android; AUSENTE na Samsung .wgt (AVPlay só tem trick play inteiro, video_tizen.c:1531-1539) e bloqueada no Android com áudio em passthrough ("Indisponível com áudio pelo receptor", faixas.c:1449, #202). Salto de QUADROS (frame step/skip) NÃO EXISTE (só saltos de 10/30/60/120 s); o pedido é ambíguo (passo de quadro ou pular quadros?). FALTA, sugestao: atraso do áudio na 2.1 (começar onde o backend deixa, Android/Media3; LG e Samsung podem não ter API, a confirmar); passo de quadro em futuro (precisa de pausa + avanço por quadro nos 4 backends, sem prova de que a TV deixa); faixa de velocidade mais larga (0,25x-3x) e slider de tamanho/opacidade da legenda em 2.0.5 se o dono quiser (pequeno). Alvo geral = o mais cedo entre as peças que faltam (atraso do áudio, 2.1). Origem: pedido do autor somprakashpathakoneplus-cell. Rascunho (EN): The subtitle style (size up to 250%, color, opacity, background, bold), the subtitle delay ruler (-10 s to +10 s) and AutoSync are already in the player under Subtitles, tab Style; aspect modes are on the Aspect ratio button (or key 0) and playback speed (0.75x-2x) is on the Audio sheet. Audio delay and frame stepping are not there yet and are on the roadmap, while HDR, Dolby Vision and passthrough depend on what each TV or player decodes, so we can only choose the source and path. Which device are you on, so we know which of these you are missing? Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho. [alvo antes: 2.1; fechada no GH 10-15]
- **#398**: S95C, 2.0.3: transparência do contorno; pede opacidade em passos de 10 e redução de tamanho sem ciclo. Exemplos estão em issue externa não disponível offline; sem log. src/player.c:desenharLegendaPrincipal aplica alpha 1/.75/.5/.25 e passa borda ao PlainAssStyle; ASS autoral preserva estilo, texto simples recebe estilo do app. src/legendasui.c usa os mesmos quatro níveis. SUSPEITA: composição do contorno/alpha ou diferença de caminho ASS/texto, não comprovada. Tamanho já é limitado em 50–300% no código; isso não prova correção do contorno nem passos de 10% de opacidade. [alvo antes: 2.0.6; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#399**: Ugoos SK2 ligado por HDMI à Sony HT-CT790: AC3 toca, TrueHD 7.1 congela vídeo; pede preferir decoder do app. android/app/src/main/java/space/nuvio/nativelegacy/NvPlayer.kt:abrirMain usa DefaultRenderersFactory com EXTENSION_RENDERER_MODE_ON e fallback: plataforma/passthrough primeiro, FFmpeg para codec não suportado. android/app/build.gradle.kts já inclui decoder FFmpeg (inclui TrueHD); presença do decoder não prova fallback quando passthrough anuncia suporte e trava. SUSPEITA: capacidades HDMI/passthrough/AudioSink, sem reprodução. Seletor explícito não está neste caminho. [alvo antes: 2.0.6; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#401**: SUSPEITA nova (09/10, relato do autor: 'Updating 1%..99%' lento, instalação nova da 2.0.2 em segundos): dados do app dentro da pasta do app (.nuvio 359 MB/3980 arquivos na C9, incluindo cache.suspeito-1853 de 195 MB nunca apagado). LG C5 webOS 26, 2.0.32 para 2.0.3 pelo Homebrew Channel, 15+ min. NAO e regressao de tamanho (ipk 2.0.3 61,1 MB/1218 arquivos; 2.0.2 60,7 MB/1215). MEDIDO no ipk 2.0.4 (58,3 MB, 1190 arquivos, 99,5 MB descomprimido): o binário nuvio-proto tem 55,3 MB e VEM COM debug_info e símbolos (file: not stripped), 30,8 MB de seções .debug*/.symtab/.strtab que gzip reduz a 10,5 MB; a arte (art/) tem 39 MB em 1142 arquivos, já comprimida (JPG/PNG/WebP). Estimativa (nao medida): strip tira ~10 MB do ipk (58,3 para ~48 MB). O Homebrew Channel baixa o ipk inteiro para /tmp, confere sha256 e chama appInstallService/dev/install (services/service.ts); a barra de download usa o content-length do servidor, nao o ipkSize (tools/hb-repo.sh ja grava ipkSize). Onde estao os 15 min (download do GitHub pela TV, gravacao em /tmp, ou appInstallService instalando 1190 arquivos) NAO foi medido: SUSPEITA. Pedir ao autor a duracao de cada fase (a barra mostra Downloading/Verifying/Installing). Alvo 2.0.5. Revisão 2026-10-12: status curado preservado; ausência de commit citando a issue não invalida o desfecho. [alvo antes: 2.0.6; fechada no GH 10-15]
- **#403**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: 2.1; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#404**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. [alvo antes: 2.0.6; fechada no GH 10-15] Escrivao 10-15: fechada no GH com status aberta sem evidencia; triar.
- **#405**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. Escrivão 14/10 14:47: integrada via tpkui. [alvo antes: 2.0.6; fechada no GH 10-15]
- **#406**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais. | Escrivão 2026-10-14 16:11: GitHub CLOSED no snapshot de issues.json (2026-10-13).
- **#436**: Triagem 2026-10-12: sem conserto comprovado nos commits/notes locais.
- **#460**: Pede botões recarregar fonte e próximo episódio. src/player.c:player_proximo_episodio e player_janela_proximo já suportam próxima mídia e oferta no fim; player_pediu_recarregar/avAtivar (AV_B_RECARREGAR) atendem Live TV, não provam botão VOD recarregar. Episódios/posplay existem; botões VOD sempre acessíveis são ampliação de UI, relacionada a #403.
- **#473**: Snapshot 2026-10-14 10:15: estado CLOSED no snapshot do gh. fechada no GitHub sem conserto local identificado. Sem commit citando a issue. Escrivao 10-15: lancada sem tag era falso; volta a aberta.
- **#474**: Snapshot 2026-10-14 10:15: estado CLOSED no snapshot do gh. fechada no GitHub; barra do topo, ver tambem #461 (agente/205-menutopo integrado). Sem commit citando a issue. Escrivao 10-15: lancada sem tag era falso; volta a aberta.
- **#476**: Snapshot 2026-10-14 10:15: estado CLOSED no snapshot do gh. fechada no GitHub; #382 tratada por b212cec1 ja em integracao/2.0.5. Sem commit citando a issue. Escrivao 10-15: lancada sem tag era falso; volta a aberta.
- **#480**: Snapshot 2026-10-14 10:15: estado CLOSED no snapshot do gh. fechada no GitHub sem conserto local identificado. Sem commit citando a issue. Escrivao 10-15: lancada sem tag era falso; volta a aberta.
- **#482**: Flicker parcial (catalog wipe antes do discovery). Ligado a 067c0c35 e fix i488perfil-fix. Escrivao 10-15 22:49: merged via agente/205-i488perfil (f51aa3d76).
- **#511**: Criada 2026-10-14 (snapshot 14:46). Escrivao 14/10 19:1x: Fix 2e05dd94; j-revg-i511legnome-q OK 18:44; merge f0ab7c91 18:45. | Escrivao 2026-10-15 11:48: prompt p-205-i511legnome em /Volumes/ExternalSSD/tmp/rev; job terminado, sem job vivo [sync 2026-10-15: GitHub closed] [sync 2026-10-15: agente/205-i511legnome integrada]
- **#513**: review w BLOQUEIA 04:48 (j-revg-i513autoplay-w); fix x 03:59 antes de w; nenhum job rodando Escrivao 10-15 07:10: BLOQUEIA x->fix i513w 05:50 (adbaf0f9f)->review v OK 06:49; branch 6 ahead. | Escrivao 2026-10-15 07:50: merge 84add4581 em integracao/2.0.5 | Escrivao 10-15 08:50: BLOQUEIA w/x/y antes; v OK 06:49; merged. | Escrivao 15/10 09:48: Android 84add4581 Success; C9 rc=255 (TV nao confirmado). | Escrivao 2026-10-15 11:48: prompt p-205-i513autoplay* em /Volumes/ExternalSSD/tmp/rev; job terminado, sem job vivo | Escrivão 10/15 12:48: agente/205-i513autoplay integrado (84add4581, instalado Android) [sync 2026-10-15: GitHub closed] [sync 2026-10-15: agente/205-i513autoplay integrada] Escrivao 10-15 22:49: merged via agente/205-i513autoplay (84add4581).
- **#529**: Escrivão 10/15 12:48: novo no GitHub 2026-10-15 (dubbing filter), sem prompt de job. [sync 2026-10-15: agente/205-i529q em_revisao] [sync 2026-10-15 19:50: agente/205-i529q merged em integracao/2.0.5 (f8fbd3bd3)] Escrivao 10-15 22:49: merged via agente/205-i529q (f8fbd3bd3).

## Auditoria de validações

Quem validou o quê, em que ref, com que resultado e onde está a prova; mais recente primeiro. Registro começa em 2026-10-09; validações anteriores estão só nos commits e nas notas.

| Data | Quem | O que | Issues | Ref | Resultado | Evidência |
|---|---|---|---|---|---|---|
| 2026-10-15 12:51 | Magpie nv-revisao (revpr490-fix6) | Review fix6 PR #490 (Vitrine) em pr/490-fix | - | pr/490-fix 46e4df866 (+fixes) | passou | /Volumes/ExternalSSD/tmp/rev/j-revpr490-fix6.log (VEREDITO OK) |
| 2026-10-15 12:48 | Magpie nv-revisao (revg p2pusb-v4) | Review fix agente/205-p2pusb (P2P USB) v4 | #486, #505 | agente/205-p2pusb tip a1e7f4cfd | falhou | /Volumes/ExternalSSD/tmp/rev/j-revg-p2pusb-v4.log (VEREDITO BLOQUEIA); fix y 13:47 sem review |
| 2026-10-15 11:48 | Magpie nv-revisao (revg master1015-2) | Review merge agente/205-master1015 (fixes 12f000886, 7693608bf) | - | 7693608bf (agente/205-master1015) | passou | /Volumes/ExternalSSD/tmp/rev/j-revg-master1015-2.log (VEREDITO OK); merged 45161fb82 |
| 2026-10-15 10:55 | job-pi (master1015 teste) | Build mac.sh + git diff --check em agente/205-master1015; suites linkadas nao rodadas | - | agente/205-master1015 | inconclusivo | /Volumes/ExternalSSD/tmp/rev/j-205-master1015-teste.log |
| 2026-10-15 10:49 | job-pi magpie/group/nv-revisao | Revisao pr490 fix5 (Vitrine resume/pagination): BLOQUEIA | - | agente/205-pr490-fix (tip 1f8309961 + fix5) | falhou | /Volumes/ExternalSSD/tmp/rev/j-revpr490-fix5.log |
| 2026-10-15 10:48 | job-pi magpie/group/nv-revisao | Revisao p2pusb v3 (agente/205-p2pusb): BLOQUEIA | #486, #505 | agente/205-p2pusb (tip 90fb9cff7 + fixes) | falhou | /Volumes/ExternalSSD/tmp/rev/j-revg-p2pusb-v3.log |
| 2026-10-15 08:50 | job-pi group/nv-revisao | Revisao pr490 fix4: BLOQUEIA | - | pr/490-fix (rev antes de 1f8309961) | falhou | /Volumes/ExternalSSD/tmp/rev/j-revpr490-fix4.log |
| 2026-10-15 08:49 | job-pi group/nv-revisao | Revisao p2pusb v2 (fix USB P2P): BLOQUEIA | #486, #505 | agente/205-p2pusb (rev antes de 90fb9cff7) | falhou | /Volumes/ExternalSSD/tmp/rev/j-revg-p2pusb-v2.log |
| 2026-10-15 08:48 | job-pi group/nv-revisao | Revisao master1015 (merge 28 commits): BLOQUEIA | - | agente/205-master1015 (rev antes de 12f000886) | falhou | /Volumes/ExternalSSD/tmp/rev/j-revg-master1015.log |
| 2026-10-15 07:50 | escrivao (mapa keeper) | review #513 autoplay (magpie nv-revisao) apos fix w | #513 | adbaf0f9f / merge 84add4581 | passou | /Volumes/ExternalSSD/tmp/rev/j-revg-i513autoplay-v.log VEREDITO OK |
| 2026-10-15 07:50 | escrivao (mapa keeper) | review vidrotodo (magpie nv-revisao) | - | 9d5115556 / 9b4fe6776 | passou | /Volumes/ExternalSSD/tmp/rev/j-revg-vidrotodo.log VEREDITO OK |
| 2026-10-15 07:50 | escrivao (mapa keeper) | review pr/493-v3 (magpie nv-revisao) | #488, #482 | pr/493-v3 (65613c562) | passou | /Volumes/ExternalSSD/tmp/rev/j-revpr493-v3.log VEREDITO OK |
| 2026-10-15 07:10 | escrivao (mapa keeper) | sync do tracking: HEAD 799d025bd, branches/jobs/verdicts do snapshot 06:50 | - | 799d025bd | passou | escrivao/estado.md + docs/issues/mapa.json (atualizado_em 2026-10-15 07:10) |
| 2026-10-15 06:50 | job-pi group/nv-revisao (revisao) | Revisao pr490 fix3: VEREDITO BLOQUEIA (fome de fila nas fontes Vitrine) | #490 | pr/490-fix (fix c 3dae8f5c 06:03 antes) | falhou | /Volumes/ExternalSSD/tmp/rev/j-revpr490-fix3.log |
| 2026-10-15 06:50 | job-pi group/nv-revisao (revisao) | Revisao pr493 v3 (head do autor + fixes portados): VEREDITO OK | #488, #482 | pr/493-v3 (tip 65613c562) | passou | /Volumes/ExternalSSD/tmp/rev/j-revpr493-v3.log |
| 2026-10-15 06:49 | job-claude (merge) | Merge da integracao/2.0.5 em agente/205-vidrotodo preservando vidro e chrome | - | merge commit 9b4fe6776 (worktree agente/205-vidrotodo) | passou | /Volumes/ExternalSSD/tmp/rev/j-merge-205-vidrotodo.log |
| 2026-10-15 06:49 | job-pi group/nv-revisao (revisao) | Revisao i513 autoplay v: VEREDITO OK | #513 | agente/205-i513autoplay (fix i513w 05:50 antes) | passou | /Volumes/ExternalSSD/tmp/rev/j-revg-i513autoplay-v.log |
| 2026-10-15 06:49 | job-pi group/nv-revisao (revisao) | Revisao p2pusb v: VEREDITO BLOQUEIA | #486, #505 | agente/205-p2pusb (fix p2pusbw 05:51 antes) | falhou | /Volumes/ExternalSSD/tmp/rev/j-revg-p2pusb-v.log |
| 2026-10-15 05:49 | job-pi group/nv-revisao (revisao) | Revisao vidrotodo: VEREDITO OK | - | agente/205-vidrotodo | passou | /Volumes/ExternalSSD/tmp/rev/j-revg-vidrotodo.log |
| 2026-10-15 04:52 | job-pi group/nv-revisao | Revisao pr491 limpo r3: BLOQUEIA | - | pr/491-limpo (r3, antes de f8fe27fad) | falhou | /Volumes/ExternalSSD/tmp/rev/j-revpr491-limpo-r3.log |
| 2026-10-15 04:50 | escrivao (mapa keeper) | instalacao Android do HEAD 71c00499e | - | 71c00499e | passou | instalado.txt: android rc=0 Success (apk Nuvio-2.0.5-android.apk) |
| 2026-10-15 04:50 | escrivao (mapa keeper) | instalacao C9 do HEAD 71c00499e | - | 71c00499e | inconclusivo | instalado.txt: c9 rc=255 (sem saida util); confirmar na TV |
| 2026-10-15 03:53 | job-pi magpie/group/nv-revisao (revisao) | Revisao aspvolta: VEREDITO OK | - | agente/205-aspvolta (merged 04:47 em HEAD 71c00499e) | passou | /Volumes/ExternalSSD/tmp/rev/j-revg-aspvolta.log |
| 2026-10-15 03:16 | job-pi magpie/group/nv-revisao (revisao) | Revisao pr491 limpo r2: VEREDITO BLOQUEIA | #491 | pr/491-limpo | falhou | /Volumes/ExternalSSD/tmp/rev/j-revpr491-limpo-r2.log |
| 2026-10-15 03:14 | job-pi magpie/group/nv-revisao (revisao) | Revisao pr490 fix: VEREDITO BLOQUEIA | #490 | pr/490-fix | falhou | /Volumes/ExternalSSD/tmp/rev/j-revpr490-fix.log |
| 2026-10-15 03:05 | job-pi magpie/group/nv-revisao (revisao) | Revisao pr489 fix: VEREDITO OK | #489 | pr/489-fix (ahead 23, não mergeado) | passou | /Volumes/ExternalSSD/tmp/rev/j-revpr489-fix.log |
| 2026-10-15 03:04 | job-pi magpie/group/nv-revisao (revisao) | Revisao barra27 x: VEREDITO OK | - | agente/205-barra27 (merged 04:47 em HEAD cb0b1a070) | passou | /Volumes/ExternalSSD/tmp/rev/j-revg-barra27-x.log |
| 2026-10-15 02:52 | job-pi magpie/group/nv-revisao (revisao) | Revisao i513 autoplay x: VEREDITO BLOQUEIA | #513 | agente/205-i513autoplay (fix-i513x DONE 03:59 depois) | falhou | /Volumes/ExternalSSD/tmp/rev/j-revg-i513autoplay-x.log |
| 2026-10-15 02:47 | job-pi magpie/group/nv-revisao (revisao) | Revisao do fix pr/493-fix (PR #493, traz #488): VEREDITO BLOQUEIA | #488 | 4851f752d (pr/493-fix) | falhou | /Volumes/ExternalSSD/tmp/rev/j-revpr493-fix.log |
| 2026-10-14 23:15 | job-pi group/nv-revisao | Revisao pr498 limpo: OK | - | pr/498-limpo | passou | /Volumes/ExternalSSD/tmp/rev/j-revpr498-limpo.log |
| 2026-10-14 23:11 | job-pi magpie/group/nv-revisao (revisao independente) | Revisao final agente/205-i429hbs apos fixz 22:28: VEREDITO OK; linguas.sh e ajustes_idioma_disco.sh rc 0 | #429 | f59554839 (agente/205-i429hbs) | passou | /Volumes/ExternalSSD/tmp/rev/j-revg-i429hbs-y.log |
| 2026-10-14 22:35 | job-pi magpie/group/nv-revisao (revisao independente) | Revisao final agente/205-i513autoplay antes do fixy: VEREDITO BLOQUEIA | #513 | agente/205-i513autoplay antes do fixy 23:07 | falhou | /Volumes/ExternalSSD/tmp/rev/j-revg-i513autoplay-y.log |
| 2026-10-14 21:46 | job-pi magpie/group/nv-revisao (revisao final) | Revisao final agente/205-i488perfil apos fix 20:03: VEREDITO OK | #488 | d16de03ad (agente/205-i488perfil) | passou | /Volumes/ExternalSSD/tmp/rev/j-revg-i488perfil-z.log |
| 2026-10-14 10:26 | job-pi | Revisao PR487 v2 (linguas declaradas addon): OK | #481 | agente/205-pr487v2 (tip ac1ba9725) | passou | /Volumes/ExternalSSD/tmp/rev/j-rev-pr487-v2.log |
| 2026-10-14 01:42 | job-pi magpie/group/nv-revisao (revisao rev3) | Revisao agente/205-p2p486 apos fix-205-p2p486-3 (01:22): VEREDITO OK | #486 | 3f961a5ad (agente/205-p2p486) | passou | /Volumes/ExternalSSD/tmp/rev/j-rev3-205-p2p486.log |
| 2026-10-12 | Claude Sonnet 5.5 | Simkl scrobble do player (start/pause/stop com porcentagem, filme e episódio, só com Simkl vinculado): 401 desfaz o vínculo do perfil, 429/5xx/400 RATE_LIMIT abrem janela de espera sem retry, stop que falha cai no histórico. Rede falsa; sem TV nem conta Simkl real. | #459 | agente/205-simkl-scrobble | passou | tests/simkl_scrobble.sh rc 0 (FAIL antes: simkl_scrobble indefinido); regressão simkl_cw, visto, vistonao, simklauth, trakt_scrobble, player_visto_trailer rc 0; tools/build-local.sh build ok |
| 2026-10-12 | Codex | Triagem detalhada offline das 17 novas issues: corpo, últimos comentários e caminhos de código; 61 rascunhos para revisão do dono. Sem reprodução física ou leitura remota de logs. | #391, #396, #398, #399, #439, #440, #450, #451, #457, #458, #459, #460, #461, #462, #466, #467, #468 | agente/205-issues-novas | inconclusivo | docs/issues/TRIAGEM-2.0.5.md; docs/issues/rascunhos/respostas-triagem-2.0.5.md; snapshot .triagem/github-issues.json |
| 2026-10-12 | Codex | Validação dos artefatos da triagem detalhada: mapa.py/--check e publico.sh rc 0; 17 sem plataforma ?/nota placeholder, demais 402 preservadas; 61 textos de rascunho em 60 seções únicas sem URLs. | #391, #396, #398, #399, #439, #440, #450, #451, #457, #458, #459, #460, #461, #462, #466, #467, #468 | agente/205-issues-novas | passou | docs/issues/TRIAGEM-2.0.5.md (Provas da triagem detalhada); docs/issues/rascunhos/respostas-triagem-2.0.5.md; /private/tmp/triagem-publico-final.log |
| 2026-10-12 | codex gpt-6.1-sol | triagem | #1, #2, #3, #4, #5, #6, #7, #8, #9, #10, #11, #12, #13, #14, #15, #16, #17, #18, #19, #20, #21, #22, #23, #24, #25, #26, #27, #28, #29, #30, #31, #32, #33, #34, #35, #36, #37, #38, #39, #40, #41, #42, #43, #44, #45, #46, #47, #48, #49, #50, #51, #52, #53, #54, #55, #56, #57, #58, #59, #60, #61, #62, #63, #64, #65, #66, #67, #68, #69, #70, #71, #72, #73, #74, #75, #76, #77, #78, #79, #80, #81, #82, #83, #84, #85, #86, #87, #88, #89, #90, #91, #92, #93, #94, #95, #96, #97, #98, #99, #100, #101, #102, #103, #104, #105, #106, #107, #108, #109, #110, #111, #112, #113, #114, #115, #116, #117, #118, #119, #120, #121, #122, #123, #124, #125, #126, #127, #128, #129, #130, #131, #132, #133, #134, #135, #136, #137, #138, #140, #141, #142, #144, #145, #146, #147, #148, #149, #150, #151, #153, #154, #155, #156, #158, #159, #160, #161, #162, #163, #164, #165, #167, #168, #169, #170, #171, #172, #173, #174, #175, #176, #177, #178, #179, #180, #181, #182, #183, #184, #185, #186, #187, #188, #189, #190, #191, #192, #193, #194, #195, #196, #197, #198, #199, #200, #201, #202, #203, #204, #205, #206, #208, #209, #210, #211, #212, #213, #214, #215, #216, #217, #221, #222, #223, #224, #225, #226, #227, #228, #229, #231, #232, #233, #234, #235, #237, #238, #239, #240, #241, #242, #243, #244, #245, #246, #247, #248, #249, #250, #252, #253, #254, #255, #256, #257, #258, #260, #261, #262, #263, #265, #266, #267, #269, #270, #271, #272, #274, #275, #276, #277, #278, #279, #280, #281, #282, #283, #284, #286, #287, #288, #289, #290, #292, #293, #294, #295, #296, #297, #298, #300, #302, #303, #304, #305, #306, #307, #308, #309, #310, #311, #312, #313, #315, #316, #317, #318, #319, #320, #321, #322, #323, #324, #325, #326, #327, #328, #329, #330, #331, #332, #333, #334, #335, #336, #337, #338, #339, #340, #341, #342, #343, #344, #345, #346, #347, #348, #349, #350, #352, #353, #354, #355, #356, #357, #358, #359, #360, #361, #362, #363, #364, #365, #366, #367, #368, #369, #370, #371, #372, #373, #374, #378, #379, #382, #383, #384, #385, #386, #387, #388, #389, #390, #391, #392, #393, #394, #396, #397, #398, #399, #400, #401, #402, #403, #404, #405, #406, #407, #409, #410, #411, #412, #413, #414, #415, #416, #417, #418, #419, #420, #421, #422, #423, #424, #425, #426, #427, #428, #429, #430, #433, #434, #436, #437, #438, #439, #440, #450, #451, #457, #458, #459, #460, #461, #462, #466, #467, #468 | agente/205-issues-triagem / v2.0.5 b7c2c1f9 | inconclusivo | TRIAGEM-2.0.5.md |
| 2026-10-11 | Feanor | Build de diagnóstico v2.0.5-aspect.1, método B em Tizen 5 | #341, #426 | v2.0.5-aspect.1 | passou | relato de teste: método B OK |
| 2026-10-11 | mackojanko | Build de diagnóstico v2.0.5-aspect.1, métodos B e D na Q80A Tizen 6 | #341, #426 | v2.0.5-aspect.1 | passou | relato de teste: métodos B e D OK |
| 2026-10-09 23:50 | Claude Opus 5.5 (sessão de coordenação), TCL via adb | Ajuste 'Source order' presente | #400 | APK ce4426f9 | passou | Ajustes > Playback > More options: 'Source order: By quality / From the addon' com cena; captura t14 |
| 2026-10-09 23:50 | Claude Opus 5.5 (sessão de coordenação), TCL via adb | Biblioteca 129 salvos x 8 na grade | - | APK 710541ab -> 25e9c3aa | passou | antes: '[biblioteca] ocultar=1 excl_meta=121 grade=8'; depois do conserto 251d5e0f: 'excl_meta=0 grade=129', tela mostra '129 titles'; Codex sem achados (/Volumes/ExternalSSD/nv-203-tmp/rev/bib129-fix-codex.txt). Não era regressão: biblioteca.c idêntico ao da v2.0.3 |
| 2026-10-09 23:50 | Claude Opus 5.5 (sessão de coordenação), TCL via adb | Gráfico 'Seu progresso' remontando a cada chamada | - | APK b8d07d3d -> ce4426f9 | passou | antes: ~1900 linhas/s de '[vistoep] grafico' na página do Silo (cache nunca batia para id com :T:E); depois do conserto 6c6d1668: 3 linhas na abertura, FPS 60.0; teste FAIL->PASS do Codex |
| 2026-10-09 23:50 | Claude Opus 5.5 (sessão de coordenação), TCL via adb | IntroDB na TCL | - | APK fd86a673 | passou | log 23:01: '[intro] 2 marcadores (tmdb)' e '[credits] source=introdb'; os 0 marcadores eram de tt6048596, que a API responde 'media not found' |
| 2026-10-09 23:50 | Claude Opus 5.5 (sessão de coordenação), TCL via adb | QR do Discord colorido com logo: leitura | - | agente/204-novidades cecbe108 | passou | zxing-cpp leu os 3 códigos (Discord, Ko-fi, Patreon) na captura do cartão, inclusive borrado/escurecido e em metade do tamanho; capturas em /Volumes/ExternalSSD/tmp/nov204-shots4 |
| 2026-10-09 23:50 | Claude Opus 5.5 (sessão de coordenação), TCL via adb | Reprodução Dolby Vision na TCL (Silo T2E6) | #409 | APK ce4426f9 | passou | abre em 11,5 s, hdr=DolbyVision, 3 recriações da superfície registradas pelo [dvtrace], sem erro; saída limpa. Brilho não dá para medir por captura: o relato 'abre escuro' segue sem prova |
| 2026-10-09 23:50 | Codex gpt-6-astra (revisão) | Revisão de confirmação da rodada 4 do nível de GPU | #410 | agente/204-gpunivel410 52dbde71 | passou | /Volumes/ExternalSSD/nv-203-tmp/rev/410b-r4-codex.txt: P2 residual resolvido, sem achados novos |
| 2026-10-09 23:50 | Claude Opus 5.5 (sessão de coordenação), TCL via adb | Spotlight: busca 'silo' na TCL | - | APK b8d07d3d | passou | Silo 2023 (série do dono) virou melhor resultado; antes era o documentário de 2015; revisão Codex em 3 rodadas, última sem achados (/Volumes/ExternalSSD/nv-203-tmp/rev/spotlight-r3-codex.txt) |
| 2026-10-09 23:50 | Claude Opus 5.5 (sessão de coordenação), TCL via adb | Suíte completa (em andamento, árvore mudou no meio) | - | integracao/2.0.3.1 710541ab..25e9c3aa | inconclusivo | /Volumes/ExternalSSD/tmp/suite-204-7105.log; falhas até agora: cachearte-wasm (módulo node ausente), central_rotulos e colecoes_teto (não linkam src/dts; video.c já chamava dts_overlay_draw na v2.0.3, não é regressão) |
| 2026-10-09 23:50 | Claude Opus 5.5 (sessão de coordenação), TCL via adb | Vistos do Silo depois de reinstalar por cima | - | APK b8d07d3d | passou | '[vistoep] tt14688458: grafico ... vistos=22/30 T1=10/10 T2=5/10 T3=7/10': igual a antes da reinstalação, nada voltou |
| 2026-10-09 22:50 | Claude Opus 5.5 (sessão de coordenação) | Onboarding do Discord ativado | - | guild 1558285501067296922 | passou | 9 canais padrão, aviso de boas-vindas, 3 tarefas (rules, bugs-and-issues, announcements); 'Onboarding is Enabled'. Ícone: dono sobe à mão. |
| 2026-10-09 22:50 | Codex gpt-6-astra (revisão) | Revisão de confirmação da rodada 2 do #412 | #412 | agente/204-tpkpreso412 6ec34f52 | passou | /Volumes/ExternalSSD/nv-203-tmp/rev/412-r2-codex.txt: sem achados P1/P2; sem TV física |
| 2026-10-09 22:50 | Codex gpt-6-astra (revisão) | Revisão de confirmação da rodada 3 do #409 | #409 | agente/204-decoder409 c51dc9ec | passou | /Volumes/ExternalSSD/nv-203-tmp/rev/409-r3-codex.txt: sem achados P1/P2 |
| 2026-10-09 22:50 | Codex gpt-6-astra (revisão) | Revisão de confirmação da rodada 3 do nível de GPU | #410 | agente/204-gpunivel410 db3c1242 | falhou | /Volumes/ExternalSSD/nv-203-tmp/rev/410b-r3-codex.txt: P2 residual (quadros alternando 1200/900 ms nunca terminam a referência). Rodada 4 pedida. |
| 2026-10-09 22:50 | Codex gpt-6-astra (revisão) | Revisão de confirmação da rodada 3 do prefetch do próximo episódio | - | agente/204-proxprefetch 61b7db4d | passou | /Volumes/ExternalSSD/nv-203-tmp/rev/prox-r3-codex.txt: sem achados P1/P2 |
| 2026-10-09 22:50 | Codex gpt-6-astra (revisão) | Revisão do cartão de novidades com Discord + QR | - | agente/204-novidades c0c2d1a8 | passou | /Volumes/ExternalSSD/nv-203-tmp/rev/nov-r3-codex.txt: sem achados P1/P2; capturas em /Volumes/ExternalSSD/tmp/nov204-shots3 conferidas pela coordenação |
| 2026-10-09 22:50 | Claude Opus 5.5 (sessão de coordenação) | Testes-alvo na integração depois dos 4 merges | #409, #412 | integracao/2.0.3.1 fd86a673 | passou | decoder409, proxprefetch, addonslista, tpk-preso, stream_parser, android_decoder409.py, novidades_cartao: rc=0 |
| 2026-10-09 22:35 | Claude Opus 5.5 (sessão de coordenação) | Conferência mecânica da documentação de funções (identificadores citados existem no código) | - | agente/doc-funcoes 29a340b3 | passou | 490 de 493 identificadores existem; os 3 errados corrigidos à mão (player.md, trakt.md, video.md) |
| 2026-10-09 22:35 | Codex gpt-6-astra (revisão) | Revisão da rodada 2 do decoder 4K / espera do release | #409 | agente/204-decoder409 6d46e336 | falhou | /Volumes/ExternalSSD/nv-203-tmp/rev/409-r2-codex.txt: 1 P2 (volume volta a 100% após a espera) + 1 P3 de teste. Rodada 3 pedida. |
| 2026-10-09 22:35 | Codex gpt-6-astra (revisão) | Revisão da rodada 2 do nível de GPU adaptativo | #410 | agente/204-gpunivel410 020fb8f8 | falhou | /Volumes/ExternalSSD/nv-203-tmp/rev/410b-r2-codex.txt: 2 P2 (Mali-400 sem saída segura; interrupção perde reavaliação do legado). Rodada 3 pedida. |
| 2026-10-09 22:35 | Codex gpt-6-astra (revisão) | Revisão do conserto do player preso no .tpk | #412 | agente/204-tpkpreso412 71c7d615 | falhou | /Volumes/ExternalSSD/nv-203-tmp/rev/412-codex.txt: P1 (Voltar durante PrepareAsync encerra o processo em TV sã). Rodada 2 pedida. |
| 2026-10-09 22:35 | Claude Opus 5.5 (sessão de coordenação) | Servidor Discord reestruturado e convite permanente gerado | - | guild 1558285501067296922 | passou | 5 categorias e 10 canais novos conferidos pela lista de canais; convite https://discord.gg/9NWr6SHyzJ com 'nunca irá expirar'. Pendente: dono apagar os 10 canais antigos (Canais de Texto/Voz). |
| 2026-10-09 22:00 | Claude Opus 5.5 (sessão de coordenação) | Implantação da API de respostas do painel no ZimaOS | - | f9fa03b2 + painel v3 | passou | /api/saude {"ok": true}; containers versionados no ar, nuvio-painel antigo só parado; publicar.sh rc=0 |
| 2026-10-09 21:51 | Codex gpt-6-astra (revisão estática) | #409 rodada 1 (decoder 4K, bloqueio por codec, Voltar) | #409 | agente/204-decoder409 | falhou | /Volumes/ExternalSSD/nv-203-tmp/rev/409-codex.txt (3 P2: erro de áudio bloqueia vídeo; codec desconhecido = HEVC; falha <2160p não bloqueia) — rodada 2 em andamento |
| 2026-10-09 21:51 | Codex gpt-6-astra (revisão estática) | #410 parte 2 rodada 1 (nível de GPU volta se não ganhar fps) | #410 | agente/204-gpunivel410 | falhou | /Volumes/ExternalSSD/nv-203-tmp/rev/410b-codex.txt (3 P2: GPU fraca grava nível 1 sem medir 0; LG zera nível salvo e nunca reavalia; interrupção compara cenas diferentes) — rodada 2 em andamento |
| 2026-10-09 21:51 | Codex gpt-6-astra (revisão estática) | Pré-carregamento do próximo episódio rodada 1 | - | agente/204-proxprefetch | falhou | /Volumes/ExternalSSD/nv-203-tmp/rev/prox-codex.txt (3 P2: TTL expira antes da contagem; cancelado conta como mudo; cacheia sem plugins) — rodada 2 em andamento |
| 2026-10-09 21:30 | Codex gpt-6-astra (revisão estática) | #410 parte 1 e #411 | #410, #411 | cc8bbdcd, a066e6c8 | passou | /Volumes/ExternalSSD/nv-203-tmp/rev/410-codex.txt e 411-codex.txt (no findings) |
| 2026-10-09 21:00 | Claude Opus 5.5 (sessão de coordenação) | Leitura do código: o perfil depende só do Trakt (relato de usuário com Simkl) | - | integracao/2.0.3.1 | passou | src/app.c carregarPerfil -> trakt_perfil; sem trakt_ativo() o estado é "Trakt desconectado" |
| 2026-10-09 20:40 | Codex gpt-6-astra (revisão estática) | OpenSubtitles "sem resposta": versão só-leitura, rodadas 2 e 3 | - | agente/204-sostream | passou | /Volumes/ExternalSSD/nv-203-tmp/rev/sostream2-codex.txt e sostream3-codex.txt (P2 de leitura sem sincronização e 200 vazio corrigidos; último P2 = republicação pela descoberta, corrigido com atômico em fonte; limite documentado: sonda terminando depois do resumo) |
| 2026-10-09 20:35 | Claude Opus 5.5 (sessão de coordenação) | Teste do add-on só de legenda com a sonda terminando no meio da busca (reproduz "OpenSubtitles v3 não respondeu" da TCL) | - | ee2c64eb (FAIL) -> agente/204-sostream (PASS) | passou | tests/addonslista.sh: 2 pedidos de stream e motivo "OpenSubtitles v3 não respondeu" antes; 1 pedido e motivo vazio depois; autoplay_alvo e addonurl rc=0 |
| 2026-10-09 20:30 | Codex gpt-6-astra (pesquisa) | Estudo do app oficial NuvioTV Android 1.1.0-beta.5 (add-ons, plugins, ordem das fontes, player, abertura, HDR) | #400 | NuvioMedia/NuvioTV 6adf0251; APK sha256 09865e1a… | passou | /Volumes/ExternalSSD/tmp/nuvio-oficial/relatorio.md (oficial preserva a ordem interna do add-on; não recria a superfície no HDR; buffer padrão 50 s; plugins até 10 scrapers) |
| 2026-10-09 20:20 | Codex gpt-6-astra (revisão estática) | OpenSubtitles "sem resposta": 1a versão (manifesto lido no fio da busca) | - | 4f5780ba (agente/204-sostream, descartado) | falhou | /Volumes/ExternalSSD/nv-203-tmp/rev/sostream-codex.txt (P1 corrida no parser do manifesto; P2 tentativa em andamento; P2 cancelamento) |
| 2026-10-09 19:40 | Claude Opus 5.5 (sessão de coordenação) | Logs da KM7 SE do relato do Reddit (15 sessões) e contagem de quedas nativas no Android por versão (D1 ids 62000..66300) | - | v2.0.2 e v2.0.3 (Android) | inconclusivo | D1 66008/66017 (ExoPlayer AudioTrack.getTimestamp), 65736 (GC da ART); Android geral: 2.0.2 70/391 e 219/1192 com queda, 2.0.3 25/400; KM7: 2.0.2 4/49, 2.0.3 5/13; arquivos em /Volumes/ExternalSSD/tmp/km7 |
| 2026-10-09 19:30 | Claude Opus 5.5 (sessão de coordenação) | Busca da sessão da LG 32LJ600B (webOS 3) do relato do Reddit no D1 | - | v2.0.3 (webOS) | inconclusivo | D1 id>64500: nenhuma linha com LJ600; webOS 3.9.3 na 2.0.3: 65SJ800V (65304, 65363, 65374 arranque queda), OLED55B7P (65317), 49UJ6300 (66016) |
| 2026-10-09 19:25 | Claude Opus 5.5 (sessão de coordenação) | Instalação da 2.0.4 (com KM7, consertos do #408) na TCL e na C9: abre, linha [tv] app=2.0.4, sem queda | - | APK de 06419b6b; ipk de bed3534c | passou | TCL logcat: "[player] recriar superficie no HDR: sim" (TCL segue recriando); C9 /tmp/nuvio.log: OLED65C9PSA webos-4.10.2 app=2.0.4, 0 SIGSEGV |
| 2026-10-09 19:20 | Codex gpt-6-astra (revisão estática) | Consertos das revisões do #408 (gancho webOS, nyx x starfish, aviso do modelo de pôster) | #390 | fc61b3aa (agente/204-ultranit) | passou | /Volumes/ExternalSSD/nv-203-tmp/rev/ultranit-codex.txt (no findings); testes FAIL->PASS: webosver, poster_modelo_longo |
| 2026-10-09 19:10 | Claude Opus 5.5 (sessão de coordenação) | #387: testes do segurar e testes que compilam as telas tocadas | #387 | 0f98379f (agente/205-segurar) | passou | tests/segurar.sh (caso "mesmo quadro" falha na r1); ilha_shot, ctxmenu_contract, busca_debounce, avisos_toast_shot, salvospainel_perf rc=0; ctxlista falha igual na base |
| 2026-10-09 19:05 | Codex gpt-6-astra (revisão estática) | Conserto KM7: Amlogic não recria a superfície de vídeo | - | 98929593 (agente/204-km7) | passou | /Volumes/ExternalSSD/nv-203-tmp/rev/km7-codex.txt (no findings; nota: log de "recria" enganoso, corrigido em ccd37565; chama de mitigação candidata, sem prova em aparelho) |
| 2026-10-09 19:05 | OpenCode glm-5.3 (revisão estática) | Conserto KM7: Amlogic não recria a superfície de vídeo | - | 98929593 | inconclusivo | /Volumes/ExternalSSD/nv-203-tmp/rev/km7-opencode.txt (parou sem veredito) |
| 2026-10-09 19:05 | Claude Opus 5.5 (sessão de coordenação) | Vídeo do #387 (menu do cartaz abre em vez da página do título) e 30 logs .tpk recentes procurando OK sem mapa | #387 | v2.0.2/v2.0.3 (.tpk) | inconclusivo | vídeo do autor: menu em 3 de 4 OKs, <0,5 s, capas já carregadas; logs D1 .tpk: só volume/power/exit sem mapa, o OK está mapeado; NV_HOLD_MS=700 (layout.h:119); a Biblioteca não loga o segurar |
| 2026-10-09 19:00 | Codex gpt-6-astra (revisão estática) | #387 r1: segurar OK com espera após pausa longa | #387 | 03213598 (agente/205-segurar) | falhou | /Volumes/ExternalSSD/nv-203-tmp/rev/387-codex.txt (P2 relógio compartilhado entre telas; P2 menu/guia/spotlight de fora; P3 log a cada quadro) — corrigidos em 0f98379f |
| 2026-10-09 19:00 | GitHub Copilot (revisão do PR) | Revisão do PR #408 | #390 | 68e89c70 | falhou | PR #408: card de novidades ainda 2.0.3; webosver.c:197 nyx descartado por erro no starfish (corrigido e6da2772); ajustes.c:3414 aviso errado do modelo longo (corrigido fc61b3aa) |
| 2026-10-09 18:58 | Claude Opus 5.5 (sessão de coordenação) | Teste do conserto KM7 (RecriaHdr): Amlogic e MStar dispensados, TCL c2.mtk recria | - | f354b7f6 (FAIL) -> 98929593 (PASS) | passou | tests/android_recria_hdr.py; tests/android_window_layout.py segue passando |
| 2026-10-09 18:50 | Claude Opus 5.5 (sessão de coordenação) | Log ABB125 do #405 (busca rápida sem Silo) lido | #405 | v2.0.3 (.tpk) | inconclusivo | D1 registro 66134: nenhuma linha da busca rápida |
| 2026-10-09 18:43 | Claude Opus 5.5 (sessão de coordenação) | Build do .ipk da LG 2.0.4 com as chaves do local.properties (o primeiro saiu sem chaves e foi descartado) | - | 68e89c70 | passou | /Volumes/ExternalSSD/tmp/b204-lg.log sem "aviso: vazio"; space.nuvio.native.legacy_2.0.4_arm.ipk em nv-2031-int |
| 2026-10-09 18:40 | ultrareview (nuvem, multiagente) | Revisão do PR #408 (2.0.4 sobre v2.0.3) | #344, #372, #378, #390, #392, #402 | 68e89c70 | passou | https://github.com/iqui27/nuvio-native-legacy/pull/408 — sem bug; 3 nits: gancho nv_webos_testar sem guarda (corrigido da6bece2), guarda duplicada em catalogo.c e separadores do #402 em 3 lugares (refatoração, 2.0.5) |
| 2026-10-09 18:38 | Claude Opus 5.5 (sessão de coordenação) | Integração da 2.0.4 depois dos merges do #402 e das notas: testes-alvo | #402 | 68e89c70 (integracao/2.0.3.1) | passou | tests/fhd402.sh e tests/stream_parser.sh rc=0 no worktree nv-2031-int |
| 2026-10-09 18:36 | OpenCode glm-5.3 (revisão estática) | Revisão r5 do #402 (faixa E2 80..AF) | #402 | b781dae1 | passou | /Volumes/ExternalSSD/nv-203-tmp/rev/402r5-opencode.txt (dois P3: borda U+2BFF sem teste e comentário; resolvidos em eb9f3226) |
| 2026-10-09 18:33 | Codex gpt-6-astra (revisão estática) | Revisão r5 do #402 (faixa E2 80..AF) | #402 | b781dae1 | passou | /Volumes/ExternalSSD/nv-203-tmp/rev/402r5-codex.txt (no findings) |
| 2026-10-09 18:30 | Claude Opus 5.5 (sessão de coordenação) | Teste do #402 com letras de U+2C00..U+2FFF antes de FHD: falha antes do conserto, passa depois; stream_parser segue passando | #402 | 077fd275 (teste, FAIL) -> b781dae1 (conserto, PASS) | passou | tests/fhd402.sh rc=1 em 077fd275, rc=0 em b781dae1; tests/stream_parser.sh rc=0 |
| 2026-10-09 18:25 | Claude Opus 5.5 (sessão de coordenação) | Relato do Reddit "2.0.3 trava ao tocar série (Android TV)": varredura das 40 sessões 2.0.3 Android mais recentes no D1 | - | v2.0.3 | nao-reproduziu | D1 registro 66138..66221; quadros de 10-75 s (66213, 66219) são app em segundo plano (ev=); 66144 pausou e perdeu a superfície; erros 2001/2004 em 66220 são canais ao vivo; o autor não estava nessa amostra: é a KM7 (ver entrada de 19:40) |
| 2026-10-09 18:15 | Claude Opus 5.5 (sessão de coordenação) | Teste do APK 2.0.4 na TCL do dono: Home, página do título, The Mentalist S1E2 tocando 3+ min, avanço no vídeo, relógio da legenda e RSS estáveis | - | APK 2.0.4 de 18:03 (integracao/2.0.3.1 antes do merge do #402) | passou | logcat da TCL 192.168.1.128 (sem queda no buffer de crash; [relogio] estável; rss ~600 MB) |
| 2026-10-09 17:42 | Codex gpt-6-astra (revisão estática) | Revisão r4 do #402: prefixo só de símbolo antes de FHD | #402 | 824e9e9d (agente/204-fhd402) | falhou | /Volumes/ExternalSSD/nv-203-tmp/rev/402r4-codex.txt (P2: letras georgianas U+2D0B/U+2D04 aceitas como símbolo); conserto em b781dae1 |

## Fora do GitHub (Reddit)

### Relatório Shield (22 itens, Reddit)

- Plataforma: Android (Shield)
- Status: desconhecido
- Release: 2.0.3 (parcial)
- Conserto: d30344ab/96ccdab1 (#6 Shield: tetos do Android e paginação da watchlist do Trakt, 2.0.3; 7b9e641a, o revert do 96ccdab1, não está em nenhuma branch); 5413db8a (Shield #19: guia "Mostrando N de M canais", Android guarda 3000 canais/128 categorias, 2.0.3); 6b3aaae1 (Shield 318.3: card do Continuar com o still do episódio; filme assistido diz Reproduzir, 2.0.3); eabe3afd/b03aa2f0/35421d1a/73c4ab4c (#266/#332: curl fora do fio principal, /dev/urandom, login por HttpURLConnection, 2.0.3)
- Próximo passo: mapeado em 09/10: o relatório de 20 itens do hboinay (provável versão do de 22) está nas entradas reddit-hboinay-NN abaixo; postar o rascunho docs/issues/rascunhos/reddit-hboinay-2026-10-09.md
- Notas: O texto dos 22 itens NÃO está no repositório nem nas issues. Só três grupos de commits citam "Shield". Mapeamento item a item: desconhecido. ATUALIZAÇÃO 09/10: chegou um relatório de 20 itens do mesmo autor e da mesma build (teste-318.3); cada item virou uma entrada reddit-hboinay-NN (ou uma nota em #353/#356). Falta confirmar se é o mesmo relatório dos 22.

### Relatório Android TV (9 itens) = #369

- Plataforma: Android
- Status: consertada-nao-lancada (parcial)
- Release: 2.0.3
- Conserto: 712557ca (tailandês); 5f04c36b (ocultar não lançados)
- Próximo passo: responder no #369 e listar os 7 itens sem commit
- Notas: Só 2 dos 9 itens têm commit identificado. #369 não tem resposta nossa.

### Samsung: faixa de áudio escondida pela TV (DTS/TrueHD)

- Plataforma: Samsung .tpk (e .wgt na fase 1)
- Status: consertada-nao-lancada (casar faixa) + planejado (aviso)
- Release: 2.0.3 (casar faixa); 2.0.5 (aviso, fase 1)
- Conserto: 4112f3f7 + efd1014b (agente/203-casarfaixa, em integracao/2.0.3): TV com menos faixas que o MKV casa por idioma, codec e canais; 0ef8ddc9 (agente/204-samsung-dts): só o plano docs/plans/samsung-dts.md, sem código; f45ddc14 (2.0.2, #313): aviso só quando TODAS as faixas são recusadas
- Próximo passo: implementar fase 1 do plano (aviso + faixa cinza na folha de áudio, 1,5-2 dias); fase 2 (ESPlayer) depende de teste de assinatura Partner em TV
- Notas: Relato do Reddit: 2 faixas no box, 1 na TV. Relacionado a #313. ESPlayer exige privilégio Partner (no 4/5 isso já quebrou a instalação, 118014).

### Samsung: pacote de 80 GB

- Plataforma: Samsung
- Status: desconhecido
- Release: -
- Conserto: nenhum encontrado
- Próximo passo: perguntar ao dono o que é
- Notas: Nada encontrado em commits, docs, notas de release ou issues (buscas por "80 GB"/"80GB"). Não preencher sem a fonte.

### Home: mais de 40 fileiras e ordem dos catálogos (hboinay item 1)

- Plataforma: Android
- Status: precisa-log
- Release: alvo sugerido 2.0.5 (teto) / sem alvo (ordem)
- Conserto: 8e058817 (agente/204-fileiras-android, não integrado): Android com fileiras ilimitadas, teto 200
- Próximo passo: teto: integrar agente/204-fileiras-android na 2.0.5 (já no roadmap); ordem: pedir log
- Notas: TETO por desenho: CAT_FIL_MAX 40 (catalogo.h:394) e FIL_LIMITE_MAX 40 (fileiras.h:60); a branch 204-fileiras-android o sobe para 200 só no Android. ORDEM: não há como afirmar sem log; ligação provável com #358 (catálogos que a Home move para "Fora da Home", 29c640b7 em agente/204-conta-catalogos). Pedir o log para ver a ordem da conta contra a da Home. Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### Continuar com fonte Nuvio mostra 5-6 itens e depois 1; a conta tem 30 (hboinay item 3)

- Plataforma: Android
- Status: precisa-log
- Release: 2.0.3 (parcial, 507d3a6b); teto alvo sugerido 2.0.5
- Conserto: 507d3a6b (v2.0.3): fonte Conta semeia o "a seguir" mesmo com Trakt/Simkl, descarta vistos velhos
- Próximo passo: pedir log da 2.0.3 (Ajustes > Sobre > Enviar log)
- Notas: O teto de cada fonte é CONT_MAX 12 (descoberta.c:2605), então 30 na conta nunca aparecem inteiros, e o "a seguir" ainda passa por filtros (episódio visto em outra fonte, série parada há mais de 60 dias com Trakt/Simkl vinculado, descoberta.c). 5-6 e depois 1 pode ser o filtro ou a lista de vistos da conta ainda não puxada (mesma suspeita do #356). Sem log é suspeita. Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### Com Trakt e Simkl vinculados, Continuar mostra vistos antigos mesmo com fonte Nuvio (hboinay item 4)

- Plataforma: Android
- Status: precisa-log
- Release: 2.0.3 (parcial, 507d3a6b + 005b3095 + c0f4054c)
- Conserto: 507d3a6b (v2.0.3); 005b3095 (v2.0.3): desmarcar ganha; c0f4054c (v2.0.3): a seguir recua para o primeiro episódio desmarcado
- Próximo passo: pedir log da 2.0.3 com a fonte em Conta Nuvio
- Notas: Na leitura do código a fonte Conta não consulta Trakt nem Simkl (descoberta.c: querTrakt só em Trakt/Todas, querSimkl idem), então os "vistos antigos" teriam de vir de continuarLocal ou do "a seguir" da conta; a 2.0.3 descarta o visto em outra fonte e o parado há mais de 60 dias. Não reproduzido. Sem log é suspeita. Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### Biblioteca: Trakt 213 e Simkl 483 de 1600+ (hboinay item 6)

- Plataforma: Android
- Status: aberta
- Release: alvo sugerido 2.0.5
- Conserto: d30344ab (v2.0.3): watchlist do Trakt paginada; 96ccdab1 (v2.0.3): tetos do Android voltam a 400/300; 7b9e641a (órfão, nenhuma branch): reverte o 96ccdab1 e devolve os tetos 2000/1000 no Android
- Próximo passo: decidir o 7b9e641a: reaproveitá-lo na 2.0.5 (Android com teto 2000 Trakt e 1000 Simkl) ou manter o teto de hoje
- Notas: Na 2.0.3 o Trakt pagina (?page=&limit=500, trakt.c:1873-1960) mas para em TRAKT_LISTA_MAX 400 (trakt.h:133) em todas as plataformas, e o Simkl em SIMKL_PTW_MAX 300 (simkl.h): quem tem 1600+ passa a ver 400/300 em vez de 213/483. O teto maior só existe no commit órfão 7b9e641a. O Simkl não pagina (all-items numa resposta), então o teto é só memória. Nota da 2.0.3 diz "até 400 itens" e vale para todas as TVs. Relacionado: #393. Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### Selo de assistido não aparece em séries nos cartazes (só em filmes) (hboinay item 7)

- Plataforma: Android
- Status: aberta
- Release: alvo sugerido 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: ler o /sync/watched/shows do Trakt uma vez (como o mapa de filmes vistos em trakt.c:1317) para marcar as séries inteiras
- Notas: cat_visto (catalogo.c:432) devolve 0 para série sem histórico; o histórico da série só é escrito quando a PÁGINA do título é aberta (extras.c:586-597, contadores de /shows/<id>/progress/watched) ou ao marcar à mão. Cartaz de série em fileira não tem selo antes disso. Vale para todas as plataformas. #352 (agente/204-biblioteca-visto, 402e2438) só leva o selo para a Biblioteca. Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### Trailers do destaque e da ficha começam pequenos e depois enchem a tela (hboinay item 8)

- Plataforma: Android
- Status: precisa-log
- Release: alvo sugerido 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: reproduzir na Shield com o log de trailer; ver a ordem posição/recorte no NvPlayer.kt
- Notas: No Android o Kotlin posiciona o recorte (video_android.c:9, video_recorte_fonte=1) e trailer_mostra_video (trailer.c:467) só espera o recorte no .tpk; no Android o plano aparece assim que toca. O pedido é esconder até o recorte assentar, como o .tpk faz (trailer.h:54-60). Hipótese, não medida. Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### Cartazes escuros demais; pede desligar o brilho animado (hboinay item 11)

- Plataforma: Android
- Status: aberta
- Release: alvo sugerido 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: dois ajustes pequenos: luz do foco (varredura) liga/desliga e força do véu do cartaz
- Notas: A varredura de luz no foco (revela_varre, revela.h:83, home.c:5426) só some com "Animações reduzidas" global (anim_politica_reduzida), que também tira todo o resto do movimento; não há ajuste próprio. O escurecimento vem do véu sobre a arte (veusDoCard, home.c:4844) e do efeito de profundidade (AJ_PROF_*), este com liga/desliga em Ajustes. Contorno de hoje: Animações reduzidas e Efeito de profundidade desligados. Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### Selos: URL do manifesto diz "isso não é um JSON de selos"; não usa os selos da conta (hboinay item 12)

- Plataforma: Android
- Status: precisa-log
- Release: alvo sugerido 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: pedir a URL do manifesto e o log; testar o JSON no selospacote_adicionar
- Notas: A mensagem sai de SELOS_ERR_JSON (ajustes.c:3047; selospacote.c:639-648): corpo que não é objeto/string JSON ou resposta maior que 4 MB. Os selos da conta só vêm de features.stream_badge_settings.stream_badge_rules no blob de ajustes do perfil (selospacote.c:588-612): se a conta não tem esse campo, nada é usado. Sem a URL e o log não dá para saber qual dos dois é. Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### Só um servidor Jellyfin e um Emby por perfil (hboinay item 13)

- Plataforma: Android
- Status: por-desenho
- Release: alvo sugerido 2.1
- Conserto: nenhum encontrado
- Próximo passo: avaliar vários servidores por tipo (hoje instância 0 = Jellyfin, 1 = Emby, mais Plex)
- Notas: Por desenho: um servidor por tipo e por perfil (jellyfin.h:5-10, JfInst; token em jellyfin-p<N>.txt). Vários exigem lista de instâncias, ids com namespace por servidor (jfid.h) e Home com mais fileiras (SRV_FIL_MAX, servidores.h). Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### Jellyfin/Emby fora das listas de fontes: add-ons permitidos, ordem, lista (hboinay item 14)

- Plataforma: Android
- Status: aberta
- Release: alvo sugerido 2.1
- Conserto: nenhum encontrado
- Próximo passo: decidir como o servidor pessoal entra em fonteregra (permitidos/ordem)
- Notas: Para título de servidor pessoal a lista de fontes é só a do servidor (addons.c:493-510, servidores_fontes_colher + stream_definir_lista) e as regras de add-on permitido e ordem (fonteregra.h) só conhecem add-ons/plugins. O servidor vem sempre primeiro por desenho (9025bfa9). Confirmar com o autor o que ele esperava ver. Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### Estilo da legenda só para a segunda legenda nos Ajustes; sincronia automática só por áudio (hboinay item 16)

- Plataforma: Android
- Status: aberta
- Release: alvo sugerido 2.0.5 (estilo)
- Conserto: nenhum encontrado
- Próximo passo: estilo da principal em Ajustes; perguntar quais legendas ele testou na sincronia
- Notas: Ajustes tem Tamanho/Cor/Fundo/Borda só da segunda legenda (ajustes.c:1268-1272; a dica diz que a da principal "é definida no player"). Sincronia: além do áudio (AJ "Sincronia por áudio") existe o AutoSync por texto (legsync.c/autosync.c, em v2.0.3), mas só em legenda EXTERNA, tendo como referência a faixa de texto do MKV (legsync.h:11-16); legenda embutida ou nativa fica indisponível. Pode não ser bug, e sim limite. Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### Só uma conta Xtream por perfil (hboinay item 17)

- Plataforma: Android
- Status: por-desenho
- Release: alvo sugerido 2.1
- Conserto: nenhum encontrado
- Próximo passo: avaliar várias contas (lista por perfil, canais com id por conta)
- Notas: Por desenho: um servidor, usuário e senha por perfil (xtream.h, arquivo xtream-p<N>.txt; id "xtream:<stream_id>" sem conta). Várias contas pedem mudar o id do canal e o cadastro nos Ajustes. Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### IPTV sem VOD/séries do Xtream (hboinay item 18)

- Plataforma: Android
- Status: por-desenho
- Release: futuro
- Conserto: nenhum encontrado
- Próximo passo: registrar como pedido; não planejado
- Notas: O módulo só lê get_live_categories e get_live_streams (xtream.c, xtream.h); não existe get_vod_* nem get_series. Seria um módulo novo (catálogo + ficha + resolução de URL /movie/ e /series/). Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### TV ao vivo só com canais do Reino Unido e EUA, de dezenas de países (hboinay item 19)

- Plataforma: Android
- Status: precisa-log
- Release: 2.0.3 (provável, 5413db8a)
- Conserto: 5413db8a (v2.0.3): guia diz "Mostrando N de M canais"; Android guarda 3000 canais e 128 categorias; 45ae240c (teste)
- Próximo passo: pedir para atualizar para a 2.0.3 e, se persistir, o log
- Notas: Antes da 2.0.3 o guia cortava em 900 canais e 48 categorias sem avisar e sumia categorias inteiras (45ae240c); com a lista ordenada isso deixaria só os primeiros países. Hipótese forte, sem confirmação do autor. Se persistir na 2.0.3, a fonte passa a ser o catálogo do add-on (Ajustes > Guia, canais por add-on, #283). Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### TV ao vivo com fps variando entre 24 e 50 (33, 34, 48) (hboinay item 20)

- Plataforma: Android
- Status: precisa-log
- Release: alvo sugerido 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: pedir log com o medidor de desempenho ligado durante um canal ao vivo
- Notas: Sem leitura de código que explique. O medidor (AJ_MEDIDOR) mede quadros da interface, não do vídeo; o vídeo ao vivo roda no Media3 (video_android.c). Pode ser decodificação, rede ou troca de taxa da tela. Só dá para dizer com o log. Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### Canais piscando em verde na TV ao vivo (hboinay item 21)

- Plataforma: Android
- Status: precisa-log
- Release: alvo sugerido 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: pedir log com o canal que pisca e a saída de vídeo da Shield
- Notas: Tela verde em vídeo costuma ser decodificador/superfície (buffer YUV sem quadro). No Android a superfície é recriada em partida HDR e mudança de aspecto (notas da 2.0.3). Não localizado no código; sem log é suspeita. Origem: Reddit u/hboinay, NVIDIA SHIELD TV Pro 2019, build teste-318.3 (base 2.0.2), 09/10/2026.

### Painel lateral (side bar) pisca depois da atualização (Ragingmuncher)

- Plataforma: desconhecida
- Status: precisa-log
- Release: sem alvo (aguardando informação do autor); relatado na 2.0.3
- Conserto: nenhum encontrado
- Próximo passo: responder no Reddit pedindo: modelo da TV e pacote (.tpk/.wgt/versão do webOS); se o ponteiro estava na tela ou só setas; em qual tela/item e se já acontecia na 2.0.2; enviar o log logo depois, com a hora aproximada (Ajustes > Sobre > Enviar log); vídeo pelo celular se for só visual
- Notas: Relato: o painel lateral (side bar) pisca depois da atualização para a 2.0.3. TV e plataforma desconhecidas. ANÁLISE de 09/10: em ~310 logs da 2.0.3 (webos, tizen, tizen-tpk, a partir de 09/10/2026 15:00 UTC) não há laço de troca de tela (no máximo 18 linhas "[transicao] tela" por log, espaçadas por segundos). O painel lateral não escreve linha de log, então isso só descarta um laço, NÃO um pisca visual. src/menu.c não mudou entre a v2.0.2 e a v2.0.3; src/ponteiro.c ganhou 11 linhas (só aparo de alvo do ponteiro). SUSPEITA, não provada: Magic Remote com o ponteiro sobre o painel com o fundo de vidro. Sem log ou vídeo é suspeita. Origem: Reddit u/Ragingmuncher, versão 2.0.3, 09/10/2026.

### Reddit (DueResult1983): 2.0.3 trava ao tocar série no Android TV; caixa inteira trava, teve de desinstalar

- Plataforma: Android (Mecool KM7 SE, Amlogic, Android 12)
- Status: aberta
- Release: -
- Conserto: nenhum encontrado
- Próximo passo: reproduzir com vídeo 4K HEVC numa caixa Amlogic de 2 GB; medir no logcat as linhas "[player] superficie" da 0b8f9290 (recriação da superfície fora do fio principal) e decidir se a 2.0.4 volta ao caminho antigo nesse tipo de aparelho
- Notas: Pessoa nuvio:266479b1 (registros D1 66153..65330). Queda nativa em 5 de 13 sessões 2.0.3 contra 4 de 49 na 2.0.2 (no geral do Android a 2.0.3 MELHOROU: 25/400 contra 70/391). Pilhas: 65736 = GC da ART (HeapTaskDaemon, ConcurrentCopying, ponteiro nulo); 66017 relata a queda da sessão 66008 = fio ExoPlayer:Playback em AudioTrack.getTimestamp -> GetPrimitiveArrayCritical (SIGSEGV 0xfffffffe); 66021/66025/66054 sem tombstone. As duas pilhas são dentro da ART, padrão de heap Java corrompido ou memória esgotada. Memória: avisos onTrimMemory 5/10/15/20/40 em várias sessões, teto de textura já em 128 MB. Sessão 66008: série tt26545992 T1E1 4K 3840x1920 retomando em 22%; a superfície é recriada 2 vezes logo no início (3x "primeiro quadro") e o log para de repente sem linha de FPS. SUSPEITA principal: 0b8f9290 (2.0.3) manda MSG_SET_VIDEO_OUTPUT null ao renderer antes de recriar a superfície; em Amlogic isso pode manter/reabrir o decoder 4K e somar memória. Outras mudanças do player Android entre v2.0.2 e v2.0.3: 2a351b76 (aspecto espera superfície estável), c3726d2d (capítulos MKV por fio lateral), ce7bcd0f (retomada). Seekr já existia na 2.0.2, descartado como novidade.

### Reddit (nossascamerasip): LG 32LJ600B (webOS 3), 2.0.3: trailer com imagem e sem som; filme com som e sem imagem

- Plataforma: LG webOS 3
- Status: precisa-log
- Release: -
- Conserto: nenhum encontrado
- Próximo passo: pedir o código do log (Ajustes > Sobre > Enviar registro) logo depois de abrir um filme; com o log, ver a linha [tv] e o caminho do vídeo (DTS, hdr, player) no webOS 3
- Notas: A mesma pessoa usa a C1 sem problema. Em 09/10 não há sessão da 32LJ600B no D1 (busca por LJ600 e por host=webos-3 desde o id 64500); há outras webOS 3.9.3 na 2.0.3 com "arranque queda" (65SJ800V 65304/65363/65374, OLED55B7P 65317). A 2.0.4 já leva a queda do webOS 3 no 2º vídeo (09ce19a0) e a leitura estrita da versão (webosver), mas nada prova que seja este defeito.

### Pedido: "Meu perfil" e status funcionando sem Trakt (usa Simkl e dá erro); sugere Telegram/Discord

- Plataforma: all
- Status: aberta
- Release: -
- Conserto: nenhum encontrado
- Próximo passo: 2.0.5: perfil com Simkl (stats do Simkl, ou montado do histórico local + Simkl) e mensagem certa quando só o Simkl está ligado; Telegram/Discord é decisão do dono
- Notas: LIDO 09/10: a tela Meu perfil chama só trakt_perfil (src/app.c carregarPerfil, ~350) e, sem Trakt, mostra "Trakt desconectado. Vincule a conta para ver seu perfil." (app.c ~3273) mesmo com o Simkl conectado. perfil.c só mostra Simkl na linha de contas (~770). "Status" do relato não está claro (status de amigos/assistindo agora também vem do Trakt).

### Estudo do fork ysosrs123/NuvioTV-Fork (pré-carregamento e outras melhorias)

- Plataforma: all
- Status: aberta
- Release: -
- Conserto: nenhum encontrado
- Próximo passo: pré-carregar fontes do próximo episódio em agente/204-proxprefetch (rodada 2); pós-créditos, lista de fontes mortas e ytId na 2.0.5
- Notas: Relatório do MiniMax M3 em /Volumes/ExternalSSD/tmp/nuvio-fork-ysosrs/relatorio.md, CONFERIDO pela coordenação: 3 afirmações erradas do M3 (já temos Preparar fonte ao abrir, foco fixo quando a lista cresce, medição passiva/StreamFit). Verdadeiros: preload do próximo (#3795), segurar cartão até pós-créditos, failover com fontes mortas persistentes, autoplay com ytId, 4 conexões fixas no ParaleloDataSource.

### Biblioteca mostra 129 salvos e 8 títulos

- Plataforma: Android TV
- Status: consertada-nao-lancada
- Release: 2.0.4
- Conserto: nenhum encontrado
- Próximo passo: nada; validado na TCL
- Notas: 'Ocultar não lançados' escondia todo título sem metadados (121 de 129). Conserto 251d5e0f.

### Spotlight: título do usuário não vinha primeiro; item cortado

- Plataforma: Android TV
- Status: consertada-nao-lancada
- Release: 2.0.4
- Conserto: nenhum encontrado
- Próximo passo: dono dizer qual foto estava errada (arte não reproduzida)
- Notas: Busca 'silo': série 2023 em 4º. Conserto em agente/204-spotlight (1de8ce4e..63a41b4b). Arte trocada não comprovada.

### Dolby Vision às vezes abre escuro na TCL; clareia ao mudar o aspecto

- Plataforma: Android TV
- Status: aberta
- Release: -
- Conserto: nenhum encontrado
- Próximo passo: dono avisar na hora em que abrir escuro para puxar o [dvtrace]
- Notas: Sem causa provada. Logs da superfície HDR em 817d3c3a. Suspeita: recriação da abertura marcada como feita antes de terminar; mudar aspecto força outra recriação.

### 'Seu progresso' errado depois de desmarcar episódios

- Plataforma: Android TV
- Status: aberta
- Release: -
- Conserto: nenhum encontrado
- Próximo passo: decidir se entra 'desmarcar daqui em diante' no menu do episódio
- Notas: No log, 'até aqui desmarcar' em T3E2 tirou T1, T2 e T3E1-2 e deixou T3E3+ marcados; nada voltou sozinho. Dois buracos de sync achados e não provados (ENTREGA-progdesmarca.md). Achado lateral consertado: cache do gráfico (6c6d1668).

### Título marcado como visto no Simkl sem assistir

- Plataforma: all
- Status: desconhecido
- Release: -
- Conserto: nenhum encontrado
- Próximo passo: triar
- Notas: Dono também observou; causa não identificada.

### Add-ons duplicados durante sincronização

- Plataforma: all
- Status: desconhecido
- Release: -
- Conserto: nenhum encontrado
- Próximo passo: triar
- Notas: Relato no Discord sem log.

### Cotonet não aparece

- Plataforma: all
- Status: desconhecido
- Release: -
- Conserto: nenhum encontrado
- Próximo passo: triar
- Notas: Relato no Discord sem log.

### Legenda embutida de texto na LG

- Plataforma: LG webOS
- Status: consertada-nao-lancada
- Release: 2.0.5
- Conserto: c8e8d983 (integracao/2.0.5)
- Próximo passo: triar
- Notas: Relato de Jaco; desenho pelo app integrado na 2.0.5.

### Add to Home respeita lista de desativados

- Plataforma: all
- Status: consertada-nao-lancada
- Release: 2.0.5
- Conserto: 85411758 (integracao/2.0.5)
- Próximo passo: triar
- Notas: Relato do Discord; correção integrada para Add to Home prevalecer sobre a lista desativada.

### Abas das paginas no topo (pedido do dono)

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 2ebf3cb2
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). sem review j-revg-abastopo OK? ver jobs. Escrivão 14/10 14:47: integrada.

### Cartao de foco expandido menor + espacamento de fileiras (Apple TV)

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 8cdf8cc9
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). branch merged, sem review. Escrivão 14/10 14:47: integrada.

### Nova fileira com um card largo so (Apple TV style)

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: fd1766cb (merge em integracao/2.0.5)
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub. Escrivão 2026-10-14 16:11: fix3 commitado 960cf84f; review anterior BLOQUEIA (j-revg-herocard-r2). | j-revg-herocard-r3 OK 16:02 (fix3 960cf84f). Escrivão 16:49: branch ainda não integrada. Escrivao 19:0x: merge em integracao/2.0.5.

### Descer de fileira mantem a coluna horizontal

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 8794a7c2
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). rodando. Escrivão 14/10 14:47: integrada.

### Banding do gradiente (dither)

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 7542cd0e
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). rodando. Escrivão 14/10 14:47: integrada.

### Novo estilo de fileira: poster 3:4 grande com overlay da cor dominante

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 8cd1834c
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). rodando. Escrivão 14/10 14:47: integrada.

### Cor dos botoes do hero + repensar imersivo/textura

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: d9d8ad6f (merge agente/205-botoes em integracao/2.0.5)
- Próximo passo: sem ação; lancar com 2.0.5 (testar no TCL antes do release)
- Notas: Pedido do dono/Discord sem issue no GitHub. Escrivão 2026-10-14 16:11: fix3 commitado d2aa0f4e; review anterior BLOQUEIA. | Merge d9d8ad6f em integracao/2.0.5 (escrivao 14/10 16:49).

### Layout do Ranking de colecao + foco cortado no topo

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 77cbb10c
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). rodando. Escrivão 14/10 14:47: integrada.

### Botao shuffle minimalista na pagina da serie

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 20213e22
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). sem review; branch nao integrada. Escrivão 14/10 14:47: integrada.

### Polir a sidebar no modo Apple TV

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 5d7d5577
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). sem review; branch nao integrada. Escrivão 14/10 14:47: integrada.

### Porcentagem em cada quadrado do fingerprint

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 08410c0a
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). sem review; branch nao integrada. Escrivão 14/10 14:47: integrada.

### Seletor de perfil: tirar a barra de luz no estilo Luz

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: cb9724ff
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). sem review; branch nao integrada. Escrivão 14/10 14:47: integrada.

### Top bar compacta e premium (vidro e tab strip portados do plx)

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 53e14513 (merge em integracao/2.0.5)
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). review OK; branch NAO integrada. Escrivão 14/10 14:47: impl/merge feito, sem review. | Escrivão 2026-10-14 16:11: mergeada 53e14513; review j-revg-topotam OK.

### Gerenciar/adicionar perfil no quick switch + 3 tamanhos (alvo 2.1)

- Plataforma: all
- Status: aberta
- Release: 2.0.6
- Conserto: agente/205-perfis21 (tip 7376c250, 1 commits fora)
- Próximo passo: backlog 2.1 (review OK, 1 ahead)
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). rodando; alvo declarado = 2.1. Escrivão 14/10 14:47: review OK, merge pendente. | Escrivão 2026-10-14 16:11: review j-revg-perfis21 OK (11:16); merge pendente. | Escrivão 14/10 19:48: sem job Escrivao 10-15 07:10: review OK mantido.

### Dicas de controle: tema/chips do rodape

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: agente/205-dicatoque (tip 0ad55379, 1 commits fora)
- Próximo passo: lancar com 2.0.5 (merge em integracao/2.0.5)
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). BLOQUEIA: agente/205-dicatoque2 nao integrado. Escrivão 14/10 14:47: sem job (perdido). | Escrivão 14/10 19:48: sem job

### Painel de estatisticas ao vivo (esquerda/direita)

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: d67a235f
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). review OK; branch NAO integrada (merge so de integracao dentro dela). Escrivão 14/10 14:47: integrada.

### Desempenho dos Ajustes (harness de caminhada + rasterizacao)

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: b65a4a9c
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). review sem veredito; branch NAO integrada. Escrivão 14/10 14:47: integrada.

### Abertura cinematica da fonte: barra com a cor da logo + soft start

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 146f73e1
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). branch NAO integrada; 1 assert pre-existente em corviva.sh. Escrivão 14/10 14:47: integrada.

### PGS/blur do plx

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: agente/205-plx-pgs (tip 13dcc6b0, 7 commits fora); db9fca6f
- Próximo passo: lancar com 2.0.5 (merge em integracao/2.0.5)
- Notas: 13 commits fora. tests/mkvass.sh rc 1 no timing de servidor lento, mesmo base db9fca6fa: nao causado por esta mudanca.

### Opcao "Teclado" (do app/do sistema) tambem na webOS/Tizen

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 4b333231 (merge agente/205-tecladolg em integracao/2.0.5)
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub. Escrivão 2026-10-14 16:11: fix 2a0ef328 commitado; review j-revf-tecladolg (07:13) BLOQUEIA antes do fix. | Escrivão 16:49: review lançado; sem veredito ainda. Escrivao 19:0x: merge 4b333231 (18:45); j-revg-tecladolg-r2 OK 18:20.

### Pesquisa: distribuir pela TizenBrew (Samsung)

- Plataforma: all
- Status: adiado
- Release: 2.0.6
- Conserto: agente/205-tizenbrew (tip 25693f33, 1 commits fora)
- Próximo passo: adiado pelo dono; rascunho na branch
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). docs em docs/tizenbrew; sem job recente. Escrivão 14/10 14:47: adiado (pesquisa; sem merge).

### Homebrew Channel webOS / mercado

- Plataforma: all
- Status: adiado
- Release: 2.0.6
- Conserto: agente/205-homebrew (tip bcb1cdea, 1 commits fora)
- Próximo passo: adiado pelo dono; rascunho na branch
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). branch com 1 commit, sem job recente. Escrivão 14/10 14:47: adiado (pesquisa; sem merge).

### OK nos relacionados do player de filme nao fazia nada

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 6b650a74
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). sem review; branch NAO integrada. Escrivão 14/10 14:47: integrada.

### Salles? ver #477: sinopse menor + opcao de aumentar nos Ajustes

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: c504588a
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). review OK; branch NAO integrada (issue #477). Escrivão 14/10 14:47: integrada.

### Triagem de branches velhas + decisao do dono feat/* = 2.2

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: agente/205-triagem-velhas (tip 3d791e8a, 1 commits fora)
- Próximo passo: lancar com 2.0.5 (merged em agente/205-triagem-velhas)
- Notas: Branch agente/205-triagem-velhas 1 commit fora.

### Glass/top bar portados do plx (tambem em dono-205-topotam)

- Plataforma: all
- Status: 5 commits nao integrados
- Release: 2.0.6
- Conserto: agente/205-topotam (356c63b8 2026-10-14 Document the PlxNative glass and tab strip port)
- Próximo passo: j-205-vidroplx done (ainda nao mergeado; sem job de merge)
- Notas: Pedido do dono/Discord sem issue no GitHub (snapshot 2026-10-14 10:15). mesmo branch de dono-205-topotam. Escrivao 10-16: ainda nao mergeado (backlog 2.1).

### Merge integracao/2.0.5 into agente/205-205-teclado

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 49a90e54
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge integracao/2.0.5 into agente/205-abas

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 3fc35494
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Fix streaming collection tab strip: catalog order alignment, sliding focus, smooth brand ba

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 7242b442; 28da074c
- Próximo passo: lancar com 2.0.5 (merge a7d0b2ee5)
- Notas: 5-6 commits fora. Nao mergeado.

### fix(gfx): require pixel-aligned opaque panel cores for clear

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 31323882
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### test(ajustes): retire the seven AJ_PROF_* depth options from the sections check

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: c28804c1
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### feat(amigos): Friends tab shows friends first, live on top, capped suggestions

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 9eb8cc2f
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge agente/205-cwinicio (fix cwinicio test signature)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: d6335ba9
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### aovivo: drive the OSD focus springs from the row draw + shot frame sequence

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: ac6d84aa; 059af14f
- Próximo passo: lancar com 2.0.5 (merge 7198e353e)
- Notas: Review OK j-revg-animacoes-d 19:53 (apos fixc3 19:41). 15 commits fora. Merge job 20:04 nao mergeou.

### fix(tex): count bytes from the Android reserve path and back off 0-byte art responses

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 48162e4d
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### tpk40: keep ROI and window geometry on the panel on Tizen 4 (#426)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 48c315d6
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### test(video): cover paused aspect on webOS and native Tizen

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 0f15688c
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(android): preserve safe numeric fields in fatal diagnostics

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: c03201c0
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### addons: preserve credential case in dedupe key; cover all addons in subtitle workers

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: c14c1e01
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### test(player): prove both live scrobble trailer guards independently

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: f1ee6afe
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Hide performance meter while a trailer plays

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: de5a9bc6; 560b7752
- Próximo passo: merged em integracao/2.0.5; testar no TCL
- Notas: Escrivão 14/10 14:47: sem job (perdido). | j-revg-bench-mm: VEREDITO OK. Escrivão 16:49: 1 commit fora, não integrada. | Escrivão 14/10 19:48: agente/205-bench-mm merged (560b7752).

### detail: expanding "Play from beginning" pill reveals the label by fade, not clip (#205)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 667eb29f
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Release the cache mutex when refusing an off-screen request

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 6cd1b3b9
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Central: allow every available shortcut tile

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 93abe26b
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Island header: inset moves only the title; clock (and 'termina as') stay right

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: b38a6a5b
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Make the Control Center title lighter: 20 Medium, polished grey #A8ADB5

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 9a51392c
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### player (#205): Cinema opening shows the add-on name only when toggled, drops the status tex

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 9846de59
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### gfx: reduce the blurred backdrop in two passes before blurring

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: f284eaeb
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(collections): keep Menu catalog selector open on failure

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 168be34d
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge integracao/2.0.5 into agente/205-colestilos

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 300a4798 (merge em integracao/2.0.5)
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: impl/merge feito, sem review. | Escrivão 2026-10-14 16:11: mergeada 300a4798; review j-revg-colestilos OK.

### Merge agente/205-dicascontrole

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 0e567d1a
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### ctxmenu: keep pending row style past the Order row; pointer click on Order cycles it

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 6d8058da
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge integracao/2.0.5 into agente/205-colstreaming

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: f34bfe41
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Polish collection Cover Flow: Home focus ring, flat depth stack, info below timeline

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 5bc7fc20
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### home: keep palco curtain band through fade-out (#205)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 02f53b6e (merge agente/205-coverflow2 em integracao/2.0.5)
- Próximo passo: sem ação; lancar com 2.0.5 (testar no TCL antes do release)
- Notas: Escrivão 2026-10-14 16:11: fix3 commitado 3d9cf03d; review anterior BLOQUEIA (j-revg-coverflow2-r2). | Merge 02f53b6e em integracao/2.0.5 (escrivao 14/10 16:49).

### Merge agente/205-ajustesperf

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: b65a4a9c
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### test(cwinicio): pass tocou=-1 seed record for local CW baseline (#205)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: d6335ba9
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### continue watching: "Trakt only" without Trakt link falls back to all sources

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 61c2b385
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Check debrid account premium status before playback

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: fe49d40a
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Fix webOS loop shutdown and keep screensaver acknowledgements current

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: e38b41a0
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge integracao/2.0.5 into agente/205-destaquegenero

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 68e56581
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Comments picker reopens on the confirmed episode's season after browsing others

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: de8fcc91
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge integracao/2.0.5 into agente/205-dicascontrole

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 0e567d1a
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge agente/205-i18nordem

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 7839b119
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Settings: the register-only footer replay must not paint

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 96c87dc7 (merge agente/205-dicatoque2 em integracao/2.0.5)
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 2026-10-14 16:11: fix2-r commitado 44b4295a; review j-revg-dicatoque2 BLOQUEIA antes. | j-revg-dicatoque2-r3 OK 15:56 (fix2-r 44b4295a). Escrivão 16:49: branch ainda não integrada.

### fix(directors): correct watched state, identity merging and portrait tint

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 599f80aa
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Guard queued surface recreation and restore HDR aspect refresh

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 2f87e063
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Fix hidden OK on the Cinema Dolby Vision screen

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 59ed8523
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge agente/205-legsyncprox

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: e0add27f
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge integracao/2.0.5 into agente/205-epcard

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 364f28d2
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### feat(detail): modern episode cards put title and synopsis below the thumbnail

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: ba3b911b
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge agente/205-notas0

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: a3b07d6d (merge agente/205-epslista em integracao/2.0.5)
- Próximo passo: sem ação; lancar com 2.0.5 (testar no TCL antes do release)
- Notas: Pedido do dono/Discord sem issue no GitHub. Escrivão 2026-10-14 16:11: impl em agente/205-epslista; review j-revg-epslista-r3 OK; branch ainda NAO integrada. | Merge a3b07d6d em integracao/2.0.5 (escrivao 14/10 16:49).

### Merge integracao/2.0.5 into agente/205-epsrapido

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 6fb82684
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(player): polish statistics margins, keycaps and meta line

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 60c6db739 (merge agente/205-estatpainel2 em integracao/2.0.5)
- Próximo passo: merged; build+install para o novo HEAD
- Notas: Review OK j-revg-estatpainel2-c 19:20. Merge concluido (60c6db739); branch sem commits fora. Escrivao 10-15 07:10: merge job sem review log (so merge/fix).

### fix(player): keep live statistics open and add animated overlay sizes

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 73d12afd
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(detail): request the known media type for TMDB titles

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 4fb1e0a8
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(i18n): translate row navigation hints in all 28 languages

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 1d452999
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge integracao/2.0.5 and preserve memory migration and RAM limits

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 8d6fefc8
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix: tint cached text RGB at draw time to restore unfocused contrast

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: b3c38e71
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### docs(guia): add phone-setup guide and Discord post

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 45fe4599
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### test(settings): screenshot test for hero catalog picker, sync language tables

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: daed07ec
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge agente/205-cardexp

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 3b66b044 (merge em integracao/2.0.5)
- Próximo passo: lancar com 2.0.5
- Notas: Pedido do dono/Discord sem issue no GitHub. Escrivão 2026-10-14 16:11: impl em agente/205-heroinfo; review j-revg-heroinfo-r3 OK; branch ainda NAO integrada. | j-revg-heroinfo-r3 OK 15:56. Escrivão 16:49: branch ainda não integrada. Escrivao 19:0x: merge em integracao/2.0.5.

### Merge integracao/2.0.5 into agente/205-homeqr

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 4f022aa3
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge integracao/2.0.5 into agente/205-i18n491: keep translated i18n entries

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: ce5b60d1
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Add missing i18n keys for drawn subtitle/source labels

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 43570696
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge integracao/2.0.5 into agente/205-i18nordem

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 7839b119
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix: start next episode countdown when the card appears

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: d1087e75
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(tpk): confine card trailer holes to the poster (#471)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 38369f89
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### feat: re-apply app icon at start when bundled art fingerprint changed

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 888ef95e
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### mapa: triagem das 17 issues novas e rascunhos de resposta da 2.0.5

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 4bd7e9c8
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### issues: post-2.0.5 triage (419 items, 2.0.6 target, evidence-based status)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 7656a6598 (merge agente/205-triagem-velhas em integracao/2.0.5)
- Próximo passo: merged em integracao/2.0.5 (7656a6598); build+install para 799d025bd; docs-only
- Notas: Escrivão 14/10 14:47: integrada. Escrivao 10-15 07:10: w OK (04:49) apos fix z (03:49); merge 7656a6598.

### test(subtitles): make external-subtitle guards fast and portable

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 027fd3c7
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix: preserve unknown subtitle type in remembered selections

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: eb29eddc
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix: keep remembered subtitle when the first stream arrives

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 9de916a9
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Test next-episode legsync: ep2 sync starts fresh in both orderings

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: e0add27f
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(epg): stop loading indicators after schedules finish (#316)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 7f3177ef
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Classify account login failures and retry transient requests

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: deb68c4b
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge branch 'integracao/2.0.5' into agente/205-menuglass

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 2e4d2434
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge integracao/2.0.5 into agente/205-menutopo

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: f3de11e7
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### docs: PLANO-MERCADO plan

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 84c19b1d
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### design(2.1): friends row mockup with 5 modern styles (#205)

- Plataforma: ?
- Status: aberta
- Release: 2.0.5
- Conserto: agente/205-mock21 (tip 9de13e4e, 1 commit fora)
- Próximo passo: backlog 2.1 (mock 21, 1 ahead, sem job, sem log)
- Notas: Escrivão 2026-10-14 16:11: 3 mockups anteriores (gamification/cinephile/redesign) ja em integracao/2.0.5; friends row 9de13e4e ainda fora. Escrivao 10-15 07:10: sem job.

### fix(network): preserve offline detection across retries and allow DNS retransmission

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 6e29915c
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### ratings: log why sources come back empty; compact card for 1 source; cast OK without person

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 0a8267de
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge agente/205-notas0

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 1602f90c (merge agente/205-notas1 em integracao/2.0.5)
- Próximo passo: sem ação; lancar com 2.0.5 (testar no TCL antes do release)
- Notas: Pedido do dono/Discord sem issue no GitHub. Escrivão 2026-10-14 16:11: impl em agente/205-notas1; review j-revg-notas1-r3 OK; branch ainda NAO integrada. | Merge 1602f90c em integracao/2.0.5 (escrivao 14/10 16:49).

### fix(tests): debrid parser expects externalUrl-only stream to be dropped

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: a468d007
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### novidades 2.0.5: card compacto com 12 itens por grupo e plataforma, Discord na ultima pagin

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 64e27f02
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(p2p): USB cache — persist scanned root, race/litter/orphan fixes (#205)

- Plataforma: ?
- Status: em_revisao
- Release: 2.1
- Conserto: agente/205-p2pusb: 9 commits fora; 90fb9cff7 fix(p2p): bound synchronous USB discovery work (fix, sem review)
- Próximo passo: review fix 90fb9cff7 (j-fix-p2pusb-w 09:50); merge; teste P2P USB na TV
- Notas: 8 ahead; nao mergeado; sem job rodando Escrivao 10-15 07:10: fix w 05:51; review v 06:49 BLOQUEIA. | Escrivao 2026-10-15 07:50: lost-ish: sem job apos BLOQUEIA v | Escrivao 10-15 08:50: fix 07:52 sem review; nao perdido (fix depois do bloqueio).

### Merge agente/205-legsyncprox

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: e0add27f
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### painel: English public build, single future group, clean platform names

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: f8b39d5b
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### painel: component polish (toolbar, table, badges, dialogs, mobile cards)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: c480642e
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### test(home): capture the stage row with the focus walking right

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: dfdbe238
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### android: fix paralelo unit tests after zero-delay open retry

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: agente/205-paraleloloop (tip 1788e8fa, 6 commits fora); 07455c12
- Próximo passo: lancar com 2.0.5 (merge c95f9b1cb)
- Notas: 9 commits fora. Nao mergeado.

### tests: plugin runner and Plugins screen / sources sheet captures

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 76467f10
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### docs(pesquisa): research report on plx-native player and UI fluidity

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 7ca41222
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Smooth poster preparation with idle lookahead and solo uploads

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: d4b7ac38
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(fpscena): require Home to own the D-pad before measuring

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: adc3fe41
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix: preserve stalled decoder recovery when live buffer grows

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: d2b29c87
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Home: apply press-dip after palcoGeo so Palco posters dip too

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 733ae868
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix: keep lateral pacing when adoption races a slow pre-busca header

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: ed57cd73
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Split the request refusal into edge and fully-off-screen levels

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: b31194ef
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### smooth posters while scrolling: never refuse cards entering the screen, prefetch next row/c

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 352befd1 (merge em integracao/2.0.5)
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: impl/merge feito, sem review. | Escrivão 2026-10-14 16:11: mergeada 352befd1; review j-revg-posterfluido-f OK.

### Merge integracao/2.0.5 into agente/205-pr489

- Plataforma: ?
- Status: em_revisao
- Release: 2.0.5
- Conserto: 22a34af1
- Próximo passo: merge pr/489-fix (review OK 03:05); TCL test
- Notas: Escrivão 14/10 14:47: integrada.

### Merge branch 'integracao/2.0.5' into agente/205-pr490

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: 75059d26; 1f8309961 Fix Vitrine resume preservation (fix, sem review)
- Próximo passo: review j-pr490-v3 (13:47, rodando) do tip de pr/490-fix; fix6 OK 12:51 antes
- Notas: Escrivão 14/10 14:47: integrada. | Escrivao 10-15 13:48: fix6 OK (j-revpr490-fix6 12:51).

### ci: port PR #491 Tizen 6+ package builds and audit all commits

- Plataforma: ?
- Status: em_revisao
- Release: 2.0.5
- Conserto: 6dd374db
- Próximo passo: review fix c (f8fe27fad) de pr/491-limpo
- Notas: Escrivão 14/10 14:47: integrada. | Escrivao 10-15 08:50: branch 0 ahead: merged. | Escrivao 10-15 13:48: mesmo item de pr/491-limpo.

### Streaming: ranking and decades info side without duplicate poster

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: c782a787
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### docs(issues): draft replies for open issues in release 2.0.5

- Plataforma: ?
- Status: aberta
- Release: 2.0.5
- Conserto: e81c1f40
- Próximo passo: triagem (1 ahead, sem job, sem log); conferir respostas-dono.json (novo, nao commitado)
- Notas: Escrivão 14/10 14:47: sem job (perdido). Escrivao 10-15 07:10: docs/issues/respostas-dono.json aparece untracked no worktree.

### test(player): wait for catalog refresh before opening fixture

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: b256ae60
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### docs(plans): mkvcore pilot report (size, tests, risks)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: ff866add
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Record successful Mac Rust and ARM product build checks

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 1e87d5bd
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### docs: rust-mapa records mkvcore flag ON in product builds (2026-10-14)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 2b721b01
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### docs: map where Rust pays off in Nuvio (2.0.6)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 8ff33132
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Bring the performance meter back when a native trailer pauses

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: f6369df4
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### test: capture startup stages and compare the baseline renderer

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 053607be
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix: restore test runner repository working directory

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 723c05b6
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Animate Control Center hold progress in both clock islands

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: a24968b2
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### feat(badges): watched badge option with Icon and Written modes

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: ea891ca9
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Series detail: move shuffle button to the start of the season row

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 9eba8c16 (merge agente/205-shufflepos em integracao/2.0.5)
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: impl/merge feito, sem review. | Escrivão 16:49: impl feita; review sem veredito ainda.

### Merge branch 'integracao/2.0.5' into agente/205-sidebarfonte

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 5c8266ad (merge agente/205-sidebarfonte em integracao/2.0.5)
- Próximo passo: sem ação; lancar com 2.0.5 (testar no TCL antes do release)
- Notas: Escrivão 14/10 14:47: impl/merge feito, sem review. | Merge 5c8266ad em integracao/2.0.5 (escrivao 14/10 16:49).

### fix(simkl): keep history fallback on original profile

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: fe1366e5
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### feat: subtitle outline colour setting (#475)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 6a260e9b
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### docs: PLANO-SKIP plan

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: d1243a46
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(intro): preserve partial retries and selected credit limits

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 58c1ef33
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### test(ponteiro): click the lowest addon row, not the Sync addons row below it

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 737ffff1
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### test(ctxlista): assert the post-morph menu-beside-card geometry

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 517a5d1d
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### test(home): link DTS sources in loading panel capture

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 05c350c4
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### test(sources): align final selection with per-quality cache ordering

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 3d7b4dee
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### test(cw): restore addon list lock stubs in removal fixture

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: bddd5ca5
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Merge branch 'integracao/2.0.5' into agente/205-testestmp

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: e53eb342
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(settings): absorb clicks on unavailable texture limits

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: bdbe9043
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Fix activity consent and identity races during token renewal

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: e2c1299a
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Keep TPK recovery seek targets until host confirmation

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 224063f2
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(player): bound EOF metadata wait and respect rewinds

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: d8cc7fa2
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(spotlight): rank all search targets before the 64-candidate cap (#405)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 219d3bf4
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Sync all Trakt show progress independently of Continue Watching source

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: cb8b976c
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### feat: size layered UI dynamically for physical TV panels

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 6c6c0296
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### vidro (#410): Contorno do vidro reaches card focus in Home, Library and Friends

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 2dfefe46
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### fix(watchstate): freeze manifest identity and persist expired queue cleanup

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 90e43bd1
- Próximo passo: lancar com 2.0.5
- Notas: Escrivão 14/10 14:47: integrada.

### Retomada morna (warm resume)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: c624b273 (merge agente/205-retomamorna em integracao/2.0.5)
- Próximo passo: lancar com 2.0.5 (testar no TCL antes do release)
- Notas: Issue #492. Review j-revg-retomamorna OK 17:06.

### fix(android): SDLActivity.nativeLowMemory (urgente)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 22924c90e; 5544c3e4c (merge agente/205-androidsdl)
- Próximo passo: instalado no Android (build 5544c3e4c, rc 0, Success)
- Notas: Job androidsdl done 20:02.

### Vidro padrao: auditoria de superficies glass (PLX glass port)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: be42293ae (merge agente/205-vidropadrao em integracao/2.0.5)
- Próximo passo: merged; build+install para o novo HEAD
- Notas: Branch agente/205-vidropadrao 10 commits fora, 2 atras. Job sonnet high. Escrivao 10-15 07:10: merge job sem review log (so merge/fix).

### M3U misto: canal/catalogo pelo tipo declarado (bug)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: lancar com 2.0.5 (merged em agente/205-m3umisto)
- Notas: Pendente: sem validacao no aparelho. Testes guia_catalogo_tv.sh e addonslista.sh rc 0.

### Chdelay (delay de canal/troca de canal)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: lancar com 2.0.5 (merge 8e203bc61)
- Notas: Branch agente/205-chdelay ja integrada (merged).

### Glass mockup: estudo de design (somente mockups)

- Plataforma: ?
- Status: aberta
- Release: 2.1
- Conserto: 030bf69df (docs/design glass chrome study), 2 commits fora de integracao/2.0.5
- Próximo passo: aguardando escolha do dono A/B/C; 2 ahead, estudo/mock, sem review
- Notas: Estudo/mockups (build/gen.py + PNGs), sem src. NOTA 10-15 07:10: notas antigas diziam "ja integrada": git mostra 2 ahead nao mergeado. Escrivao 10-15 07:10: sem review; depende de decisao do dono. Escrivao 10-16: espera decisao do dono (2.1).

### Barra 27 (branch agente/205-barra27)

- Plataforma: Android/TCL?
- Status: integrada
- Release: 2.0.5
- Conserto: agente/205-barra27: 2 commits fora
- Próximo passo: lancar com 2.0.5 (merged em agente/205-barra27)
- Notas: criado pelo escrivao 22:4x

### Cabecalho colunas (agente/205-colhdr)

- Plataforma: ?
- Status: aberta
- Release: 2.0.5
- Conserto: agente/205-colhdr: 1 commit fora
- Próximo passo: descartar: colhdrlinha ja integrado (c44f8afbe); branch so mock
- Notas: 1 ahead; fix 21:18, sem review depois Escrivao 10-15 07:10: colhdrlinha merged c44f8afbe; colhdr fica como mock 1 ahead.

### VIDAA (agente/205-vidaa)

- Plataforma: VIDAA
- Status: integrada
- Release: 2.0.5
- Conserto: 81b418469 (Merge agente/205-vidaa, integracao/2.0.5); d0bf5ba79 (Merge integracao/2.0.5 into agente/205-vidaa); agente/205-vidaa
- Próximo passo: merged em integracao/2.0.5 (81b418469, d0bf5ba79); veredito final j-revg-vidaa-6 OK apos j-fix-vidaa-6; build+install; emcc/tpk com chaves e TV VIDAA fisica pendentes
- Notas: criado pelo escrivao 22:4x Escrivao 10-15 07:10: sem review depois do fix 22:46. Escrivao 10-16: merged (81b418469). Reviews anteriores BLOQUEIA (probe, sync, tracks) -> fixes -> j-revg-vidaa-6 OK (veredito mais recente do branch). Pendente emcc/tpk com chaves e TV VIDAA fisica. Escrivao 10-16 01:50: merge ainda nao instalado na TV.

### PR #493 (ghbarker, ligado a #488)

- Plataforma: Android/TCL?
- Status: em_revisao
- Release: 2.0.5
- Conserto: agente/205-pr493: 12 commits fora
- Próximo passo: merge pr/493-v3 (65613c562, review OK 06:50); substitui pr/493 velho; TV teste troca de perfil
- Notas: agente/205-pr493 12 ahead; pr/493-fix e o fix

### PR #498 limpo (pr/498-limpo)

- Plataforma: ?
- Status: em_revisao
- Release: 2.0.5
- Conserto: pr/498-limpo: 12 commits fora (tip 22ed066e2 22:31)
- Próximo passo: merge pr/498-limpo (12 ahead) ou dono; review OK (j-revpr498-limpo)
- Notas: PR #498; 12 ahead; nao mergeado Escrivao 10-15 07:10: 12 ahead, review OK mantido; falta merge. | Escrivao 2026-10-15 07:50: review OK, falta merge | Escrivao 10-15 08:50: ainda nao mergeado. | Escrivao 10-15 13:48: review OK, branch 22ed066e2 nao mergeado.

### PR #491 limpo (pr/491-limpo)

- Plataforma: ?
- Status: em_revisao
- Release: 2.0.5
- Conserto: pr/491-limpo: 52 commits fora (tip f8fe27fad 05:53)
- Próximo passo: review fix c (f8fe27fad, 05:53); BLOQUEIA r3 04:52 antes; sem job rodando
- Notas: PR #491; 51 ahead; nao mergeado Escrivao 10-15 07:10: fix c 05:53 cancela probes DTS; precisa review depois do r3. | Escrivao 2026-10-15 07:50: precisa review do fix c | Escrivao 10-15 08:50: fix c sem review. | Escrivao 10-15 13:48: sem review depois do fix c. LOST se nao revisado.

### PR #489 fix (pr/489-fix)

- Plataforma: ?
- Status: em_revisao
- Release: 2.0.5
- Conserto: pr/489-fix: 23 commits fora (tip 49f7a53bd e45ff15dd)
- Próximo passo: merge pr/489-fix em integracao/2.0.5 (review OK j-revpr489-fix 03:05, 23 ahead, nada depois); testar
- Notas: PR #489; 23 ahead; nao mergeado Escrivao 10-15 07:10: review OK 03:05; 23 ahead; nenhum fix depois. | Escrivao 2026-10-15 07:50: pronto para merge, nao mergeado | Escrivao 10-15 08:50: ainda nao mergeado.

### PR #490 fix (pr/490-fix)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: pr/490-fix: 8 commits fora (tip 3dae8f5c 06:03)
- Próximo passo: review j-pr490-v3 (13:47, rodando) do tip de pr/490-fix (46e4df866 ou posterior)
- Notas: PR #490; 7 ahead; nao mergeado Escrivao 10-15 07:10: fix c 06:03 (3dae8f5c); review fix3 06:50 BLOQUEIA (P2 starvation da fila). | Escrivao 2026-10-15 07:50: lost-ish: BLOQUEIA sem fix depois | Escrivao 10-15 08:50: fix v 07:59 aguarda review. | Escrivao 10-15 13:48: fix6 OK 12:51; v3 job rodando.

### PR #493 fix (pr/493-fix)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: pr/493-fix: 10 commits fora (tip a0bc2d982 03:52)
- Próximo passo: nada: PR #493 ja mergeado (e3e21a590); pr/493-fix (9 ahead) antigo, nao mergear
- Notas: PR #493 (carrega #488); 10 ahead; nao mergeado Escrivao 10-15 07:10: autor publicou head nova; pr/493-v3 porta os fixes e passa review OK 06:50 (j-revpr493-v3). Continuar com pr/493-v3. | Escrivao 10-15 13:48: PR #493 merged e3e21a590; review v3 OK (06:50).

### Vidro todo (agente/205-vidrotodo)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: agente/205-vidrotodo: 10 commits fora (tip 9b4fe6776 merge do HEAD); 9d5115556 (merge agente/205-vidrotodo em integracao/2.0.5)
- Próximo passo: instalado 84add4581 (Android Success, C9 rc=255); teste TV glass/top chrome
- Notas: criado pelo escrivao 2026-10-15 Escrivao 10-15 07:10: antes estava "sem review log"; agora review OK 05:49 e merge da integracao no branch 06:49. Branch ainda 10 ahead. | Escrivao 2026-10-15 07:50: merge 9d5115556 em integracao/2.0.5; review OK 05:49 | Escrivao 10-15 08:50: merged 9d5115556; ainda nao validado na TV.

### Aspecto volta (agente/205-aspvolta)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 71c00499e (merge agente/205-aspvolta em integracao/2.0.5; afb08475f)
- Próximo passo: merged (71c00499e); este HEAD ja tem install Android (instalado.txt 04:50)
- Notas: criado pelo escrivao 2026-10-15 Escrivao 10-15 07:10: review OK j-revg-aspvolta 03:53; merge 71c00499e; instalado android rc=0.

### Estilo 2 (agente/205-estilo2)

- Plataforma: ?
- Status: aberta
- Release: 2.0.5
- Conserto: agente/205-estilo2: 2 commits fora
- Próximo passo: aguardando escolha do dono A/B/C (recomendado C); 2 ahead, mock only, sem review
- Notas: criado pelo escrivao 2026-10-15 Escrivao 10-15 07:10: sem review; depende de decisao do dono.

### PR #493 v3 (pr/493-v3) — head do autor + fixes portados

- Plataforma: ?
- Status: em_revisao
- Release: 2.0.5
- Conserto: pr/493-v3: 3 commits fora (5694694cc autor, 65613c562 port dos fixes)
- Próximo passo: merge pr/493-v3 no PR #493 (review OK 06:50, 3 ahead, nao mergeado); build TV e teste de troca de perfil
- Notas: Escrivao 10-15 07:10: job j-pr493-v3 05:49 (10 suites rc 0); review j-revpr493-v3 06:50 VEREDITO OK, sem P0-P2. Pendente: build TV e teste de troca de perfil em aparelho. | Escrivao 2026-10-15 07:50: review OK, falta merge | Escrivao 10-15 08:50: ainda nao mergeado no snapshot 08:48.

### Merge master 1015 em integracao/2.0.5 (28 commits)

- Plataforma: todas
- Status: integrada
- Release: 2.0.5
- Conserto: 249a5f8eb (Merge agente/205-master1015, integracao/2.0.5); agente/205-master1015
- Próximo passo: sair com 2.0.5
- Notas: Escrivao 15/10 09:48: branch agente/205-master1015 (28 ahead) sem item de issue; criado pelo escrivao. Escrivao 10-15 10:50: BLOQUEIA 08:48; fix pronto 09:49.

### 2.1: avaliar filme/episodio apos assistir (pagina do titulo ou popup nos creditos finais)

- Plataforma: todas
- Status: aberta
- Release: 2.1
- Conserto: nenhum encontrado
- Próximo passo: backlog 2.1; sem job
- Notas: Pedido de usuario erolasan, 2026-10-14. Escrivao 10-15: backlog-21.

### 2.1: sync manual de legenda com menu superior estilo Stremio TV, ajustar enquanto assiste

- Plataforma: todas
- Status: aberta
- Release: 2.1
- Conserto: nenhum encontrado
- Próximo passo: backlog 2.1; sem job
- Notas: Discord the last envoy 14/10. Escrivao 10-15: backlog-21.

### Branch agente/205-aniskipmap (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: d115df435 (Merge agente/205-aniskipmap, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-apoio (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: ccba355cc (Merge agente/205-apoio, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-aspectob (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-auditoria (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-autosync-ref (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: c1b1f0679 (Merge agente/205-autosync-ref, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-cfrestaura (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: d5a2db894 (Merge agente/205-cfrestaura, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-colhdrlinha (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: c44f8afbe (Merge agente/205-colhdrlinha, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-colposter (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 05b94ae4a (Merge agente/205-colposter, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-cw-progresso (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 922c6cc0b (Merge agente/205-cw-progresso, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-decisoes (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-detalhe-meta (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-dicasabrir (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-diretorpol (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: c285fbf44 (Merge agente/205-diretorpol, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-dirfoto (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 624bbb680 (Merge agente/205-dirfoto, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-ecossistema (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 61e1bb029 (Merge agente/205-ecossistema, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-espaco (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-estatdicas (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 780fdc04b (Merge agente/205-estatdicas, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-fileiraui (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 9e421e472 (Merge agente/205-fileiraui, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-fontesmortas (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: cceeb07e8 (Merge agente/205-fontesmortas, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-junta-orfas (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: b639873f1 (Merge agente/205-junta-orfas, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-junta-sync (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-legmatch (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 0c015b5f5 (Merge agente/205-legmatch, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-legref-tpk (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-legsync-ep (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 83fab1724 (Merge agente/205-legsync-ep, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-legtempo (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-listaaddon (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-logorapido (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: c08a210db (Merge agente/205-logorapido, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-logos (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 0bdeedc3a (Merge agente/205-logos, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-master1015b (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 8cb54305f (Merge agente/205-master1015b, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-merge-master (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: dc58ec9ed (Merge agente/205-merge-master, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-notas2 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: bfc0aab6e (Merge agente/205-notas2, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-novidades-colecao (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 410cc7bb9 (Merge agente/205-novidades-colecao, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-palco2 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: f906f4a83 (Merge agente/205-palco2, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-palcoveu (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: b095d9cdc (Merge agente/205-palcoveu, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-perfmedir (sem issue)

- Plataforma: ?
- Status: em_revisao
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: merge agente/205-perfmedir (review OK)
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-poscreditos (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: fc136af0f (Merge agente/205-poscreditos, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-pr431 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: c272ad6b7 (Merge agente/205-pr431, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-pr452 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: f9ee80a8b (Merge agente/205-pr452, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-pr454 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 4781366a0 (Merge agente/205-pr454, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-pr455 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 78f745fe0 (Merge agente/205-pr455, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-pr465 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 45f8b5027 (Merge agente/205-pr465, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-pr491 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: ba8e97297 (Merge agente/205-pr491, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-prghbarker (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 6fd6083b6 (Merge agente/205-prghbarker, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-prnovos (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 6ebf3b479 (Merge agente/205-prnovos, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-roi6 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-seekr-chave-propria (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-segurar (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 5057bd728 (Merge agente/205-segurar, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-semdica (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: f20ce1b0e (Merge agente/205-semdica, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-sidebarfundo (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: bbe721818 (Merge agente/205-sidebarfundo, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-syncmao (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-tmdbhost (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-tpkemu (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: da8d070a6 (Merge agente/205-tpkemu, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-urfix (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 86613a07a (Merge agente/205-urfix, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-ursamsung (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-vidroperf (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-vistos2 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: 3b787e0d3 (Merge agente/205-vistos2, integracao/2.0.5)
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch agente/205-vitlogo1015 (sem issue)

- Plataforma: ?
- Status: em_revisao
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: merge agente/205-vitlogo1015 (review OK)
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch fix/tpk-selo-hdr (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/431 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/452 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/454 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/455 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/465 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/487-v2 (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/489-fix (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/490-fix (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/490-v3 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/491-limpo (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/493-autor-v3 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/493-fix (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/493-v3 (sem issue)

- Plataforma: ?
- Status: integrada
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: testar na TV; sai com 2.0.5
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/498-fix2 (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/498-limpo (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/518-fix2 (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/519-fix2 (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/520-fix2 (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/522-fix2 (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/523-fix2 (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/526-fix2 (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/527-fix2 (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### Branch pr/531-fix2 (sem issue)

- Plataforma: ?
- Status: em_job
- Release: 2.0.5
- Conserto: nenhum encontrado
- Próximo passo: sem review: revisar ou descartar
- Notas: Criado no sync 2026-10-15 pelo escrivão: sem issue nem pedido no GitHub.

### TCL Android TV: crash ao tocar filme (memoria/buffer)

- Plataforma: Android (TCL 2.4 GB)
- Status: integrada
- Release: 2.0.5
- Conserto: 844b78f68 (Merge agente/205-tclmem, integracao/2.0.5); agente/205-tclmem
- Próximo passo: merged em integracao/2.0.5 (844b78f68); review OK (j-revg2-tclmem); build+install no HEAD novo; conferir na TCL (sem adb)
- Notas: Pedido do dono (TCL, 2.4 GB): "tocou o filme, travou tudo e fechou o app". Job j-205-tclmem + 2 fixes; review BLOQUEIA (j-revg-tclmem: P2-1/P2-2) -> OK apos fixes (j-revg2-tclmem OK). Escrivao 10-16 01:50: HEAD 4359b10f5, 140 commits a frente do instalado cb7ece972; instalado na TV e anterior.

### webOS vidro lento em detalhes/busca/explorar/ajustes (C9)

- Plataforma: LG C9 (webOS)
- Status: integrada
- Release: 2.0.5
- Conserto: 20a522eaf (Merge agente/205-vidrotelas, integracao/2.0.5); agente/205-vidrotelas
- Próximo passo: merged em integracao/2.0.5 (20a522eaf); review OK (j-revg-vidrotelas); build+install no HEAD novo; medir FPS no C9 real
- Notas: Owner: home rapida, mas sidebar de fontes/busca/explorar/ajustes ruins na C9. Fix: hash do backdrop ignora drift sub-1/256, off-screen e sky clock. Review j-revg-vidrotelas OK. Escrivao 10-16 01:50: merge ainda nao instalado na TV.

### Overdraw por tela (fill por frame) no webOS C9

- Plataforma: LG C9 (webOS)
- Status: integrada
- Release: 2.0.5
- Conserto: 3e3a909d5 (Merge agente/205-overdraw, integracao/2.0.5); 11cf6868c perf(gfx): draw corner light only where its radius reaches; 819033833 perf(gfx): large rings draw only their border strips; 1131422c0 test(overdraw): per-screen fill and text-raster budget; 33f478b75 perf(search): tint cached key labels instead of rasterizing each eased tone; 5c0ec1df0 fix(gfx): split ring/corner light keep deferred Dynamic background deferred; agente/205-overdraw
- Próximo passo: merged em integracao/2.0.5 (3e3a909d5); BLOQUEIA (j-revg-overdraw) -> OK (j-revg2-overdraw apos j-fix-overdraw); build+install; medir no C9
- Notas: Job de medicao/harness por tela (detalhe, busca, explorar, ajustes). Itens pendentes: cards de comentario desenhados 3x por coluna (precisa OK do dono), veil/shadow dao ~1.6 telas, ilha de Ajustes 2290x1160; nao testado na C9. Escrivao 10-16 01:50: 4 commits + fix 5c0ec1df0; veredito final OK. Itens ainda fora: skip de redraw ocioso no laco principal, sombra sob vidro nao opaco (~0.35-0.87 tela), caixas de glow grandes (~0.2 tela) — mudam pixels, precisam OK do dono.

### Cartaz do card de amigos em paisagem + shell dos cards de pessoas

- Plataforma: all
- Status: integrada
- Release: 2.0.5
- Conserto: 498ff29fc (Merge agente/205-amigoscapa, integracao/2.0.5); 8d39118d9 fix(amigos): friends row cards use the home card shell and fill with landscape art; 4dad19c3e fix(amigos): people cards use the home no-art skeleton, feed radius and a large centered avatar; agente/205-amigoscapa
- Próximo passo: merged em integracao/2.0.5 (498ff29fc); review OK (j-revg-amigoscapa, apos j-fix2-amigoscapa); build+install
- Notas: Owner: "so muda o formato do poster" no card paisagem de Amigos assistindo + contorno dos cards de pessoas (+ Adicionar). Criado pelo escrivao 00:20. Escrivao 10-16 01:50: 2 commits; vidro_padrao.sh rc=1 por 3 FAILs pre-existentes (identicos em integracao/2.0.5 limpa).

### Cartoes ao vivo do painel de estatisticas e dicas de tecla na TV

- Plataforma: TV (saida 3840)
- Status: integrada
- Release: 2.0.5
- Conserto: 3e267063d (Merge agente/205-estatresp, integracao/2.0.5); agente/205-estatresp
- Próximo passo: merged em integracao/2.0.5 (3e267063d); review OK (j-revg-estatresp); build+install no HEAD novo; falta TV fisica
- Notas: Cards ao vivo cortados + dicas de tecla iluminadas na saida de TV. Review j-revg-estatresp OK (suite GL completa nao rodada, so syntax-only). Escrivao 10-16 01:50: merge ainda nao instalado na TV.

### vidro/reuso de backdrop no webOS (vidroc9)

- Plataforma: LG C9 (webOS)
- Status: integrada
- Release: 2.0.5
- Conserto: 0b5c0135b (Merge agente/205-vidroc9, integracao/2.0.5); agente/205-vidroc9
- Próximo passo: merged em integracao/2.0.5 (0b5c0135b); review OK (j-revg-vidroc9-b); build+install no HEAD novo; testar no C9
- Notas: Fix: vidroHash diferente nunca reusa o backdrop do frame anterior; contadores so no webOS. j-revg-vidroc9 BLOQUEIA -> j-fix-vidroc9 -> j-revg-vidroc9-b OK. Escrivao 10-16 01:50: merge ainda nao instalado na TV.

### Merge do master 1015 + cards ao vivo (agente/205-mestre1015)

- Plataforma: todas
- Status: integrada
- Release: 2.0.5
- Conserto: 249a5f8eb (Merge agente/205-mestre1015, integracao/2.0.5); c15d7eedb (Merge branch integracao/2.0.5 into agente/205-mestre1015); 0e12b2903 (Merge origin/master into agente/205-mestre1015); agente/205-mestre1015
- Próximo passo: merged em integracao/2.0.5 (249a5f8eb); review OK (j-revg-mestre1015-d); build+install no HEAD novo
- Notas: 28 commits do master + recapture de vidro no webOS + retomada Android. BLOQUEIA (recapture/reload) -> fix 4b6340226 -> OK. Escrivao 10-16 01:50: merge ainda nao instalado na TV.

### Ajustes: suites de teste depois do merge do master

- Plataforma: todas
- Status: integrada
- Release: 2.0.5
- Conserto: 84dc18246 (Merge agente/205-ajustestestes, integracao/2.0.5); agente/205-ajustestestes
- Próximo passo: merged em integracao/2.0.5 (84dc18246); review OK (j-revg-ajustestestes); build+install; sai com 2.0.6
- Notas: Conserta parsing/assercoes das suites de Ajustes depois do merge do master. Review OK. Escrivao 10-16 01:50: merge ainda nao instalado na TV.

### Passada final de desempenho antes do lancamento (pedido do dono: "algo esta roubando muito FPS")

- Plataforma: all
- Status: integrada
- Release: 2.0.6
- Conserto: 4359b10f5 (Merge agente/205-perffinal, integracao/2.0.5); 8be255b35 perf(perfil): skip the covered black clear under psestilos backgrounds; 0181e0050 fix(home): declare video_registro_negado outside the tpk-only video.h include; agente/205-perffinal
- Próximo passo: merged em integracao/2.0.5 (4359b10f5); review OK (j-revg-perffinal); build+install; aplicados so itens que nao mudam pixels
- Notas: Owner: "algo esta roubando muito FPS" — passada final antes do release. Perfil 8.72->7.72 (3->2 full-screen draws). Deixado de fora por mudar pixels: skip de redraw ocioso (laco principal redesenha todo frame), sombra sob vidro nao opaco (~0.35-0.87 tela), glow grande + 3 retangulos alpha-0.09 (~0.2 tela). Escrivao 10-16 01:50: merge ainda nao instalado na TV.

### Bump 2.0.5 -> 2.0.6 (owner aprovou o lancamento da 2.0.6)

- Plataforma: all
- Status: em_revisao
- Release: 2.0.6
- Conserto: 7310f53f4 release: 2.0.6 (version bump, what's-new card in 29 languages, notes); agente/205-bump206 (1 ahead de integracao/2.0.5; nao mergeado)
- Próximo passo: job j-205-bump206 done (7310f53f4, tests rc=0); review j-revg-bump206 rodando (log vazio); mergear antes de build+install
- Notas: Owner aprovou o release 2.0.6. Notas 2.0.6 precisam cobrir: sinal do atraso de legenda, crash de memoria na TCL, telas webOS mais suaves (vidro/overdraw), fileira de amigos em paisagem + cards de pessoas, motivo do trailer do hero bloqueado, dicas de controle uma vez por versao. Escrivao 10-16 01:50. Escrivao 10-16 02:00: bump 2.0.6 feito (7310f53f4); versao_206.sh, novidades_cartao.sh e varredura-i18n.py rc=0. repo.json (Homebrew) ainda 2.0.5 (commit separado). Notas em docs/releases/2.0.6/NOVIDADES.md + NOTAS.md; rascunho de Discord em /Volumes/ExternalSSD/tmp/rev/discord-206.md (nao postado).
