# Player de Música — Simulador em C

Trabalho universitário de Estruturas de Dados (UTP, 2026/1).  
Demonstra **lista duplamente encadeada**, **pilha** e **fila** implementadas do zero em C99, com reprodução real de áudio via [miniaudio](https://github.com/mackron/miniaudio).

---

## Estrutura do projeto

```
estudo-dirigido/
├── Makefile
├── README.md
├── main.c / main.h
├── tipos.h                  ← tipos centrais compartilhados (NodoMusica, Pilha, Fila, Playlist…)
├── biblioteca_func/         ← lista dupla da biblioteca de músicas
├── pilha_func/              ← pilha encadeada (histórico de reprodução)
├── fila_func/               ← fila encadeada (fila de reprodução)
├── playlist_func/           ← lista duplamente encadeada de playlists
├── input_func/              ← teclado não bloqueante (Linux + Windows)
├── audio_func/              ← wrapper miniaudio (áudio real)
├── player_func/             ← loop de reprodução ao vivo
├── menu_func/               ← todos os menus de texto
└── musicas/                 ← coloque seus arquivos de áudio aqui
```

---

## Dependências

| Plataforma | Requisito |
|---|---|
| Linux | `gcc`, `make`, `libc` (pthread, m, dl — presentes na maioria das distros) |
| Windows | [MinGW-w64](https://code.visualstudio.com/docs/cpp/config-mingw) (`gcc`, `mingw32-make`) |
| Cross Linux → Windows | `mingw-w64` (`sudo apt install mingw-w64`) |

Nenhuma biblioteca externa além de `miniaudio.h`, que já está inclusa em `audio_func/`.

---

## Como compilar

### Linux

```bash
cd estudo-dirigido
make
```

O executável gerado é `player`.

### Windows (MinGW)

```cmd
cd estudo-dirigido
mingw32-make
```

O executável gerado é `player.exe`.

### Cross-compilar para Windows a partir do Linux

```bash
make windows
```

Gera `player.exe` usando o compilador `x86_64-w64-mingw32-gcc`.  
Requer: `sudo apt install mingw-w64`

### Limpar arquivos gerados

```bash
make clean            # remove .o e o binário nativo (Linux/Windows)
make clean-windows    # remove player.exe (quando gerado por cross-compilação)
make clean-all        # remove tudo
```

---

## Como usar

1. Coloque arquivos `.mp3`, `.wav` ou `.flac` dentro da pasta `musicas/`.
2. Execute o player:
   ```bash
   ./player        # Linux
   player.exe      # Windows
   ```
3. No menu principal, cadastre músicas informando título, artista e caminho do arquivo  
   (ex.: `musicas/bohemian_rhapsody.mp3`). A duração é detectada automaticamente.

### Controles durante a reprodução (sem precisar apertar ENTER)

| Tecla | Ação |
|---|---|
| `k` | Pausar / retomar |
| `j` | Avançar 5 segundos |
| `l` | Voltar 5 segundos |
| `p` | Próxima música |
| `o` | Música anterior |
| `s` | Shuffle on/off |
| `q` | Parar e voltar ao menu |

---

## Estruturas de dados utilizadas

### Lista duplamente encadeada (`biblioteca_func/`, `playlist_func/`)
- Nodos `NodoMusica` com ponteiros `prox` e `ant`.
- Usada na biblioteca principal e, via `EntradaPlaylist` (invólucro), dentro de cada playlist.
- A separação em invólucros evita corromper a cadeia da biblioteca ao reutilizar os ponteiros.

### Pilha encadeada (`pilha_func/`)
- Lista simplesmente encadeada com ponteiro de topo.
- Armazena **ponteiros** para `NodoMusica` da biblioteca (nunca cópias).
- Operações: `push`, `pop`, `peek`, `contem`, `listar`.

### Fila encadeada (`fila_func/`)
- Lista simplesmente encadeada com ponteiros de início e fim.
- Armazena **ponteiros** para `NodoMusica` da biblioteca (nunca cópias).
- Operações: `enqueue`, `dequeue`, `peek`, `contem`, `listar`.

---

## Política de remoção

Uma música **não pode ser removida** da biblioteca enquanto estiver referenciada em:
- qualquer playlist,
- o histórico de reprodução (pilha), ou
- a fila de reprodução.

O sistema exibe uma mensagem explicando o bloqueio. Remova a música das estruturas antes de excluí-la da biblioteca.

---

## Observações técnicas

- `MINIAUDIO_IMPLEMENTATION` definido apenas em `audio_func/audio.c` para evitar símbolos duplicados no linker.
- `atexit(input_modo_normal)` garante a restauração do terminal mesmo em saída abrupta (Ctrl+C).
- Buffer de entrada limpo com `getchar()` em loop — sem `fflush(stdin)` (comportamento indefinido fora do Windows).
- `dormir_ms()` usa `nanosleep` no Linux e `Sleep` no Windows para a pausa de 200 ms entre iterações do loop.
